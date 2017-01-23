//
// Created by netmind on 16. 12. 1.
//

#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "../uvcpp/Ipc.h"
#include "tlog.h"
#include "../uvcpp/MsgTask.h"
#include "../uvcpp/UvTimer.h"
#include "../uvcpp/UvPipe.h"
#include "../uvcpp/UvTcp.h"
#include "../uvcpp/HandleReceiver.h"
#include "../uvcpp/HandleSender.h"
#include "testmain.h"

#ifdef __linux
#include "../uvcpp/StreamIpc.h"
#endif
using namespace std;
using namespace uvcpp;

template <typename T> unique_ptr<T> getIpcMsg(upUvObj upobj) {
	return unique_ptr<T>(  dynamic_cast<T*>( upobj.release() )  );
}

class MyMsgObj: public UvObj {
public:
	MyMsgObj(std::string& msg) {
		extraString = msg;
	}

	std::string extraString;
};



TEST(basic, ipc) {

	std::string teststr = "my data msg";
	std::string recvstr;
	int recvCnt=0;
	int sendCnt=0;

	Ipc ipc;
	thread subtask = thread([&](){
		UvContext::open();
		ipc.open([&](IpcMsg& msg) {
			ald("msg recv: id=%u, p1=%u, p2=%u", msg.msgId, msg.param1, msg.param2);
			if(msg.msgId == 1000) {
				unique_ptr<MyMsgObj> umsg = getIpcMsg<MyMsgObj>(move(msg._upUserObj));
				if(umsg) {
					ald("  recv user msg: %s", umsg->extraString);
					recvstr = umsg->extraString;
					recvCnt++;
				} else {
					assert(0);
				}

			} else if(msg.msgId == 1001) {
				ipc.close();
			} else if(msg.msgId == 1002) {
				ald("send msg received.");
				++sendCnt;
				msg.returnValue = sendCnt;
			}
		});
		UvContext::run();
		UvContext::close();
	});
	this_thread::sleep_for(chrono::milliseconds(300));
	ipc.postMsg(1000, 1, 2, unique_ptr<MyMsgObj>( new MyMsgObj(teststr)));
	ipc.postMsg(1000, 1, 2, unique_ptr<MyMsgObj>( new MyMsgObj(teststr)));
	ipc.postMsg(1000, 1, 2, unique_ptr<MyMsgObj>( new MyMsgObj(teststr)));
	ipc.postMsg(1000, 1, 2, unique_ptr<MyMsgObj>( new MyMsgObj(teststr)));
	ipc.postMsg(1000, 1, 2, unique_ptr<MyMsgObj>( new MyMsgObj(teststr)));

	for(int i=0;i<100;i++) {
		ipc.sendMsg(1002, 1, 2, nullptr);
	}
	ipc.postMsg(1001, 1, 2, unique_ptr<MyMsgObj>( new MyMsgObj(teststr)));

	subtask.join();
	ASSERT_STREQ(teststr.c_str(), recvstr.c_str());
	ASSERT_EQ(5, recvCnt);
	ASSERT_EQ(100, sendCnt);
}

TEST(basic, msgtask) {
#define UM_POST (MsgTask::TM_USER+1)
#define UM_SEND (MsgTask::TM_USER+2)

	class MyTask: public MsgTask {
	public:
		UvTimer _timer;
		int _cnt;
		int startcheck=0;
		virtual void OnMsgProc(IpcMsg& msg) {
			if(msg.msgId == MsgTask::TM_INIT) {
				ald("task init");
				_cnt = 0;
				this_thread::sleep_for(chrono::milliseconds(200));
				startcheck = 1;
			} else if(msg.msgId == MsgTask::TM_CLOSE) {
				ald("task closing");
				_timer.stop();
			} else if(msg.msgId == UM_POST) {
				_timer.init();
				_timer.start(100, 100, [this]() {
					ald("task timer expired");
					_cnt++;
				});
			} else if(msg.msgId == UM_SEND) {
				this_thread::sleep_for(chrono::milliseconds(200));
				int* psendresult = (int*)msg.sendUserData;
				*psendresult += 100;
			}
		}
	};
	MyTask task;
	task.start(nullptr);
	ASSERT_EQ(1, task.startcheck);
	int sendresult=100;
	task.sendMsg(UM_SEND, 0, 0, &sendresult);
	ASSERT_EQ(200, sendresult);
	task.postMsg(UM_POST);
	this_thread::sleep_for(chrono::milliseconds(280));
	task.stop();
	ASSERT_EQ(2, task._cnt);
	ald("test end");
}



class WorkerTask: public MsgTask {
public:
	UvPipe pipe;
	void OnMsgProc(IpcMsg &msg)	override {
		if(msg.msgId == TM_INIT) {
			pipe.init(1);
			pipe.readStart([this](upReadBuffer upbuf) {

			});
		} else if(msg.msgId == TM_CLOSE) {
			pipe.close();
		}
	}
};

#ifdef __linux
TEST(basic, handlepass) {
	int ret;
	std::string teststr="1234";
	std::string recvstr;
	std::string echostr;
	UvContext::open();
	int fds[2]; // fds[0] read end, fds[1] is write end
	pipe(fds);

	UvTcp server;
	UvPipe pips;
	UvPipe pipc;
	UvTcp client;
	UvTcp child;
	socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
	pips.init(1);
	ret = pips.open(fds[1]);
	assert(!ret && "server pipe open error");
//	pips.bind("pipes");

	pips.readStart([&](upReadBuffer upbuf) {
		auto cnt = pips.pendingCount();
		ald("pending count=%d", cnt);
		if(cnt>0) {
			child.init();
			pips.accept(&child);
			child.readStart([&](upReadBuffer upbuf) {
				recvstr.assign(upbuf->buffer, upbuf->size);
				ald("recv: %s", recvstr);
				child.write(recvstr);
			});
			child.setOnCnnLis([&](int status) {
				if(status) {
					ald("child disconnected");
					pips.close();
					child.close();
				}
			});

		}
	});

	pipc.init(1);
	ret = pipc.open(fds[0]);
	assert(!ret && "client pipe open error");

	server.init();
	server.bindAndListen(9090, "0.0.0.0", [&]() {
		ald("incoming tcp cnn");
		UvTcp temptcp;
		temptcp.init();
		server.accept(&temptcp);
		ret = pipc.write2(&temptcp);
		assert(!ret && "write2 error");
	});

	client.init();
	client.connect("127.0.0.1", 9090, [&](int status) {
		if(!status) {
			ald("client connected");
			client.readStart([&](upReadBuffer upbuf) {
				ald("client read,...");
				echostr.assign(upbuf->buffer, upbuf->size);
				client.close();
				pipc.close();
				server.close();
			});
			client.write(teststr);
		} else {
			ald("client disconnected");
			assert(0 && "client disconnected");
		}
	});
	UvContext::run();
	UvContext::close();

	::close(fds[0]);
	::close(fds[1]);

	ASSERT_STREQ(teststr.c_str(), recvstr.c_str());
	ASSERT_STREQ(teststr.c_str(), echostr.c_str());
}
#endif

#ifdef __linux
TEST(basic, streamipc) {
	class ChildTask: public MsgTask {
	public:
		StreamIpc strmipc;
		UvTcp childCnn;
		virtual void OnMsgProc(IpcMsg &msg)  {
			if(msg.msgId == TM_INIT) {
				ali("tast init....");
				auto ret = strmipc.open([this]() {
					ali("child incoming");
					childCnn.init();
					strmipc.accept(&childCnn);
					childCnn.readStart([this](upReadBuffer upbuf) {
						childCnn.write(upbuf->buffer, upbuf->size);
					});
					childCnn.setOnCnnLis([this](int status) {
						if(status) {
							ali("child disconnected");
							childCnn.close();
							postExit();
							ali("posting exit");
						}
					});
				});
			} else if(msg.msgId == TM_CLOSE) {
				ali("task closing");
				strmipc.close();
			}
		}
	};

	std::string echostr;
	std::string teststr="1234";
	UvTcp server;
	UvTcp client;
	ChildTask childtask;
	int ret;

	UvContext::open();

	childtask.start();
	ret = childtask.strmipc.connectIpc();
	assert(!ret && "connectip ipc error");

	server.init();
	server.bindAndListen(9090, "0.0.0.0", [&]() {
		ali("incoming");
		UvTcp temptcp;
		temptcp.init();
		server.accept(&temptcp);
		auto ret = childtask.strmipc.sendUvStream(&temptcp);
		assert(!ret);
	});
	client.init();
	client.connect("127.0.0.1",  9090, [&](int status) {
		if(!status) {
			client.readStart([&](upReadBuffer upbuf) {
				echostr.assign(upbuf->buffer, upbuf->size);
				ali("client read str=%s", echostr);
				client.close();
				server.close();
				childtask.strmipc.disconnectIpc();
			});
			client.write(teststr);
		} else {
			assert(0 && "### unexpected client disconnection");
		}
	});

	UvContext::run();
	childtask.wait();
	UvContext::close();


	ASSERT_STREQ(teststr.c_str(), echostr.c_str());
}
#endif

#if 0
#ifdef __linux
TEST(basic, pipe) {
	UvContext ctx;
	ctx.openWithDefaultLoop();
	UvPipe pipec, pipes;
	std::string recvstr;
	int fds[2];
	pipe(fds);
	pipec.open(fds[0]);
	pipes.open(fds[1]);
	pipes.readStart([&](upUvReadBuffer upbuf) {
		recvstr.assign(upbuf->buffer, upbuf->size);
		pipec.close();
		pipes.close();
	});
	pipec.write("1234");
	ctx.run();
	uv_loop_close(ctx.getLoop());
	ASSERT_STREQ("1234", recvstr.c_str());
}
#endif
#endif

TEST(ipc, sender) {
	UvContext::open();
	MsgTask _task;
	HandleReceiver _receiver;
	HandleSender _sender;

	UvTcp _child;
	UvTcp _client;
	UvTcp _server;

#ifdef _WIN32
	std::string pipename = "//./pipe/worker_01";
#else
	std::string pipename = "worker_01";
#endif

	_task.setOnListener([&](IpcMsg &msg) {
		if(msg.msgId==MsgTask::TM_INIT) {
			int ret;
			ali("task init");
			ret = _receiver.open(pipename, [&](UvPipe* pipe, uv_handle_type type) {
				ali("receiver on");
				ASSERT_EQ(uv_handle_type::UV_TCP, type);
				_child.init();
				pipe->accept(&_child);
				_child.readStart([&](upReadBuffer upbuf) {
					string rs(upbuf->buffer, upbuf->size);
					_child.write(rs);
				});
			});
			ASSERT_EQ(0, ret);
		} else if(msg.msgId == MsgTask::TM_CLOSE) {
			ali("task closing");
			_child.close();
			_receiver.close();
		}
	});
	_task.start();
	_sender.connect(pipename.c_str());

	_server.init();
	_server.bindAndListen(9090, "0.0.0.0", [&]() {
		ali("incoming connection");
		logflush();
		UvTcp tmptcp;
		tmptcp.init();
		int  ret = _server.accept(&tmptcp);
		assert(ret==0);
		ret = _sender.sendHandle(&tmptcp);
		assert(ret==0);
		tmptcp.close();
	});

	UvTimer _timer;
	_timer.init();
	_timer.start(100, 0, [&](){
		ali("timer on");
		logflush();
		_client.init();
		_client.connect("127.0.0.1", 9090, [&](int status) {
			if(!status) {
				_client.readStart([&](upReadBuffer upbuf) {
					_client.close();
					_sender.close();
					_task.postExit();

				});
			}
		});
		_timer.close();
	});


	UvContext::run();
	UvContext::close();
}

TEST(ipc, handlereceiver) {
	UvContext::open();
	HandleReceiver _recv;
	UvPipe _pipe;
	UvTcp _client;
	UvTcp _svr;
	UvTcp _child;
	_svr.init();
	_svr.bindAndListen(9090, "0.0.0.0", [&](){
		ali("on incoming");
		_child.init();
		_svr.accept(&_child);
//		_pipe.write2(&_child);
//		_child.close();
	});

	_recv.open("//./pipe/recv", [&](UvPipe* pipe, uv_handle_type type) {
		ali("recv handle");
	});

	_pipe.init(1);
	ali("pipe connecting...");
	_pipe.connect("//./pipe/recv", [&](int status) {
		if(!status) {
			ali("pipe connected");
			logflush();
		}
	});


	UvTimer _timer;
	_timer.init();
	ali("timer starting...");
	_timer.start(100, 0, [&]() {
		ali("on timer");
		_timer.stop();
		_client.init();
		_client.connect("127.0.0.1", 9090, [&](int status) {
			if(!status) {
				ali("client connected");
				logflush();
			}
		});

	});


	UvContext::run();
	UvContext::close();
}
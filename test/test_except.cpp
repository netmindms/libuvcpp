//
// Created by netmind on 16. 12. 13.
//

#include <gtest/gtest.h>
#include <unordered_map>
#include "../uvcpp/UvContext.h"
#include "../uvcpp/UvTcp.h"
#include "tlog.h"
#include "../uvcpp/UvTimer.h"
#include "../uvcpp/UvPoll.h"
#include "../uvcpp/MsgTask.h"
#include "../uvcpp/UvPipe.h"
#ifdef __linux
#include "../uvcpp/StreamIpc.h"
#endif
using namespace uvcpp;

TEST(except, connect) {
	UvContext::open();
	UvTcp tcp;
	int callbackCnt=0;
	tcp.init();
	tcp.connect("111.111.111.111", 9090, [&](int status) {
		callbackCnt++;
		if(!status) {
			uld("connected");
		} else {
			uld("disconnected");
			tcp.close();
		}
	});
	tcp.close();
	UvContext::run();
	UvContext::close();
	ASSERT_EQ(0, callbackCnt);
}

TEST(except, listen) {
	UvContext::open();
	UvTcp tcp;
	int callbackCnt=0;
	tcp.init();
	tcp.listen([&]() {
		callbackCnt++;
	}, 9090);
	tcp.close();
	UvContext::run();
	UvContext::close();
	ASSERT_EQ(0, callbackCnt);
}

TEST(except, timer) {
	UvContext::open();
	UvTimer timer;
	timer.init();
	int callbackCnt=0;
	timer.start(100, 100, [&]() {
		callbackCnt++;
	});
	timer.stop();
	UvContext::run();
	UvContext::close();
	ASSERT_EQ(0, callbackCnt);
}

TEST(except, timerrep) {
	UvContext::open();
	UvTimer timer;
	timer.init();
	int callbackCnt=0;
	timer.start(100, 100, [&]() {
		uld("expired");
		callbackCnt++;
		timer.setRepeat(50);
		if(callbackCnt>=3) {
			timer.stop();
		}
	});
	UvContext::run();
	UvContext::close();
}

#ifdef __linux
namespace {
	class ChildCnn : public UvTcp {
	public:
		ChildCnn() {
			id = 0;
		}

		uint32_t id;
	};

	class TcpClient : public UvTcp {
	public:
		std::list<TcpClient>::iterator iditr;
	};

	TEST(except, tcpmassive) {
		class ChildTask : public MsgTask {
		public:
			virtual void OnMsgProc(IpcMsg &msg) {
				if (msg.msgId == TM_INIT) {
					uli("child task init...");
					_idSeed = 0;
					_strmIpc.open([this]() {
						auto id = ++_idSeed;
						if (id == 0) id = ++_idSeed;
						auto &cnn = _childCnnMap[id];
						cnn.init();
						_strmIpc.accept(&cnn);
						cnn.id = id;
						activeChild(cnn);
					});
				} else if (msg.msgId == TM_CLOSE) {
					uli("task closing...");
					_strmIpc.close();
				}
			}

			void connectStreamIpc() {
				_strmIpc.connectIpc();
			}

			StreamIpc _strmIpc;
		private:
			uint32_t _idSeed;

			std::unordered_map<uint32_t, ChildCnn> _childCnnMap;

			void activeChild(ChildCnn &cnn) {
				uld("new child cnn,...id=%d", cnn.id);
				cnn.readStart([this, &cnn](upReadBuffer upbuf) {
					cnn.write(upbuf->buffer, upbuf->size); // echo message
				});
				cnn.setOnCnnLis([this, &cnn](int status) {
					if (status) {
						uld("child disconnected");
						cnn.close();
						_childCnnMap.erase(cnn.id);
						// dead zone from here
						// WARN: do not add any code from here
					}
				});
			}
		};

		int ret;
		uint32_t rrobin = 0;
		std::string teststr = "1234";
		UvTcp server;
		ChildTask childtasks[4];

		UvContext::open();

		for (int i = 0; i < 4; i++) {
			childtasks[i].start();
			ret = childtasks[i]._strmIpc.connectIpc();
			assert(!ret && "### stream ipc connect error");
		}

		server.init();
		server.bindAndListen(9090, "127.0.0.1", [&]() {
			UvTcp temptcp;
			temptcp.init();
			server.accept(&temptcp);
			auto taskid = (rrobin++) % 4;
			childtasks[taskid]._strmIpc.sendUvStream(&temptcp);
		});

		// all clients


		std::list<TcpClient> clientlist;
		UvTimer itertimer;
		int clientConnectCnt = 0;
		int trycnt = 0;
		itertimer.init();
		itertimer.start(1, 1, [&]() {
			if (trycnt >= 100) {
				uld("iter timer stop");
				itertimer.stop();
			} else {
				clientlist.emplace_front();
				auto itr = clientlist.begin();
				itr->iditr = itr;
				trycnt++;
				auto client = &(*itr);
				itr->init();
				itr->connect("127.0.0.1", 9090, [&, client](int status) {
					if (!status) {
						clientConnectCnt++;
						if(clientConnectCnt==100) {
							uli("100th connected");
						}
						client->readStart([&](upReadBuffer upbuf) {
							std::string ts(upbuf->buffer, upbuf->size);
							assert(ts == teststr);
							client->close();
						});
						client->write(teststr);
					} else {
						assert(0);
					}
				});
			}
		});

		UvTimer serverTimer;
		serverTimer.init();
		serverTimer.start(3000, 3000, [&]() {
			serverTimer.stop();
			server.close();
			for (int i = 0; i < 4; i++) {
				childtasks[i]._strmIpc.disconnectIpc();
				childtasks[i].stop();
			}
		});
		UvContext::run();
		UvContext::close();
		ASSERT_EQ(100, clientConnectCnt);
	}
}
#endif


#ifdef __linux
#include <sys/inotify.h>
TEST(except, inotify) {
	FILE* st = fopen("test.txt", "wb");
	fclose(st);
	UvContext::open();
	UvPoll ipoll;
	int fd = inotify_init();
	inotify_add_watch(fd, "test.txt", IN_CLOSE_WRITE);
	ipoll.init(fd);
	ipoll.start(UV_READABLE, [&](int event) {
		inotify_event evt;
		::read(ipoll.getFd(), &evt, sizeof(evt));
		uld("file closed on write");
		ipoll.stop();
		::close(fd);
	});
	st = fopen("test.txt", "a+b");
	fwrite("1234", 1, 4, st);
	UvTimer timer;
	timer.init();
	timer.start(1000, 1000, [&]() {
		uld("timer stop");
		timer.stop();
		fclose(st);
	});
	UvContext::run();
	UvContext::close();
}
#endif
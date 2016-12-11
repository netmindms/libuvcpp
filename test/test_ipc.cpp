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
		UvContext subctx;
		subctx.open();
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
		subctx.run();
		subctx.close();
	});
	this_thread::sleep_for(chrono::milliseconds(100));
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
	class MyTask: public MsgTask {
	public:
		UvTimer _timer;
		int _cnt;
		int startcheck=0;
		void OnMsgProc(IpcMsg& msg) {
			if(msg.msgId == MsgTask::TM_INIT) {
				ald("task init");
				_cnt = 0;
				_timer.init();
				_timer.start(100, 100, [this]() {
					ald("task timer expired");
					_cnt++;
				});
				startcheck = 1;
			} else if(msg.msgId == MsgTask::TM_CLOSE) {
				ald("task closing");
				_timer.stop();
			}
		}
	};
	MyTask task;
	task.start(nullptr);
	ASSERT_EQ(1, task.startcheck);
	this_thread::sleep_for(chrono::milliseconds(280));
	task.stop();
	ASSERT_EQ(2, task._cnt);
	ald("test end");
}

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
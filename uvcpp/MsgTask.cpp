//
// Created by netmind on 16. 12. 1.
//

#include <condition_variable>
#include "MsgTask.h"

using namespace std;

namespace uvcpp {

	MsgTask::MsgTask() {

	}

	MsgTask::~MsgTask() {

	}

	int MsgTask::start(void *arg) {
		std::condition_variable startSync;
		std::mutex startMutex;

		std::unique_lock<mutex> ulock(startMutex);
		_msgThread = thread([&, this]() {
			UvContext ctx;
			ctx.open();
			std::unique_lock<mutex> tlock(startMutex);
			_ipc.open([&](IpcMsg& msg) {
				OnMsgProc(msg);
				if(msg.msgId == TM_CLOSE) {
					_ipc.close();
				}
			});

			IpcMsg msg;
			msg.msgId = TM_INIT;
			msg.isSync = false;
			OnMsgProc(msg);
			startSync.notify_one();
			tlock.unlock();

			ctx.run();
			ctx.close();
		});
		startSync.wait(ulock);
		return 0;
	}

	void MsgTask::stop() {
		_ipc.sendMsg(TM_CLOSE, 0, 0, nullptr);
		_msgThread.join();
	}

	void MsgTask::OnMsgProc(IpcMsg &msg) {
		if(_lis) {
			_lis(msg);
		}
	}

	void MsgTask::postExit() {
		_ipc.postMsg(TM_CLOSE, 0, 0, nullptr);
	}

	void MsgTask::wait() {
		if(_msgThread.joinable()) {
			_msgThread.join();
		}
	}

	void MsgTask::setOnListener(MsgTask::Lis lis) {
		_lis = lis;
	}
}
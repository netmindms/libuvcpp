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
			std::unique_lock<mutex> tlock(startMutex);
			_ipc.open([&](IpcMsg& msg) {
				OnMsgProc(msg);
			});

			IpcMsg msg;
			msg.msgId = TM_INIT;
			msg.isSync = false;
			OnMsgProc(msg);
			startSync.notify_one();
			tlock.unlock();

		});
		startSync.wait(ulock);
		return 0;
	}

	void MsgTask::stop() {

	}

	void MsgTask::OnMsgProc(IpcMsg &msg) {

	}
}
//
// Created by netmind on 16. 12. 1.
//

#include <condition_variable>
#include "MsgTask.h"
#include "uvcpplog.h"

using namespace std;

namespace uvcpp {

	MsgTask::MsgTask() {

	}

	MsgTask::~MsgTask() {

	}

	int MsgTask::start(void *arg) {
		ald("task starting...");
		std::condition_variable startSync;
		std::mutex startMutex;

		std::unique_lock<mutex> ulock(startMutex);
		_msgThread = thread([&, this]() {
			UvContext::open();
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
			msg.sendUserData = arg;
			OnMsgProc(msg);
			startSync.notify_one();
			tlock.unlock();

			UvContext::run();
			UvContext::close();
		});
		startSync.wait(ulock);
		ald("task starting sync ok");
		return 0;
	}

	void MsgTask::stop() {
		ald("task stopping");
		_ipc.sendMsg(TM_CLOSE, 0, 0, nullptr);
		_msgThread.join();
		ald("task stop ok");
	}

	void MsgTask::OnMsgProc(IpcMsg &msg) {
		if(_lis) {
			_lis(msg);
		}
	}

	void MsgTask::postExit() {
		ald("post exiting,");
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

	int MsgTask::postMsg(uint32_t msgid, uint32_t p1, uint32_t p2, upUvObj userobj) {
		return _ipc.postMsg(msgid, p1, p2, move(userobj));
	}

	int MsgTask::sendMsg(uint32_t msgid, uint32_t p1, uint32_t p2, void* userdata) {
		return _ipc.sendMsg(msgid, p1, p2, userdata);
	}
}
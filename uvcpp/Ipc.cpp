//
// Created by netmind on 16. 12. 1.
//

#include "Ipc.h"
#include "uvcpplog.h"
using namespace std;

namespace uvcpp {
	Ipc::Ipc() {
		_isClosing = false;
	}
	Ipc::~Ipc() {

	}


	int Ipc::open(Lis lis) {
		uld("ipc open, ...");
		_lis = lis;
		_async.init([this]() {
			upIpcMsg upmsg;
			for(;!_isClosing;) {
				_msgQue.lock();
				upmsg = _msgQue.pop_front();
				_msgQue.unlock();
				if(upmsg) {
					if(upmsg->isSync) {
						ulv("ripc sync message");
						unique_lock<mutex> msgulock(*(upmsg->msgMutex));
						ulv("ripc lock ok");
						_lis(*(upmsg.get()));
						upmsg->msgCv->notify_one();
						ulv("ripc notify ok");
						msgulock.unlock();
						ulv("ripc unlock ok");
					} else {
						_lis(*(upmsg.get()));
					}
				} else {
					break;
				}
			}
		});
		return 0;
	}

	void Ipc::close() {
		uld("ipc closeHandle");
		_msgQue.lock();
		_isClosing = true;
		_async.close(nullptr);
		_msgQue.unlock();
		ulv("ipc closeHandle ok");
	}

	int Ipc::postMsg(uint32_t msgid, uint32_t p1, uint32_t p2, upUvObj userobj) {
		uld("postMsg, msgid=%u, p1=%d, p2=%d", msgid, p1, p2);
		_msgQue.lock();
		auto upmsg = _msgQue.allocObj();
		upmsg->msgId = msgid;
		upmsg->param1 = p1;
		upmsg->param2 = p2;
		upmsg->_upUserObj = move(userobj);
		upmsg->isSync = false;
		_msgQue.push(move(upmsg));
		_async.send();
		_msgQue.unlock();
		return 0;
	}

	int Ipc::sendMsg(uint32_t msgid, uint32_t p1, uint32_t p2, void* userdata) {
		upIpcMsg upmsg;
		_msgQue.lock();

		upmsg = _msgQue.allocObj();
		upmsg->msgId = msgid;
		upmsg->param1 = p1;
		upmsg->param2 = p2;
//		upmsg->_upUserObj = move(userobj);
		upmsg->sendUserData = userdata;
		std::mutex mtx;
		std::condition_variable cv;
		upmsg->msgMutex = &mtx;
		upmsg->msgCv = &cv;
		upmsg->isSync = true;
		std::unique_lock<mutex> ulock( mtx ); // lock을 거는 이유: msg큐에 push한 후 task switching 되어 wait 하기 전에 메시지 처리 되는 것을 방지 한다.
		ulv("send msgq lock ok");
		_msgQue.push(move(upmsg));
		_async.send();
		_msgQue.unlock();
		ulv("send msgq unlock ok");
		cv.wait(ulock);
		ulv("send lock wait ok");
		ulock.unlock();
		return 0;
	}



}

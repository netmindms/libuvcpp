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
		ald("ipc open, ...");
		_lis = lis;
		_msgQue.open(10);
		_async.open([this]() {
			upIpcMsg upmsg;
			for(;!_isClosing;) {
				_msgQue.lock();
				upmsg = _msgQue.pop();
				_msgQue.unlock();
				if(upmsg) {
					if(upmsg->isSync) {
						alv("ripc sync message");
						unique_lock<mutex> msgulock(*(upmsg->msgMutex));
						alv("ripc lock ok");
						_lis(*(upmsg.get()));
						upmsg->msgCv->notify_one();
						alv("ripc notify ok");
						msgulock.unlock();
						alv("ripc unlock ok");
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
		ald("ipc close");
		_msgQue.lock();
		_isClosing = true;
		_async.close();
		_msgQue.unlock();
		alv("ipc close ok");
	}

	int Ipc::postMsg(uint32_t msgid, int p1, int p2, upUvObj userobj) {
		ald("postMsg, msgid=%u, p1=%d, p2=%d", msgid, p1, p2);
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

	int Ipc::sendMsg(uint32_t msgid, int p1, int p2, upUvObj userobj) {
		upIpcMsg upmsg;
		_msgQue.lock();

		upmsg = _msgQue.allocObj();
		upmsg->msgId = msgid;
		upmsg->param1 = p1;
		upmsg->param2 = p2;
		upmsg->_upUserObj = move(userobj);
		std::mutex mtx;
		std::condition_variable cv;
		upmsg->msgMutex = &mtx;
		upmsg->msgCv = &cv;
		upmsg->isSync = true;
		std::unique_lock<mutex> ulock( mtx ); // lock을 거는 이유: msg큐에 push한 후 task switching 되어 wait 하기 전에 메시지 처리 되는 것을 방지 한다.
		alv("send msgq lock ok");
		_msgQue.push(move(upmsg));
		_async.send();
		_msgQue.unlock();
		alv("send msgq unlock ok");
		cv.wait(ulock);
		alv("send lock wait ok");
		ulock.unlock();
		return 0;
	}



}

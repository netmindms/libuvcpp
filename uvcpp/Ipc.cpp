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
						ald("ripc sync message");
						unique_lock<mutex> msgulock(*(upmsg->msgMutex));
						ald("ripc lock ok");
						_lis(*(upmsg.get()));
						upmsg->msgCv->notify_one();
						ald("ripc notify ok");
						msgulock.unlock();
						ald("ripc unlock ok");
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
		_msgQue.lock();
		_isClosing = true;
		_async.close();
		_msgQue.unlock();
	}

	int Ipc::postMsg(uint32_t msgid, int p1, int p2, upUvObj userobj) {
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
		ald("sendMsg");
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
		ald("slock ok");
		_msgQue.push(move(upmsg));
		_async.send();
		_msgQue.unlock();
		ald("s msgque unlock ok");
		cv.wait(ulock);
		ald("slock wait ok");
		ulock.unlock();
		return 0;
	}



}

//
// Created by netmind on 16. 12. 1.
//

#include "Ipc.h"

using namespace std;

namespace uvcpp {
	int Ipc::open(Lis lis) {
		_lis = lis;
		_msgQue.open(10);
		_async.open([this]() {
			upIpcMsg upmsg;
			for(;;) {
				_msgQue.lock();
				upmsg = _msgQue.pop();
				_msgQue.unlock();
				if(upmsg) {
					_lis(*(upmsg.get()));
				} else {
					break;
				}
			}
		});
		return 0;
	}

	void Ipc::close() {
		_async.close();
		_msgQue.lock();
		_msgQue.clear();
		_msgQue.unlock();
	}

	int Ipc::postMsg(uint32_t msgid, int p1, int p2, upUvObj userobj) {
		_msgQue.lock();
		auto upmsg = _msgQue.allocObj();
		upmsg->msgId = msgid;
		upmsg->param1 = p1;
		upmsg->param2 = p2;
		upmsg->_upUserObj = move(userobj);
		_msgQue.push(move(upmsg));
		_async.send();
		_msgQue.unlock();
		return 0;
	}
}

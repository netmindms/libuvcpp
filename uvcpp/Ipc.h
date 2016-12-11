//
// Created by netmind on 16. 12. 1.
//

#ifndef UVCPPPRJ_IPC_H
#define UVCPPPRJ_IPC_H

#include "ObjQue.h"
#include "UvAsync.h"
#include "IpcMsg.h"

namespace uvcpp {
	class Ipc {
	public:
		typedef std::function<void(IpcMsg& msg)> Lis;
		Ipc();

		virtual ~Ipc();

		int open(Lis lis);

		void close();

		int postMsg(uint32_t msgid, uint32_t p1=0, uint32_t p2=0, upUvObj userobj=nullptr);

		int sendMsg(uint32_t msgid, uint32_t p1=0, uint32_t p2=0, void* userdata=nullptr);
	private:
		Lis _lis;
		UvAsync _async;
		ObjQue<IpcMsg> _msgQue;
		bool _isClosing;
	};

}


#endif //UVCPPPRJ_IPC_H

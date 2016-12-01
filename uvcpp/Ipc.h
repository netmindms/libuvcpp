//
// Created by netmind on 16. 12. 1.
//

#ifndef UVCPPPRJ_IPC_H
#define UVCPPPRJ_IPC_H

#include <nmdutil/ObjMemQue.h>
#include "UvAsync.h"
#include "IpcMsg.h"

namespace uvcpp {
	class Ipc {
	public:
		typedef std::function<void(IpcMsg& msg)> Lis;
		int open(Lis lis);
		void close();
		int postMsg(uint32_t msgid, int p1, int p2, upUvObj userobj);
		int sendMsg(uint32_t msgid, int p1, int p2, upUvObj userobj);
	private:
		Lis _lis;
		UvAsync _async;
		nmdu::ObjMemQue<IpcMsg> _msgQue;
	};

}


#endif //UVCPPPRJ_IPC_H

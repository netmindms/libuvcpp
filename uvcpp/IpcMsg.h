//
// Created by netmind on 16. 12. 1.
//

#ifndef UVCPPPRJ_IPCMSG_H
#define UVCPPPRJ_IPCMSG_H

#include <cstdint>
#include <memory>
#include <condition_variable>
#include "UvObj.h"

namespace uvcpp {

	struct IpcMsg {
		uint32_t msgId;
		uint32_t param1;
		uint32_t param2;
		std::unique_ptr<UvObj> _upUserObj;
		void* sendUserData;
		std::condition_variable* msgCv;
		std::mutex* msgMutex;
		bool isSync;
		int returnValue;
	};

	typedef std::unique_ptr<IpcMsg> upIpcMsg;
}

#endif //UVCPPPRJ_IPCMSG_H

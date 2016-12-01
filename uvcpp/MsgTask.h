//
// Created by netmind on 16. 12. 1.
//

#ifndef UVCPPPRJ_MSGTASK_H
#define UVCPPPRJ_MSGTASK_H

#include <thread>
#include "Ipc.h"

namespace uvcpp {
	class MsgTask {
	public:
		enum MSG {
			TM_INIT=0,
			TM_CLOSE=1,
		};
		MsgTask();

		virtual ~MsgTask();

		int start(void* arg);
		void stop();

		virtual void OnMsgProc(IpcMsg& msg);

	private:
		Ipc _ipc;
		std::thread _msgThread;
	};
}

#endif //UVCPPPRJ_MSGTASK_H

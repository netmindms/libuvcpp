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
		class Runnable: public UvObj {
		private:
			friend class MsgTask;
			std::function<void()> func;
		};

		typedef std::function<void(IpcMsg&)> Lis;
		enum MSG {
			TM_INIT=0,
			TM_CLOSE,
			TM_RUNNABLE,
			TM_RUNNABLE_ASYNC,
			TM_USER=1000,
		};
		MsgTask();

		virtual ~MsgTask();

		int start(void* arg=nullptr);
		void stop();
		int postMsg(uint32_t msgid, uint32_t p1=0, uint32_t p2=0, upUvObj userobj=nullptr);

		int sendMsg(uint32_t msgid, uint32_t p1=0, uint32_t p2=0, void* userdata=nullptr);

		int runOnThread(std::function<void()> func);

		int runOnThreadAsync(std::function<void()> func);

		void postExit();
		void wait();
		void setOnListener(Lis lis);
		virtual void OnMsgProc(IpcMsg& msg);

	private:
		Lis _lis;
		Ipc _ipc;
		std::thread _msgThread;
	};
}

#endif //UVCPPPRJ_MSGTASK_H

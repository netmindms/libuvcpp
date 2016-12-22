//
// Created by netmind on 16. 12. 22.
//

#ifndef UVCPPPRJ_UVSIGNAL_H
#define UVCPPPRJ_UVSIGNAL_H

#include "UvHandle.h"

namespace uvcpp {
	class UvSignal : public UvHandle {
	public:
		typedef std::function<void()> Lis;
		int init();
		int start(int signum, Lis lis);
		void stop(bool isclose=true);
	private:
		static void signal_cb(uv_signal_t* handle, int signum);
		Lis _lis;
	};
}

#endif //UVCPPPRJ_UVSIGNAL_H

//
// Created by netmind on 16. 12. 22.
//

#ifndef UVCPPPRJ_UVSPAWN_H
#define UVCPPPRJ_UVSPAWN_H

#include "UvHandle.h"

namespace uvcpp {
	class UvSpawn : public UvHandle {
	public:
		typedef std::function<void(int64_t, int)> Lis;
		int spawn(uv_process_options_t* options, Lis lis);
		int processKill(int signum);
	private:
		Lis _lis;
		static void exit_cb(uv_process_t*, int64_t exit_status, int term_signal);
	};
}

#endif //UVCPPPRJ_UVSPAWN_H

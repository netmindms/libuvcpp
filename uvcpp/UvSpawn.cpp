//
// Created by netmind on 16. 12. 22.
//

#include "UvSpawn.h"
#define RAWH() ((uv_process_t*)getRawHandle())
#define GETOBJH(H) ((UvSpawn*)(((HandleHolder*)H->data))->uvh)

namespace uvcpp {

	int UvSpawn::spawn(uv_process_options_t *options, Lis lis) {
		_lis = lis;
		initHandle();
		options->exit_cb = exit_cb;
		return uv_spawn(getLoop(), RAWH(), options );
	}

	void UvSpawn::exit_cb(uv_process_t *rawh, int64_t exit_status, int term_signal) {
		auto uvh = GETOBJH(rawh);
		uvh->_lis(exit_status, term_signal);
	}

	int UvSpawn::processKill(int signum) {
		return uv_process_kill(RAWH(), signum);
	}

}
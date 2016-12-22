//
// Created by netmind on 16. 12. 22.
//

#include "UvSignal.h"

#define RAWH() ((uv_signal_t*)getRawHandle())
#define GETOBJH(H) ((UvSignal*)(((HandleHolder*)H->data))->uvh)

namespace uvcpp {

	int UvSignal::init() {
		initHandle();
		return uv_signal_init(getLoop(), RAWH());
	}

	int UvSignal::start(int signum, UvSignal::Lis lis) {
		_lis = lis;
		return uv_signal_start(RAWH(), signal_cb, signum);
	}

	void UvSignal::signal_cb(uv_signal_t *handle, int signum) {
		auto uvh = GETOBJH(handle);
		uvh->_lis();
	}

	void UvSignal::stop(bool isclose) {
		uv_signal_stop(RAWH());
		if(isclose) {
			close();
		}
	}
}
/*
 * UvTimer2.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#include "UvTimer.h"
#include "uvcppdef.h"
#include "uvcpplog.h"

using namespace uvcpp;
#define RAWH() ((uv_timer_t*)getRawHandle())
#define GETOBJH(H) ((UvTimer*)(((HandleHolder*)H->data))->uvh)

namespace uvcpp {
	UvTimer::UvTimer() {

	}

	UvTimer::~UvTimer() {
		close();
	}

	int UvTimer::start(uint64_t period, uint64_t first_expire, Lis lis) {
		assert(RAWH());
		_lis = lis;
		return uv_timer_start(RAWH(), timer_cb, first_expire, period);
	}

	void UvTimer::reset() {
		uv_timer_again((uv_timer_t*)getRawHandle());
	}

	void UvTimer::stop(bool bclose) {
		if(RAWH()) {
			uv_timer_stop((uv_timer_t*)getRawHandle());
		}
		if(bclose) {
			close();
		}
	}

	void UvTimer::timer_cb(uv_timer_t *rawh) {
		auto ptimer = GETOBJH(rawh);
		ptimer->_lis();
	}

	int UvTimer::init() {
		auto ret = initHandle();
		if(!ret) {
			return uv_timer_init(getLoop(), (uv_timer_t*)getRawHandle());
		}
		return ret;
	}

}
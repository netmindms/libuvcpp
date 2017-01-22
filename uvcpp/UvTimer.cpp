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
	}

	int UvTimer::start(uint64_t timeout, uint64_t repeat, Lis lis) {
		assert(RAWH());
		_lis = lis;
		return uv_timer_start(RAWH(), timer_cb, timeout, repeat);
	}

	void UvTimer::again() {
		uv_timer_again((uv_timer_t*)getRawHandle());
	}

	void UvTimer::stop(bool bclose) {
		if(_status == UvHandle::INITIALIZED) {
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
//			_period = 0;
			return uv_timer_init(getLoop(), (uv_timer_t*)getRawHandle());
		}
		return ret;
	}

	void UvTimer::setRepeat(uint64_t repeat) {
		uv_timer_set_repeat(RAWH(), repeat);
		if(repeat == 0) {
			stop();
		}
	}

	uint64_t UvTimer::getRepeat() {
		return uv_timer_get_repeat(RAWH());
	}

}
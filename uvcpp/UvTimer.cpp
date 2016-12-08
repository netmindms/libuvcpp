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

namespace uvcpp {
	UvTimer::UvTimer() {

	}

	UvTimer::~UvTimer() {
		close();
	}

	void UvTimer::timerStart(uint64_t period, uint64_t first_expire, Lis lis) {
		_lis = lis;
		uv_timer_start((uv_timer_t*)getRawHandle(), timer_cb, first_expire, period);
	}

	void UvTimer::reset() {
		uv_timer_again((uv_timer_t*)getRawHandle());
	}

	void UvTimer::timerStop() {
		uv_timer_stop((uv_timer_t*)getRawHandle());
	}

	void UvTimer::timer_cb(uv_timer_t *rawh) {
		auto ptimer = (UvTimer*)rawh->data;
		ptimer->_lis();
	}

//	void UvTimer::close(UvHandle::CloseLis lis) {
//		auto rawh = (uv_timer_t*)getRawHandle();
//		if (rawh) {
//			uv_timer_stop(rawh);
//		}
//		UvHandleOwner::close(lis);
//	}

	UvTimer *UvTimer::init() {
		auto ptimer = new UvTimer;
		uv_timer_init(UvContext::getLoop(), (uv_timer_t*)ptimer->getRawHandle());
		ptimer->registerContext();
		return ptimer;
	}

}
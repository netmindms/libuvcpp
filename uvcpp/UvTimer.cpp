/*
 * UvTimer2.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#include "UvTimer.h"
#include "UvTimerHandle.h"
#include "uvcppdef.h"
namespace uvcpp {
	UvTimer::UvTimer() {
		_ohandle = nullptr;

	}

	UvTimer::~UvTimer() {
	}

	void UvTimer::set(uint64_t period, uint64_t first_expire, Lis lis) {
		if (!_ohandle) {
			_lis = lis;
			auto ctx = UvContext::getContext();
			_ohandle = ctx->createHandle(this);
			uv_timer_t* rawh = (uv_timer_t*)_ohandle->getRawHandle();
			uv_timer_init(ctx->getLoop(), rawh);
			uv_timer_start(rawh, timer_cb, first_expire, period);
		}
	}

	void UvTimer::reset() {
		if (!_ohandle) {
			uv_timer_again((uv_timer_t*)_ohandle->getRawHandle());
		}
	}

	void UvTimer::kill() {
		if (_ohandle) {
			uv_timer_stop((uv_timer_t*)_ohandle->getRawHandle());
			_ohandle->close(); _ohandle = nullptr;
		}
	}

	void UvTimer::timer_cb(uv_timer_t *rawh) {
		ASSERT_RAW_UVHANDLE(rawh);
		auto ptimer = GET_UVHANDLE_OWNER(UvTimer, rawh);
		ptimer->_lis();
	}

}
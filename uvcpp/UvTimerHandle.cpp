/*
 * UvTimerHandle.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#include <uv.h>
#include <nmdutil/nmdlog.h>
#include "UvTimerHandle.h"

namespace uvcpp {
	UvTimerHandle::UvTimerHandle() {
		// TODO Auto-generated constructor stub

	}

	UvTimerHandle::~UvTimerHandle() {
		// TODO Auto-generated destructor stub
	}

	int UvTimerHandle::open() {
		uv_timer_init(_ctx->getLoop(), &_handle);
		ali("timer init, active=%d", uv_is_active((uv_handle_t *) &_handle));
		return 0;
	}

	uv_handle_t *UvTimerHandle::getRawHandle() {
		return (uv_handle_t *) &_handle;
	}

	int UvTimerHandle::timer_start(uint64_t expire, uint64_t period, Lis lis) {
		_lis = lis;
		return uv_timer_start(&_handle, timer_cb, expire, period);
	}

	int UvTimerHandle::timer_stop() {
		return uv_timer_stop(&_handle);
	}

	int UvTimerHandle::timer_again() {
		return uv_timer_again(&_handle);
	}

	void UvTimerHandle::timer_cb(uv_timer_t *handle) {
		auto ptimer = (UvTimerHandle *) handle->data;
		ptimer->_lis();
	}
}
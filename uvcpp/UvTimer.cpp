/*
 * UvTimer2.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#include "UvTimer.h"
#include "UvTimerHandle.h"

namespace uvcpp {
	UvTimer::UvTimer() {
		_handle = nullptr;

	}

	UvTimer::~UvTimer() {
	}

	void UvTimer::set(uint64_t period, uint64_t first_expire, Lis lis) {
		if (!_handle) {
			auto ctx = UvContext::getContext();
			_handle = ctx->createUvHandle<UvTimerHandle>();
			_handle->open();
			_handle->timer_start(first_expire, period, lis);
		}
	}

	void UvTimer::reset() {
		if (!_handle) {
			_handle->timer_again();
		}
	}

	void UvTimer::kill() {
		if (_handle) {
			_handle->timer_stop();
			_handle->close();
			_handle = nullptr;
		}
	}

}
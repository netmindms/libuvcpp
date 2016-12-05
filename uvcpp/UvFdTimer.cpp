/*
 * UvFdTimer.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#ifdef __linux

#include "uvcpplog.h"
#include <sys/timerfd.h>

#include "UvFdTimer.h"

namespace uvcpp {
	UvFdTimer::UvFdTimer() {
		_fd = -1;
		_fireCount = 0;
		_rawh = nullptr;
	}

	UvFdTimer::~UvFdTimer() {
		kill();
	}

	void UvFdTimer::setUsec(uint64_t usec, uint64_t first_usec, Lis lis) {
		if (!_rawh) {
			auto ctx = UvContext::getContext();
			if (ctx) {
				_rawh = (uv_poll_t*)createHandle("fdtimer");
				auto fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
				ali("timerfd create fd=%d", fd);
				if (fd > 0) {
					uv_poll_init(_ctx->getLoop(), _rawh, fd);
				}
				_fd = fd;
				_lis = lis;

				mTimerSpec.it_interval.tv_sec = usec / 1000000;
				mTimerSpec.it_interval.tv_nsec = (usec % 1000000) * 1000;
				uint64_t iusec = (first_usec == 0) ? usec : first_usec;
				mTimerSpec.it_value.tv_sec = iusec / 1000000;
				mTimerSpec.it_value.tv_nsec = (iusec % 1000000) * 1000;

				timerfd_settime(_fd, 0, &mTimerSpec, NULL);
				uv_poll_start(_rawh, UV_READABLE, poll_cb);
			} else {
				ale("### context not found");
				assert(0);
			}

		}
	}

	void UvFdTimer::set(uint64_t expire, uint64_t period, Lis lis) {
		setUsec(expire * 1000, period * 1000, lis);
	}

	void UvFdTimer::reset() {
		pause();
		resume();
	}

	void UvFdTimer::kill() {
		if (_rawh) {
			uv_poll_stop(_rawh);
			close();
			::close(_fd);
			_fd = -1;
			_rawh = nullptr;
		}
	}

	void UvFdTimer::pause(void) {
		struct itimerspec ts;
		memset(&ts, 0, sizeof(ts));
		timerfd_settime(_fd, 0, &ts, NULL);
	}

	void UvFdTimer::resume(void) {
		timerfd_settime(_fd, 0, &mTimerSpec, NULL);
	}

	void UvFdTimer::pollCb(int status, int events) {
		read(_fd, &_fireCount, sizeof(_fireCount));
		ald("fd count=%lu", _fireCount);
		_lis();
	}

	void UvFdTimer::poll_cb(uv_poll_t *handle, int status, int events) {
		ali("poll callback");
		auto pfdtimer = UvHandleOwner::getHandleOwner<UvFdTimer>((uv_handle_t*)handle);
		pfdtimer->pollCb(status, events);
	}

	uint64_t UvFdTimer::getFireCount() {
		return _fireCount;
	}
}
#endif // __linux

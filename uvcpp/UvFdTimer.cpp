/*
 * UvFdTimer.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#ifdef __linux

#include <nmdutil/nmdlog.h>
#include <sys/timerfd.h>

#include "UvFdTimer.h"
#include "UvPollHandle.h"

namespace uvcpp {
	UvFdTimer::UvFdTimer() {
		_fd = -1;
		_handle = nullptr;
		_fireCount = 0;
	}

	UvFdTimer::~UvFdTimer() {
		// TODO Auto-generated destructor stub
	}

	void UvFdTimer::setUsec(uint64_t usec, uint64_t first_usec, Lis lis) {
		if (!_handle) {
			auto ctx = UvContext::getContext();
			if (ctx) {
				_handle = ctx->createUvHandle<UvPollHandle>();
				auto fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
				ali("timerfd create fd=%d", fd);
				if (fd > 0) {
					_handle->open(fd);
				}
				_fd = fd;
				_lis = lis;

				mTimerSpec.it_interval.tv_sec = usec / 1000000;
				mTimerSpec.it_interval.tv_nsec = (usec % 1000000) * 1000;
				uint64_t iusec = (first_usec == 0) ? usec : first_usec;
				mTimerSpec.it_value.tv_sec = iusec / 1000000;
				mTimerSpec.it_value.tv_nsec = (iusec % 1000000) * 1000;

				timerfd_settime(_fd, 0, &mTimerSpec, NULL);
				_handle->poll_start(UV_READABLE, [this](int events) {
					if (events | UV_READABLE) {
						read(_fd, &_fireCount, sizeof(_fireCount));
						_lis();
					}
				});

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
		if (_handle) {
			_handle->poll_stop();
			_handle->close();
			_handle = nullptr;
			::close(_fd);
			_fd = -1;
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

}
#endif // __linux

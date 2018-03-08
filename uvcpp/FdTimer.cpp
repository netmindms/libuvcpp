/*
 * UvFdTimer.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#ifdef __linux

#include "uvcpplog.h"
#include <sys/timerfd.h>

#include "FdTimer.h"
#define RAWH() ((uv_idle_t*)getRawHandle())
#define GETOBJH(H) ((UvIdle*)(((HandleHolder*)H->data))->uvh)
namespace uvcpp {
	FdTimer::FdTimer() {
		_fireCount = 0;
		_fd = -1;
	}

	FdTimer::~FdTimer() {
	}

	void FdTimer::startUsec(uint64_t expire_us, uint64_t period_usec, Lis lis) {
		_lis = lis;

		mTimerSpec.it_interval.tv_sec = period_usec / 1000000;
		mTimerSpec.it_interval.tv_nsec = (period_usec % 1000000) * 1000;

		mTimerSpec.it_value.tv_sec = expire_us / 1000000;
		mTimerSpec.it_value.tv_nsec = (expire_us % 1000000) * 1000;

		timerfd_settime(_fd, 0, &mTimerSpec, NULL);
		auto ret = UvPoll::start(UV_READABLE, [&](int events) {
			int rcnt = read(_fd, &_fireCount, sizeof(_fireCount));
			_lis();
		});
		assert(!ret);
	}

	void FdTimer::start(uint64_t expire, uint64_t period, Lis lis) {
		startUsec(expire * 1000, period * 1000, lis);
	}

	void FdTimer::again() {
		pause();
		resume();
	}

	void FdTimer::stop(bool isclose) {
		if(_fd>=0) {
			UvPoll::stop(isclose);
			::close(_fd);
			_fd = -1;
		}
	}

	void FdTimer::pause(void) {
		struct itimerspec ts;
		memset(&ts, 0, sizeof(ts));
		timerfd_settime(_fd, 0, &ts, NULL);
	}

	void FdTimer::resume(void) {
		timerfd_settime(_fd, 0, &mTimerSpec, NULL);
	}

	uint64_t FdTimer::getFireCount() {
		return _fireCount;
	}

	int FdTimer::init() {
		_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
		uld("timerfd create fd=%d", _fd);
		return UvPoll::init(_fd);
	}

	void FdTimer::close(UvHandle::CloseLis lis) {
		if(_fd>=0) {
			UvPoll::close(lis);
			::close(_fd); _fd=-1;
		}
	}

}
#endif // __linux

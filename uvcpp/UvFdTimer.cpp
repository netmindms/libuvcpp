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
#define RAWH() ((uv_idle_t*)getRawHandle())
#define GETOBJH(H) ((UvIdle*)(((HandleHolder*)H->data))->uvh)
namespace uvcpp {
	UvFdTimer::UvFdTimer() {
		_fireCount = 0;
	}

	UvFdTimer::~UvFdTimer() {
		kill();
	}

	void UvFdTimer::setUsec(uint64_t usec, uint64_t first_usec, Lis lis) {
		_lis = lis;

		mTimerSpec.it_interval.tv_sec = usec / 1000000;
		mTimerSpec.it_interval.tv_nsec = (usec % 1000000) * 1000;
		uint64_t iusec = (first_usec == 0) ? usec : first_usec;
		mTimerSpec.it_value.tv_sec = iusec / 1000000;
		mTimerSpec.it_value.tv_nsec = (iusec % 1000000) * 1000;

		timerfd_settime(_fd, 0, &mTimerSpec, NULL);
		auto ret = UvPoll::start(UV_READABLE, [&](int events) {
			read(_fd, &_fireCount, sizeof(_fireCount));
			_lis();
		});
		assert(!ret);
	}

	void UvFdTimer::set(uint64_t expire, uint64_t period, Lis lis) {
		setUsec(expire * 1000, period * 1000, lis);
	}

	void UvFdTimer::reset() {
		pause();
		resume();
	}

	void UvFdTimer::kill(bool isclose) {
		UvPoll::stop(isclose);
	}

	void UvFdTimer::pause(void) {
		struct itimerspec ts;
		memset(&ts, 0, sizeof(ts));
		timerfd_settime(_fd, 0, &ts, NULL);
	}

	void UvFdTimer::resume(void) {
		timerfd_settime(_fd, 0, &mTimerSpec, NULL);
	}



	uint64_t UvFdTimer::getFireCount() {
		return _fireCount;
	}

	int UvFdTimer::init() {
		auto fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
		ali("timerfd create fd=%d", fd);
		return UvPoll::init(fd);
	}

}
#endif // __linux

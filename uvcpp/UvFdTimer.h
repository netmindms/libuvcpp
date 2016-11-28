/*
 * UvFdTimer.h
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#ifndef UVFDTIMER_H_
#define UVFDTIMER_H_

#ifndef __linux
#error "This API is not supported in this OS"
#endif

#include <sys/timerfd.h>
#include <cstdint>
#include <functional>

#include "UvPollHandle.h"

namespace uvcpp {
	class UvFdTimer {
	public:
		typedef std::function<void()> Lis;
		UvFdTimer();
		virtual ~UvFdTimer();
		void setUsec(uint64_t expire, uint64_t period, Lis lis);
		void set(uint64_t expire, uint64_t period, Lis lis);
		void reset();
		void kill();
		void pause();
		void resume();

	private:
		Lis _lis;
		UvPollHandle *_handle;
		int _fd;
		uint64_t _fireCount;
		struct itimerspec mTimerSpec;
	};
}


#endif /* UVFDTIMER_H_ */

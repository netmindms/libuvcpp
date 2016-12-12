/*
 * UvFdTimer.h
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#ifndef UVCPPPRJ_UVFDTIMER_H_
#define UVCPPPRJ_UVFDTIMER_H_

#ifndef __linux
#error "This API is not supported in this OS"
#endif

#include <sys/timerfd.h>
#include <cstdint>
#include <functional>
#include "UvPoll.h"


namespace uvcpp {
	class FdTimer: public UvPoll {
	public:
		typedef std::function<void()> Lis;
		FdTimer();
		virtual ~FdTimer();
		int init();
		void setUsec(uint64_t expire, uint64_t period, Lis lis);
		void set(uint64_t expire, uint64_t period, Lis lis);
		void reset();
		void kill(bool isclose=true);
		void pause();
		void resume();
		uint64_t getFireCount();


	private:
		Lis _lis;
		uint64_t _fireCount;
		struct itimerspec mTimerSpec;
	};
}


#endif /* UVCPPPRJ_UVFDTIMER_H_ */

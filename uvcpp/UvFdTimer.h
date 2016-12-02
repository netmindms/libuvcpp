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


#include "UvHandleOwner.h"

namespace uvcpp {
	class UvFdTimer: public UvHandleOwner {
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
		uint64_t getFireCount();

	private:
		Lis _lis;
		int _fd;
		uint64_t _fireCount;
		uv_poll_t* _rawh;
		struct itimerspec mTimerSpec;
		static void poll_cb(uv_poll_t *handle, int status, int events);
		void pollCb(int status, int events);
	};
}


#endif /* UVCPPPRJ_UVFDTIMER_H_ */

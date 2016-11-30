/*
 * UvTimer2.h
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#ifndef UVTIMER_H_
#define UVTIMER_H_

#include <cstdint>
#include <functional>

#include "UvContext.h"
#include "UvTimerHandle.h"

namespace uvcpp {
	class UvTimer {
	public:
		typedef std::function<void()> Lis;

		UvTimer();

		virtual ~UvTimer();

		void set(uint64_t period, uint64_t first_expire, Lis lis);

		void reset();

		void kill();

	private:
		UvHandle *_ohandle;
		Lis _lis;
		static void timer_cb(uv_timer_t* handle);
	};
}
#endif /* UVTIMER_H_ */

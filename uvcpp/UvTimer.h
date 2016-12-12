/*
 * UvTimer2.h
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#ifndef UVCPPPRJ_UVTIMER_H_
#define UVCPPPRJ_UVTIMER_H_

#include <cstdint>
#include <functional>
#include "UvHandle.h"

namespace uvcpp {
	class UvTimer : public UvHandle {
	public:
		typedef std::function<void()> Lis;

		UvTimer();

		virtual ~UvTimer();

		int init();

		int start(uint64_t period, uint64_t first_expire, Lis lis);

		void reset();

		void stop(bool close=true);

		void setRepeat(uint64_t repeat);

	private:
		Lis _lis;
		static void timer_cb(uv_timer_t* handle);
	};
}
#endif /* UVCPPPRJ_UVTIMER_H_ */

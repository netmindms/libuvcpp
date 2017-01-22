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

		int start(uint64_t timeout, uint64_t repeat, Lis lis);

		void again();

		void stop(bool close=true);

		void setRepeat(uint64_t repeat);

		uint64_t getRepeat();

	private:
		Lis _lis;
//		uint64_t _period;
		static void timer_cb(uv_timer_t* handle);
	};
}
#endif /* UVCPPPRJ_UVTIMER_H_ */

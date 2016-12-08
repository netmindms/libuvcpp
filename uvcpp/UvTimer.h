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

#include "UvContext.h"
#include "UvHandleOwner.h"

namespace uvcpp {
	class UvTimer : public UvHandle {
	public:
		typedef std::function<void()> Lis;


		static UvTimer* init();

		void timerStart(uint64_t period, uint64_t first_expire, Lis lis);

		void reset();

		void timerStop();


	private:
		uv_timer_t* _timerHandle;
		Lis _lis;

		UvTimer();
		virtual ~UvTimer();
		static void timer_cb(uv_timer_t* handle);
	};
}
#endif /* UVCPPPRJ_UVTIMER_H_ */

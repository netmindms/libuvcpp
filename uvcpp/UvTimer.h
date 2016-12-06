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
	class UvTimer : public UvHandleOwner {
	public:
		typedef std::function<void()> Lis;

		UvTimer();

		virtual ~UvTimer();

		void timerStart(uint64_t period, uint64_t first_expire, Lis lis);

		void reset();

		void timerStop(UvHandle::CloseLis lis=nullptr);

		void close(UvHandle::CloseLis lis) override;

	private:
		Lis _lis;
		static void timer_cb(uv_timer_t* handle);
	};
}
#endif /* UVCPPPRJ_UVTIMER_H_ */

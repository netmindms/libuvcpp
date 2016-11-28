/*
 * UvTimerHandle.h
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#ifndef UVTIMERHANDLE_H_
#define UVTIMERHANDLE_H_

#include <functional>
#include "UvBaseHandle.h"

namespace uvcpp {
	class UvTimerHandle : public UvBaseHandle {
	public:
		typedef std::function<void()> Lis;
		UvTimerHandle();
		virtual ~UvTimerHandle();
		uv_handle_t *getRawHandle() override;
		int open();
		int timer_start(uint64_t expire, uint64_t period, Lis lis);
		int timer_stop();
		int timer_again();
	private:
		uv_timer_t _handle;
		Lis _lis;

		static void timer_cb(uv_timer_t *handle);
	};
}
#endif /* UVTIMERHANDLE_H_ */

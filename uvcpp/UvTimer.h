/*
 * UvTimer2.h
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#ifndef UVTIMER2_H_
#define UVTIMER2_H_

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
		UvTimerHandle *_handle;
	};
}
#endif /* UVTIMER2_H_ */

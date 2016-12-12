//
// Created by netmind on 16. 12. 12.
//

#ifndef UVCPPPRJ_PERIODICTIMER_H
#define UVCPPPRJ_PERIODICTIMER_H


#include <chrono>
#include "UvTimer.h"

namespace uvcpp {
	class PeriodicTimer {
	public:
		PeriodicTimer();

		virtual ~PeriodicTimer();

		int start(uint64_t msec, std::function<void()> lis);

		void stop();

	private:
		std::function<void()> _lis;
		UvTimer _timer;
		std::chrono::system_clock::time_point _fireTime;
		std::chrono::system_clock::duration _period;
	};
}

#endif //UVCPPPRJ_PERIODICTIMER_H

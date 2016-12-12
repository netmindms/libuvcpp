//
// Created by netmind on 16. 12. 12.
//

#include "PeriodicTimer.h"
#include "uvcpplog.h"

using namespace std;
using namespace chrono;

namespace uvcpp {
	PeriodicTimer::PeriodicTimer() {

	}

	PeriodicTimer::~PeriodicTimer() {

	}

	int PeriodicTimer::start(uint64_t msec, std::function<void()> lis) {
		auto ret = _timer.init();
		if(!ret) {
			_lis = lis;
			_fireTime = system_clock::now();
			_period = system_clock::duration(chrono::milliseconds(msec));
			_timer.start(msec, msec, [this]() {

				_lis();
				_fireTime += _period;
				auto timediff = system_clock::now() - _fireTime;
				ald("timediff=%lld", duration_cast<chrono::milliseconds>(timediff).count());
				_timer.setRepeat(duration_cast<milliseconds>(_period - timediff).count());

			});
		}
		return ret;
	}

	void PeriodicTimer::stop() {
		_timer.stop();
	}


}

//
// Created by netmind on 18. 2. 23.
//

#include "TimerMgr.h"

namespace uvcpp {
	TimerMgr::TimerMgr() {
		_handleSeed = 0;

	}

	TimerMgr::~TimerMgr() {
		clearAll();
	}

	uint32_t TimerMgr::setTimeout(uint64_t start, uint64_t interval, std::function<void(uint32_t fire_handle)> lis) {
		if(++_handleSeed==0) ++_handleSeed;
		_timers.emplace_back();
		auto& tm = _timers.back();
		tm.init();
		tm.handle = _handleSeed;
		tm.start(start, interval, [this, &tm, lis, interval]() {
			_dummyList.clear();
			if(!interval) {
				tm.close();
				gotoDummy(tm.handle);
			}
			lis(tm.handle);
		});
		return tm.handle;
	}

	void TimerMgr::clearTimeout(uint32_t handle) {
		for(auto itr=_timers.begin(); itr!=_timers.end();itr++) {
			if(handle == itr->handle) {
				itr->close();
				_dummyList.splice(_dummyList.end(), _timers, itr);
				break;
			}
		}
	}

	void TimerMgr::clearAll() {
		for(auto& tm: _timers) {
			tm.close();
		}
		_dummyList.splice(_dummyList.end(), _timers);
	}

	void TimerMgr::gotoDummy(uint32_t handle) {
		for(auto itr=_timers.begin(); itr!=_timers.end();itr++) {
			if(handle == itr->handle) {
				_dummyList.splice(_dummyList.end(), _timers, itr);
				break;
			}
		}
	}

	std::unique_ptr<TimerMgr> TimerMgr::createTimerMgr() {
		return std::unique_ptr<TimerMgr>(new TimerMgr);
	}
}

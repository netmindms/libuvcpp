//
// Created by netmind on 18. 2. 23.
//

#ifndef NETMEDIA_TIMERMGR_H
#define NETMEDIA_TIMERMGR_H

#include "UvTimer.h"

namespace uvcpp {
	class TimerMgr {
	public:
		virtual ~TimerMgr();

		uint32_t setTimeout(uint64_t start, uint64_t interval, std::function<void(uint32_t fire_handle)> lis);

		void clearTimeout(uint32_t handle);

		void clearAll();

		static std::unique_ptr<TimerMgr> createTimerMgr();

	private:
		class TmTimer : public UvTimer {
		private:
			friend class TimerMgr;
			uint32_t handle;
		};
		TimerMgr();

		uint32_t _handleSeed;
		std::list<TmTimer> _timers;
		std::list<TmTimer> _dummyList;

		void gotoDummy(uint32_t handle);
	};

	typedef std::unique_ptr<TimerMgr> spTimerMgr;
}


#endif //NETMEDIA_TIMERMANAGER_H

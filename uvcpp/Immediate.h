//
// Created by netmind on 17. 10. 31.
//

#ifndef UVCPPPRJ_IMMEDIATE_H
#define UVCPPPRJ_IMMEDIATE_H

#include "UvAsync.h"

namespace uvcpp {
	class Immediate {
	public:
		Immediate();

		uint32_t setImmediate(std::function<void()> lis);

		void abort(uint32_t handle);

		void close();

	private:
		struct Msg {
			uint32_t handle;
			std::function<void()> lis;
			Msg(uint32_t handle, std::function<void()> cb);
		};
		uint32_t _handleSeed;
		UvAsync _async;
		std::list<std::unique_ptr<Msg>> _msgList;
	};
}


#endif //UVCPPPRJ_IMMEDIATE_H

//
// Created by netmind on 17. 11. 3.
//

#ifndef UVCPPPRJ_IMMEDIATEWRAPPER_H
#define UVCPPPRJ_IMMEDIATEWRAPPER_H

#include <cstdint>
#include <list>
#include <functional>
#include "Immediate.h"

namespace uvcpp {
	class ImmediateWrapper {
	public:
		virtual ~ImmediateWrapper();

		uint32_t setImmediate(std::function<void(uint32_t)> lis);

		void abort(uint32_t handle);

		void close();

		static std::unique_ptr<ImmediateWrapper> create(Immediate* immd);

	private:
		Immediate* _immd;
		std::list<uint32_t> _handles;

		ImmediateWrapper(Immediate* immd);

	};

	typedef std::unique_ptr<ImmediateWrapper> upImmediateWrapper;
}


#endif //UVCPPPRJ_IMMEDIATEWRAPPER_H

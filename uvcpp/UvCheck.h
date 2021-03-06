//
// Created by netmind on 16. 11. 30.
//

#ifndef UVCPPPRJ_UVCHECK_H
#define UVCPPPRJ_UVCHECK_H

#include <functional>
#include "UvHandle.h"

namespace uvcpp {
	class UvCheck : public UvHandle  {
	public:
		typedef std::function<void()> Lis;
		UvCheck();

		virtual ~UvCheck();
		int init();
		int start(Lis lis);
		void stop(bool isclose=true);
	private:
		Lis _lis;
		static void check_cb(uv_check_t* handle);
	};
}

#endif //UVCPPPRJ_UVCHECK_H

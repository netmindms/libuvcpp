//
// Created by netmind on 16. 11. 30.
//

#ifndef UVCPPPRJ_UVCHECK_H
#define UVCPPPRJ_UVCHECK_H

#include <functional>
#include "UvHandle.h"
#include "UvHandleOwner.h"

namespace uvcpp {
	class UvCheck : public UvHandle  {
	public:
		typedef std::function<void()> Lis;

		static UvCheck* init();

		int start(Lis lis);

		void stop();

	private:
		Lis _lis;

		UvCheck();
		virtual ~UvCheck();
		static void check_cb(uv_check_t* handle);
	};
}

#endif //UVCPPPRJ_UVCHECK_H

//
// Created by netmind on 16. 11. 30.
//

#ifndef UVCPPPRJ_UVCHECK_H
#define UVCPPPRJ_UVCHECK_H

#include <functional>
#include "UvHandle.h"

namespace uvcpp {
	class UvCheck  {
	public:
		typedef std::function<void()> Lis;
		UvCheck();

		virtual ~UvCheck();
		int open(Lis lis);
		void close();
	private:
		Lis _lis;
		UvHandle* _ohandle;
		static void check_cb(uv_check_t* handle);
	};
}

#endif //UVCPPPRJ_UVCHECK_H

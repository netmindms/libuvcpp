//
// Created by netmind on 16. 12. 1.
//

#ifndef UVCPPPRJ_UVIDLE_H
#define UVCPPPRJ_UVIDLE_H

#include "UvHandleOwner.h"

namespace uvcpp {
	class UvIdle : public UvHandleOwner {
	public:
		typedef std::function<void()> Lis;
		UvIdle();

		virtual ~UvIdle();

		int open(Lis lis);
	private:
		Lis _lis;
		static void idle_cb(uv_idle_t* rawh);

	};
}

#endif //UVCPPPRJ_UVIDLE_H

//
// Created by netmind on 16. 12. 1.
//

#ifndef UVCPPPRJ_UVIDLE_H
#define UVCPPPRJ_UVIDLE_H

#include "UvHandleOwner.h"

namespace uvcpp {
	class UvIdle : public UvHandle {
	public:
		typedef std::function<void()> Lis;

		static UvIdle* init();

		int start(Lis lis);

		void stop();

	private:
		Lis _lis;

		UvIdle();

		virtual ~UvIdle();
		static void idle_cb(uv_idle_t* rawh);

	};
}

#endif //UVCPPPRJ_UVIDLE_H

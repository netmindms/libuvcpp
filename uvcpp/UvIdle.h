//
// Created by netmind on 16. 12. 1.
//

#ifndef UVCPPPRJ_UVIDLE_H
#define UVCPPPRJ_UVIDLE_H


#include "UvHandle.h"

namespace uvcpp {
	class UvIdle : public UvHandle {
	public:
		typedef std::function<void()> Lis;
		UvIdle();

		virtual ~UvIdle();

		int init();

		int start(Lis lis);
		void stop(bool isclose=true);

	private:
		Lis _lis;
		static void idle_cb(uv_idle_t* rawh);

	};
}

#endif //UVCPPPRJ_UVIDLE_H

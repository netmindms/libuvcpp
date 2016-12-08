//
// Created by netmind on 16. 11. 30.
//

#ifndef UVCPPPRJ_UVPREPARE_H
#define UVCPPPRJ_UVPREPARE_H

#include "UvHandle.h"

namespace uvcpp {
	class UvPrepare: public UvHandle {
	public:
		typedef std::function<void()> Lis;

		static UvPrepare* init();
		int start(Lis lis);
		void stop();

	private:
		Lis _lis;

		static void prepare_cb(uv_prepare_t* rawh);

		UvPrepare();
		virtual ~UvPrepare();
	};
}

#endif //UVCPPPRJ_UVPREPARE_H

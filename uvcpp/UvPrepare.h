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
		UvPrepare();

		virtual ~UvPrepare();
		int init();
		int start(Lis lis);
		void stop(bool isclose=true);

	private:
		static void prepare_cb(uv_prepare_t* rawh);
		Lis _lis;
	};
}

#endif //UVCPPPRJ_UVPREPARE_H

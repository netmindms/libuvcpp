//
// Created by netmind on 16. 11. 30.
//

#ifndef UVCPPPRJ_UVPREPARE_H
#define UVCPPPRJ_UVPREPARE_H

#include "UvHandle.h"
#include "UvHandleOwner.h"

namespace uvcpp {
	class UvPrepare: public UvHandleOwner {
	public:
		typedef std::function<void()> Lis;
		UvPrepare();

		virtual ~UvPrepare();
		int open(Lis lis);
		void prepareStop();

	private:
		static void prepare_cb(uv_prepare_t* rawh);
		Lis _lis;
	};
}

#endif //UVCPPPRJ_UVPREPARE_H

//
// Created by netmind on 16. 11. 30.
//

#ifndef UVCPPPRJ_UVPREPARE_H
#define UVCPPPRJ_UVPREPARE_H

#include "UvHandle.h"

namespace uvcpp {
	class UvPrepare {
	public:
		typedef std::function<void()> Lis;
		UvPrepare();

		virtual ~UvPrepare();
		int open(Lis lis);
		void close();

	private:
		static void prepare_cb(uv_prepare_t* rawh);
		UvHandle *_ohandle;
		Lis _lis;
	};
}

#endif //UVCPPPRJ_UVPREPARE_H

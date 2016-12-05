//
// Created by netmind on 16. 11. 30.
//

#include <cassert>
#include "uvcpplog.h"
#include "uvcppdef.h"
#include "UvCheck.h"

namespace uvcpp {

#define GETRAWHANDLE() (uv_check_t*)_ohandle->getRawHandle()

	UvCheck::UvCheck() {
		_ohandle = nullptr;
		ald("const uvcheck, obj=%x", (long)this);
	}

	UvCheck::~UvCheck() {

	}

	int UvCheck::open(Lis lis) {
		// allocaton ohandle from context
		// ...
		auto ctx = UvContext::getContext();
		//_ohandle = ctx->createHandle(this);
		createHandle("check");
		_lis = lis;
		uv_check_t* ph = GETRAWHANDLE();
		uv_check_init(ctx->getLoop(), ph);
		return uv_check_start(ph, check_cb);
	}

	void UvCheck::check_cb(uv_check_t *rawh) {
		ald("check callback, data=%x", (long)rawh->data);
		ASSERT_RAW_UVHANDLE(rawh);
		UvCheck *pcheck = GET_UVHANDLE_OWNER(UvCheck, rawh);
		assert(pcheck);
		if(pcheck) {
			if(pcheck->_lis) {
				pcheck->_lis();
			}
		}
	}

	void UvCheck::close() {
		uv_check_stop(GETRAWHANDLE());
		UvHandleOwner::close();
	}

}

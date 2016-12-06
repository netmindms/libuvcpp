//
// Created by netmind on 16. 11. 30.
//

#include "uvcppdef.h"
#include "UvAsync.h"
#include "uvcpplog.h"

namespace uvcpp {
	UvAsync::UvAsync() {

	}

	UvAsync::~UvAsync() {

	}

	int UvAsync::open(Lis lis) {
		auto ctx = UvContext::getContext();
		_lis = lis;
		auto rawh = (uv_async_t*)createHandle("async");
		uv_async_init(ctx->getLoop(), rawh, async_cb);
		return 0;
	}

	void UvAsync::async_cb(uv_async_t *rawh) {
		ASSERT_RAW_UVHANDLE(rawh);
		auto pasync = GET_UVHANDLE_OWNER(UvAsync, rawh);
		pasync->_lis();
	}

	int UvAsync::send() {
		return uv_async_send((uv_async_t*)_ohandle->getRawHandle());
	}


}
//
// Created by netmind on 16. 11. 30.
//

#include "uvcppdef.h"
#include "UvAsync.h"

namespace uvcpp {
	UvAsync::UvAsync() {

	}

	UvAsync::~UvAsync() {

	}

	int UvAsync::open(Lis lis) {
		auto ctx = UvContext::getContext();
		_lis = lis;
		_ohandle = ctx->createHandle(this);
		uv_async_init(ctx->getLoop(), (uv_async_t*)_ohandle->getRawHandle(), async_cb);
		return 0;
	}

	void UvAsync::async_cb(uv_async_t *rawh) {
		ASSERT_RAW_UVHANDLE(rawh);
		auto pasync = GET_UVHANDLE_OWNER(UvAsync, rawh);
		pasync->_lis();
	}

	void UvAsync::close() {
		if(_ohandle) {
			_ohandle->close(); _ohandle = nullptr;
		}
	}

	int UvAsync::send() {
		return uv_async_send((uv_async_t*)_ohandle->getRawHandle());
	}
}
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


	void UvAsync::async_cb(uv_async_t *rawh) {
		assert(rawh->data);
		auto async = (UvAsync*)rawh->data;
		assert(async->_lis);
		async->_lis();
	}

	int UvAsync::send() {
//		return uv_async_send((uv_async_t*)_ohandle->getRawHandle());
		return uv_async_send((uv_async_t*)getRawHandle());
	}

	UvAsync* UvAsync::init(UvAsync::Lis lis) {
		auto async = new UvAsync;
		auto loop = UvContext::getContext()->getLoop();
		auto ret = uv_async_init(loop, (uv_async_t*)async->getRawHandle(), async_cb);
		async->registerContext();
		async->_lis = lis;
		return async;
	}


}
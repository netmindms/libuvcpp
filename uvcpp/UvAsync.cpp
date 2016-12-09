//
// Created by netmind on 16. 11. 30.
//

#include "uvcppdef.h"
#include "UvAsync.h"
#include "uvcpplog.h"


#define RAWH() ((uv_async_t*)getRawHandle())
#define GETOBJH(H) ((UvAsync*)(((HandleHolder*)H->data))->uvh)

namespace uvcpp {
	UvAsync::UvAsync() {

	}

	UvAsync::~UvAsync() {

	}

	int UvAsync::init(Lis lis) {
		initHandle();
		auto ctx = UvContext::getContext();
		_lis = lis;
		uv_async_init(getLoop(), RAWH(), async_cb);
		return 0;
	}

	void UvAsync::async_cb(uv_async_t *rawh) {
		auto pasync = GETOBJH(rawh);
		pasync->_lis();
	}

	int UvAsync::send() {
		return uv_async_send(RAWH());
	}


}
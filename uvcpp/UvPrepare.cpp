//
// Created by netmind on 16. 11. 30.
//

#include "uvcppdef.h"
#include "UvPrepare.h"

namespace uvcpp {

	UvPrepare::UvPrepare() {
		_ohandle = nullptr;
	}

	UvPrepare::~UvPrepare() {

	}

	int UvPrepare::open(UvPrepare::Lis lis) {
		auto ctx = UvContext::getContext();
		int ret;
		if(ctx) {
			_lis = lis;
			_ohandle = ctx->createHandle(this);
			uv_prepare_t* prawh = (uv_prepare_t*)_ohandle->getRawHandle();
			ret = uv_prepare_init(ctx->getLoop(), prawh);
			assert(!ret);
			ret = uv_prepare_start(prawh, prepare_cb);
		} else {
			assert(ctx);
			return -1;
		}
		return ret;
	}

	void UvPrepare::prepare_cb(uv_prepare_t *rawh) {
		ASSERT_RAW_UVHANDLE(rawh);
		auto pprepare = GET_UVHANDLE_OWNER(UvPrepare, rawh);
		pprepare->_lis();
	}

	void UvPrepare::close() {
		if(_ohandle) {
			uv_prepare_stop((uv_prepare_t*)_ohandle->getRawHandle());
			UvHandleOwner::close();
		}
	}
}
//
// Created by netmind on 16. 11. 30.
//

#include "uvcppdef.h"
#include "UvPrepare.h"
#include "uvcpplog.h"

namespace uvcpp {

	UvPrepare::UvPrepare() {
	}

	UvPrepare::~UvPrepare() {

	}


	void UvPrepare::prepare_cb(uv_prepare_t *rawh) {
		auto prepare = (UvPrepare*)rawh->data;
		if(prepare->_lis) {
			prepare->_lis();
		}
	}

	UvPrepare *UvPrepare::init() {
		UvPrepare* prepare = new UvPrepare();
		uv_prepare_init(UvContext::getLoop(), (uv_prepare_t*)prepare->getRawHandle());
		prepare->registerContext();
		return prepare;
	}

	int UvPrepare::start(UvPrepare::Lis lis) {
		_lis = lis;
		return uv_prepare_start((uv_prepare_t*)getRawHandle(), prepare_cb);
	}

	void UvPrepare::stop() {
		uv_prepare_stop((uv_prepare_t*)getRawHandle());
	}

}
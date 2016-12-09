//
// Created by netmind on 16. 11. 30.
//

#include "uvcppdef.h"
#include "UvPrepare.h"
#include "uvcpplog.h"

#define RAWH() ((uv_prepare_t*)getRawHandle())
#define GETOBJH(H) ((UvPrepare*)(((HandleHolder*)H->data))->uvh)

namespace uvcpp {

	UvPrepare::UvPrepare() {
	}

	UvPrepare::~UvPrepare() {

	}

	int UvPrepare::init() {
		initHandle();
		return uv_prepare_init(getLoop(), RAWH());
	}

	void UvPrepare::prepare_cb(uv_prepare_t *rawh) {
		auto prepare = GETOBJH(rawh);
		prepare->_lis();
	}

	void UvPrepare::stop(bool isclose) {
		uv_prepare_stop(RAWH());
		if(isclose) {
			close();
		}
	}

	int UvPrepare::start(UvPrepare::Lis lis) {
		_lis = lis;
		return uv_prepare_start(RAWH(), prepare_cb);
	}

}
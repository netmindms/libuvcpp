//
// Created by netmind on 16. 11. 30.
//

#include <cassert>
#include "uvcpplog.h"
#include "uvcppdef.h"
#include "UvCheck.h"

#define RAWH() ((uv_check_t*)getRawHandle())
#define GETOBJH(H) ((UvCheck*)(((HandleHolder*)H->data))->uvh)

namespace uvcpp {

	UvCheck::UvCheck() {
		ald("const uvcheck, obj=%x", (long)this);
	}

	UvCheck::~UvCheck() {
		close();
	}

	int UvCheck::init() {
		initHandle();
		return uv_check_init(getLoop(), (uv_check_t*)getRawHandle());
	}

	void UvCheck::check_cb(uv_check_t *rawh) {
//		ald("check callback, data=%x", (long)rawh->data);
		auto check = GETOBJH(rawh);
		assert(check);
		if(check) {
			if(check->_lis) {
				check->_lis();
			}
		}
	}

	int UvCheck::start(UvCheck::Lis lis) {
		assert(_handleHolder);
		_lis = lis;
		return uv_check_start((uv_check_t*)getRawHandle(), check_cb);
	}

	void UvCheck::stop(bool isclose) {
		uv_check_stop(RAWH());
		if(isclose) {
			close();
		}
	}

}

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
	}

	UvCheck::~UvCheck() {

	}


	void UvCheck::check_cb(uv_check_t *rawh) {
		ald("check callback, data=%x", (long)rawh->data);
		UvCheck *pcheck = (UvCheck*)rawh->data;
		if(pcheck->_lis) {
			pcheck->_lis();
		}
	}

	UvCheck *UvCheck::init() {
		auto check = new UvCheck();
		uv_check_init(UvContext::getLoop(), (uv_check_t*)check->getRawHandle());
		check->registerContext();
		return check;
	}

	int UvCheck::start(UvCheck::Lis lis) {
		_lis = lis;
		return uv_check_start((uv_check_t*)getRawHandle(), check_cb);
	}

	void UvCheck::stop() {
		uv_check_stop((uv_check_t*)getRawHandle());
	}
}

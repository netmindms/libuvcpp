//
// Created by netmind on 16. 12. 1.
//

#include "UvIdle.h"
#include "uvcpplog.h"

namespace uvcpp {

	UvIdle::UvIdle() {

	}

	UvIdle::~UvIdle() {

	}

	int UvIdle::open(UvIdle::Lis lis) {
		_lis = lis;
		auto rawh = (uv_idle_t*)createHandle("idle");
		uv_idle_init(getLoop(), rawh);
		return uv_idle_start(rawh, idle_cb);
	}

	void UvIdle::idle_cb(uv_idle_t *rawh) {
		auto pidle = UvHandleOwner::getHandleOwner<UvIdle>((uv_handle_t*)rawh);
		pidle->_lis();
	}

}
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


	void UvIdle::idle_cb(uv_idle_t *rawh) {
		auto idle = (UvIdle*)rawh->data;
		if(idle->_lis) {
			idle->_lis();
		}
	}

	UvIdle *UvIdle::init() {
		auto idle = new UvIdle();
		uv_idle_init(UvContext::getLoop(), (uv_idle_t*)idle->getRawHandle());
		idle->registerContext();
		return idle;
	}

	int UvIdle::start(UvIdle::Lis lis) {
		_lis = lis;
		return uv_idle_start((uv_idle_t*)getRawHandle(), idle_cb);
	}

	void UvIdle::stop() {
		_lis = nullptr;
		uv_idle_stop((uv_idle_t*)getRawHandle());
	}


}
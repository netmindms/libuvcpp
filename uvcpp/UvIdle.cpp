//
// Created by netmind on 16. 12. 1.
//

#include "uvcpplog.h"
#include "UvIdle.h"

#define RAWH() ((uv_idle_t*)getRawHandle())
#define GETOBJH(H) ((UvIdle*)(((HandleHolder*)H->data))->uvh)

namespace uvcpp {

	UvIdle::UvIdle() {

	}

	UvIdle::~UvIdle() {

	}

	int UvIdle::init() {
		auto ret = initHandle();
		if(!ret) {
			return uv_idle_init(getLoop(), RAWH());
		} else {
			return ret;
		}
	}

	void UvIdle::idle_cb(uv_idle_t *rawh) {
		auto idle = GETOBJH(rawh);
		idle->_lis();
	}

	int UvIdle::start(UvIdle::Lis lis) {
		_lis = lis;
		return uv_idle_start(RAWH(), idle_cb);
	}

	void UvIdle::stop(bool isclose) {
		uv_idle_stop(RAWH());
		if(isclose) {
			close();
		}
	}

}
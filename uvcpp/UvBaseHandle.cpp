/*
 * EdSocket.cpp
 *
 *  Created on: Nov 22, 2016
 *      Author: netmind
 */

#define LOG_LEVEL LOG_DEBUG

#include <nmdutil/nmdlog.h>
#include "UvBaseHandle.h"
#include "UvContext.h"

using namespace std;
namespace uvcpp {


	UvBaseHandle::UvBaseHandle() {
		_ctx = nullptr;
		_prev = _next = nullptr;
		_rawhandle = nullptr;
	}

	UvBaseHandle::~UvBaseHandle() {
	}

//int UvBaseHandle::open() {
//	if (!_ctx) {
//		_ctx = UvContext::getContext();
//		return 0;
//	} else {
//		return -1;
//	}
//}

	void UvBaseHandle::close() {
		if (_rawhandle) {
			ali("handle active=%d", uv_is_active(_rawhandle));
			uv_close(_rawhandle, UvContext::handle_close_cb);
		}
	}

	int UvBaseHandle::init() {
		if (!_ctx) {
			_ctx = UvContext::getContext();
			_rawhandle = getRawHandle();
			_rawhandle->data = this;
//		open();
			return 0;
		} else {
			return -1;
		}
	}

	void UvBaseHandle::OnHandleClosed() {
		ali("close handle cb");
	}

}

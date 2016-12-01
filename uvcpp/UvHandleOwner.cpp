//
// Created by netmind on 16. 11. 30.
//

#include "UvHandleOwner.h"

namespace uvcpp {

	UvHandleOwner::UvHandleOwner() {
		_ohandle = nullptr;
		_ctx = nullptr;
	}

	UvHandleOwner::~UvHandleOwner() {
		assert(!_ohandle);
		close();
	}

	void UvHandleOwner::closeAsync(UvHandle::CloseLis lis) {
		if(_ohandle) {
			auto rawh = GET_RAWH(uv_handle_t, _ohandle);
			if(!uv_is_closing(rawh)) {
				_ohandle->setOnCloseListener(lis);
				_ohandle->close();
			}
		}
	}

	void UvHandleOwner::close() {
		if(_ohandle) {
			closeAsync(nullptr);
			_ohandle->setOnCloseListener(nullptr);
			_ohandle = nullptr;
		}
	}

	uv_handle_t *UvHandleOwner::createHandle() {
		if(!_ohandle) {
			_ctx = UvContext::getContext();
			if(_ctx) {
				_ohandle = _ctx->createHandle(this);
				return _ohandle->getRawHandle();
			}
		}
	}
}

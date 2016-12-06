//
// Created by netmind on 16. 11. 30.
//

#include "UvHandleOwner.h"
#include "uvcpplog.h"

namespace uvcpp {

	UvHandleOwner::UvHandleOwner() {
		_ohandle = nullptr;
		_ctx = nullptr;
	}

	UvHandleOwner::~UvHandleOwner() {
		assert(!_ohandle);
		close(nullptr);
	}

	uv_handle_t *UvHandleOwner::createHandle(const char*
#ifndef NDEBUG
	name
#endif
	) {
		if(!_ohandle) {
			_ctx = UvContext::getContext();
			if(_ctx) {
				_ohandle = _ctx->createHandle(this, name);
				return _ohandle->getRawHandle();
			}
		}
	}

	uv_loop_t *UvHandleOwner::getLoop() {
		return _ctx->getLoop();
	}

	uv_handle_t *UvHandleOwner::getRawHandle() {
		if(_ohandle) {
			return _ohandle->getRawHandle();
		} else {
			return nullptr;
		}
	}

	void UvHandleOwner::setOnCloseListener(UvHandle::CloseLis lis) {
		_ohandle->_clis = lis;
	}

	void UvHandleOwner::close(UvHandle::CloseLis lis) {
		if(_ohandle) {
			auto rawh = GET_RAWH(uv_handle_t);
			_ohandle->_clis = lis;
			if (!uv_is_closing(rawh)) {
				_ohandle->close();
			}
			if(!lis) {
				_ohandle = nullptr;
			}
		}
	}

	void UvHandleOwner::closeNow() {
		close(nullptr);
	}
}

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
		closeHandle();
	}

	void UvHandleOwner::closeHandleAsync() {
		if(_ohandle) {
			auto rawh = GET_RAWH(uv_handle_t);
			if (!uv_is_closing(rawh)) {
				_ohandle->close();
				if(!_ohandle->_clis) {
					_ohandle = nullptr;
				}
			}
		}

	}

	void UvHandleOwner::closeHandle() {
		closeHandleAsync();
		if(_ohandle) {
			_ohandle->_clis = nullptr; // closeHandle callback이 호출 되지 않도록 한다.
			_ohandle = nullptr;
		}

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
}

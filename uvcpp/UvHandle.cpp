//
// Created by netmind on 16. 11. 30.
//

#include <atomic>
#include <sstream>
#include "UvHandle.h"
#include "../test/tlog.h"

namespace uvcpp {
	static std::atomic_uint _gHandleIdSeed;

	UvHandle::UvHandle() {
		_ctx = nullptr;
		_prev = nullptr;
		_next = nullptr;
//		memset(&_rawHandle, 0, sizeof(_rawHandle)); // TODO: test
		_rawHandle.handle.data = nullptr;
		_status = 0;
		_userData = nullptr;
	}

	UvHandle::~UvHandle() {

	}

	void UvHandle::close(CloseLis lis) {
		_clis = lis;
		if(uv_is_closing(&_rawHandle.handle) == false) {
			ald("   closeHandle handle, name=%s", _handleName);
			uv_close((uv_handle_t *) &_rawHandle, close_cb);
		}
	}
	void *UvHandle::getUserData() {
		return _userData;
	}

	void UvHandle::setOnCloseListener(UvHandle::CloseLis lis) {
		_clis = lis;
	}

	int UvHandle::initHandle() {
		if(!_ctx) {
			_ctx = UvContext::getContext();
		} else {
			ale("### already initialized");
			assert(0);
			return -1;
		}
		return 0;
	}

	uv_loop_t *UvHandle::getLoop() {
		return _ctx->getLoop();
	}

	void UvHandle::registerContext() {
		_rawHandle.handle.data = this;
		_ctx = UvContext::getContext();
		_ctx->initHandle(this, nullptr);
	}


	void UvHandle::close_cb(uv_handle_t *phandle) {
		ald("close callback..........");
		assert(phandle->data);
		UvHandle* ohandle = (UvHandle*)phandle->data;
		if(ohandle->_clis) {
			ohandle->_clis();
			ohandle->_clis = nullptr;
		}
		ohandle->_ctx->deleteHandle(ohandle);
	}

	void UvHandle::setObjName() {
#ifndef NDEBUG
		std::stringstream ss;
		ss << std::string(typeid(*this).name()) << '_' <<  _gHandleIdSeed++;
		_handleName = ss.str();
#endif
	}

}
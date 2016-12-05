//
// Created by netmind on 16. 11. 30.
//

#include "UvHandle.h"
#include "../test/tlog.h"

namespace uvcpp {
	UvHandle::UvHandle() {
		_ctx = nullptr;
		_prev = nullptr;
		_next = nullptr;
		_rawHandle.handle.data = nullptr;
		_status = 0;
		_userData = nullptr;
	}

	UvHandle::~UvHandle() {

	}

	void UvHandle::close() {
		if (_rawHandle.handle.data) {
//				ali("handle active=%d", uv_is_active(_rawhandle));
			ald("   close handle, name=%s, cb=%x", _handleName, (long)UvContext::handle_close_cb);
			uv_close((uv_handle_t *) &_rawHandle, UvContext::handle_close_cb);
//			ali("   uv closing=%d", uv_is_closing(&_rawHandle.handle));
		}
	}

	int UvHandle::init(void* user_data) {
		if (!_ctx) {
			_userData = user_data;
			_ctx = UvContext::getContext();
			_rawHandle.handle.data = this;
			return 0;
		} else {
			return -1;
		}
	}

	void *UvHandle::getUserData() {
		return _userData;
	}

	void UvHandle::setOnCloseListener(UvHandle::CloseLis lis) {
		_clis = lis;
	}

}
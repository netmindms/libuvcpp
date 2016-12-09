//
// Created by netmind on 16. 11. 30.
//

#include "UvHandle.h"
#include "../test/tlog.h"
#define RAWH() ((uv_handle_t*)(&_handleHolder->rawh.handle))
#define GETOBJH(H) ((UvHandle*)(((HandleHolder*)H->data))->uvh)
namespace uvcpp {
	UvHandle::UvHandle() {
		_ctx = nullptr;
		_prev = nullptr;
		_next = nullptr;
//		memset(&_rawHandle, 0, sizeof(_rawHandle)); // TODO: test
		_rawHandle.handle.data = nullptr;
		_status = 0;
		_userData = nullptr;
		_handleHolder = nullptr;
	}

	UvHandle::~UvHandle() {

	}

	void UvHandle::close(CloseLis lis) {
		if(_handleHolder) {
//				ali("handle active=%d", uv_is_active(_rawhandle));
			ald("   closeHandle handle, name=%s, cb=%x", _handleName, (long) UvContext::handle_close_cb);
			_handleHolder->closeLis = lis;
			uv_close((uv_handle_t *) &_handleHolder->rawh.handle, UvContext::handle_close_cb);
//			ali("   uv closing=%d", uv_is_closing(&_rawHandle.handle));
		}
	}

//	int UvHandle::init(void* user_data) {
//		if (!_ctx) {
//			_userData = user_data;
//			_ctx = UvContext::getContext();
//			_rawHandle.handle.data = this;
//			return 0;
//		} else {
//			return -1;
//		}
//	}

	void *UvHandle::getUserData() {
		return _userData;
	}

	void UvHandle::setOnCloseListener(UvHandle::CloseLis lis) {
		_clis = lis;
	}

	int UvHandle::initHandle() {
		_ctx = UvContext::getContext();
		if(_ctx) {
			_handleHolder = _ctx->createHandleHolder();
			_handleHolder->ctx = _ctx;
			_handleHolder->rawh.handle.data = _handleHolder;
			_handleHolder->uvh = this;

		} else {
			ale("### context not found");
			assert(0);
			return -1;
		}
	}

	uv_loop_t *UvHandle::getLoop() {
		return _ctx->getLoop();
	}

	std::unique_ptr<UvWriteInfo> UvHandle::allocWrite() {
		return _handleHolder->_writeReqQue.pop();
	}

	int UvHandle::write(const char *buf, size_t len) {
		auto upwr = _handleHolder->_writeReqQue.allocObj();
		upwr->fillBuf(buf, len);
		upwr->req.data = _handleHolder;
		auto ret = uv_write(&upwr->req, (uv_stream_t*)RAWH(), &upwr->uvBuf, 1, UvContext::write_cb);
		if (!ret) {
			_handleHolder->_writeReqQue.push(move(upwr));
		} else {
			ale("### uv write fail, ret=%d", ret);
		}
		return ret;
	}

	int UvHandle::write(const std::string &msg) {
		return write(msg.data(), msg.size());
	}
}
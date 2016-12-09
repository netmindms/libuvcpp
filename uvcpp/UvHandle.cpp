//
// Created by netmind on 16. 11. 30.
//

#include <atomic>
#include <sstream>
#include "UvHandle.h"
#include "../test/tlog.h"
#define RAWH() ((uv_handle_t*)(&_handleHolder->rawh.handle))
#define GETOBJH(H) ((UvHandle*)(((HandleHolder*)H->data))->uvh)

std::atomic_uint _gHandleIdSeed;

namespace uvcpp {
	UvHandle::UvHandle() {
		_ctx = nullptr;
		_status = 0;
		_userData = nullptr;
		_handleHolder = nullptr;
	}

	UvHandle::~UvHandle() {
		if(_handleHolder) {
			ale("### handle not closed");
			assert(0);
			close();
		}

	}

	void UvHandle::close(CloseLis lis) {
		if(_handleHolder) {
//				ali("handle active=%d", uv_is_active(_rawhandle));
			ald("   closeHandle handle, name=%s, cb=%x", _handleName, (long) UvContext::handle_close_cb);
			_handleHolder->closeLis = lis;
			_handleHolder->writeLis = nullptr;
			_handleHolder->sendLis = nullptr;
			uv_close((uv_handle_t *) &_handleHolder->rawh.handle, UvContext::handle_close_cb);
			if(!lis) {
				_handleHolder = nullptr;
			}
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


	int UvHandle::initHandle() {
		if(_handleHolder) {
			ale("### handle already initialized");
			assert(0);
			return -1;
		}
		_ctx = UvContext::getContext();
		if(_ctx) {
			_handleHolder = _ctx->createHandleHolder(this);
			_handleHolder->ctx = _ctx;
			_handleHolder->rawh.handle.data = _handleHolder;
			_handleHolder->uvh = this;
		} else {
			ale("### context not found");
			assert(0);
			return -1;
		}
		return 0;
	}

	uv_loop_t *UvHandle::getLoop() {
		return _ctx->getLoop();
	}


	int UvHandle::write(const char *buf, size_t len) {
		auto upwr = _handleHolder->_writeReqQue.allocObj();
		upwr->fillBuf(buf, len);
		upwr->req.data = _handleHolder;
		auto ret = uv_write(&upwr->req, (uv_stream_t *) RAWH(), &upwr->uvBuf, 1, UvContext::handle_write_cb);
		if (!ret) {
			_handleHolder->_writeReqQue.push(move(upwr));
		} else {
			ale("### uv write fail, ret=%d", ret);
		}
		return ret;
	}

	int UvHandle::send(const char* buf, size_t len, const struct sockaddr* addr) {
		auto upsd = _handleHolder->_sendReqQue.allocObj();
		upsd->fillBuf(buf, len);
		upsd->req.data = _handleHolder;
		return uv_udp_send(&upsd->req, (uv_udp_t*)RAWH(), &upsd->uvBuf, 1, addr, UvContext::handle_send_cb);
	}

	int UvHandle::write(const std::string &msg) {
		return write(msg.data(), msg.size());
	}

	void UvHandle::setObjName() {

	}

	uv_handle_t* UvHandle::getRawHandle() {
		return _handleHolder ? &_handleHolder->rawh.handle : nullptr;
	}

}
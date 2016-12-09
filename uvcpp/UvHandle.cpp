//
// Created by netmind on 16. 11. 30.
//

#include <atomic>
#include <sstream>
#include "UvHandle.h"
#include "../test/tlog.h"
#define RAWH() ((uv_handle_t*)(&_handleHolder->rawh.handle))
#define GETOBJH(H) ((UvHandle*)(((HandleHolder*)H->data))->uvh)

namespace uvcpp {
	UvHandle::UvHandle() {
		_ctx = nullptr;
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
			ald("   closing handle, name=%s, cb=%x", _handleHolder->handleName, (long) UvContext::handle_close_cb);
			_handleHolder->closeLis = lis;
			_handleHolder->writeLis = nullptr;
			_handleHolder->sendLis = nullptr;
			uv_close(&_handleHolder->rawh.handle, UvContext::handle_close_cb);
			if(!lis) {
				_handleHolder = nullptr;
			}
		}
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
		auto upwr = _handleHolder->writeReqQue.allocObj();
		upwr->fillBuf(buf, len);
		upwr->req.data = _handleHolder;
		auto ret = uv_write(&upwr->req, (uv_stream_t *) RAWH(), &upwr->uvBuf, 1, UvContext::handle_write_cb);
		if (!ret) {
			_handleHolder->writeReqQue.push(move(upwr));
		} else {
			ale("### uv write fail, ret=%d", ret);
		}
		return ret;
	}

	int UvHandle::send(const char* buf, size_t len, const struct sockaddr* addr) {
		auto upsd = _handleHolder->sendReqQue.allocObj();
		upsd->fillBuf(buf, len);
		upsd->req.data = _handleHolder;
		auto ret = uv_udp_send(&upsd->req, (uv_udp_t*)RAWH(), &upsd->uvBuf, 1, addr, UvContext::handle_send_cb);
		if(!ret) {
			_handleHolder->sendReqQue.push(move(upsd));
		} else {
			assert(0);
		}
		return ret;
	}

	int UvHandle::write(const std::string &msg) {
		return write(msg.data(), msg.size());
	}

#if 0
	void UvHandle::setObjName() {
#ifndef NDEBUG
		std::stringstream ss;
		ss << std::string(typeid(*this).name()) << '_' <<  _gHandleIdSeed++;
		_handleName = ss.str();
#endif
	}
#endif

	uv_handle_t* UvHandle::getRawHandle() {
		return _handleHolder ? &_handleHolder->rawh.handle : nullptr;
	}

}
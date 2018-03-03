//
// Created by netmind on 16. 11. 30.
//

#include <atomic>
#include <sstream>
#include "UvHandle.h"
#include "uvcpplog.h"

#define RAWH() ((uv_handle_t*)(&_handleHolder->rawh.handle))
#define GETOBJH(H) ((UvHandle*)(((HandleHolder*)H->data))->uvh)

namespace uvcpp {
	UvHandle::UvHandle() {
		_ctx = nullptr;
		_handleHolder = nullptr;
		_status = IDLE;
		userData = nullptr;
	}

	UvHandle::~UvHandle() {
		if(_status != IDLE) {
			ule("### handle not closed, obj_class=%s", typeid(*this).name());
			assert(0);
			close();
		}

	}

	void UvHandle::close(CloseLis lis) {
		if(_status == INITIALIZED) {
			assert(_ctx == UvContext::getContext() && "### uv context must be the same that initiating, This may be caused thread calling init() and close() is differ");
			uld("   closing handle, name=%s, cb=%x", _handleHolder->handleName, (long) UvContext::handle_close_cb);

			if(uv_is_closing(&_handleHolder->rawh.handle)==0) {
				uv_close(&_handleHolder->rawh.handle, UvContext::handle_close_cb);
			}
			if(!lis) {
				_status = IDLE;
				_handleHolder->uvh = nullptr;
				_handleHolder = nullptr;
			} else {
				_handleHolder->closeLis = lis;
				_status = CLOSING;
			}
		} else if(_status == CLOSING) {
			if(lis==nullptr) {
				_handleHolder->closeLis = nullptr;
				_handleHolder = nullptr;
				_status = IDLE;
			} else {
				assert(0);
				_handleHolder->closeLis = lis;
			}
		}
	}




	int UvHandle::initHandle() {
		if(_status != IDLE) {
			ule("### handle already initialized");
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
			ule("### context not found");
			assert(0);
			return -1;
		}
		_status = INITIALIZED;
		return 0;
	}


	uv_loop_t *UvHandle::getLoop() {
		return _ctx->getLoop();
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

	UvHandle::HANDLE_STATUS UvHandle::getHandleStatus() {
		return _status;
	}

}
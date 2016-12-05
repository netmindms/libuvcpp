/*
 * EdContext.cpp
 *
 *  Created on: Nov 22, 2016
 *      Author: netmind
 */

#include "uvcpplog.h"
#include "UvContext.h"
#include "UvHandle.h"

namespace uvcpp {

	thread_local UvContext *_gEdContext;

	void UvContext::dumpHandle(UvHandle *plast) {
		alv("dump handles, ");
		for (; plast;) {
			ald(" name=%s", plast->_handleName);
			plast = plast->_prev;
		}
	}


	UvContext::UvContext() {
		_loop = nullptr;
		_createLoop = false;
		_pendingHandleCnt = 0;
		_handleLast = nullptr;
		_handleIdSeed = 0;
	}

	UvContext::~UvContext() {
		close();
	}


	void UvContext::open(uv_loop_t *loop) {
		if(!loop) {
			_loop = new uv_loop_t;
			_createLoop = true;
			loop = _loop;
			uv_loop_init(_loop);
		}
		_loop = loop;
		_gEdContext = this;
		memset(&_prepareHandle, 0, sizeof(_prepareHandle));
	}

	void UvContext::openWithDefaultLoop() {
		open(uv_default_loop());
	}


	void UvContext::close() {
		assert(_handleLast==nullptr);
		if (_createLoop) {
			if (_loop) {
				uv_loop_close(_loop);
				delete _loop;
				_loop = nullptr;
			}
			_createLoop = false;
		}
	}

	UvContext *UvContext::getContext() {
		assert(_gEdContext);
		return _gEdContext;
	}


	uv_loop_t *UvContext::getLoop() {
		return _loop;
	}





	void UvContext::initHandle(UvHandle *handle, void *user_data) {
		if (_handleLast) {
			handle->_prev = _handleLast;
			handle->_next = nullptr;
			_handleLast->_next = handle;
			_handleLast = handle;
		} else {
			_handleLast = handle;
			handle->_next = nullptr;
			handle->_prev = nullptr;
		}
		handle->init(user_data);
//		ald("init handle, cnt=%d", _pendingHandleCnt);
	}

	int UvContext::run() {
		return uv_run(_loop, UV_RUN_DEFAULT);
	}

	void UvContext::handle_close_cb(uv_handle_t *phandle) {
		assert(phandle->data);
		UvHandle* ohandle = (UvHandle*)phandle->data;
		if(ohandle->_clis) {
			ohandle->_clis();
		}
		ohandle->_ctx->deleteHandle(ohandle);
	}

	void UvContext::deleteHandle(UvHandle *phandle) {
		if (!phandle) {
			return;
		}
		auto tprev = phandle->_prev;
		auto tnext = phandle->_next;
		if (tprev) {
			tprev->_next = phandle->_next;
		}
		if (tnext) {
			tnext->_prev = phandle->_prev;
		}
		if (tprev == nullptr && tnext == nullptr) {
			_handleLast = nullptr;
		}
		ald("delete handle, name=%s, remain=%d", GET_UV_HANDLE_NAME(phandle), _pendingHandleCnt-1);
		delete phandle;
		--_pendingHandleCnt;
	}

	UvHandle *UvContext::createHandle(void *user_data, const char*
#ifndef NDEBUG
	typestr
#endif
	) {
		auto ohandle = new UvHandle;
		++_pendingHandleCnt;
		++_handleIdSeed;
#ifndef NDEBUG
		char buf[100];
		snprintf(buf, sizeof(buf), "%s_%u", typestr, _handleIdSeed);
		ohandle->_handleName = buf;
		ald("create handle, handle_name: %s, cnt=%d", ohandle->_handleName, _pendingHandleCnt);
#endif

		initHandle(ohandle, user_data);
		return ohandle;
	}

	int UvContext::handleCount() {
		return _pendingHandleCnt;
	}


}
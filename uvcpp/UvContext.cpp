/*
 * EdContext.cpp
 *
 *  Created on: Nov 22, 2016
 *      Author: netmind
 */

#include <nmdutil/nmdlog.h>
#include "UvContext.h"
#include "UvBaseHandle.h"

namespace uvcpp {


	thread_local UvContext *_gEdContext;

	void UvContext::dumpHandle(UvBaseHandle *plast) {
		ald("dump handles, ");
		for (; plast;) {
			ald("    phandle=%x", (long) plast);
			plast = plast->_prev;
		}
	}


	UvContext::UvContext() {
		_loop = nullptr;
		_createLoop = false;
		_handleLast = nullptr;
		_pendingHandleCnt = 0;
	}

	UvContext::~UvContext() {
		close();
	}

//	int UvContext::open() {
//		_loop = new uv_loop_t;
//		uv_loop_init(_loop);
//		_createLoop = true;
//		open(_loop);
//		return 0;
//	}

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

//	uv_prepare_init(_loop, &_prepareHandle);
//	_prepareHandle.data = this;
//	uv_prepare_start(&_prepareHandle, prepare_cb);
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


	void UvContext::deleteHandle(UvBaseHandle *phandle) {
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
		delete phandle;
		--_pendingHandleCnt;
		ald("delete handle, phandle=%x, remain=%d", (long) phandle, _pendingHandleCnt);
		dumpHandle(_handleLast);
	}

//void UvContext::prepare_cb(uv_prepare_t* handle) {
//	UvContext* pctx = (UvContext*)handle->data;
//	if(pctx) {
//		ali("prepare callback...");
//	}
//}




	void UvContext::initUvHandle(UvBaseHandle *handle) {
		if (_handleLast) {
			handle->_prev = _handleLast;
			handle->_next = nullptr;
			_handleLast->_next = (UvBaseHandle *) handle;
			_handleLast = handle;
		} else {
			_handleLast = handle;
			handle->_next = nullptr;
			handle->_prev = nullptr;
		}
		handle->init();
		ald("create handle, rawhandle=%x, cnt=%d", (long) handle->_rawhandle, _pendingHandleCnt);
		dumpHandle(_handleLast);
	}


	void UvContext::handle_close_cb(uv_handle_t *phandle) {
		ali("handle close callback, phandle=%x", (long) phandle);
		auto uvhandle = (UvBaseHandle *) phandle->data;
		uvhandle->OnHandleClosed();
		uvhandle->_ctx->deleteHandle(uvhandle);
	}

	int UvContext::run() {
		return uv_run(_loop, UV_RUN_DEFAULT);
	}

}
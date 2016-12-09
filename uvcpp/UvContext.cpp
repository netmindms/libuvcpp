/*
 * EdContext.cpp
 *
 *  Created on: Nov 22, 2016
 *      Author: netmind
 */

#include "uvcpplog.h"
#include "UvContext.h"
#include "UvHandle.h"

using namespace std;
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
//		if (_handleLast) {
//			handle->_prev = _handleLast;
//			handle->_next = nullptr;
//			_handleLast->_next = handle;
//			_handleLast = handle;
//		} else {
//			_handleLast = handle;
//			handle->_next = nullptr;
//			handle->_prev = nullptr;
//		}
//		handle->init(user_data);
//		ald("init handle, cnt=%d", _pendingHandleCnt);
	}

	int UvContext::run() {
		return uv_run(_loop, UV_RUN_DEFAULT);
	}

	void UvContext::handle_close_cb(uv_handle_t *phandle) {
		assert(phandle->data);
		auto holder = (HandleHolder*)phandle->data;
		if(holder->closeLis) {
			holder->closeLis();
		}
		holder->ctx->deleteHandle(holder);
	}

	void UvContext::deleteHandle(HandleHolder *phandle) {
		if (!phandle) {
			return;
		}
		auto tprev = phandle->prev;
		auto tnext = phandle->next;
		if (tprev) {
			tprev->next = phandle->next;
		}
		if (tnext) {
			tnext->prev = phandle->prev;
		}
		if (tprev == nullptr && tnext == nullptr) {
			_handleLast = nullptr;
		}
//		ald("delete handle, name=%s, remain=%d", GET_UV_HANDLE_NAME(phandle), _pendingHandleCnt-1);
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

	HandleHolder *UvContext::createHandleHolder() {
		auto holder = new HandleHolder;
		if (_handleLast) {
			holder->prev = _handleLast;
			holder->next = nullptr;
			_handleLast->next = holder;
			_handleLast = holder;
		} else {
			_handleLast = holder;
			holder->next = nullptr;
			holder->prev = nullptr;
		}
		++_pendingHandleCnt;
		return holder;

	}

	void UvContext::closeHandle(HandleHolder *holder) {
		if(uv_is_closing((uv_handle_t*)(&holder->rawh.handle)) == false) {
			uv_close((uv_handle_t*)(&holder->rawh.handle), handle_close_cb);
		}
	}

	void UvContext::write_cb(uv_write_t *req, int status) {
		alv("write cb, status=%d, ptcp=%0x", status, (uint64_t) req->data);
		auto holder = ((HandleHolder *) req->data);
		auto up =holder->_writeReqQue.pop();
		if (holder->writeLis) {
			holder->writeLis(status);
		}
		holder->_writeReqQue.recycleObj(move(up));
	}

}
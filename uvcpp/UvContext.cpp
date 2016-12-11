/*
 * EdContext.cpp
 *
 *  Created on: Nov 22, 2016
 *      Author: netmind
 */

#include <atomic>
#include <sstream>
#include "uvcpplog.h"
#include "UvContext.h"
#include "UvStream.h"

std::atomic_uint _gHandleIdSeed;

using namespace std;
namespace uvcpp {

	thread_local UvContext *_gUvContext=nullptr;

	UvContext::UvContext() {
		_loop = nullptr;
		_createLoop = false;
		_pendingHandleCnt = 0;
		_handleLast = nullptr;
	}

	UvContext::~UvContext() {

	}


	void UvContext::open(uv_loop_t *loop) {
		if(_gUvContext) {
			alw("*** UvContext already opened. UvContext is a singletone object.");
			return;
		}
		auto ctx = new UvContext();

		if(!loop) {
			ctx->_loop = new uv_loop_t;
			ctx->_createLoop = true;
			loop = ctx->_loop;
			uv_loop_init(ctx->_loop);
		}
		ctx->_loop = loop;
		_gUvContext = ctx;
	}

	void UvContext::openWithDefaultLoop() {
		UvContext::open(uv_default_loop());
	}


	void UvContext::close() {
		if(_gUvContext) {
			auto ctx = _gUvContext;
			assert(ctx->_handleLast == nullptr);
			if (ctx->_createLoop) {
				if (ctx->_loop) {
					uv_loop_close(ctx->_loop);
					delete ctx->_loop;
					ctx->_loop = nullptr;
				}
				ctx->_createLoop = false;
			}
			delete ctx;
			_gUvContext = nullptr;
		}
	}

	UvContext *UvContext::getContext() {
		assert(_gUvContext);
		return _gUvContext;
	}


	uv_loop_t *UvContext::getLoop() {
		return _loop;
	}



	int UvContext::run(uv_run_mode mode) {
		assert(_gUvContext);
		return uv_run(_gUvContext->_loop, mode);
	}

	void UvContext::handle_close_cb(uv_handle_t *phandle) {
		assert(phandle->data);
		auto holder = (HandleHolder*)phandle->data;
		if(holder->closeLis) {
			holder->closeLis();
		}
		holder->ctx->deleteHandle(holder);
	}

	void UvContext::deleteHandle(HandleHolder *holder) {
		if (!holder) {
			return;
		}
		auto tprev = holder->prev;
		auto tnext = holder->next;
		if (tprev) {
			tprev->next = holder->next;
		}
		if (tnext) {
			tnext->prev = holder->prev;
		}
		if (tprev == nullptr && tnext == nullptr) {
			_handleLast = nullptr;
		}

		if(holder->uvh) {
			holder->uvh->_handleHolder = nullptr;
		}
		ald("delete handle, name=%s, remain=%d", holder->handleName, _pendingHandleCnt-1);
		delete holder;
		--_pendingHandleCnt;
	}

	int UvContext::handleCount() {
		return _pendingHandleCnt;
	}

	HandleHolder *UvContext::createHandleHolder(UvHandle* uvh) {
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
		holder->uvh = uvh;
#ifndef NDEBUG
		std::stringstream ss;
		ss << std::string(typeid(*uvh).name()) << '_' <<  _gHandleIdSeed++;
		holder->handleName = ss.str();
#endif
		++_pendingHandleCnt;
		ald("create handle holder, handle_name=%s, count=%u", holder->handleName, _pendingHandleCnt);
		return holder;
	}

	void UvContext::closeHandle(HandleHolder *holder) {
		if(uv_is_closing((uv_handle_t*)(&holder->rawh.handle)) == false) {
			uv_close((uv_handle_t*)(&holder->rawh.handle), handle_close_cb);
		}
	}

	void UvContext::handle_write_cb(uv_write_t *req, int status) {
		alv("write cb, status=%d, ptcp=%0x", status, (uint64_t) req->data);
		auto holder = ((HandleHolder *) req->data);
		assert(holder);
		auto up =holder->writeReqQue.pop();
		holder->writeReqQue.recycleObj(move(up));
	}

	void UvContext::handle_send_cb(uv_udp_send_t *req, int status) {
		alv("send cb, status=%d, psock=%0x", status, (uint64_t) req->data);
		auto holder = (HandleHolder *) req->data;
		assert(holder);
		auto up =holder->sendReqQue.pop();
		holder->sendReqQue.recycleObj(move(up));
	}

	void UvContext::handle_read_alloc_cb(uv_handle_t *handle, size_t suggesited_size, uv_buf_t *puvbuf) {

		auto holder = (HandleHolder*)handle->data;
		assert(holder);
		auto upbuf = holder->readBufQue.allocObj();
		auto tbuf = upbuf->allocBuffer(suggesited_size);
		puvbuf->len = tbuf.first;
		puvbuf->base = tbuf.second;
		holder->readBufQue.push(move(upbuf));
		alv("alloc buffer, buf_base=%x", (long)puvbuf->base);
	}

	void UvContext::handle_connect_cb(uv_connect_t *req, int status) {
		ali("on connect, status=%d", status);
		auto holder = (HandleHolder*)req->data;
		assert(holder);
		auto strm = (UvStream*)(holder->uvh);
		if(strm) {
			strm->procConnectCallback(status);
		}
	}

	void UvContext::handle_read_cb(uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf) {
		ald("readcb, nread=%d, buf_base=%x", nread, (long)buf->base);
		auto holder = (HandleHolder*)handle->data;
		auto uprbuf = holder->readBufQue.pop();
		if (nread > 0) {
			if(holder->uvh) {
				uprbuf->size = nread;
				((UvStream*)holder->uvh)->procReadCallback(move(uprbuf));
			}
		} else if(nread == UV_EOF) {
			if(holder->uvh) {
				((UvStream*)holder->uvh)->procConnectCallback(-1);
			}
		} else {
			assert(0);
		}
	}

	void UvContext::handle_listen_cb(uv_stream_t *server, int status) {
		auto holder = (HandleHolder*)server->data;
		auto strm = (UvStream*)(holder->uvh);
		if(strm) {
			strm->procListenCallback(status);
		}
	}

}
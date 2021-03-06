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
#include "UvUdp.h"
#include "UvFsEvent.h"
#include "UvGetAddrInfo.h"
#include "UvGetNameInfo.h"
#include "ImmediateWrapper.h"
#include "Immediate.h"

std::atomic_uint _gHandleIdSeed;

using namespace std;
namespace uvcpp {
#ifdef __GNUC__
    #if (__GNUC__ <= 4)
        #if (__GNUC_MINOR__ < 9)
            __thread UvContext *_gUvContext=nullptr;
        #else
            thread_local UvContext *_gUvContext=nullptr;
        #endif
    #else
        thread_local UvContext *_gUvContext=nullptr;
    #endif
#else
	thread_local UvContext *_gUvContext=nullptr;
#endif



	UvContext::UvContext() {
		_loop = nullptr;
		_createLoop = false;
		_pendingHandleCnt = 0;
		_handleLast = nullptr;
		_ctxImmd = nullptr;
	}

	UvContext::~UvContext() {
		if(_ctxImmd) {
			delete _ctxImmd;
		}
	}


	void UvContext::open(uv_loop_t *loop) {
		if(_gUvContext) {
			ulw("*** UvContext already opened. UvContext is a singletone object.");
			return;
		}
		auto ctx = new UvContext();
		ctx->_ctxImmd = new Immediate();
		if(!loop) {
			ctx->_loop = new uv_loop_t;
			ctx->_createLoop = true;
			loop = ctx->_loop;
			uv_loop_init(ctx->_loop);
		}
		ctx->_loop = loop;
		ctx->_commonHandleSeed = 0;
		_gUvContext = ctx;
	}

	void UvContext::openWithDefaultLoop() {
		UvContext::open(uv_default_loop());
	}


	void UvContext::close() {
		if(_gUvContext) {
			auto ctx = _gUvContext;
			if(ctx->_ctxImmd) {
				ctx->_ctxImmd->close();
				// Immediate delete는 destructor에서 한다.
			}
			if(ctx->_handleLast != nullptr) {
				ule("### handle not closed, handle_class=%s", ctx->_handleLast->handleName.data());
				assert(0);
			}

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
		} else {
			_handleLast = tprev;
		}

		ulv("delete handle, name=%s, remain=%d", holder->handleName.data(), _pendingHandleCnt-1);
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
		ulv("create handle holder, handle_name=%s, count=%u", holder->handleName.data(), _pendingHandleCnt);
		return holder;
	}

	void UvContext::closeHandle(HandleHolder *holder) {
		if(uv_is_closing((uv_handle_t*)(&holder->rawh.handle)) == false) {
			uv_close((uv_handle_t*)(&holder->rawh.handle), handle_close_cb);
		}
	}

	void UvContext::handle_write_cb(uv_write_t *req, int status) {
		ulv("write cb, status=%d, ptcp=%p", status, req->data);
		auto holder = ((HandleHolder *) req->data);
		assert(holder);
		auto up = holder->writeReqQue.pop_front();
		if(holder->uvh) {
			((UvStream*)holder->uvh)->procWriteCallback(status);
		}
		holder->writeReqQue.recycle(move(up));
	}

	void UvContext::handle_send_cb(uv_udp_send_t *req, int status) {
		ulv("send cb, status=%d, psock=%p", status, req->data);
		auto holder = (HandleHolder *) req->data;
		assert(holder);
		auto up = holder->sendReqQue.pop_front();
		if(holder->uvh) {
			((UvUdp*)holder->uvh)->procSendCallback(status);
		}
		holder->sendReqQue.recycle(move(up));
	}

	void UvContext::handle_read_alloc_cb(uv_handle_t *handle, size_t suggesited_size, uv_buf_t *puvbuf) {
		// FixMe: test
		if(!suggesited_size) {
			uli("******* read alloc cb, suggestied size is zero");
		}
		auto holder = (HandleHolder*)handle->data;
		assert(holder);
		auto upbuf = holder->readBufQue.allocObj();
		auto tbuf = upbuf->allocBuffer(suggesited_size);
		puvbuf->len = tbuf.first;
		puvbuf->base = tbuf.second;
		holder->readBufQue.push_back(move(upbuf));
		ulv("alloc buffer, buf_base=%p, suggested_size=%ld", puvbuf->base, suggesited_size);
	}

	void UvContext::handle_connect_cb(uv_connect_t *req, int status) {
		uld("on connect, status=%d", status);
		auto holder = (HandleHolder*)req->data;
		assert(holder);
		auto strm = (UvStream*)(holder->uvh);
		if(strm) {
			strm->procConnectCallback(status);
		}
	}

	/*
	 * uv_read_cb is not called after read_stop()
	 */
	void UvContext::handle_read_cb(uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf) {
		ulv("readcb, nread=%ld, buf_base=%p", nread, buf->base);
		auto holder = (HandleHolder*)handle->data;
		auto uprbuf = holder->readBufQue.pop_front();
		assert(buf->base==uprbuf->buffer);
		if (nread > 0) {
			if(holder->uvh) {
				uprbuf->size = nread;
				((UvStream*)holder->uvh)->procReadCallback(move(uprbuf));
			}
		} else if(nread == UV_EOF) {
			if(holder->uvh) {
				((UvStream*)holder->uvh)->procConnectCallback(-1);
			}
		} else if(nread < 0) {
			if(holder->uvh) {
				((UvStream*)holder->uvh)->procConnectCallback(-1);
			}
		} else {
			// nread == 0 인 경우.
			// this means EWOULDBOCK
			// TODO: research 필요.
		}
	}

	void UvContext::handle_listen_cb(uv_stream_t *server, int status) {
		auto holder = (HandleHolder*)server->data;
		auto strm = (UvStream*)(holder->uvh);
		if(strm) {
			strm->procListenCallback(status);
		}
	}

	void UvContext::handle_recv_cb(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags) {
		ulv("readcb, nread=%ld, buf_base=%p", nread, buf->base);
		auto holder = (HandleHolder*)handle->data;
		// FixMe: test code
		if(holder->readBufQue.size()>1) {
			uli("*** read que size=%ld", holder->readBufQue.size());
		}

		assert(holder);
		auto uprbuf = holder->readBufQue.pop_front();
		assert(buf->base==uprbuf->buffer);
		uprbuf->size = nread;
		if (nread >= 0) {
			// nread==0 && buf->base != nullptr  ==> empty udp packet recevied.
			// nread==0 && buf->base == nullptr  ==> threre is nothing to read.
			if(holder) {
				auto udp = (UvUdp*)(holder->uvh);
				udp->procRecvCallback(move(uprbuf), addr, flags);
			}

		} else {
			assert(0);
		}
	}

	void UvContext::handle_close_cb(uv_handle_t *phandle) {
		assert(phandle->data);
		auto holder = (HandleHolder*)phandle->data;
		auto uvh = (UvHandle*)holder->uvh;
		if(uvh) {
			uvh->_status = UvHandle::IDLE;
		}
		if(holder->closeLis) {
			holder->closeLis(uvh);
		}
		holder->ctx->deleteHandle(holder);
	}

	void UvContext::handle_shutdown_cb(uv_shutdown_t *req, int status) {
		auto holder = (HandleHolder*)req->data;
		assert(holder);
		auto strm = (UvStream*)(holder->uvh);
		if(strm) {
			strm->procShutdownCallback(status);
		}
	}

	AddrInfoReq *UvContext::allocAddrInfoReq() {
		_addrReqList.emplace_front();
		auto itr = _addrReqList.begin();
		itr->req.data = &(*itr);
		itr->itr = itr;
		itr->ctx = this;
		return &(*itr);
	}

	void UvContext::req_getaddrinfo_cb(uv_getaddrinfo_t *req, int status, struct addrinfo *res) {
		auto addr = (AddrInfoReq*)req->data;
		if(addr->obj) {
			UvGetAddrInfo* uvaddrinfo = ((UvGetAddrInfo*)addr->obj);
			((UvGetAddrInfo*)addr->obj)->procCallback(status, res);
		}
		uv_freeaddrinfo(res);
		addr->ctx->_addrReqList.erase(addr->itr);
	}

	NameInfoReq *UvContext::allocNameInfoReq() {
		_nameReqList.emplace_front();
		auto itr = _nameReqList.begin();
		itr->req.data = &(*itr);
		itr->itr = itr;
		itr->ctx = this;
		return &(*itr);
	}

	void UvContext::req_getnameinfo_cb(uv_getnameinfo_t *req, int status, const char *hostname, const char *service) {
		auto namereq = (NameInfoReq*)req->data;
		if(namereq->obj) {
			UvGetNameInfo* uvnameinfo = ((UvGetNameInfo*)namereq->obj);
			uvnameinfo->procCallback(status, hostname, service);
		}
		namereq->ctx->_nameReqList.erase(namereq->itr);
	}

	uint32_t UvContext::newCommonHandle() {
		auto &seed = _gUvContext->_commonHandleSeed;
		if(++seed==0) {
			seed++;
		}
		return seed;
	}

	std::unique_ptr<ImmediateWrapper> UvContext::createImmediate() {
		auto* ctx = _gUvContext;
//		if(ctx->_ctxImmd==nullptr) {
//			ctx->_ctxImmd = new Immediate();
//		}
		return ImmediateWrapper::create(ctx->_ctxImmd);
	}

	uint32_t UvContext::setImmediate(std::function<void(uint32_t)> lis) {
		return _gUvContext->_ctxImmd->setImmediate(lis);
	}

	void UvContext::abortImmediate(uint32_t handle) {
		_gUvContext->_ctxImmd->abort(handle);
	}

}
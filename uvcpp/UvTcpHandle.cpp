/*
 * UvTcpHandle.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#include <nmdutil/nmdlog.h>
#include "UvEvent.h"
#include "UvTcpHandle.h"

namespace uvcpp {
	using namespace std;

	UvTcpHandle::UvTcpHandle() {
		_cnnHandle.data = nullptr;
		_readSize = 0;
	}

	UvTcpHandle::~UvTcpHandle() {
	}

	int UvTcpHandle::open(CnnLis clis, ReadLis rlis) {
		_cnnLis = clis;
		_readLis = rlis;
		_writeReqQue.open(10);
		return uv_tcp_init(_ctx->getLoop(), &_rawhandle);
	}


	int UvTcpHandle::connect(const char *ipaddr, uint16_t port) {
		sockaddr_in req_addr;
		uv_ip4_addr(ipaddr, port, &req_addr);
		_cnnHandle.data = (void *) this;
		uv_tcp_connect(&_cnnHandle, &_rawhandle, (struct sockaddr *) &req_addr, on_connect);
//	_status = SOCK_STATUS_CONNECTING;
		return 0;
	}

	int UvTcpHandle::write(const char *buf, size_t len) {
		auto upwr = _writeReqQue.allocObj();
		upwr->fillBuf(buf, len);
		upwr->req.data = this;
		auto ret = uv_write(&upwr->req, (uv_stream_t *) &_rawhandle, &upwr->uvBuf, 1, on_write_end);
		if (!ret) {
			_writeReqQue.push(move(upwr));
		} else {
			ale("### uv write fail, ret=%d", ret);
		}
		return ret;

	}

	void UvTcpHandle::OnHandleClosed() {
		ali("on tcp handle closed, write_que=%d", _writeReqQue.getQueCnt());
		assert(_writeReqQue.getQueCnt() == 0);
	}

	void UvTcpHandle::on_write_end(uv_write_t *req, int status) {
		ali("write cb, status=%d, psock=%0x", status, (uint64_t) req->data);
		auto psock = (UvTcpHandle *) req->data;
		if (psock) {
			auto up = psock->_writeReqQue.pop();
			psock->_writeReqQue.recycleObj(move(up));
		}
	}


	uv_handle_t *UvTcpHandle::getRawHandle() {
		return (uv_handle_t *) &_rawhandle;
	}

	void UvTcpHandle::on_connect(uv_connect_t *puvcnn, int status) {
		ali("on connect, status=%d", status);
		UvTcpHandle *ptcp = (UvTcpHandle *) puvcnn->data;
		if (status == 0) {
			uv_read_start((uv_stream_t *) &ptcp->_rawhandle, alloc_cb, read_cb);
			if (ptcp->_cnnLis) {
				ptcp->_cnnLis(UvEvent::CONNECTED);
			}
		} else {
			if (ptcp->_cnnLis) {
				ptcp->_cnnLis(UvEvent::DISCONNECTED);
			}
		}
	}

	void UvTcpHandle::alloc_cb(uv_handle_t *handle, size_t suggesited_size, uv_buf_t *puvbuf) {
		UvTcpHandle *ptcph = (UvTcpHandle *) handle->data;
		auto uprbuf = ptcph->_readBufQue.allocObj();
		auto tbuf = uprbuf->allocBuffer(suggesited_size);
		puvbuf->len = tbuf.first;
		puvbuf->base = tbuf.second;
		ptcph->_readBufQue.push(move(uprbuf));
	}

	void UvTcpHandle::read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *) {
		ali("readcb, nread=%d", nread);
		auto ptcph = (UvTcpHandle *) stream->data;
		auto uprbuf = ptcph->_readBufQue.pop();
		if (nread > 0) {
			uprbuf->size = nread;
			if (ptcph->_readLis) {
				ptcph->_readLis(move(uprbuf));
			}
		} else {
			if (ptcph->_cnnLis) {
				ptcph->_cnnLis(UvEvent::DISCONNECTED);
			}
			ptcph->_readLis = nullptr;
			ptcph->_cnnLis = nullptr;
		}
	}


	size_t UvTcpHandle::writeQueCnt() {
		return _writeReqQue.getQueCnt();
	}

	void UvTcpHandle::setReadOnListener(UvTcpHandle::ReadLis lis) {
		_readLis = lis;
	}

	void UvTcpHandle::setCnnOnListener(UvTcpHandle::CnnLis lis) {
		_cnnLis = lis;
	}

	int UvTcpHandle::bind(const struct sockaddr *addr, unsigned int flags) {
		return uv_tcp_bind(&_rawhandle, addr, flags);
	}

	int UvTcpHandle::listen(int backlogs) {
		uv_listen((uv_stream_t *) &_rawhandle, backlogs, connection_cb);
		return 0;
	}

	void UvTcpHandle::connection_cb(uv_stream_t *server, int status) {
		auto ptcph = (UvTcpHandle *) server->data;
		ptcph->_cnnLis(UvEvent::INCOMING);
	}

	UvTcpHandle *UvTcpHandle::accept() {
		auto ptcph = _ctx->createUvHandle<UvTcpHandle>();
		uv_tcp_init(_ctx->getLoop(), &ptcph->_rawhandle);
		auto ret = uv_accept((uv_stream_t *) &_rawhandle, (uv_stream_t *) &ptcph->_rawhandle);
		if (!ret) {
			uv_read_start((uv_stream_t *) &ptcph->_rawhandle, alloc_cb, read_cb);
			return ptcph;
		} else {
			_ctx->deleteHandle(ptcph);
			return nullptr;
		}
	}
}
//
// Created by netmind on 16. 11. 28.
//

#include <nmdutil/nmdlog.h>
#include "UvUdpHandle.h"

namespace uvcpp {
	UvUdpHandle::UvUdpHandle() {
		_rawHandle.data = nullptr;
	}

	UvUdpHandle::~UvUdpHandle() {

	}

	uv_handle_t *UvUdpHandle::getRawHandle() {
		return (uv_handle_t *) &_rawHandle;
	}

	int UvUdpHandle::open(UvUdpHandle::ReadLis lis) {
		_readLis = lis;
		_readBufQue.open(10);
		_writeReqQue.open(10);
		auto ret = uv_udp_init(_ctx->getLoop(), &_rawHandle);
		if (!ret) {
			ret = uv_udp_recv_start(&_rawHandle, alloc_cb, recv_cb);
		}
		return ret;
	}

	int UvUdpHandle::bind(const struct sockaddr *addr, unsigned int flags) {
		return uv_udp_bind(&_rawHandle, addr, flags);
	}

	void UvUdpHandle::recv_cb(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr,
							  unsigned flags) {
		ali("readcb, nread=%d", nread);
		auto phandle = (UvUdpHandle *) handle->data;
		auto uprbuf = phandle->_readBufQue.pop();
		if (nread > 0) {
			uprbuf->size = nread;
			if (phandle->_readLis) {
				phandle->_readLis(addr, move(uprbuf));
			}
		}
	}

	void UvUdpHandle::send_cb(uv_udp_send_t *req, int status) {
		ali("send cb, status=%d, psock=%0x", status, (uint64_t) req->data);
		auto psock = (UvUdpHandle *) req->data;
		if (psock) {
			auto up = psock->_writeReqQue.pop();
			psock->_writeReqQue.recycleObj(move(up));
		}
	}

	void UvUdpHandle::alloc_cb(uv_handle_t *handle, size_t suggesited_size, uv_buf_t *puvbuf) {
		auto pudpch = (UvUdpHandle *) handle->data;
		auto uprbuf = pudpch->_readBufQue.allocObj();
		auto tbuf = uprbuf->allocBuffer(suggesited_size);
		puvbuf->len = tbuf.first;
		puvbuf->base = tbuf.second;
		pudpch->_readBufQue.push(move(uprbuf));
	}

	int UvUdpHandle::send(const char *buf, const struct sockaddr *addr, uv_udp_send_cb send_cb) {

		return 0;
	}

}
//
// Created by netmind on 16. 11. 28.
//

#include "uvcpplog.h"
#include "UvUdp.h"

#include "uvcppdef.h"

namespace uvcpp {
	UvUdp::UvUdp() {
		_remoteAddr = nullptr;
	}

	UvUdp::~UvUdp() {

		if(_remoteAddr) {
			free(_remoteAddr);
		}
	}

	int UvUdp::bind(const struct sockaddr *addr, unsigned int flags) {
		return uv_udp_bind((uv_udp_t*)getRawHandle(), addr, flags);
	}

	int UvUdp::bind(const char *ipaddr, uint16_t port, unsigned int flags) {
		sockaddr_in addr;
		uv_ip4_addr(ipaddr, port, &addr);
		return bind((const sockaddr*)&addr, flags);
	}


	int UvUdp::send(const char *buf, size_t len, const struct sockaddr *addr) {
		auto upwr = _writeReqQue.allocObj();
		upwr->fillBuf(buf, len);
		upwr->req.data = this;
		auto ret = uv_udp_send(&upwr->req, (uv_udp_t*)getRawHandle(), &upwr->uvBuf, 1, addr, send_cb);
		if (!ret) {
			_writeReqQue.push(move(upwr));
		} else {
			ale("### uv write fail, ret=%d", ret);
		}
		return ret;

	}


	int UvUdp::send(const char *buf, size_t len) {
		if(_remoteAddr) {
			return send(buf, len, _remoteAddr);
		} else {
			assert(0);
			return -1;
		}
	}

	int UvUdp::send(const std::string &msg, const sockaddr *addr) {
		return send(msg.data(), msg.size(), addr);
	}

	int UvUdp::send(const std::string& msg) {
		return send(msg.data(), msg.size());
	}

	void UvUdp::setRemoteIpV4Addr(const char *ipaddr, uint16_t port) {
		if(_remoteAddr) {
			free(_remoteAddr);
		}
		_remoteAddr = (sockaddr*)malloc(sizeof(sockaddr_in));
		uv_ip4_addr(ipaddr, port, (sockaddr_in*)_remoteAddr);
	}


	void UvUdp::alloc_cb(uv_handle_t *handle, size_t suggesited_size, uv_buf_t *puvbuf) {
		ald("alloc cb");
		UvUdp* udp = (UvUdp*)handle->data;
		auto uprbuf = udp->_readBufQue.allocObj();
		auto tbuf = uprbuf->allocBuffer(suggesited_size);
		puvbuf->len = tbuf.first;
		puvbuf->base = tbuf.second;
		udp->_readBufQue.push(move(uprbuf));
	}

	void UvUdp::recv_cb(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags) {
		alv("readcb, nread=%d", nread);
		auto udp = (UvUdp*)handle->data;
		auto uprbuf = udp->_readBufQue.pop();
		if (nread > 0) {
			uprbuf->size = nread;
			if (udp->_readLis) {
				udp->_readLis(addr, move(uprbuf));
			}
		}
	}

	void UvUdp::send_cb(uv_udp_send_t *req, int status) {
		alv("send cb, status=%d, psock=%0x", status, (uint64_t) req->data);
		auto psock = (UvUdp *) req->data;
		if (psock) {
			auto up = psock->_writeReqQue.pop();
			psock->_writeReqQue.recycleObj(move(up));
		}
	}


	UvUdp *UvUdp::init() {
		auto udp = new UvUdp();
		uv_udp_init(UvContext::getLoop(), (uv_udp_t*)udp->getRawHandle());
		udp->registerContext();
		return udp;
	}

	int UvUdp::recvStart(UvUdp::Lis lis) {
		_readLis = lis;
		return uv_udp_recv_start((uv_udp_t*)getRawHandle(), alloc_cb, recv_cb);
	}

	void UvUdp::recvStop() {
		uv_udp_recv_stop((uv_udp_t*)getRawHandle());
	}


}
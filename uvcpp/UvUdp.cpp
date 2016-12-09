//
// Created by netmind on 16. 11. 28.
//

#include "uvcpplog.h"
#include "UvUdp.h"

#include "uvcppdef.h"

#define RAWH() ((uv_udp_t*)getRawHandle())
#define GETOBJH(H) ((UvUdp*)(((HandleHolder*)H->data))->uvh)


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
		return uv_udp_bind(RAWH(), addr, flags);
	}

	int UvUdp::bind(const char *ipaddr, uint16_t port, unsigned int flags) {
		sockaddr_in addr;
		uv_ip4_addr(ipaddr, port, &addr);
		return bind((const sockaddr*)&addr, flags);
	}

	int UvUdp::init() {
		initHandle();
		return uv_udp_init(getLoop(), RAWH());
	}


	int UvUdp::recvStart(ReadLis lis) {
		_readLis = lis;
		return uv_udp_recv_start(RAWH(), UvContext::handle_read_alloc_cb, recv_cb);
	}


	int UvUdp::send(const char *buf, size_t len, const struct sockaddr *addr) {
		return UvHandle::send(buf, len, addr);
	}

	int UvUdp::send(const char *buf, size_t len) {
		if(_remoteAddr) {
			return UvHandle::send(buf, len, _remoteAddr);
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


//	void UvUdp::alloc_cb(uv_handle_t *handle, size_t suggesited_size, uv_buf_t *puvbuf) {
//		auto udp = GETOBJH(handle);
//		auto uprbuf = udp->_readBufQue.allocObj();
//		auto tbuf = uprbuf->allocBuffer(suggesited_size);
//		puvbuf->len = tbuf.first;
//		puvbuf->base = tbuf.second;
//		udp->_readBufQue.push(move(uprbuf));
//	}

	void UvUdp::recv_cb(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags) {
		alv("readcb, nread=%d", nread);
		auto holder = (HandleHolder*)handle->data;
		assert(holder);
		auto udb = GETOBJH(handle);
		auto uprbuf = holder->readBufQue.pop();
		if (nread >= 0) { // TODO:
			uprbuf->size = nread;
			if (udb->_readLis) {
				udb->_readLis(addr, move(uprbuf));
			}
		} else {
			assert(0);
		}
	}

	int UvUdp::recvStop() {
		return uv_udp_recv_stop(RAWH());
	}


}
//
// Created by netmind on 16. 11. 28.
//

#include <nmdutil/nmdlog.h>
#include "UvUdp.h"

#include "uvcppdef.h"

#define GET_UDP_RAWH(UVHANDLE) GET_RAWH(uv_udp_t, UVHANDLE)

namespace uvcpp {
	UvUdp::UvUdp() {
		_ohandle = nullptr;
		_remoteAddr = nullptr;
	}

	UvUdp::~UvUdp() {
		close();

		if(_remoteAddr) {
			free(_remoteAddr);
		}
	}

	int UvUdp::bind(const struct sockaddr *addr, unsigned int flags) {
		return uv_udp_bind(GET_UDP_RAWH(_ohandle), addr, flags);
	}

	int UvUdp::bind(const char *ipaddr, uint16_t port, unsigned int flags) {
		sockaddr_in addr;
		uv_ip4_addr(ipaddr, port, &addr);
		return bind((const sockaddr*)&addr, flags);
	}

	int UvUdp::open(Lis lis) {
		auto ctx = UvContext::getContext();
		int ret;
		if(ctx) {
			_readLis = lis;
			_ohandle = ctx->createHandle(this);
			_readBufQue.open(10);
			_writeReqQue.open(10);
			ret = uv_udp_init(ctx->getLoop(), GET_UDP_RAWH(_ohandle));
			if(ret) {
				ale("### udp init error");
			}
		} else {
			ret = -1;
		}
		return ret;
	}

	void UvUdp::close() {
		if(_ohandle != nullptr) {
			ald("closing udp ...");
			uv_udp_recv_stop(GET_UDP_RAWH(_ohandle));
			UvHandleOwner::close();
		}
	}

	int UvUdp::readStart() {
		if(_ohandle) {
			return uv_udp_recv_start(GET_UDP_RAWH(_ohandle), alloc_cb, recv_cb);
		}
		return -1;
	}

	int UvUdp::send(const char *buf, size_t len, const struct sockaddr *addr) {
		auto upwr = _writeReqQue.allocObj();
		upwr->fillBuf(buf, len);
		upwr->req.data = this;
		auto ret = uv_udp_send(&upwr->req, GET_UDP_RAWH(_ohandle), &upwr->uvBuf, 1, addr, send_cb);
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
		UvUdp* pudpch = GET_UVHANDLE_OWNER(UvUdp, handle);
		auto uprbuf = pudpch->_readBufQue.allocObj();
		auto tbuf = uprbuf->allocBuffer(suggesited_size);
		puvbuf->len = tbuf.first;
		puvbuf->base = tbuf.second;
		pudpch->_readBufQue.push(move(uprbuf));
	}

	void UvUdp::recv_cb(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags) {
		ali("readcb, nread=%d", nread);
		auto phandle = GET_UVHANDLE_OWNER(UvUdp, handle);
//		auto phandle = (UvUdp *) handle->data;
		auto uprbuf = phandle->_readBufQue.pop();
		if (nread > 0) {
			uprbuf->size = nread;
			if (phandle->_readLis) {
				phandle->_readLis(addr, move(uprbuf));
			}
		}
	}

	void UvUdp::send_cb(uv_udp_send_t *req, int status) {
		ali("send cb, status=%d, psock=%0x", status, (uint64_t) req->data);
		auto psock = (UvUdp *) req->data;
		if (psock) {
			auto up = psock->_writeReqQue.pop();
			psock->_writeReqQue.recycleObj(move(up));
		}
	}


}
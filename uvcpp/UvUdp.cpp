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
		auto ret = initHandle();
		if(!ret) {
			return uv_udp_init(getLoop(), RAWH());
		} else {
			return ret;
		}
	}


	int UvUdp::recvStart(RecvLis lis) {
		_recvLis = lis;
		return uv_udp_recv_start(RAWH(), UvContext::handle_read_alloc_cb, UvContext::handle_recv_cb);
	}


	int UvUdp::send(const char* buf, size_t len, const struct sockaddr* addr) {
		auto upsd = _handleHolder->sendReqQue.allocObj();
		upsd->fillBuf(buf, len);
		upsd->req.data = _handleHolder;
		auto ret = uv_udp_send(&upsd->req, (uv_udp_t*)RAWH(), &upsd->uvBuf, 1, addr, UvContext::handle_send_cb);
		if(!ret) {
			_handleHolder->sendReqQue.push(move(upsd));
		} else {
			assert(0);
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

	int UvUdp::recvStop() {
		return uv_udp_recv_stop(RAWH());
	}

	void UvUdp::procRecvCallback(upReadBuffer upbuf, const struct sockaddr *addr, unsigned flags) {
		if(_status == UvHandle::INITIALIZED) {
			_recvLis(move(upbuf), addr, flags);
		}
	}


}
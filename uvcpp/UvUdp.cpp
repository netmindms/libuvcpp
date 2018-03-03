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

	int UvUdp::initEx(unsigned int flags) {
		auto ret = initHandle();
		if(!ret) {
			return uv_udp_init_ex(getLoop(), RAWH(), flags);
		} else {
			return ret;
		}
	}



	int UvUdp::recvStart(RecvLis lis) {
		if(lis) {
			_recvLis = lis;
		}
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
			char tmp[50];
			ule("### udp send error, ret=%d, buf=%x, len=%d, addr=%s", ret, (uint64_t)buf, uv_ip4_name((sockaddr_in*)addr, tmp, sizeof(tmp)));
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
		assert(_recvLis != nullptr);
		_recvLis(move(upbuf), addr, flags);
	}

	void UvUdp::setOnRecvLis(UvUdp::RecvLis lis) {
		_recvLis = lis;
	}

	int UvUdp::getSockName(struct sockaddr *name, int *namelen) {
		return uv_udp_getsockname(RAWH(), name, namelen);
	}

	int UvUdp::trySend(const char *buf, size_t len, const struct sockaddr *addr) {
		if(_handleHolder->sendReqQue.getQueCnt() == 0 ) {
			uv_buf_t uvbuf;
			uvbuf.base = (char*)buf;
			uvbuf.len = len;
			return uv_udp_try_send(RAWH(), &uvbuf, 1, addr);
		}
		return UV_EAGAIN ;
	}

	int UvUdp::setMemberShip(const char *multicast_addr, const char *interface_addr, uv_membership membership) {
		return uv_udp_set_membership(RAWH(), multicast_addr, interface_addr, membership);
	}

	int UvUdp::setMulticastLoop(int on) {
		return uv_udp_set_multicast_loop(RAWH(), on);
	}

	int UvUdp::setMulticastTtl(int ttl) {
		return uv_udp_set_multicast_ttl(RAWH(), ttl);
	}

	int UvUdp::setMulticastInterface(const char *interface_addr) {
		return uv_udp_set_multicast_interface(RAWH(), interface_addr);
	}

	int UvUdp::setBroadcast(int on) {
		return uv_udp_set_broadcast(RAWH(), on);
	}

	int UvUdp::setTtl(int ttl) {
		return uv_udp_set_ttl(RAWH(), ttl);
	}

	void UvUdp::procSendCallback(int status) {
		if(_sendLis != nullptr) {
			_sendLis(status);
		}
	}

	void UvUdp::setOnSendLis(UvUdp::SendLis lis) {
		_sendLis = lis;
	}


}
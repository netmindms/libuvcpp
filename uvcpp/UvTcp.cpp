/*
 * UvTcp3.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#include "UvTcp.h"
#include "uvcpplog.h"

using namespace std;

#define RAWH() ((uv_tcp_t*)getRawHandle())


namespace uvcpp {
	UvTcp::UvTcp() {
	}

	UvTcp::~UvTcp() {

	}


	int UvTcp::connect(const sockaddr *addr, UvStream::CnnLis lis) {
		setConnectionReq(lis);
		return uv_tcp_connect(&_handleHolder->cnnReq, RAWH(), (struct sockaddr *) addr, UvContext::handle_connect_cb);
	}

	int UvTcp::connect(const char *ipaddr, uint16_t port, CnnLis lis) {
		sockaddr_in req_addr;
		uv_ip4_addr(ipaddr, port, &req_addr);
		setConnectionReq(lis);
		return uv_tcp_connect(&_handleHolder->cnnReq, RAWH(), (struct sockaddr *) &req_addr, UvContext::handle_connect_cb);
	}


	int UvTcp::bind(const struct sockaddr *addr, unsigned int flags) {
		return uv_tcp_bind(RAWH(), addr, flags);
	}

	int UvTcp::bindAndListen(uint16_t port, const char *ipaddr, UvStream::ListenLis lis, int backlogs) {
		sockaddr_in inaddr;
		uv_ip4_addr(ipaddr, port, &inaddr);
		bind((sockaddr *) &inaddr, 0);
		return UvStream::listen(lis, backlogs);
	}

	int UvTcp::init() {
		auto ret = initHandle();
		if(!ret) {
			return uv_tcp_init(getLoop(), RAWH());
		} else {
			return ret;
		}
	}

	int UvTcp::initEx(unsigned int flags) {
		auto ret = initHandle();
		if(!ret) {
			return uv_tcp_init_ex(getLoop(), RAWH(), flags);
		} else {
			return ret;
		}
	}

	int UvTcp::noDelay(int enable) {
		return uv_tcp_nodelay(RAWH(), enable);
	}

	int UvTcp::keepAlive(int enable, unsigned int delay) {
		return uv_tcp_keepalive(RAWH(), enable, delay);
	}

	int UvTcp::simultaneousAccepts(int enable) {
		return uv_tcp_simultaneous_accepts(RAWH(), enable);
	}

	int UvTcp::getSockName(struct sockaddr *name, int *namelen) {
		return uv_tcp_getsockname(RAWH(), name, namelen);
	}

	int UvTcp::getPeerName(struct sockaddr *name, int *namelen) {
		return uv_tcp_getpeername(RAWH(), name, namelen);
	}


}

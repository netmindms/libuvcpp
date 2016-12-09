/*
 * UvTcp3.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#include "UvTcp.h"
#include "uvcpplog.h"
#include "UvEvent.h"

using namespace std;

#define RAWH() ((uv_tcp_t*)getRawHandle())


namespace uvcpp {
	UvTcp::UvTcp() {
		_cnnHandle.data = nullptr;
	}

	UvTcp::~UvTcp() {

	}


	int UvTcp::connect(const char *ipaddr, uint16_t port, CnnLis lis) {
		sockaddr_in req_addr;
		uv_ip4_addr(ipaddr, port, &req_addr);
		setConnectionReq(lis);
//		_cnnHandle.data = (void *) this;
//		setOnCnnLis(lis);
		return uv_tcp_connect(&_handleHolder->cnnReq, RAWH(), (struct sockaddr *) &req_addr, UvContext::handle_connect_cb);
	}


//	int UvTcp::accept(UvTcp* newtcp) {
//		auto newrawh = (uv_stream_t *)newtcp->getRawHandle();
//		auto ret = uv_accept((uv_stream_t *) RAWH(), newrawh);
//		if (!ret) {
//			return ret;
//		} else {
//			ale("### accept error");
//			assert(0);
//			newtcp->close();
//			return -1;
//		}
//	}


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
		initHandle();
		return uv_tcp_init(getLoop(), RAWH());
	}



}

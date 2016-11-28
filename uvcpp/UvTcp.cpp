/*
 * UvTcp3.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#include "UvTcp.h"
#include <nmdutil/nmdlog.h>

using namespace std;

namespace uvcpp {
	UvTcp::UvTcp() {
		_ohandle = nullptr;
	}

	UvTcp::~UvTcp() {
		close();
	}

	int UvTcp::open(UvTcpHandle::CnnLis clis, UvTcpHandle::ReadLis rlis) {
		auto ctx = UvContext::getContext();
		if (ctx) {
			_ohandle = ctx->createUvHandle<UvTcpHandle>();
			_ohandle->open(clis, rlis);
		}
		return 0;
	}

	int UvTcp::connect(const char *ipaddr, uint16_t port) {
		ali("connecting...");
		return _ohandle->connect(ipaddr, port);
	}

	int UvTcp::write(const char *buf, size_t len) {
		return _ohandle->write(buf, len);
	}

	void UvTcp::close() {
		if (_ohandle) {
			ali("closing...");
			_ohandle->close();
			_ohandle = nullptr;
		}
	}

	size_t UvTcp::writeQueCnt() {
		if (_ohandle) {
			return _ohandle->writeQueCnt();
		} else {
			return 0;
		}
	}

	int UvTcp::accept(UvTcp *newcnn) {
		if (_ohandle) {
			auto newohandle = _ohandle->accept();
			newcnn->_ohandle = newohandle;
			return 0;
		} else {
			return -1;
		}
	}

	void UvTcp::setOnReadLis(UvTcpHandle::ReadLis lis) {
		_ohandle->setReadOnListener(lis);
	}

	void UvTcp::setOnCnnLis(UvTcpHandle::CnnLis lis) {
		_ohandle->setCnnOnListener(lis);
	}

	int UvTcp::listen(uint16_t port, const char *ipaddr, int backlogs) {
		sockaddr_in inaddr;
		uv_ip4_addr(ipaddr, port, &inaddr);
		_ohandle->bind((sockaddr *) &inaddr, 0);
		_ohandle->listen(backlogs);
		return 0;
	}

}

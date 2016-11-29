//
// Created by netmind on 16. 11. 28.
//

#include <nmdutil/nmdlog.h>
#include "UvUdp.h"

namespace uvcpp {
	UvUdp::UvUdp() {
		_ohandle = nullptr;
	}

	UvUdp::~UvUdp() {
		close();
	}

	int UvUdp::bind(const struct sockaddr *addr, unsigned int flags) {
		return _ohandle->bind(addr, flags);
	}

	int UvUdp::open(UvUdpHandle::ReadLis lis) {
		_ohandle = UvContext::getContext()->createUvHandle<UvUdpHandle>();
		_ohandle->open(lis);
		return 0;
	}

	void UvUdp::close() {
		if(_ohandle != nullptr) {
			ald("closing udp ...");
			_ohandle->readStop();
			_ohandle->close(); _ohandle = nullptr;
		}
	}

	int UvUdp::readStart() {
		if(_ohandle) {
			return _ohandle->readStart();
		}
		return -1;
	}

}
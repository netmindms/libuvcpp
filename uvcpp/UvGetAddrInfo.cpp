//
// Created by netmind on 16. 12. 23.
//

#include "UvContext.h"
#include "UvGetAddrInfo.h"

using namespace std;

namespace uvcpp {

	UvGetAddrInfo::UvGetAddrInfo() {
		_req = nullptr;
	}

	UvGetAddrInfo::~UvGetAddrInfo() {
		if(_req) {
			assert(!_req && "### Error: addrinfo request is not free");
			close();
		}
	}

	int UvGetAddrInfo::init() {
		_ctx = UvContext::getContext();
		if(_ctx) {
			_req = _ctx->allocAddrInfoReq();
			_req->obj = this;
			return 0;
		} else {
			assert(_ctx && "### Error: context not initialized in current thread");
			return -1;
		}
	}

	int UvGetAddrInfo::getAddrInfo(const char *node, const char *service, const struct addrinfo *hints, Lis lis) {
		_lis = lis;
		return uv_getaddrinfo(_ctx->getLoop(), &(_req->req), UvContext::req_getaddrinfo_cb, node, service, hints);
	}

	void UvGetAddrInfo::close() {
		if(_req) {
			_req->obj = nullptr;
			_req = nullptr;
		}
	}

	void UvGetAddrInfo::procCallback(int status, struct addrinfo* res) {
		_lis(status, res);
	}


}
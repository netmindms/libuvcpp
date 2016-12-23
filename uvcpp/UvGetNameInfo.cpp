//
// Created by netmind on 16. 12. 23.
//

#include "UvContext.h"
#include "UvGetNameInfo.h"


namespace uvcpp {
	UvGetNameInfo::UvGetNameInfo() {
		_req = nullptr;
	}

	UvGetNameInfo::~UvGetNameInfo() {
		if(_req) {
			assert(!_req && "### Error: nameiinfo request is not free");
			close();
		}
	}

	int UvGetNameInfo::init() {
		auto _ctx = UvContext::getContext();
		if(_ctx) {
			_req = _ctx->allocNameInfoReq();
			_req->obj = this;
			return 0;
		} else {
			assert(_ctx && "### Error: context not initialized in current thread");
			return -1;
		}
	}

	int UvGetNameInfo::getNameInfo(const struct sockaddr *addr, int flags, Lis lis) {
		auto ctx = UvContext::getContext();
		assert(ctx && "### Error: context is not initialized in current thread");
		_lis = lis;
		return uv_getnameinfo(ctx->getLoop(), &(_req->req), UvContext::req_getnameinfo_cb, addr, flags);
	}

	void UvGetNameInfo::procCallback(int status, const char *hostname, const char *service) {
		_lis(status, hostname, service);
	}

	void UvGetNameInfo::close() {
		if(_req) {
			_req->obj = nullptr;
			_req = nullptr;
		}
	}


}
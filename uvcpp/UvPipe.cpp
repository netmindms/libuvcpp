//
// Created by netmind on 16. 12. 7.
//

#include "UvPipe.h"

#define RAWH() ((uv_pipe_t*)getRawHandle())
#define GETOBJH(H) ((UvPipe*)(((HandleHolder*)H->data))->uvh)


namespace uvcpp {

	UvPipe::UvPipe() {

	}

	UvPipe::~UvPipe() {

	}

	int UvPipe::init(int ipc) {
		auto ret = initHandle();
		if(!ret) {
			return uv_pipe_init(getLoop(), RAWH(), ipc);
		} else {
			return ret;
		}
	}

	int UvPipe::bind(const char *name) {
		return uv_pipe_bind(RAWH(), name);
	}

	void UvPipe::connect(const char *name, UvStream::CnnLis lis) {
		setOnCnnLis(lis);
		auto creq = setConnectionReq(lis);
		uv_pipe_connect(creq, RAWH(), name, UvContext::handle_connect_cb);
	}

	int UvPipe::getSockName(char *buffer, size_t *size) {
		return uv_pipe_getsockname(RAWH(), buffer, size);
	}

	int UvPipe::getPeerName(char *buffer, size_t *size) {
		return uv_pipe_getpeername(RAWH(), buffer, size);
	}

	void UvPipe::pendingInstances(int count) {
		uv_pipe_pending_instances(RAWH(), count);
	}

	int UvPipe::pendingCount() {
		return uv_pipe_pending_count(RAWH());
	}

	uv_handle_type UvPipe::pendingType() {
		return uv_pipe_pending_type(RAWH());
	}
}
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
		initHandle();
		return uv_pipe_init(getLoop(), RAWH(), ipc);
	}

	int UvPipe::bind(const char *name) {
		return uv_pipe_bind(RAWH(), name);
	}

	void UvPipe::connect(const char *name, UvStream::CnnLis lis) {
		setOnCnnLis(lis);
		_cnnReq.data = this;
		uv_pipe_connect(&_cnnReq, RAWH(), name, UvStream::connect_cb);
	}
}
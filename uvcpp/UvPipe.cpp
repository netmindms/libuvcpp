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

	int UvPipe::open(uv_file fd) {
//		UvStream::open("pipe");
		return uv_pipe_open(RAWH(), fd);
	}
}
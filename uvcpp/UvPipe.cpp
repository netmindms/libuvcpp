//
// Created by netmind on 16. 12. 7.
//

#include "UvPipe.h"


namespace uvcpp {

	UvPipe::UvPipe() {

	}

	UvPipe::~UvPipe() {

	}

	int UvPipe::open(uv_file fd) {
//		UvStream::open("pipe");
		return uv_pipe_open((uv_pipe_t*)getRawHandle(), fd);
	}
}
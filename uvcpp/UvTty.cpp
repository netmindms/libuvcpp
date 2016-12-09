//
// Created by netmind on 16. 12. 7.
//

#include "UvTty.h"

#define RAWH() ((uv_tty_t*)getRawHandle())
#define GETOBJH(H) ((UvTty*)(((HandleHolder*)H->data))->uvh)


namespace uvcpp {

	UvTty::UvTty() {

	}

	UvTty::~UvTty() {

	}

	int UvTty::init(uv_file fd, int readable) {
		initHandle();
		return uv_tty_init(getLoop(), RAWH(), fd, readable);
	}
}
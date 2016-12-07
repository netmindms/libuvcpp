//
// Created by netmind on 16. 12. 7.
//

#include "UvTty.h"

namespace uvcpp {

	UvTty::UvTty() {

	}

	UvTty::~UvTty() {

	}

	int UvTty::open(uv_file fd, int readable) {
		UvStream::open("tty");
		return uv_tty_init(getLoop(), (uv_tty_t*)_rawh, fd, readable);
	}
}
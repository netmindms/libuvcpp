//
// Created by netmind on 16. 12. 7.
//

#include "UvTty.h"

namespace uvcpp {

	UvTty::UvTty() {

	}

	UvTty::~UvTty() {

	}

	UvTty *UvTty::init(uv_file fd, int readable) {
		UvTty* tty = new UvTty();
		uv_tty_init(UvContext::getLoop(), (uv_tty_t*)tty->getRawHandle(), fd, readable);
		tty->registerContext();
		return tty;
	}

}
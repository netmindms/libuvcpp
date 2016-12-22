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
		auto ret = initHandle();
		if(!ret) {
			return uv_tty_init(getLoop(), RAWH(), fd, readable);
		} else {
			return ret;
		}
	}

	int UvTty::setMode(uv_tty_mode_t mode) {
		return uv_tty_set_mode(RAWH(), mode);
	}

	int UvTty::resetMode(void) {
		return uv_tty_reset_mode();
	}

	int UvTty::getWinSize(int *width, int *height) {
		return uv_tty_get_winsize(RAWH(), width, height);
	}
}
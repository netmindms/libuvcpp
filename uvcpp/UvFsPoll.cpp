//
// Created by netmind on 12/19/16.
//

#include "UvFsPoll.h"
#define RAWH() ((uv_fs_poll_t*)getRawHandle())
namespace uvcpp {

	UvFsPoll::UvFsPoll() {

	}

	UvFsPoll::~UvFsPoll() {

	}

	int UvFsPoll::init() {
		initHandle();
		return uv_fs_poll_init(getLoop(), RAWH());
	}

	void UvFsPoll::stop(bool isclose) {
		if(isclose) {
			close();
		}
	}

	int UvFsPoll::start(const char *path, unsigned int interval, UvFsPoll::Lis lis) {
		_lis = lis;
		return uv_fs_poll_start(RAWH(), fs_poll_cb, path, interval);
	}

	void UvFsPoll::fs_poll_cb(uv_fs_poll_t* handle, int status, const uv_stat_t *prev, const uv_stat_t *curr) {
		auto holder = (HandleHolder*)handle->data;
		UvFsPoll* fpoll = (UvFsPoll*)holder->uvh;
		fpoll->_lis(status, prev, curr);
	}
}
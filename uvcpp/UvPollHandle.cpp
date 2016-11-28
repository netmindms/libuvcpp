/*
 * UvPollHandle.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#include <nmdutil/nmdlog.h>
#include "UvPollHandle.h"

namespace uvcpp {
	UvPollHandle::UvPollHandle() {

	}

	UvPollHandle::~UvPollHandle() {
	}

	uv_handle_t *UvPollHandle::getRawHandle() {
		return (uv_handle_t *) &_handle;
	}

	int UvPollHandle::open(int fd) {
		return uv_poll_init(_ctx->getLoop(), &_handle, fd);
	}

	int UvPollHandle::poll_start(int events, Lis lis) {
		_lis = lis;
		return uv_poll_start(&_handle, events, poll_cb);
	}

	int UvPollHandle::poll_stop() {
		return uv_poll_stop(&_handle);
	}

	void UvPollHandle::poll_cb(uv_poll_t *handle, int status, int events) {
		ali("poll callback");
		auto ph = (UvPollHandle *) handle->data;
		ph->_lis(events);
	}

}
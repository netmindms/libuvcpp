//
// Created by netmind on 16. 12. 5.
//

#include "UvPoll.h"
#include "uvcpplog.h"

#define RAWH() GET_RAWH(uv_poll_t)

namespace uvcpp {


	UvPoll::UvPoll() {
		_fd = -1;
	}

	UvPoll::~UvPoll() {
		assert(_fd<=0);
	}

	int UvPoll::open(int fd, int events, Lis lis) {
		if(_fd <= 0) {
			_fd =fd;
			_lis = lis;
			auto _rawh = (uv_poll_t*)createHandle("poll");
			uv_poll_init(_ctx->getLoop(), _rawh, _fd);
			uv_poll_start(_rawh, events, poll_cb);
		} else {
			ale("### already opened");
			return -1;
		}
	}

#if 0
	int UvPoll::pollStart(int events, UvPoll::Lis lis) {
		if(_fd > 0) {
			_lis = lis;
			return uv_poll_start(RAWH(), events, poll_cb);
		}
		return -1;
	}

	int UvPoll::pollStop(UvHandle::CloseLis lis) {
		close(lis);
	}
#endif
	void UvPoll::poll_cb(uv_poll_t *rawh, int status, int events) {
		ASSERT_RAW_UVHANDLE(rawh);
		auto pollhandle = GET_UVHANDLE_OWNER(UvPoll, rawh);
		pollhandle->_lis(events);
	}

	void UvPoll::close(UvHandle::CloseLis lis) {
		if(_fd > 0) {
			uv_poll_stop(RAWH());
			::close(_fd); _fd = -1;
		}
		UvHandleOwner::close(lis);
	}
}
//
// Created by netmind on 16. 12. 5.
//

#include "UvPoll.h"
#include "uvcpplog.h"

#define RAWH() ((uv_poll_t*)getRawHandle())
#define GETOBJH(H) ((UvPoll*)(((HandleHolder*)H->data))->uvh)


namespace uvcpp {


	UvPoll::UvPoll() {
		_fd = -1;
	}

	UvPoll::~UvPoll() {
		assert(_fd<=0);
	}

	int UvPoll::init(int fd) {
		if(_fd < 0) {
			_fd =fd;
			uv_poll_init(getLoop(), RAWH(), _fd);
		} else {
			ale("### already opened");
			return -1;
		}
	}

	int UvPoll::start(int events, UvPoll::Lis lis) {
		if(_fd > 0) {
			_lis = lis;
			return uv_poll_start(RAWH(), events, poll_cb);
		}
		return -1;
	}

	void UvPoll::stop(bool isclose) {
		if(_fd > 0) {
			uv_poll_stop(RAWH());
			if (isclose) {
				close();
			}
			_fd = -1;
		}

	}
	void UvPoll::poll_cb(uv_poll_t *rawh, int status, int events) {
		auto pollhandle = GETOBJH(rawh);
		pollhandle->_lis(events);
	}

	int UvPoll::getFd() {
		return _fd;
	}

}
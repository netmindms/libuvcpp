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


	int UvPoll::pollStart(int events, UvPoll::Lis lis) {
		if(_fd > 0) {
			_lis = lis;
			return uv_poll_start((uv_poll_t*)getRawHandle(), events, poll_cb);
		}
		return -1;
	}

	int UvPoll::pollStop() {
		uv_poll_stop((uv_poll_t*)getRawHandle());
	}


	void UvPoll::poll_cb(uv_poll_t *rawh, int status, int events) {
		auto *pollobj = (UvPoll*)rawh->data;
		if(pollobj->_lis) {
			pollobj->_lis(events);
		}
	}


	int UvPoll::getFd() {
		return _fd;
	}

	UvPoll *UvPoll::init(int fd, std::unique_ptr<UvPoll> pollobj) {
		if(fd<0) {
			return nullptr;
		}
		UvPoll* ptr;
		if(!pollobj) {
			ptr = new UvPoll;
		} else {
			ptr = pollobj.release();
		}
		uv_poll_init(UvContext::getLoop(), (uv_poll_t*)ptr->getRawHandle(), fd);
		ptr->registerContext();
		return ptr;
	}
}
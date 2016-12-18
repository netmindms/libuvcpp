//
// Created by netmind on 12/18/16.
//

#include "UvFsEvent.h"
namespace  uvcpp {
	UvFsEvent::UvFsEvent() {

	}

	UvFsEvent::~UvFsEvent() {

	}

	int UvFsEvent::init() {
		initHandle();
		return uv_fs_event_init(getLoop(), (uv_fs_event_t*)getRawHandle());
	}

	int UvFsEvent::start(const char *path, unsigned int flags, UvFsEvent::Lis lis) {
		_lis = lis;
		return uv_fs_event_start((uv_fs_event_t*)getRawHandle(), handle_fs_event_cb, path, flags);
	}

	void UvFsEvent::stop(bool isclose) {
		uv_fs_event_stop((uv_fs_event_t*)getRawHandle());
		if(isclose) {
			close();
		}
	}

	int UvFsEvent::getPath(char *buffer, size_t *size) {
		return uv_fs_event_getpath((uv_fs_event_t*)getRawHandle(), buffer, size);
	}

	void UvFsEvent::handle_fs_event_cb(uv_fs_event_t *handle, const char *filename, int events, int status) {
		auto holder = (HandleHolder*)handle->data;
		((UvFsEvent*)(holder->uvh))->_lis(filename, events, status);
	}


}
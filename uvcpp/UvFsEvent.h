//
// Created by netmind on 12/18/16.
//

#ifndef UVCPPPRJ_UVFSEVENT_H
#define UVCPPPRJ_UVFSEVENT_H

#include "UvHandle.h"

namespace uvcpp {
	class UvFsEvent : public UvHandle {
		friend class UvContext;
	public:
		typedef std::function<void(const char* filename, int events, int status)> Lis;

		UvFsEvent();

		virtual ~UvFsEvent();

		int init();

		int start(const char* path, unsigned int flags, Lis lis);

		void stop(bool isclose=true);

		int getPath(char* buffer, size_t* size);
	private:
		Lis _lis;
		static void handle_fs_event_cb(uv_fs_event_t* handle, const char* filename, int events, int status);

	};
}

#endif //UVCPPPRJ_UVFSEVENT_H

//
// Created by netmind on 12/19/16.
//

#ifndef UVCPPPRJ_UVFSPOLL_H
#define UVCPPPRJ_UVFSPOLL_H

#include "UvHandle.h"

namespace uvcpp {
	class UvFsPoll : public UvHandle {
	public:
		typedef std::function<void(int, const uv_stat_t*, const uv_stat_t*)> Lis;

		UvFsPoll();

		virtual ~UvFsPoll();

		int init();

		int start(const char* path, unsigned int interval, Lis lis);

		void stop(bool isclose=true);

	private:
		Lis _lis;
		static void fs_poll_cb(uv_fs_poll_t* handle, int status, const uv_stat_t* prev, const uv_stat_t* curr);
	};
}

#endif //UVCPPPRJ_UVFSPOLL_H

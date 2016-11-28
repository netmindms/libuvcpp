/*
 * UvPollHandle.h
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#ifndef UVPOLLHANDLE_H_
#define UVPOLLHANDLE_H_
#include "UvBaseHandle.h"

namespace uvcpp {
	class UvPollHandle : public UvBaseHandle {
	public:
		typedef std::function<void(int events)> Lis;

		UvPollHandle();

		virtual ~UvPollHandle();

		int open(int fd);

		int poll_start(int events, Lis lis);

		int poll_stop();

	private:
		uv_poll_t _handle;
		Lis _lis;

		uv_handle_t *getRawHandle() override;

		static void poll_cb(uv_poll_t *handle, int status, int events);

	};
}
#endif /* UVPOLLHANDLE_H_ */

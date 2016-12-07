//
// Created by netmind on 16. 12. 5.
//

#ifndef UVCPPPRJ_UVPOLL_H
#define UVCPPPRJ_UVPOLL_H

#include "UvHandleOwner.h"

namespace uvcpp {
	class UvPoll : public UvHandleOwner {
	public:
		typedef std::function<void(int)> Lis;
		UvPoll();

		virtual ~UvPoll();

		int open(int fd, int events, Lis lis);
//		int pollStart(int events, Lis lis);
//		int pollStop(UvHandle::CloseLis lis=nullptr);

		void close(UvHandle::CloseLis lis) override;

	private:
		static void poll_cb(uv_poll_t* handle, int status, int events);
		Lis _lis;
	protected:
		int _fd;
	};
}

#endif //UVCPPPRJ_UVPOLL_H

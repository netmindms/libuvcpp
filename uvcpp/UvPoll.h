//
// Created by netmind on 16. 12. 5.
//

#ifndef UVCPPPRJ_UVPOLL_H
#define UVCPPPRJ_UVPOLL_H

#include <memory>
#include "UvHandleOwner.h"

namespace uvcpp {
	class UvPoll : public UvHandle {
	public:
		typedef std::function<void(int)> Lis;

		static UvPoll* init(int fd, std::unique_ptr<UvPoll> pollobj=nullptr);

		int pollStart(int events, Lis lis);
		int pollStop();
		int getFd();

	private:
		int _fd;
		Lis _lis;

		static void poll_cb(uv_poll_t* handle, int status, int events);

	public:
		UvPoll();
		virtual ~UvPoll();
	};
}

#endif //UVCPPPRJ_UVPOLL_H

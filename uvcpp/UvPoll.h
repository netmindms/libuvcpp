//
// Created by netmind on 16. 12. 5.
//

#ifndef UVCPPPRJ_UVPOLL_H
#define UVCPPPRJ_UVPOLL_H


#include "UvHandle.h"

namespace uvcpp {
	class UvPoll : public UvHandle {
	public:
		typedef std::function<void(int)> Lis;
		UvPoll();

		virtual ~UvPoll();

		int init(int fd);
		int start(int events, Lis lis);
		void stop(bool isclose=true);
		int getFd();
	private:
		static void poll_cb(uv_poll_t* handle, int status, int events);
		Lis _lis;
	protected:
		int _fd;
	};
}

#endif //UVCPPPRJ_UVPOLL_H

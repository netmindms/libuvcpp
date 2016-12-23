//
// Created by netmind on 16. 12. 23.
//

#ifndef UVCPPPRJ_UVGETNAMEINFO_H
#define UVCPPPRJ_UVGETNAMEINFO_H

#include <functional>
#include "AddrInfoReq.h"

namespace uvcpp {
	class UvGetNameInfo {
		friend class UvContext;
	public:
		UvGetNameInfo();

		virtual ~UvGetNameInfo();

		typedef std::function<void(int status, const char *hostname, const char *service)> Lis;

		int init();

		void close();

		int getNameInfo(const struct sockaddr* addr, int flags, Lis lis);
	private:
		Lis _lis;
		NameInfoReq* _req;
		void procCallback(int status, const char *hostname, const char *service);
	};
}

#endif //UVCPPPRJ_UVGETNAMEINFO_H

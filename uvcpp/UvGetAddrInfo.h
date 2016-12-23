//
// Created by netmind on 16. 12. 23.
//

#ifndef UVCPPPRJ_UVGETADDRINFO_H
#define UVCPPPRJ_UVGETADDRINFO_H

#include <functional>
#include "AddrInfoReq.h"

namespace uvcpp {

	class UvGetAddrInfo {
		friend class UvContext;
	public:
		typedef std::function<void(int, struct addrinfo*)> Lis;

		UvGetAddrInfo();

		virtual ~UvGetAddrInfo();

		int init();
		int getAddrInfo(const char* node, const char* service, const struct addrinfo* hints, Lis lis);
		void close();
	private:
		Lis _lis;
		AddrInfoReq* _req;
		UvContext* _ctx;

		void procCallback(int status, struct addrinfo* info);
	};
}

#endif //UVCPPPRJ_UVGETADDRINFO_H

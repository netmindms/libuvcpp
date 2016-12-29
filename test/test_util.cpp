//
// Created by netmind on 16. 12. 23.
//

#include <gtest/gtest.h>
#include "../uvcpp/UvContext.h"
#include "../uvcpp/UvGetAddrInfo.h"
#include "tlog.h"
#include "../uvcpp/UvGetNameInfo.h"

using namespace uvcpp;

TEST(util, addrinfo) {
	UvContext::open();
	UvGetNameInfo uvname;
	UvGetAddrInfo uvaddr;
	sockaddr_in resolvedInaddr;

	uvaddr.init();
	uvaddr.getAddrInfo("www.google.co.kr", nullptr, nullptr, [&](int status, struct addrinfo* info) {
		ali("addr info result");
		struct addrinfo* addrs = info;
		for(;addrs;) {
			if(addrs->ai_addr) {
				if(addrs->ai_family == AF_INET) {
					sockaddr_in *inaddr = (sockaddr_in*)addrs->ai_addr;
					char dst[128];
					uv_ip4_name(inaddr, dst, 128);
					ali("ipv4 addrss=%s", dst);
					resolvedInaddr = *inaddr;
					// reverse
					uvname.init();
					uvname.getNameInfo((addrs->ai_addr), 0, [&](int status, const char* hostname, const char* service) {
						ASSERT_EQ(0, status);
						uvname.close();
					});
					break;
				}
			} else {
				break;
			}
			addrs = addrs->ai_next;
		}
		uvaddr.close();
	});

	UvContext::run();
	UvContext::close();
}
//
// Created by netmind on 16. 12. 23.
//

#ifndef UVCPPPRJ_ADDRINFOREQ_H
#define UVCPPPRJ_ADDRINFOREQ_H

#include <list>
#include <uv.h>
#include "UvContext.h"
namespace uvcpp {
	struct AddrInfoReq {
		std::list<AddrInfoReq>::iterator itr;
		uv_getaddrinfo_t req;
		void* obj;
		UvContext* ctx;
	};

	struct NameInfoReq {
		std::list<NameInfoReq>::iterator itr;
		uv_getnameinfo_t req;
		void* obj;
		UvContext* ctx;
	};
}
#endif //UVCPPPRJ_ADDRINFOREQ_H

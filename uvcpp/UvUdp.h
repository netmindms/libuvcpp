//
// Created by netmind on 16. 11. 28.
//

#ifndef UVCPPPRJ_UVUDP_H
#define UVCPPPRJ_UVUDP_H

#include <functional>
#include "UvReadBuffer.h"
#include "UvUdpHandle.h"

namespace uvcpp {

	class UvUdp {
	public:
		UvUdp();

		virtual ~UvUdp();
		int open(UvUdpHandle::ReadLis lis);
		int bind(const struct sockaddr *addr, unsigned int flags);
		void close();
		int readStart();
	private:
		UvUdpHandle* _ohandle;
	};

}

#endif //UVCPPPRJ_UVUDP_H

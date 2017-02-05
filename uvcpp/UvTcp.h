/*
 * UvTcp3.h
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#ifndef UVCPPPRJ_UVTCP_H_
#define UVCPPPRJ_UVTCP_H_

#include <memory>
#include <utility>
#include "ObjQue.h"


#include "ReadBuffer.h"
#include "UvWriteInfo.h"
#include "UvStream.h"

namespace uvcpp {
	class UvTcp : public UvStream {
	public:

		UvTcp();

		virtual ~UvTcp();

		int init();

		int initEx(unsigned int flags);

		int noDelay(int enable);

		int keepAlive(int enable, unsigned int delay);

		int simultaneousAccepts(int enable);

		int getSockName(struct sockaddr* name, int* namelen);

		int getPeerName(struct sockaddr* name, int* namelen);

		int connect(const char *ipaddr, uint16_t port, UvStream::CnnLis lis=nullptr);

		int connect(const sockaddr *addr, UvStream::CnnLis lis=nullptr);

		int bind(const struct sockaddr *addr, unsigned int flags);

		int bindAndListen(uint16_t port, const char *ipaddr, UvStream::ListenLis lis, int backlogs = 128);

	};
}
#endif /* UVCPPPRJ_UVTCP_H_ */

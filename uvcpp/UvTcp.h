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


#include "UvReadBuffer.h"
#include "UvWriteInfo.h"
#include "UvStream.h"

namespace uvcpp {
	class UvTcp : public UvStream {
	public:

		UvTcp();

		virtual ~UvTcp();

		int init();

		int connect(const char *ipaddr, uint16_t port, UvStream::CnnLis lis);

		int bind(const struct sockaddr *addr, unsigned int flags);

		int bindAndListen(uint16_t port, const char *ipaddr, UvStream::ListenLis lis, int backlogs = 128);

	};
}
#endif /* UVCPPPRJ_UVTCP_H_ */

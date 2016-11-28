/*
 * UvTcp3.h
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#ifndef UVTCP3_H_
#define UVTCP3_H_

#include <memory>
#include <utility>

#include "UvTcpHandle.h"

namespace uvcpp {
	class UvTcp {
	public:

		UvTcp();

		virtual ~UvTcp();

		int open(UvTcpHandle::CnnLis clis, UvTcpHandle::ReadLis rlis);

		int write(const char *buf, size_t len);

		int connect(const char *ipaddr, uint16_t port);

		int accept(UvTcp *newcnn);

		void close();

		int listen(uint16_t port, const char *ipaddr = nullptr, int backlogs = 10);

		size_t writeQueCnt();

		//std::pair<ssize_t, std::unique_ptr<char[]>> fetchReadBuf();
		void setOnCnnLis(UvTcpHandle::CnnLis lis);

		void setOnReadLis(UvTcpHandle::ReadLis lis);

	private:
		UvTcpHandle *_ohandle;

//	CnnLis _cnnLis;
//	ReadLis _readLis;
		void openChild(UvTcpHandle *newhandle);
	};
}
#endif /* UVTCP3_H_ */

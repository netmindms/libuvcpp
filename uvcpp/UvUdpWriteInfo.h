//
// Created by netmind on 16. 11. 28.
//

#ifndef UVCPPPRJ_UVUDPWRITEINFO_H
#define UVCPPPRJ_UVUDPWRITEINFO_H

#include <cstdint>
#include <cstdlib>
#include <uv.h>

namespace uvcpp {
	class UvUdpWriteInfo {
	public:
		UvUdpWriteInfo();

		virtual ~UvUdpWriteInfo();

		char *bufAlloc(size_t size);

		void reset();

		size_t fillBuf(const char *ptr, size_t len);

		size_t capacity;
		char *buf;
		size_t size;
		uv_udp_send_t req;
		uv_buf_t uvBuf;
	};
}

#endif //UVCPPPRJ_UVUDPWRITEINFO_H

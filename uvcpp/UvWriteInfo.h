/*
 * UvWriteInfo.h
 *
 *  Created on: Nov 23, 2016
 *      Author: netmind
 */

#ifndef UVCPPPRJ_UVWRITEINFO_H_
#define UVCPPPRJ_UVWRITEINFO_H_

#include <uv.h>

namespace uvcpp {
	class UvWriteInfo {
	public:
		UvWriteInfo();
		virtual ~UvWriteInfo();
		char *bufAlloc(size_t size);
		void reset();
		size_t fillBuf(const char *ptr, size_t len);
		size_t capacity;
		char *buf;
		size_t size;
		uv_write_t req;
		uv_buf_t uvBuf;

	};
}
#endif /* UVCPPPRJ_UVWRITEINFO_H_ */

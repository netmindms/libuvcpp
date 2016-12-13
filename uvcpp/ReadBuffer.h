/*
 * UvReadBuffer.h
 *
 *  Created on: Nov 26, 2016
 *      Author: netmind
 */

#ifndef UVCPPPRJ_READBUFFER_H_
#define UVCPPPRJ_READBUFFER_H_

#include <cstdlib>
#include <cstdint>
#include <utility>
#include <memory>

namespace uvcpp {
	class ReadBuffer {
	public:
		ReadBuffer();

		virtual ~ReadBuffer();

		int fillBuffer(const char *ptr, size_t len);

		std::pair<size_t, char *> allocBuffer(size_t size);

		char *buffer;
		size_t capacity;
		size_t size;
	};

	typedef std::unique_ptr<ReadBuffer> upReadBuffer;
}
#endif /* UVCPPPRJ_READBUFFER_H_ */

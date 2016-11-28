/*
 * UvReadBuffer.h
 *
 *  Created on: Nov 26, 2016
 *      Author: netmind
 */

#ifndef UVREADBUFFER_H_
#define UVREADBUFFER_H_

#include <cstdlib>
#include <cstdint>
#include <utility>
#include <memory>

namespace uvcpp {
	class UvReadBuffer {
	public:
		UvReadBuffer();

		virtual ~UvReadBuffer();

		int fillBuffer(const char *ptr, size_t len);

		std::pair<size_t, char *> allocBuffer(size_t size);

		char *buffer;
		size_t capacity;
		size_t size;
	};

	typedef std::unique_ptr<UvReadBuffer> upUvReadBuffer;
}
#endif /* UVREADBUFFER_H_ */

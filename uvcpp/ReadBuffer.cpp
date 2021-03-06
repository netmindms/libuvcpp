/*
 * UvReadBuffer.cpp
 *
 *  Created on: Nov 26, 2016
 *      Author: netmind
 */

#include <cstring>
#include "ReadBuffer.h"

namespace uvcpp {
	ReadBuffer::ReadBuffer() {
		size = 0;
		capacity = 0;
		buffer = nullptr;
	}

	ReadBuffer::~ReadBuffer() {
		if (buffer) {
			delete[] buffer;
		}
	}

	int ReadBuffer::fillBuffer(const char *ptr, size_t len) {
		if (capacity < len) {
			delete[] buffer;
			buffer = new char[len];
			if (buffer) {
				capacity = len;
			}
		}

		if (capacity > 0) {
			memcpy(buffer, ptr, len);
			size = len;
			return len;
		} else {
			return -1;
		}
	}

	std::pair<size_t, char *> ReadBuffer::allocBuffer(size_t size) {
		if (size > capacity) {
			auto tbuf = new char[size];
			if (tbuf) {
				delete[] buffer;
				capacity = size;
				buffer = tbuf;
			} else {
				return {0, nullptr};
			}
		}
		return {capacity, buffer};
	}

}
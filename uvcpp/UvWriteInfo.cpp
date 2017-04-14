/*
 * UvWriteInfo.cpp
 *
 *  Created on: Nov 23, 2016
 *      Author: netmind
 */

#include <cstdlib>
#include <cstring>
#include "UvWriteInfo.h"

namespace uvcpp {
	UvWriteInfo::UvWriteInfo() {
		capacity = 0;
		size = 0;
		buf = nullptr;

	}

	UvWriteInfo::~UvWriteInfo() {
		reset();
	}

	char *UvWriteInfo::bufAlloc(size_t len) {
		if (capacity < len) {
			delete[] buf;
			buf = new char[len];
			if (buf) {
				capacity = len;
				size = 0;
			}
			return buf;
		} else {
			return buf;
		}
	}

	void UvWriteInfo::reset() {
		if (buf) {
			delete[] buf;
			buf = nullptr;
			capacity = 0;
			size = 0;
		}
	}

	size_t UvWriteInfo::fillBuf(const char *ptr, size_t len) {
		uvBuf.base = bufAlloc(len);
		if (uvBuf.base) {
			memcpy(uvBuf.base, ptr, len);
			uvBuf.len = len;
		} else {
			uvBuf.len = 0;
		}
		return uvBuf.len;
	}
}
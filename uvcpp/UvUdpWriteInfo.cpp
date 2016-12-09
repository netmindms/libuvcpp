//
// Created by netmind on 16. 11. 28.
//

#include <cstring>
#include "UvUdpWriteInfo.h"

namespace uvcpp {
	UvUdpWriteInfo::UvUdpWriteInfo() {
		capacity = 0;
		size = 0;
		buf = nullptr;
	}

	UvUdpWriteInfo::~UvUdpWriteInfo() {
		reset();
	}

	char *UvUdpWriteInfo::bufAlloc(size_t len) {
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

	void UvUdpWriteInfo::reset() {
		if (buf) {
			delete[] buf;
			buf = nullptr;
			capacity = 0;
			size = 0;
		}
	}

	size_t UvUdpWriteInfo::fillBuf(const char *ptr, size_t len) {
		uvBuf.base = bufAlloc(len);
		if (uvBuf.base) {
			memcpy(uvBuf.base, ptr, len);
			uvBuf.len = len;
		} else {
			uvBuf.base = nullptr;
			uvBuf.len = 0;
		}
		return uvBuf.len;
	}

}
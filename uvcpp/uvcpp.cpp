//
// Created by netmind on 16. 12. 5.
//


#include "uvcpplog.h"
#include "uvcpp.h"

namespace uvcpp {
	void SetLogLevel(int level) {
		NMDU_SET_LOG_LEVEL(level);
	}

	void SetFileLogLevel(int level) {
		NMDU_SET_LOG_LEVEL_FILE(level);
	}

	void SetFileLog(const char* path) {
		NMDU_SET_LOG_FILE(path);
	}
}

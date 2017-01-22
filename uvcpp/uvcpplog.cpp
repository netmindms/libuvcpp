/*
 * alog.cpp
 *
 *  Created on: Apr 13, 2015
 *      Author: netmind
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <chrono>
#include <sys/time.h>
#include <pthread.h>
#include <thread>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <fileapi.h>

#include "uvcpplog.h"
#include "uvcpp_format.h"

using namespace std;

namespace uvcpp {

LogInst* _gDefLogInst=nullptr;

struct _NMDLOG_MOD_ {
public:
	_NMDLOG_MOD_() {
		_gDefLogInst = new LogInst;
	}
	virtual ~_NMDLOG_MOD_() {
		if(_gDefLogInst) delete _gDefLogInst;
	}
};
static _NMDLOG_MOD_ _LOCAL_MODULE_INST;

LogInst* getDefLogInst() {
	return _gDefLogInst;
}

string GetLogTimeNow() {
	struct timeval tv;
	gettimeofday(&tv, (struct timezone*)nullptr);
	struct tm ttm;
	localtime_r(&tv.tv_sec, &ttm);
	char buf[30];
	::sprintf(buf, "%02d:%02d:%02d.%03d", ttm.tm_hour, ttm.tm_min, ttm.tm_sec, (int)(tv.tv_usec/1000));
	return string(buf);
}

#if 0
void prtlog(int level, const char* format, fmt::ArgList args) {
	if(level<=gPrintLogLevel || level <=gFileLogLevel) {
		gNmdLogInst.lock();
		fmt::MemoryWriter w;
		//	w.write(format, args);
		fmt::printf(w, (fmt::CStringRef)format, args);
//		w << '\n';
		if(level<=gPrintLogLevel) std::fwrite(w.data(), 1, w.size(), stdout);
		if(level<=gFileLogLevel) gNmdLogInst.fileWrite(w.data(), w.size());
		gNmdLogInst.unlock();
	}

}
void setOutLogLevel(int level) {
	gPrintLogLevel = level;
}

int setFileLog(int level, const char *path) {
	gNmdLogInst.lock();
	gFileLogLevel = level;
	auto ret = gNmdLogInst.openLogFile(path);
	gNmdLogInst.unlock();
	return ret;
}

#endif



LogInst::LogInst() {
	mLevel = LOG_WARN;
	mFileLevel = -1;
	mFd = -1;
	mMaxFileSize = 10*1024*1024; // default 10MB
}

LogInst::~LogInst() {
	if(mFd>=0) {
		close(mFd);
	}
}

void LogInst::lock() {
	mMutex.lock();
}

void LogInst::unlock() {
	mMutex.unlock();
}

int LogInst::setLogFile(const char* path, size_t max) {
	if(mFd>=0) {
		close(mFd);
		mFd = -1;
	}
	struct stat s;
	auto ret = stat(path, &s);
	if(!ret) {
//		mode_t mode = (S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | S_IROTH);
		int flag = O_APPEND|O_WRONLY;
		mFd = open(path, flag);
//		mFd = open(path, flag, mode);
	} else {
		mode_t mode = (S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | S_IROTH);
		int flag;
		flag = O_CREAT|O_WRONLY;
		mFd = open(path, flag, mode);
	}
	if(mFd>=0) {
#ifdef __linux
		char* fullpath = realpath(path, nullptr);
#elif _WIN32
		char fullpath[512];
		GetFullPathName(path, 512, fullpath, nullptr);
#endif
		if(fullpath) {
			mMaxFileSize = max;
			mFilePath = fullpath;
			free(fullpath);
		}
	}
	return mFd>=0?0:-1;
}

void LogInst::checkSize() {
	if(mFd>=0) {
		auto fs = lseek(mFd, 0, SEEK_CUR);
		if(fs >= mMaxFileSize) {
			close(mFd); mFd = -1;
			backupFile();
			setLogFile(mFilePath.data(), mMaxFileSize);
		}
	}
}

//int LogInst::writeFile(const char *ptr, size_t len) {
//	auto wret = write(mFd, ptr, len);
//	checkSize();
//	return wret;
//}

int LogInst::backupFile() {
	if(mBackupFolder.empty()) {
		if(mFilePath != "") {
			char* tmp = strdup(mFilePath.data());
			assert(tmp);
			char* base = dirname(tmp);
			mBackupFolder = string(base)+"/.backup";
			free(tmp);
		} else {
			return -1;
//			char* tmp = get_current_dir_name();
//			char* base = basename(tmp);
//			mBackupFolder = base;
//			free(tmp);
		}
	}

	if(mFilePath != "") {
		struct stat s;
		auto ret = stat(mBackupFolder.data(), &s);
		if(ret) {
#ifdef __linux
			ret = mkdir(mBackupFolder.data(), 0775);
#elif _WIN32
			ret = mkdir(mBackupFolder.data());
#endif
			if(ret) return -1;
		}

		char *tmp = strdup(mFilePath.data());
		char *fn = basename(tmp);
		string path = mBackupFolder +"/" + fn;
		remove(path.data());
		free(tmp);
		return rename(mFilePath.data(), path.data());
	} else {
		return -1;
	}
	return 0;
}

//
//void NmduMemPrintf(uvcpp::fmt::MemoryWriter& w, uvcpp::fmt::StringRef sref, uvcpp::fmt::ArgList args) {
//	fmt::printf(w, sref, args);
//}
//FMT_VARIADIC(void, NmduMemPrintf, uvcpp::fmt::MemoryWriter&, uvcpp::fmt::StringRef)
} // namespace uvcpp


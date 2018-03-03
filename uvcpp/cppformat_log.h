/*
 * nmdlog.h
 *
 *  Created on: Apr 13, 2015
 *      Author: netmind
 */

#ifndef UVCPPPRJ_CPPFORMATLOG_H_
#define UVCPPPRJ_CPPFORMATLOG_H_

#include <unistd.h>
#include <string.h>
#include <memory>
#include <string>
#include <mutex>

namespace uvcpp {

#define UVCLOG_NONE 0
#define UVCLOG_ERROR 1
#define UVCLOG_WARN 2
#define UVCLOG_NOTICE 3
#define UVCLOG_INFO 4
#define UVCLOG_DEBUG 5
#define UVCLOG_VERBOSE 6


#if defined(UVCLOG_LEVEL_NONE)
#define UVCLOG_LEVEL UVCLOG_NONE
#elif defined(UVCLOG_LEVEL_ERROR)
#define UVCLOG_LEVEL UVCLOG_ERROR
#elif defined(LOG_LEVEL_WARN)
#define UVCLOG_LEVEL UVCLOG_WARN
#elif defined(UVCLOG_LEVEL_NOTICE)
#define UVCLOG_LEVEL UVCLOG_NOTICE
#elif defined(UVCLOG_LEVEL_INFO)
#define UVCLOG_LEVEL UVCLOG_INFO
#elif defined(UVCLOG_LEVEL_DEBUG)
#define UVCLOG_LEVEL UVCLOG_DEBUG
#elif defined(UVCLOG_LEVEL_VERBOSE)
#define UVCLOG_LEVEL UVCLOG_VERBOSE
#endif


#ifndef UVCLOG_LEVEL
#define UVCLOG_LEVEL UVCLOG_VERBOSE
#endif

#define C_UVCLOG_ERROR "E"
#define C_UVCLOG_WARN "W"
#define C_UVCLOG_NOTICE "N"
#define C_UVCLOG_INFO "I"
#define C_UVCLOG_DEBUG "D"
#define C_UVCLOG_VERBOSE "V"

#define UVC_DEFAULT_LOG_INST (uvcpp::_gDefLogInst)

	class LogInst {
	public:
		LogInst();

		virtual ~LogInst();

		void lock();

		void unlock();

		void level(int level) {
			mLevel = level;
		}

		int level() {
			return mLevel;
		}

		void levelFile(int level) {
			mFileLevel = level;
		}

		int levelFile() {
			return mFileLevel;
		}

		int setLogFile(const char *path, size_t max = 10 * 1024 * 1024);

		int getFileFd() {
			return mFd;
		}

		inline void writeFile(const char *ptr, size_t len) {
			lock();
			write(mFd, ptr, len);
			checkSize();
			unlock();
		};

		inline void writeLog(FILE *scr_st, const char *ptr, size_t len) {
			lock();
			std::fwrite(ptr, 1, len, scr_st);
			unlock();
		};
	private:
		std::mutex mMutex;
		int mLevel;
		int mFileLevel;
		int mFd;
		std::string mFilePath;
		std::string mBackupFolder;
		ssize_t mMaxFileSize;

		void checkSize();

		int backupFile();
	};


	extern LogInst *_gDefLogInst;

	std::string GetLogTimeNow();

	void GetLogTimeNow(char *buf);

	LogInst *getDefLogInst();

	template<size_t N>
	constexpr size_t comp_length(char const (&)[N]) {
		return N - 1;
	}

	constexpr int comp_checkdot(const char *str, const int p, const int n) {
		return (p <= 0) ? n : str[p] == '.' ? p : comp_checkdot(str, p - 1, n + 1);
	}

	constexpr int comp_checkslash(const char *str, const int p, const int n) {
#ifdef __WIN32
		return p<0?0: str[p]=='\\'?p+1:comp_checkslash(str, p-1, n+1);
#else
		return p < 0 ? 0 : str[p] == '/' ? p + 1 : comp_checkslash(str, p - 1, n + 1);
#endif
	}


#ifndef UVCLOCAL_LOG_INST
#define UVCLOCAL_LOG_INST UVC_DEFAULT_LOG_INST
#endif

#define UVC_SET_LOG_LEVEL(L) UVCLOCAL_LOG_INST->level(L)
#define UVC_SET_LOG_LEVEL_FILE(L) UVCLOCAL_LOG_INST->levelFile(L)
#define UVC_SET_LOG_FILE(F) UVCLOCAL_LOG_INST->setLogFile(F)
#define UVCPPLOGPUT(LL, OUT, FMTSTR, ...) do {\
    if(UVCLOG_LEVEL>=LL) { /*compile time log level check*/ \
        if(UVCLOCAL_LOG_INST->level()>=LL || UVCLOCAL_LOG_INST->levelFile()>=LL ) { \
                    constexpr int s=uvcpp::comp_checkslash(__FILE__, uvcpp::comp_length(__FILE__), 0); \
                    constexpr int e=uvcpp::comp_checkdot(__FILE__, uvcpp::comp_length(__FILE__), 0);\
                    char logbuf[2048];\
                    /*char sn[NAME_MAX+1];*/\
                    char sn[e-s+1];\
                    memcpy(sn, __FILE__+s, e-s);sn[e-s]=0; \
                    int loglen = snprintf(logbuf, sizeof(logbuf)-1, "%s <" C_ ##LL ">[%s:%d] " FMTSTR "\n", \
                        uvcpp::GetLogTimeNow().c_str(), sn /*__FILE__+ nmdu::comp_checkslash(__FILE__, nmdu::comp_length(__FILE__), 0)*/, \
                        __LINE__, ## __VA_ARGS__);\
            if(UVCLOCAL_LOG_INST->level()>=LL) {\
                UVCLOCAL_LOG_INST->writeLog(OUT, logbuf, loglen); \
            }\
            if(UVCLOCAL_LOG_INST->levelFile()>=LL) {\
                UVCLOCAL_LOG_INST->writeFile(logbuf, loglen);\
            }\
        } \
    }\
    } while(0)

#define ule(FMTSTR, ...) UVCPPLOGPUT(UVCLOG_ERROR, stderr, FMTSTR, ## __VA_ARGS__)
#define ulw(FMTSTR, ...) UVCPPLOGPUT(UVCLOG_WARN, stderr, FMTSTR, ## __VA_ARGS__)
#define uln(FMTSTR, ...) UVCPPLOGPUT(UVCLOG_NOTICE, stdout, FMTSTR, ## __VA_ARGS__)
#define uli(FMTSTR, ...) UVCPPLOGPUT(UVCLOG_INFO, stdout, FMTSTR, ## __VA_ARGS__)
#define uld(FMTSTR, ...) UVCPPLOGPUT(UVCLOG_DEBUG, stdout, FMTSTR, ## __VA_ARGS__)
#define ulv(FMTSTR, ...) UVCPPLOGPUT(UVCLOG_VERBOSE, stdout, FMTSTR, ## __VA_ARGS__)

}


#endif /* UVCPPPRJ_CPPFORMATLOG_H_ */

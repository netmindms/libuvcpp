/*
 * nmdlog.h
 *
 *  Created on: Apr 13, 2015
 *      Author: netmind
 */

#ifndef UVCPPPRJ_CPPFORMATLOG_H_
#define UVCPPPRJ_CPPFORMATLOG_H_


/* add follwing in compile option
 *  NMDU_FILE_NAME="\"$(<F)\""
 *
 */


/* Insert below lines in your app log file
 * In here, gYourLogInst is an instance pointer of uvcpp::LogInst

**** Sample Application log header(your_log.h) *****
#ifndef LOCAL_LOG_INST
#define LOCAL_LOG_INST your_log_instance_name // LogInst instance pointer
//#define LOCAL_LOG_INST DEFAULT_LOG_INST
#endif
#include <nmdutil/nmdlog.h>
EXTERN_LOG_INSTANCE(your_log_instance_name); //==> extern uvcpp::LogInst* your_log_instance_name;


**** Sample Application log source(your_log.cpp) *****
#include "your_log.h"
MAKE_LOG_INSTANCE(your_log_instance_name);

*/

#include <unistd.h>
#include <string.h>
#include <memory>
#include <string>
#include <mutex>
#include "uvcpp_format.h"

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
	int setLogFile(const char* path, size_t max=10*1024*1024);
	int getFileFd() {
		return mFd;
	}
	inline void writeFile(const char* ptr, size_t len) {
		lock();
		write(mFd, ptr, len);
		checkSize();
		unlock();
	};
	inline void writeLog(FILE* scr_st, const char* ptr, size_t len) {
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



extern LogInst* _gDefLogInst;
std::string GetLogTimeNow();
LogInst* getDefLogInst();

template< size_t N >
constexpr size_t comp_length( char const (&)[N] )
{
  return N-1;
}
constexpr int  comp_checkdot(const char* str, const int p, const int n) {
	return (p<=0)?n: str[p]=='.'?p:comp_checkdot(str, p-1, n+1);
}
constexpr int comp_checkslash(const char* str, const int p, const int n) {
#ifdef __WIN32
	return p<0?0: str[p]=='\\'?p+1:comp_checkslash(str, p-1, n+1);
#else
	return p<0?0: str[p]=='/'?p+1:comp_checkslash(str, p-1, n+1);
#endif
}

//void NmduMemPrintf(uvcppfmt::MemoryWriter& w, uvcppfmt::StringRef sref, uvcppfmt::ArgList args);
inline void NmduMemPrintf(uvcppfmt::MemoryWriter& w, const char* sref, uvcppfmt::ArgList args) {
	uvcppfmt::printf<char>(w, sref, args);
}
FMT_VARIADIC(void, NmduMemPrintf, uvcppfmt::MemoryWriter&, const char*)

inline void NmduMemPrintf2(uvcppfmt::MemoryWriter& w, const char level, const char* fname, int fnamelen, int lineno, const char* sref, uvcppfmt::ArgList args) {
//	fmt::printf(w, "%s [%c:%s:%d] ", uvcpp::GetLogTimeNow(), level)
	w<<uvcpp::GetLogTimeNow()<<" <"<< level << ">[";
//	fmt::printf(w, "%s [%c:", uvcpp::GetLogTimeNow(), level);
	char tmpf[fnamelen+1]; memcpy(tmpf, fname, fnamelen);tmpf[fnamelen]=0;
	w<<tmpf;
	w<<":"<<lineno<<"] ";
	uvcppfmt::printf<char>(w, sref, args);
}
FMT_VARIADIC(void, NmduMemPrintf2, uvcppfmt::MemoryWriter&, const char, const char*, int, int, const char*)

#ifndef UVCLOCAL_LOG_INST
#define UVCLOCAL_LOG_INST UVC_DEFAULT_LOG_INST
#endif

#ifdef UVCLOCAL_LOG_INST
#define UVC_SET_LOG_LEVEL(L) UVCLOCAL_LOG_INST->level(L)
#define UVC_SET_LOG_LEVEL_FILE(L) UVCLOCAL_LOG_INST->levelFile(L)
#define UVC_SET_LOG_FILE(F) UVCLOCAL_LOG_INST->setLogFile(F)
#if 0
#define UVCPPLOGPUT(LL, OUT, FMTSTR, ...) do {\
	if(LOG_LEVEL>=LL) {\
		uvcppfmt::MemoryWriter w;\
		/*LOCAL_LOG_INST->lock();\ */ \
		if(LOCAL_LOG_INST->level()>=LL) {\
			uvcpp::NmduMemPrintf(w, "%s [" C_ ##LL ":%s:%d] " FMTSTR "\n", \
					uvcpp::GetLogTimeNow(), NMDU_FILE_NAME, __LINE__, ## __VA_ARGS__);\
			/*std::fwrite(w.data(), 1, w.size(), OUT);\ */ \
			LOCAL_LOG_INST->writeLog(OUT, w.data(), w.size()); \
		}\
		if(LOCAL_LOG_INST->levelFile()>=LL) {\
			LOCAL_LOG_INST->writeFile(w.data(), w.size());\
		}\
		/*LOCAL_LOG_INST->unlock();\*/ \
	}\
	}while(0)
#else
#define UVCPPLOGPUT(LL, OUT, FMTSTR, ...) do {\
	if(UVCLOG_LEVEL>=LL) { /*compile time log level check*/ \
		if(UVCLOCAL_LOG_INST->level()>=LL || UVCLOCAL_LOG_INST->levelFile()>=LL ) { \
			uvcppfmt::MemoryWriter w;\
					/*nmdu::NmduMemPrintf2(w, C_ ##LL [0], NMDU_FILE_NAME,\
							nmdu::comp_checkdot(NMDU_FILE_NAME, nmdu::comp_length(NMDU_FILE_NAME), 0),\
							__LINE__, FMTSTR "\n", \
							## __VA_ARGS__);*/\
					int s=uvcpp::comp_checkslash(__FILE__, uvcpp::comp_length(__FILE__), 0); \
					int e=uvcpp::comp_checkdot(__FILE__, uvcpp::comp_length(__FILE__), 0);\
					char sn[512]; memcpy(sn, __FILE__+s, e-s);sn[e-s]=0; \
					uvcpp::NmduMemPrintf(w, "%s <" C_ ##LL ">[%s:%d] " FMTSTR "\n", \
						uvcpp::GetLogTimeNow(), sn /*__FILE__+ nmdu::comp_checkslash(__FILE__, nmdu::comp_length(__FILE__), 0)*/, \
						__LINE__, ## __VA_ARGS__);\
			if(UVCLOCAL_LOG_INST->level()>=LL) {\
				UVCLOCAL_LOG_INST->writeLog(OUT, w.data(), w.size()); \
			}\
			if(UVCLOCAL_LOG_INST->levelFile()>=LL) {\
				UVCLOCAL_LOG_INST->writeFile(w.data(), w.size());\
			}\
		} \
	}\
	}while(0)
#endif
#define ule(FMTSTR, ...) UVCPPLOGPUT(UVCLOG_ERROR, stderr, FMTSTR, ## __VA_ARGS__)
#define ulw(FMTSTR, ...) UVCPPLOGPUT(UVCLOG_WARN, stderr, FMTSTR, ## __VA_ARGS__)
#define uln(FMTSTR, ...) UVCPPLOGPUT(UVCLOG_NOTICE, stdout, FMTSTR, ## __VA_ARGS__)
#define uli(FMTSTR, ...) UVCPPLOGPUT(UVCLOG_INFO, stdout, FMTSTR, ## __VA_ARGS__)
#define uld(FMTSTR, ...) UVCPPLOGPUT(UVCLOG_DEBUG, stdout, FMTSTR, ## __VA_ARGS__)
#define ulv(FMTSTR, ...) UVCPPLOGPUT(UVCLOG_VERBOSE, stdout, FMTSTR, ## __VA_ARGS__)
#else
#define ale(FMTSTR, ...) { if(UVCLOG_LEVEL>=UVCLOG_ERROR) { if(UVCLOCAL_LOG_INST->level()>=UVCLOG_ERROR) { UVCLOCAL_LOG_INST->lock();uvcppfmt::fprintf(stderr, "%s [E:%s:%d] " FMTSTR "\n", uvcpp::GetLogTimeNow(), NMDU_FILE_NAME, __LINE__, ## __VA_ARGS__);LOCAL_LOG_INST->unlock(); }; if(LOCAL_LOG_INST->levelFile()>=LOG_ERROR) {LOCAL_LOG_INST->lock(); std::string s = uvcppfmt::sprintf("%s [E:%s:%d] " FMTSTR "\n", uvcpp::GetLogTimeNow(), NMDU_FILE_NAME, __LINE__, ## __VA_ARGS__);LOCAL_LOG_INST->writeFile(s.data(), s.size());LOCAL_LOG_INST->unlock();} } }
#define alw(FMTSTR, ...) { if(UVCLOG_LEVEL>=UVCLOG_WARN) { if(UVCLOCAL_LOG_INST->level()>=UVCLOG_WARN) { UVCLOCAL_LOG_INST->lock();uvcppfmt::fprintf(stderr, "%s [W:%s:%d] " FMTSTR "\n", uvcpp::GetLogTimeNow(), NMDU_FILE_NAME, __LINE__, ## __VA_ARGS__);LOCAL_LOG_INST->unlock(); }; if(LOCAL_LOG_INST->levelFile()>=LOG_WARN) {LOCAL_LOG_INST->lock(); std::string s = uvcppfmt::sprintf("%s [W:%s:%d] " FMTSTR "\n", uvcpp::GetLogTimeNow(), NMDU_FILE_NAME, __LINE__, ## __VA_ARGS__);LOCAL_LOG_INST->writeFile(s.data(), s.size());LOCAL_LOG_INST->unlock();} } }
#define aln(FMTSTR, ...) { if(UVCLOG_LEVEL>=UVCLOG_NOTICE) { if(UVCLOCAL_LOG_INST->level()>=UVCLOG_NOTICE) { UVCLOCAL_LOG_INST->lock();uvcppfmt::printf("%s [N:%s:%d] " FMTSTR "\n", uvcpp::GetLogTimeNow(), NMDU_FILE_NAME, __LINE__, ## __VA_ARGS__);LOCAL_LOG_INST->unlock(); }; if(LOCAL_LOG_INST->levelFile()>=LOG_NOTICE) {LOCAL_LOG_INST->lock(); std::string s = uvcppfmt::sprintf("%s [N:%s:%d] " FMTSTR "\n", uvcpp::GetLogTimeNow(), NMDU_FILE_NAME, __LINE__, ## __VA_ARGS__);LOCAL_LOG_INST->writeFile(s.data(), s.size());LOCAL_LOG_INST->unlock();} } }
#define ali(FMTSTR, ...) { if(UVCLOG_LEVEL>=UVCLOG_INFO) { if(UVCLOCAL_LOG_INST->level()>=UVCLOG_INFO) { UVCLOCAL_LOG_INST->lock();uvcppfmt::printf("%s [I:%s:%d] " FMTSTR "\n", uvcpp::GetLogTimeNow(), NMDU_FILE_NAME, __LINE__, ## __VA_ARGS__);LOCAL_LOG_INST->unlock(); }; if(LOCAL_LOG_INST->levelFile()>=LOG_INFO) {LOCAL_LOG_INST->lock(); std::string s = uvcppfmt::sprintf("%s [I:%s:%d] " FMTSTR "\n", uvcpp::GetLogTimeNow(), NMDU_FILE_NAME, __LINE__, ## __VA_ARGS__);LOCAL_LOG_INST->writeFile(s.data(), s.size());LOCAL_LOG_INST->unlock();} } }
#define ald(FMTSTR, ...) { if(UVCLOG_LEVEL>=UVCLOG_DEBUG) { if(UVCLOCAL_LOG_INST->level()>=UVCLOG_DEBUG) { UVCLOCAL_LOG_INST->lock();uvcppfmt::printf("%s [D:%s:%d] " FMTSTR "\n", uvcpp::GetLogTimeNow(), NMDU_FILE_NAME, __LINE__, ## __VA_ARGS__);LOCAL_LOG_INST->unlock(); }; if(LOCAL_LOG_INST->levelFile()>=LOG_DEBUG) {LOCAL_LOG_INST->lock(); std::string s = uvcppfmt::sprintf("%s [D:%s:%d] " FMTSTR "\n", uvcpp::GetLogTimeNow(), NMDU_FILE_NAME, __LINE__, ## __VA_ARGS__);LOCAL_LOG_INST->writeFile(s.data(), s.size());LOCAL_LOG_INST->unlock();} } }
#define alv(FMTSTR, ...) { if(UVCLOG_LEVEL>=UVCLOG_VERBOSE) { if(UVCLOCAL_LOG_INST->level()>=UVCLOG_VERBOSE) { UVCLOCAL_LOG_INST->lock();uvcppfmt::printf("%s [V:%s:%d] " FMTSTR "\n", uvcpp::GetLogTimeNow(), NMDU_FILE_NAME, __LINE__, ## __VA_ARGS__);LOCAL_LOG_INST->unlock(); }; if(LOCAL_LOG_INST->levelFile()>=LOG_VERBOSE) {LOCAL_LOG_INST->lock(); std::string s = uvcppfmt::sprintf("%s [V:%s:%d] " FMTSTR "\n", uvcpp::GetLogTimeNow(), NMDU_FILE_NAME, __LINE__, ## __VA_ARGS__);LOCAL_LOG_INST->writeFile(s.data(), s.size());LOCAL_LOG_INST->unlock();} } }
#endif
}

#endif /* UVCPPPRJ_CPPFORMATLOG_H_ */

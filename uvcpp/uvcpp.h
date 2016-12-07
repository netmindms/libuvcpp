//
// Created by netmind on 16. 12. 5.
//

#ifndef UVCPPPRJ_UVCPP_H
#define UVCPPPRJ_UVCPP_H

#include "UvContext.h"
#include "UvEvent.h"
#include "UvAsync.h"
#include "UvTcp.h"
#include "UvUdp.h"
#include "UvCheck.h"
#include "UvIdle.h"
#include "UvPoll.h"
#include "UvTimer.h"
#include "Ipc.h"
#include "MsgTask.h"
#include "UvPrepare.h"
#include "ObjQue.h"

#ifdef __linux
#include "UvFdTimer.h"
#endif

namespace uvcpp {
	void SetLogLevel(int level);
	void SetFileLogLevel(int level);
	void SetFileLog(const char* path);
}

#endif //UVCPPPRJ_UVCPP_H


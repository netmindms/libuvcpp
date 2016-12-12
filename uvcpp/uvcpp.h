//
// Created by netmind on 16. 12. 5.
//

#ifndef UVCPPPRJ_UVCPP_H
#define UVCPPPRJ_UVCPP_H

#include "UvContext.h"
#include "UvAsync.h"
#include "UvStream.h"
#include "UvTcp.h"
#include "UvUdp.h"
#include "UvCheck.h"
#include "UvIdle.h"
#include "UvPoll.h"
#include "UvTimer.h"
#include "UvPrepare.h"
#include "UvPipe.h"
#include "UvTty.h"

namespace uvcpp {
	void SetLogLevel(int level);
	void SetFileLogLevel(int level);
	void SetFileLog(const char* path);
}

#endif //UVCPPPRJ_UVCPP_H


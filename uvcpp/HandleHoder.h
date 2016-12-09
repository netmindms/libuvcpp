//
// Created by netmind on 16. 12. 9.
//

#ifndef UVCPPPRJ_HANDLEHODER_H
#define UVCPPPRJ_HANDLEHODER_H

#include <uv.h>
#include <functional>

#include "ObjQue.h"
#include "UvWriteInfo.h"
#include "UvUdpWriteInfo.h"


namespace uvcpp {
	class UvContext;
	class UvHandle;
	struct HandleHolder {
		HandleHolder *prev;
		HandleHolder *next;
		uv_any_handle rawh;
		UvHandle* uvh;
		UvContext* ctx;
		ObjQue<UvWriteInfo> _writeReqQue;
		std::function<void()> closeLis;
		ObjQue<UvUdpWriteInfo> _sendReqQue;
		std::function<void(int status)> writeLis;
		std::function<void(int status)> sendLis;
	};
}
#endif //UVCPPPRJ_HANDLEHODER_H

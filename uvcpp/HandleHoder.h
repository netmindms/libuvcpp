//
// Created by netmind on 16. 12. 9.
//

#ifndef UVCPPPRJ_HANDLEHODER_H
#define UVCPPPRJ_HANDLEHODER_H

#include <string>
#include <uv.h>
#include <functional>

#include "ObjQue.h"
#include "UvWriteInfo.h"
#include "UvUdpWriteInfo.h"
#include "ReadBuffer.h"


namespace uvcpp {
	class UvContext;
	class UvHandle;
	struct HandleHolder {
		HandleHolder *prev;
		HandleHolder *next;
		uv_any_handle rawh;
		UvHandle* uvh;
		UvContext* ctx;
		ObjQue<ReadBuffer> readBufQue;
		ObjQue<UvWriteInfo> writeReqQue;
		ObjQue<UvUdpWriteInfo> sendReqQue;
		std::function<void()> closeLis;
		uv_connect_t cnnReq;

		std::string handleName; // for debugging
	};
}
#endif //UVCPPPRJ_HANDLEHODER_H

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
#include "UvReadBuffer.h"


namespace uvcpp {
	class UvContext;
	class UvHandle;
	struct HandleHolder {
		HandleHolder *prev;
		HandleHolder *next;
		uv_any_handle rawh;
		UvHandle* uvh;
		UvContext* ctx;
		ObjQue<UvReadBuffer> readBufQue;
		ObjQue<UvWriteInfo> writeReqQue;
		ObjQue<UvUdpWriteInfo> sendReqQue;
//		std::function<void(int status)> writeLis;
//		std::function<void(int status)> sendLis;
//		std::function<void(upUvReadBuffer)> readLis;
//		std::function<void(int)> cnnLis;
		std::function<void()> closeLis;
//		std::function<void()> listenLis;
		uv_connect_t cnnReq;

		std::string handleName;
	};
}
#endif //UVCPPPRJ_HANDLEHODER_H

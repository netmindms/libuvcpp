//
// Created by netmind on 16. 12. 9.
//

#ifndef UVCPPPRJ_HANDLEHODER_H
#define UVCPPPRJ_HANDLEHODER_H

#include <uv.h>
#include <functional>
#include "UvWriteInfo.h"
#include "ObjQue.h"


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
		std::function<void(int status)> writeLis;
	};
}
#endif //UVCPPPRJ_HANDLEHODER_H

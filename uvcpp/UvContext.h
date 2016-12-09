/*
 * EdContext.h
 *
 *  Created on: Nov 22, 2016
 *      Author: netmind
 */

#ifndef UVCPPPRJ_UVCONTEXT_H_
#define UVCPPPRJ_UVCONTEXT_H_

#include <uv.h>
#include "UvHandle.h"
#include "HandleHoder.h"

namespace uvcpp {

	class UvHandle;

	class UvContext {
		friend class UvHandle;
		friend class UvHandleOwner;
	public:
		UvContext();

		virtual ~UvContext();

		void open(uv_loop_t *loop=nullptr);

		void openWithDefaultLoop();

		void close();

		static UvContext *getContext();

		uv_loop_t *getLoop();

		UvHandle* createHandle(void* user_data, const char* typestr);
		HandleHolder* createHandleHolder();

		int handleCount();

		int run();
		static void handle_close_cb(uv_handle_t *phandle);
		void closeHandle(HandleHolder* holder);
		static void write_cb(uv_write_t *req, int status);
	private:
		bool _createLoop;
		uv_loop_t *_loop;
		uv_prepare_t _prepareHandle;
		HandleHolder *_handleLast;
		int _pendingHandleCnt;
		uint32_t _handleIdSeed;

		void dumpHandle(UvHandle *plast);
		void deleteHandle(HandleHolder *pHandle);
		void initHandle(UvHandle *handle, void *user_data);

	};


}
#endif /* UVCPPPRJ_UVCONTEXT_H_ */

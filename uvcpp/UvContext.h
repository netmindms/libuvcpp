/*
 * EdContext.h
 *
 *  Created on: Nov 22, 2016
 *      Author: netmind
 */

#ifndef UVCONTEXT_H_
#define UVCONTEXT_H_

#include <uv.h>
#include "UvHandle.h"

namespace uvcpp {
	class UvBaseHandle;
	class UvHandle;

	class UvContext {
		friend class UvBaseHandle;
		friend class UvTcpHandle;
		friend class UvHandle;

	public:
		UvContext();

		virtual ~UvContext();

		void open(uv_loop_t *loop=nullptr);

		void openWithDefaultLoop();

		void close();

		static UvContext *getContext();

		uv_loop_t *getLoop();

		UvHandle* createHandle(void* user_data);

		int run();
		static void handle_close_cb(uv_handle_t *phandle);

	private:
		bool _createLoop;
		uv_loop_t *_loop;
		uv_prepare_t _prepareHandle;
		UvHandle *_handleLast;
		int _pendingHandleCnt2;

		void dumpHandle(UvHandle *plast);
		void deleteHandle(UvHandle *pHandle);
		void initHandle(UvHandle *handle, void *user_data);
	};


}
#endif /* UVCONTEXT_H_ */

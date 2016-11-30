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

//		int open();

		void open(uv_loop_t *loop=nullptr);

		void openWithDefaultLoop();

		void close();

		static UvContext *getContext();

		uv_loop_t *getLoop();

		template<typename T>
		T *createUvHandle() {
			T *baseh = new T;
			++_pendingHandleCnt;
			initUvHandle(baseh);
			return baseh;
		}

		UvHandle* createHandle(void* user_data);


		int run();
		static void handle_close_cb(uv_handle_t *phandle);
		static void handle2_close_cb(uv_handle_t *phandle);

	private:
		bool _createLoop;
		uv_loop_t *_loop;
		uv_prepare_t _prepareHandle;
		UvBaseHandle *_handleLast;
		UvHandle *_handle2Last;
		int _pendingHandleCnt;
		int _pendingHandleCnt2;

		void deleteHandle(UvBaseHandle *phandle);

		void initUvHandle(UvBaseHandle *phandle);
		void dumpHandle(UvBaseHandle *plast);


		void deleteHandle2(UvHandle *pHandle);

		void initHandle2(UvHandle *handle, void* user_data);
	};


}
#endif /* UVCONTEXT_H_ */

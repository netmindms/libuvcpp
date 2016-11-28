/*
 * EdContext.h
 *
 *  Created on: Nov 22, 2016
 *      Author: netmind
 */

#ifndef UVCONTEXT_H_
#define UVCONTEXT_H_

#include <uv.h>
namespace uvcpp {
	class UvBaseHandle;

	class UvContext {
//	friend class UvHandle;
		friend class UvBaseHandle;

		friend class UvTcpHandle;

	public:
		UvContext();

		virtual ~UvContext();

//		int open();

		void open(uv_loop_t *loop=nullptr);

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

		int run();

	private:
		bool _createLoop;
		uv_loop_t *_loop;
		uv_prepare_t _prepareHandle;
		UvBaseHandle *_handleLast;
		int _pendingHandleCnt;

		void deleteHandle(UvBaseHandle *phandle);

		void initUvHandle(UvBaseHandle *phandle);

		void dumpHandle(UvBaseHandle *plast);

		static void handle_close_cb(uv_handle_t *phandle);

	};

}
#endif /* UVCONTEXT_H_ */

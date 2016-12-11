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
	public:
		UvContext();

		virtual ~UvContext();

		void open(uv_loop_t *loop=nullptr);

		void openWithDefaultLoop();

		void close();

		static UvContext *getContext();

		uv_loop_t *getLoop();

		UvHandle* createHandle(void* user_data, const char* typestr);
		HandleHolder* createHandleHolder(UvHandle* uvh);

		int handleCount();

		int run();
		static void handle_close_cb(uv_handle_t *phandle);
		void closeHandle(HandleHolder* holder);
		static void handle_write_cb(uv_write_t *req, int status);
		static void handle_send_cb(uv_udp_send_t* req, int status);
		static void handle_read_alloc_cb(uv_handle_t *handle, size_t suggesited_size, uv_buf_t *puvbuf);
		static void handle_connect_cb(uv_connect_t *puvcnn, int status);
		static void handle_read_cb(uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf);
		static void handle_listen_cb(uv_stream_t *server, int status);
	private:
		bool _createLoop;
		uv_loop_t *_loop;
		uv_prepare_t _prepareHandle;
		HandleHolder *_handleLast;
		int _pendingHandleCnt;
		uint32_t _handleIdSeed;

		void dumpHandle(UvHandle *plast);
		void deleteHandle(HandleHolder *holder);
		void initHandle(UvHandle *handle, void *user_data);

	};



}
#endif /* UVCPPPRJ_UVCONTEXT_H_ */

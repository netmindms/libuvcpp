/*
 * EdContext.h
 *
 *  Created on: Nov 22, 2016
 *      Author: netmind
 */

#ifndef UVCPPPRJ_UVCONTEXT_H_
#define UVCPPPRJ_UVCONTEXT_H_

#include <cstdint>
#include <uv.h>
#include "UvHandle.h"
#include "HandleHoder.h"
#include "AddrInfoReq.h"

namespace uvcpp {

	class UvHandle;
	class Immediate;
	class ImmediateWrapper;

	class UvContext {
		friend class UvHandle;
		friend class UvGetAddrInfo;
		friend class UvGetNameInfo;
		friend class UvWork;

	public:

		static void open(uv_loop_t *loop=nullptr);

		static void openWithDefaultLoop();

		static void close();

		static UvContext *getContext();

		static int run(uv_run_mode mode=UV_RUN_DEFAULT);

		static uint32_t newCommonHandle();

		uv_loop_t *getLoop();

		UvHandle* createHandle(void* user_data, const char* typestr);
		HandleHolder* createHandleHolder(UvHandle* uvh);

		int handleCount();

		static std::unique_ptr<ImmediateWrapper> createImmediate();

		void closeHandle(HandleHolder* holder);
		static void handle_close_cb(uv_handle_t *phandle);
		static void handle_write_cb(uv_write_t *req, int status);
		static void handle_send_cb(uv_udp_send_t* req, int status);
		static void handle_read_alloc_cb(uv_handle_t *handle, size_t suggesited_size, uv_buf_t *puvbuf);
		static void handle_connect_cb(uv_connect_t *puvcnn, int status);
		static void handle_read_cb(uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf);
		static void handle_listen_cb(uv_stream_t *server, int status);
		static void handle_recv_cb(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags);
		static void handle_shutdown_cb(uv_shutdown_t* req, int status);

	private:

		bool _createLoop;
		uv_loop_t *_loop;
		HandleHolder *_handleLast;
		int _pendingHandleCnt;
		std::list<AddrInfoReq> _addrReqList;
		std::list<NameInfoReq> _nameReqList;
		uint32_t _commonHandleSeed;
		Immediate* _ctxImmd;

		std::list<uv_work_t> _workList;

		void dumpHandle(UvHandle *plast);
		void deleteHandle(HandleHolder *holder);
		void initHandle(UvHandle *handle, void *user_data);

		AddrInfoReq* allocAddrInfoReq();
		static void req_getaddrinfo_cb(uv_getaddrinfo_t* req, int status, struct addrinfo* res);

		NameInfoReq* allocNameInfoReq();
		static void req_getnameinfo_cb(uv_getnameinfo_t* req, int status, const char* hostname, const char* service);

		UvContext();

		virtual ~UvContext();

	};



}
#endif /* UVCPPPRJ_UVCONTEXT_H_ */

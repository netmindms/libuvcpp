//
// Created by netmind on 16. 11. 28.
//

#ifndef UVCPPPRJ_UVUDPHANDLE_H
#define UVCPPPRJ_UVUDPHANDLE_H

#include <nmdutil/ObjMemQue.h>
#include "UvBaseHandle.h"
#include "UvWriteInfo.h"
#include "UvReadBuffer.h"

namespace uvcpp {
	class UvUdpHandle : public UvBaseHandle {
	public:
		typedef std::function<void(const struct sockaddr *, std::unique_ptr<UvReadBuffer>)> ReadLis;

		UvUdpHandle();

		virtual ~UvUdpHandle();

		int open(ReadLis lis);
		int bind(const struct sockaddr *addr, unsigned int flags);
//		int send(const uv_buf_t bufs[], unsigned int nbufs, const struct sockaddr* addr, uv_udp_send_cb send_cb)
		int send(const char* buf, const struct sockaddr* addr, uv_udp_send_cb send_cb);

	protected:
		ReadLis _readLis;
		uv_udp_t _rawHandle;
		nmdu::ObjMemQue<UvWriteInfo> _writeReqQue;
		nmdu::ObjMemQue<UvReadBuffer> _readBufQue;

		uv_handle_t *getRawHandle() override;

		static void alloc_cb(uv_handle_t *handle, size_t suggesited_size, uv_buf_t *puvbuf);

		static void
		recv_cb(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags);

		static void send_cb(uv_udp_send_t *req, int status);
	};

}
#endif //UVCPPPRJ_UVUDPHANDLE_H

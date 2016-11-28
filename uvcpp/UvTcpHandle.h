/*
 * UvTcpHandle.h
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#ifndef UVTCPHANDLE_H_
#define UVTCPHANDLE_H_

#include <nmdutil/ObjMemQue.h>
#include <memory>
#include <utility>

#include "UvBaseHandle.h"
#include "UvWriteInfo.h"
#include "UvReadBuffer.h"

namespace uvcpp {
	class UvTcpHandle : public UvBaseHandle {
	public:
		typedef std::function<void(int)> CnnLis;
		typedef std::function<void(std::unique_ptr<UvReadBuffer>)> ReadLis;

		UvTcpHandle();

		virtual ~UvTcpHandle();

		int open(CnnLis clis, ReadLis rlis);

		int connect(const char *ipaddr, uint16_t port);

		int write(const char *buf, size_t len);

		int bind(const struct sockaddr *addr, unsigned int flags);

		int listen(int backlogs);

		UvTcpHandle *accept();

		size_t writeQueCnt();

		void setCnnOnListener(CnnLis lis);

		void setReadOnListener(ReadLis lis);

	private:
		CnnLis _cnnLis;
		ReadLis _readLis;

		uv_handle_t *getRawHandle() override;

		uv_tcp_t _rawhandle;
		uv_connect_t _cnnHandle;
		nmdu::ObjMemQue<UvWriteInfo> _writeReqQue;
		nmdu::ObjMemQue<UvReadBuffer> _readBufQue;
//	std::unique_ptr<char[]> _upReadBuf;
		size_t _readSize;

		void OnHandleClosed() override;
		static void on_connect(uv_connect_t *puvcnn, int status);
		static void on_write_end(uv_write_t *req, int status);
		static void alloc_cb(uv_handle_t *handle, size_t suggesited_size, uv_buf_t *puvbuf);
		static void read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
		static void connection_cb(uv_stream_t *server, int status);
	};
}
#endif /* UVTCPHANDLE_H_ */

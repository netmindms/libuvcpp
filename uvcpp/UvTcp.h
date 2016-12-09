/*
 * UvTcp3.h
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#ifndef UVCPPPRJ_UVTCP_H_
#define UVCPPPRJ_UVTCP_H_

#include <memory>
#include <utility>
#include "ObjQue.h"


#include "UvReadBuffer.h"
#include "UvWriteInfo.h"
#include "UvStream.h"

namespace uvcpp {
	class UvTcp : public UvStream {
	public:
		typedef std::function<void(int)> CnnLis;

		UvTcp();

		virtual ~UvTcp();

		int init();

		int connect(const char *ipaddr, uint16_t port, CnnLis lis);

		int accept(UvTcp *newcnn);

		int bind(const struct sockaddr *addr, unsigned int flags);


		int bindAndListen(uint16_t port, const char *ipaddr, UvStream::ListenLis lis, int backlogs = 128);

		size_t writeQueCnt();

		void setOnCnnLis(CnnLis lis);

		void setOnReadLis(ReadLis lis);

	private:
		CnnLis _cnnLis;
		ReadLis _readLis;
		uv_connect_t _cnnHandle;
		size_t _readSize;
		ObjQue<UvWriteInfo> _writeReqQue;
		ObjQue<UvReadBuffer> _readBufQue;
		//void openChild(UvTcpHandle *newhandle);

		static void alloc_cb(uv_handle_t *handle, size_t suggesited_size, uv_buf_t *puvbuf);
		static void read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
		static void connect_cb(uv_connect_t *puvcnn, int status);
		static void connection_cb(uv_stream_t *server, int status);
		static void on_write_end(uv_write_t *req, int status);
	};
}
#endif /* UVCPPPRJ_UVTCP_H_ */

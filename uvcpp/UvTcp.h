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


#include "UvHandleOwner.h"
#include "UvReadBuffer.h"
#include "UvWriteInfo.h"

namespace uvcpp {
	class UvTcp : public UvHandleOwner {
	public:
		typedef std::function<void(int)> CnnLis;
		typedef std::function<void(std::unique_ptr<UvReadBuffer>)> ReadLis;

		UvTcp();

		virtual ~UvTcp();

		int open(CnnLis clis=nullptr, ReadLis rlis=nullptr);

		int write(const char *buf, size_t len);

		int write(const std::string& msg);

		int connect(const char *ipaddr, uint16_t port);

		int accept(UvTcp *newcnn, CnnLis clis=nullptr, ReadLis rlis=nullptr);

		int bind(const struct sockaddr *addr, unsigned int flags);

		void close(UvHandle::CloseLis lis) override;

		int listen(uint16_t port, const char *ipaddr = nullptr, int backlogs = 10);
		int listen(int backlogs);

		size_t writeQueCnt();

		void setOnCnnLis(CnnLis lis);

		void setOnReadLis(ReadLis lis);

	private:
		CnnLis _cnnLis;
		ReadLis _readLis;
		uv_connect_t _cnnHandle;
		size_t _readSize;
		uv_tcp_t* _rawh;
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

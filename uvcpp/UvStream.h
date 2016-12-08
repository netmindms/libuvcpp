//
// Created by netmind on 16. 12. 7.
//

#ifndef UVCPPPRJ_UVSTREAM_H
#define UVCPPPRJ_UVSTREAM_H

#include "UvWriteInfo.h"
#include "UvReadBuffer.h"
#include "ObjQue.h"
#include "UvHandle.h"

namespace uvcpp {
	class UvStream : public UvHandle {
	public:
		typedef std::function<void(std::unique_ptr<UvReadBuffer>)> ReadLis;
		typedef std::function<void(int event)> ConnectionLis;
		typedef std::function<void(int)> CnnLis;
		UvStream();

		virtual ~UvStream();


		int accept(UvStream* newclient);

//		virtual int open(const char* name);
		int readStart(ReadLis lis);
		int readStop();

		int listen(int backlogs, ConnectionLis lis);


		int write(const char *buf, size_t len);
		int write(const std::string& msg);

		void setOnCnnLis(CnnLis lis);

	private:
		CnnLis _cnnLis;
		ReadLis _readLis;
		ConnectionLis _connectionLis; // for incoming connection
		ObjQue<UvWriteInfo> _writeReqQue;
		ObjQue<UvReadBuffer> _readBufQue;

		static void alloc_cb(uv_handle_t *handle, size_t suggesited_size, uv_buf_t *puvbuf);
		static void read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
		static void write_cb(uv_write_t *req, int status);
		static void connection_cb(uv_stream_t *server, int status);

	protected:
		static void connect_cb(uv_connect_t *puvcnn, int status);
	};
}

#endif //UVCPPPRJ_UVSTREAM_H

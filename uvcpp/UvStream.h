//
// Created by netmind on 16. 12. 7.
//

#ifndef UVCPPPRJ_UVSTREAM_H
#define UVCPPPRJ_UVSTREAM_H

#include "UvHandleOwner.h"
#include "UvWriteInfo.h"
#include "UvReadBuffer.h"
#include "ObjQue.h"

namespace uvcpp {
	class UvStream : public UvHandleOwner {
	public:
		typedef std::function<void(std::unique_ptr<UvReadBuffer>)> ReadLis;

		UvStream();

		virtual ~UvStream();

		virtual int open(const char* name);
		int readStart(ReadLis lis);
		int readStop();

		int write(const char *buf, size_t len);
		int write(const std::string& msg);

	protected:
		uv_stream_t* _rawh;
	private:
		ReadLis _readLis;
		ObjQue<UvWriteInfo> _writeReqQue;
		ObjQue<UvReadBuffer> _readBufQue;

		static void alloc_cb(uv_handle_t *handle, size_t suggesited_size, uv_buf_t *puvbuf);
		static void read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
		static void write_cb(uv_write_t *req, int status);
	};
}

#endif //UVCPPPRJ_UVSTREAM_H

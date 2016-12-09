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
		typedef std::function<void()> ListenLis;
		UvStream();

		virtual ~UvStream();

		int readStart(ReadLis lis);
		int readStop();

		int listen(ListenLis lis, int backlogs=128);

	private:
		ReadLis _readLis;
		ListenLis _listenLis;

		ObjQue<UvReadBuffer> _readBufQue;

		static void alloc_cb(uv_handle_t *handle, size_t suggesited_size, uv_buf_t *puvbuf);
		static void read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
		static void connection_cb(uv_stream_t *server, int status);
	};
}

#endif //UVCPPPRJ_UVSTREAM_H

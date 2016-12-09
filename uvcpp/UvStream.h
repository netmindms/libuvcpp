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
		typedef std::function<void(int)> CnnLis;
		UvStream();

		virtual ~UvStream();

		int readStart(ReadLis lis);
		int readStop();

		int listen(ListenLis lis, int backlogs=128);

		int accept(UvStream* newtcp);

		void setOnCnnLis(CnnLis lis);

		void setOnReadLis(ReadLis lis);

		void setConnectionReq(CnnLis lis);

	private:
		CnnLis _cnnLis;
		ReadLis _readLis;
		ListenLis _listenLis;

		static void read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
		static void connection_cb(uv_stream_t *server, int status);

	protected:
		static void connect_cb(uv_connect_t *puvcnn, int status);
	};
}

#endif //UVCPPPRJ_UVSTREAM_H

//
// Created by netmind on 16. 12. 7.
//

#include "UvStream.h"
#include "uvcpplog.h"

#define RAWH() ((uv_stream_t*)getRawHandle())
#define GETOBJH(H) ((UvStream*)(((HandleHolder*)H->data))->uvh)

namespace uvcpp {
	UvStream::UvStream() {
	}
	UvStream::~UvStream() {

	}

	void UvStream::alloc_cb(uv_handle_t *handle, size_t suggesited_size, uv_buf_t *puvbuf) {
		auto stream = GETOBJH(handle);
		auto uprbuf = stream->_readBufQue.allocObj();
		auto tbuf = uprbuf->allocBuffer(suggesited_size);
		puvbuf->len = tbuf.first;
		puvbuf->base = tbuf.second;
		stream->_readBufQue.push(move(uprbuf));
	}

	void UvStream::read_cb(uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf) {
		alv("readcb, nread=%d", nread);
		auto stream = GETOBJH(handle);
		assert(stream->_readLis!=nullptr);
		auto uprbuf = stream->_readBufQue.pop();
		if (nread >= 0) {
			uprbuf->size = nread;
			if (stream->_readLis) {
				stream->_readLis(move(uprbuf));
			}
		}
	}

	int UvStream::readStart(ReadLis lis) {
		_readLis = lis;
		return uv_read_start((uv_stream_t *)getRawHandle(), alloc_cb, read_cb);
	}

	int UvStream::readStop() {
		return uv_read_stop((uv_stream_t *)getRawHandle());
	}

//	int UvStream::write(const char *buf, size_t len) {
//		auto upwr = allocWrite();
//		upwr->fillBuf(buf, len);
//		upwr->req.data = this;
//		auto ret = uv_write(&upwr->req, RAWH(), &upwr->uvBuf, 1, write_cb);
//		if (!ret) {
//			_writeReqQue.push(move(upwr));
//		} else {
//			ale("### uv write fail, ret=%d", ret);
//		}
//		return ret;
//	}


	int UvStream::listen(ListenLis lis, int backlogs) {
		_listenLis = lis;
		uv_listen((uv_stream_t *) RAWH(), backlogs, connection_cb);
		return 0;
	}


	void UvStream::connection_cb(uv_stream_t *server, int status) {
		auto ptcph = GETOBJH(server);
		assert(ptcph->_listenLis != nullptr);
		ptcph->_listenLis();
	}


}
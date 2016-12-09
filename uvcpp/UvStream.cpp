//
// Created by netmind on 16. 12. 7.
//

#include "UvStream.h"
#include "uvcpplog.h"
#include "UvEvent.h"

#define RAWH() ((uv_stream_t*)getRawHandle())
#define GETOBJH(H) ((UvStream*)(((HandleHolder*)H->data))->uvh)

namespace uvcpp {
	UvStream::UvStream() {
	}
	UvStream::~UvStream() {

	}

	void UvStream::read_cb(uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf) {
		alv("readcb, nread=%d", nread);
		auto holder = (HandleHolder*)handle->data;
		auto stream = GETOBJH(handle);
		assert(stream->_readLis!=nullptr);
		auto uprbuf = holder->readBufQue.pop();
		if (nread > 0) {
			uprbuf->size = nread;
			if (stream->_readLis) {
				stream->_readLis(move(uprbuf));
			}
		} else if(nread == UV_EOF) {
			if(stream->_cnnLis) {
				stream->_cnnLis(-1);
			}
		} else {
			assert(0);
		}
	}

	int UvStream::readStart(ReadLis lis) {
		_readLis = lis;
		return uv_read_start((uv_stream_t *)getRawHandle(), UvContext::handle_read_alloc_cb, read_cb);
	}

	int UvStream::readStop() {
		return uv_read_stop((uv_stream_t *)getRawHandle());
	}

//	int UvStream::write(const char *buf, size_t len) {
//		auto upwr = allocWrite();
//		upwr->fillBuf(buf, len);
//		upwr->req.data = this;
//		auto ret = uv_write(&upwr->req, RAWH(), &upwr->uvBuf, 1, handle_write_cb);
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

	int UvStream::accept(UvStream* newstrm) {
		auto newrawh = (uv_stream_t *)newstrm->getRawHandle();
		auto ret = uv_accept(RAWH(), newrawh);
		if (!ret) {
			return ret;
		} else {
			ale("### accept error");
			assert(0);
			newstrm->close();
			return -1;
		}
	}



	void UvStream::connection_cb(uv_stream_t *server, int status) {
		auto ptcph = GETOBJH(server);
		assert(ptcph->_listenLis != nullptr);
		ptcph->_listenLis();
	}

	void UvStream::connect_cb(uv_connect_t *puvcnn, int status) {
		ali("on connect, status=%d", status);
		auto uvh = (UvStream*)puvcnn->data;
		assert(uvh->_cnnLis);
		uvh->_cnnLis(status);
	}

	void UvStream::setOnReadLis(ReadLis lis) {
		_readLis = lis;
	}

	void UvStream::setOnCnnLis(CnnLis lis) {
		_cnnLis = lis;
	}

	void UvStream::setConnectionReq(UvStream::CnnLis lis) {
		_handleHolder->cnnLis = lis;
		_handleHolder->cnnReq.data = _handleHolder;
	}


}
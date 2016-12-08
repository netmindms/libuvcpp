//
// Created by netmind on 16. 12. 7.
//

#include "UvStream.h"
#include "uvcpplog.h"
#include "UvStreamEvent.h"

namespace uvcpp {
	UvStream::UvStream() {
	}
	UvStream::~UvStream() {

	}



	void UvStream::read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
		alv("readcb, nread=%d", nread);
		auto ptcp = (UvStream*)stream->data;
		assert(ptcp->_readLis!=nullptr);
		auto uprbuf = ptcp->_readBufQue.pop();
		if (nread > 0) {
			uprbuf->size = nread;
			if (ptcp->_readLis) {
				ptcp->_readLis(move(uprbuf));
			} else {

			}
		} else if (nread == UV_EOF) {
			if(ptcp->_cnnLis) {
				ptcp->_cnnLis(UvStreamEvent::DISCONNECTED);
			}
		} else {
			assert(0);
		}
	}

	int UvStream::readStart(ReadLis lis) {
		_readLis = lis;
		return uv_read_start((uv_stream_t *)getRawHandle(), alloc_cb, read_cb);
	}

	int UvStream::readStop() {
		return uv_read_stop((uv_stream_t *)getRawHandle());
	}

	int UvStream::write(const char *buf, size_t len) {
		auto upwr = _writeReqQue.allocObj();
		upwr->fillBuf(buf, len);
		upwr->req.data = this;
		auto ret = uv_write(&upwr->req, (uv_stream_t *) getRawHandle(), &upwr->uvBuf, 1, write_cb);
		if (!ret) {
			_writeReqQue.push(move(upwr));
			ald("write que cnt=%u", _writeReqQue.getQueCnt());
		} else {
			ale("### uv write fail, ret=%d", ret);
		}
		return ret;
	}

	int UvStream::write(const std::string &msg) {
		return write(msg.data(), msg.size());
	}

	int UvStream::listen(int backlogs, ConnectionLis lis) {
		_connectionLis = lis;
		return uv_listen((uv_stream_t *)getRawHandle(), backlogs, connection_cb);
	}

//	int UvStream::open(const char *name) {
//		if(!_rawh) {
//			_rawh = (uv_stream_t *) createHandle(name);
//			if (_rawh) {
//				_writeReqQue.open(10);
//				return 0;
//			}
//		} else {
//			ale("handle already opened...");
//			return -1;
//		}
//		return 0;
//	}



	void UvStream::write_cb(uv_write_t *req, int status) {
		alv("write cb, status=%d, ptcp=%0x", status, (uint64_t) req->data);
		auto ptcp = (UvStream *) req->data;
		if (ptcp) {
			auto up = ptcp->_writeReqQue.pop();
			ald("write que cnt=%u", ptcp->_writeReqQue.getQueCnt());
			ptcp->_writeReqQue.recycleObj(move(up));
		} else {
			assert(0);
		}
	}

	int UvStream::accept(UvStream* newclient) {
		return uv_accept((uv_stream_t*)getRawHandle(), (uv_stream_t*)newclient->getRawHandle());
	}

	void UvStream::connection_cb(uv_stream_t *server, int status) {
		auto ptcph = (UvStream*)server->data;
		ptcph->_connectionLis(UvStreamEvent::INCOMING);
	}

	void UvStream::alloc_cb(uv_handle_t *handle, size_t suggesited_size, uv_buf_t *puvbuf) {
		auto ptcph = (UvStream*)handle->data;
		auto uprbuf = ptcph->_readBufQue.allocObj();
		auto tbuf = uprbuf->allocBuffer(suggesited_size);
		puvbuf->len = tbuf.first;
		puvbuf->base = tbuf.second;
		ptcph->_readBufQue.push(move(uprbuf));
	}


	void UvStream::connect_cb(uv_connect_t *puvcnn, int status) {
		ali("on connect, status=%d", status);
		UvStream *ptcp = (UvStream *) puvcnn->data;
		if (status == 0) {
//			uv_read_start((uv_stream_t *)ptcp->_rawh, alloc_cb, read_cb);
			if (ptcp->_cnnLis) {
				ptcp->_cnnLis(UvStreamEvent::CONNECTED);
			}
		} else {
			if (ptcp->_cnnLis) {
				ptcp->_cnnLis(UvStreamEvent::DISCONNECTED);
			}
		}
	}

	void UvStream::setOnCnnLis(UvStream::CnnLis lis) {
		_cnnLis = lis;
	}
}
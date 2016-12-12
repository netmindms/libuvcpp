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

	int UvStream::readStart(ReadLis lis) {
		_readLis = lis;
		return uv_read_start((uv_stream_t *)getRawHandle(), UvContext::handle_read_alloc_cb, UvContext::handle_read_cb);
	}

	int UvStream::readStop() {
		return uv_read_stop((uv_stream_t *)getRawHandle());
	}

	int UvStream::listen(ListenLis lis, int backlogs) {
		_listenLis = lis;
		return uv_listen(RAWH(), backlogs, UvContext::handle_listen_cb);
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

	int UvStream::write(const char *buf, size_t len) {
		auto upwr = _handleHolder->writeReqQue.allocObj();
		upwr->fillBuf(buf, len);
		upwr->req.data = _handleHolder;
		auto ret = uv_write(&upwr->req, (uv_stream_t *) RAWH(), &upwr->uvBuf, 1, UvContext::handle_write_cb);
		if (!ret) {
			_handleHolder->writeReqQue.push(move(upwr));
		} else {
			ale("### uv write fail, ret=%d", ret);
		}
		return ret;
	}

	int UvStream::write(const std::string &msg) {
		return write(msg.data(), msg.size());
	}


	void UvStream::setOnCnnLis(CnnLis lis) {
		if(!_cnnLis) {
			_cnnLis = lis;
		}
	}

	uv_connect_t* UvStream::setConnectionReq(UvStream::CnnLis lis) {
		_cnnLis = lis;
		_handleHolder->cnnReq.data = _handleHolder;
		return &_handleHolder->cnnReq;
	}

	void UvStream::procReadCallback(upUvReadBuffer upbuf) {
		if(_status == UvHandle::INITIALIZED) {
			_readLis(move(upbuf));
		}
	}

	void UvStream::procConnectCallback(int status) {
		if(_status == UvHandle::INITIALIZED) {
			_cnnLis(status);
		}
	}
	void UvStream::procListenCallback(int status) {
		ald("listen callback, status=%d", status);
		if(_status == UvHandle::INITIALIZED) {
			_listenLis();
		}
	}


}
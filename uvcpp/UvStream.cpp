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
		if(lis) {
			_readLis = lis;
		}
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
		auto ret = uv_write(&upwr->req, RAWH(), &upwr->uvBuf, 1, UvContext::handle_write_cb);
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

	int UvStream::tryWrite(const char *buf, size_t len) {
		if(_handleHolder->writeReqQue.getQueCnt() == 0 ) {
			uv_buf_t uvbuf;
			uvbuf.base = (char*)buf;
			uvbuf.len = len;
			return uv_try_write(RAWH(), &uvbuf, 1);
		}
		return UV_EAGAIN ;
	}


	int UvStream::write2(UvStream* send_handle) {
		auto upwr = _handleHolder->writeReqQue.allocObj();
		upwr->fillBuf(".", 1);
		upwr->req.data = _handleHolder;
		auto ret = uv_write2(&upwr->req, RAWH(), &upwr->uvBuf, 1, (uv_stream_t*)send_handle->getRawHandle(), UvContext::handle_write_cb);
		if (!ret) {
			_handleHolder->writeReqQue.push(move(upwr));
//			send_handle->releaseRawHandle();
			send_handle->close();
		} else {
			ale("### uv write fail, ret=%d", ret);
		}
		return ret;
	}



	void UvStream::setOnCnnLis(CnnLis lis) {
		_cnnLis = lis;
	}

	uv_connect_t* UvStream::setConnectionReq(UvStream::CnnLis lis) {
		if(lis) {
			_cnnLis = lis;
		}
		assert(_cnnLis != nullptr);
		_handleHolder->cnnReq.data = _handleHolder;
		return &_handleHolder->cnnReq;
	}

	void UvStream::procReadCallback(upReadBuffer upbuf) {
		alv("stream read callback");
		assert(_readLis != nullptr);
		_readLis(move(upbuf));
	}

	void UvStream::procConnectCallback(int status) {
		if(_status == UvHandle::INITIALIZED) {
			if(_cnnLis) {
				_cnnLis(status);
			}
		}
	}
	void UvStream::procListenCallback(int status) {
		if(_status == UvHandle::INITIALIZED) {
			_listenLis();
		}
	}

	void UvStream::recycleReadBuffer(upReadBuffer upbuf) {
		_handleHolder->readBufQue.recycleObj(move(upbuf));
	}

	void UvStream::setOnReadLis(UvStream::ReadLis lis) {
		_readLis = lis;
	}

	uint32_t UvStream::getWriteQueCnt() {
		return _handleHolder->writeReqQue.getQueCnt();
	}

	uint32_t UvStream::getSendQueCnt() {
		return _handleHolder->sendReqQue.getQueCnt();
	}

	int UvStream::isReadable() {
		return uv_is_readable(RAWH());
	}

	int UvStream::isWritable() {
		return uv_is_writable(RAWH());
	}

	int UvStream::setBlocking(int blocking) {
		return uv_stream_set_blocking(RAWH(), blocking);
	}

	int UvStream::shutDown(UvStream::ShutdownLis lis) {
		_shutdownLis = lis;
		_handleHolder->shutdownReq.data = _handleHolder;
		return uv_shutdown(&(_handleHolder->shutdownReq), RAWH(), UvContext::handle_shutdown_cb);
	}

	void UvStream::procShutdownCallback(int status) {
		if(_shutdownLis) {
			_shutdownLis(status);
		}
	}


}
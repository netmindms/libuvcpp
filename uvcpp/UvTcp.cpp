/*
 * UvTcp3.cpp
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#include "UvTcp.h"
#include "uvcpplog.h"
#include "UvEvent.h"

using namespace std;

namespace uvcpp {
	UvTcp::UvTcp() {
		_cnnHandle.data = nullptr;
		_readSize = 0;
		_rawh = nullptr;
	}

	UvTcp::~UvTcp() {

	}

	int UvTcp::open(CnnLis clis, ReadLis rlis) {
		if(!_rawh) {
			_rawh = (uv_tcp_t *) createHandle("tcp");
			if (_rawh) {
				if(clis) {
					_cnnLis = clis;
				}
				if(rlis) {
					_readLis = rlis;
				}
				_writeReqQue.open(10);
				return uv_tcp_init(_ctx->getLoop(), _rawh);
			}
		} else {
			ale("handle already opened...");
			return -1;
		}
	}

	int UvTcp::connect(const char *ipaddr, uint16_t port) {
		sockaddr_in req_addr;
		uv_ip4_addr(ipaddr, port, &req_addr);
		_cnnHandle.data = (void *) this;
		return uv_tcp_connect(&_cnnHandle, _rawh, (struct sockaddr *) &req_addr, connect_cb);
	}

//	int UvTcp::connect(const char *ipaddr, uint16_t port) {
//		ali("connecting...");
//		return _ohandle->connect(ipaddr, port);
//	}

	int UvTcp::write(const char *buf, size_t len) {
		auto upwr = _writeReqQue.allocObj();
		upwr->fillBuf(buf, len);
		upwr->req.data = this;
		auto ret = uv_write(&upwr->req, (uv_stream_t *) _rawh, &upwr->uvBuf, 1, on_write_end);
		if (!ret) {
			_writeReqQue.push(move(upwr));
		} else {
			ale("### uv write fail, ret=%d", ret);
		}
		return ret;

	}

	void UvTcp::close() {
		if (_ohandle) {
			ald("closing...");
			_rawh = nullptr;
			UvHandleOwner::close();
		}
	}

	size_t UvTcp::writeQueCnt() {
		return _writeReqQue.getQueCnt();
	}

	int UvTcp::accept(UvTcp* newtcp, CnnLis clis, ReadLis rlis) {
		newtcp->open(clis, rlis);
		auto newrawh = (uv_stream_t *)newtcp->_ohandle->getRawHandle();
		auto ret = uv_accept((uv_stream_t *) _rawh, newrawh);
		if (!ret) {
			return uv_read_start(newrawh, alloc_cb, read_cb);
		} else {
			ale("### accept error");
			assert(0);
			newtcp->close();
			return -1;
		}
	}


	void UvTcp::setOnReadLis(ReadLis lis) {
		_readLis = lis;
	}

	void UvTcp::setOnCnnLis(CnnLis lis) {
		_cnnLis = lis;
	}

	int UvTcp::bind(const struct sockaddr *addr, unsigned int flags) {
		return uv_tcp_bind(_rawh, addr, flags);
	}

	int UvTcp::listen(uint16_t port, const char *ipaddr, int backlogs) {
		sockaddr_in inaddr;
		uv_ip4_addr(ipaddr, port, &inaddr);
		bind((sockaddr *) &inaddr, 0);
		listen(backlogs);
		return 0;
	}

	int UvTcp::listen(int backlogs) {
		uv_listen((uv_stream_t *) _rawh, backlogs, connection_cb);
		return 0;
	}


	void UvTcp::connect_cb(uv_connect_t *puvcnn, int status) {
		ali("on connect, status=%d", status);
		UvTcp *ptcp = (UvTcp *) puvcnn->data;
		if (status == 0) {
			uv_read_start((uv_stream_t *)ptcp->_rawh, alloc_cb, read_cb);
			if (ptcp->_cnnLis) {
				ptcp->_cnnLis(UvEvent::CONNECTED);
			}
		} else {
			if (ptcp->_cnnLis) {
				ptcp->_cnnLis(UvEvent::DISCONNECTED);
			}
		}
	}

	void UvTcp::connection_cb(uv_stream_t *server, int status) {
		auto ptcph = UvHandleOwner::getHandleOwner<UvTcp>((uv_handle_t*)server);
		assert(ptcph->_cnnLis != nullptr);
		ptcph->_cnnLis(UvEvent::INCOMING);
	}


	void UvTcp::alloc_cb(uv_handle_t *handle, size_t suggesited_size, uv_buf_t *puvbuf) {
		UvTcp *ptcph = UvHandleOwner::getHandleOwner<UvTcp>(handle);
		auto uprbuf = ptcph->_readBufQue.allocObj();
		auto tbuf = uprbuf->allocBuffer(suggesited_size);
		puvbuf->len = tbuf.first;
		puvbuf->base = tbuf.second;
		ptcph->_readBufQue.push(move(uprbuf));
	}

	void UvTcp::on_write_end(uv_write_t *req, int status) {
		alv("write cb, status=%d, ptcp=%0x", status, (uint64_t) req->data);
		auto ptcp = (UvTcp *) req->data;
		if (ptcp) {
			auto up = ptcp->_writeReqQue.pop();
			ptcp->_writeReqQue.recycleObj(move(up));
		}
	}

	void UvTcp::read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *) {
		alv("readcb, nread=%d", nread);
		auto ptcp = UvHandleOwner::getHandleOwner<UvTcp>((uv_handle_t*)stream);
		assert(ptcp->_readLis!=nullptr);
		auto uprbuf = ptcp->_readBufQue.pop();
		if (nread > 0) {
			uprbuf->size = nread;
			if (ptcp->_readLis) {
				ptcp->_readLis(move(uprbuf));
			}
		} else {
			if (ptcp->_cnnLis) {
				ptcp->_cnnLis(UvEvent::DISCONNECTED);
			}
			ptcp->_readLis = nullptr;
			ptcp->_cnnLis = nullptr;
		}
	}

	int UvTcp::write(const string &msg) {
		return write(msg.data(), msg.size());
	}


}

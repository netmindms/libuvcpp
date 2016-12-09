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

#define RAWH() ((uv_tcp_t*)getRawHandle())


namespace uvcpp {
	UvTcp::UvTcp() {
		_cnnHandle.data = nullptr;
		_readSize = 0;
	}

	UvTcp::~UvTcp() {

	}


	int UvTcp::connect(const char *ipaddr, uint16_t port, CnnLis lis) {
		sockaddr_in req_addr;
		uv_ip4_addr(ipaddr, port, &req_addr);
		_cnnHandle.data = (void *) this;
		_cnnLis = lis;
		return uv_tcp_connect(&_cnnHandle, RAWH(), (struct sockaddr *) &req_addr, connect_cb);
	}


	size_t UvTcp::writeQueCnt() {
		return _writeReqQue.getQueCnt();
	}

	int UvTcp::accept(UvTcp* newtcp) {
		auto newrawh = (uv_stream_t *)newtcp->getRawHandle();
		auto ret = uv_accept((uv_stream_t *) RAWH(), newrawh);
		if (!ret) {
			return ret;
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
		return uv_tcp_bind(RAWH(), addr, flags);
	}

	int UvTcp::bindAndListen(uint16_t port, const char *ipaddr, UvStream::ListenLis lis, int backlogs) {
		sockaddr_in inaddr;
		uv_ip4_addr(ipaddr, port, &inaddr);
		bind((sockaddr *) &inaddr, 0);
		return UvStream::listen(lis, backlogs);
	}



	void UvTcp::connect_cb(uv_connect_t *puvcnn, int status) {
		ali("on connect, status=%d", status);
		auto uvh = (UvTcp*)puvcnn->data;
		assert(uvh->_cnnLis);
		if (status == 0) {
			uvh->_cnnLis(UvEvent::CONNECTED);
		} else {
			uvh->_cnnLis(UvEvent::DISCONNECTED);
		}
	}


//	void UvTcp::on_write_end(uv_write_t *req, int status) {
//		alv("write cb, status=%d, ptcp=%0x", status, (uint64_t) req->data);
//		auto ptcp = (UvTcp *) req->data;
//		if (ptcp) {
//			auto up = ptcp->_writeReqQue.pop();
//			ptcp->_writeReqQue.recycleObj(move(up));
//		}
//	}

//	void UvTcp::read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *) {
//		alv("readcb, nread=%d", nread);
//		auto ptcp = UvHandleOwner::getHandleOwner<UvTcp>((uv_handle_t*)stream);
//		assert(ptcp->_readLis!=nullptr);
//		auto uprbuf = ptcp->_readBufQue.pop();
//		if (nread > 0) {
//			uprbuf->size = nread;
//			if (ptcp->_readLis) {
//				ptcp->_readLis(move(uprbuf));
//			}
//		} else {
//			if (ptcp->_cnnLis) {
//				ptcp->_cnnLis(UvEvent::DISCONNECTED);
//			}
//			ptcp->_readLis = nullptr;
//			ptcp->_cnnLis = nullptr;
//		}
//	}

//	int UvTcp::write(const string &msg) {
//		return write(msg.data(), msg.size());
//	}

//	void UvTcp::close(UvHandle::CloseLis lis) {
//		UvHandleOwner::close(lis);
//		_rawh = nullptr;
//	}

	int UvTcp::init() {
		initHandle();
		return uv_tcp_init(getLoop(), RAWH());
	}



}

//
// Created by netmind on 17. 1. 23.
//

#ifdef _WIN32
#include <winsock2.h>
#endif

#include <gtest/gtest.h>

#include "../uvcpp/uvcpp.h"
#include "tlog.h"


using namespace std;
using namespace uvcpp;

TEST(perf, udp) {
	int ret;
	UvContext::open();
	UvUdp _send;
	UvUdp _recv;
	UvTimer _timer;

	uint16_t _preRecvSeq=0;
	uint16_t _sendSeq=0;
	int _sendCnt=0;

	_recv.init();
	_recv.bind("0.0.0.0", 9090);
	_recv.recvStart([&](upReadBuffer upbuf, const sockaddr*, unsigned) {
		if(upbuf->size>0) {
			uint16_t seq = ntohs(*((uint16_t *) upbuf->buffer));
			int16_t diffseq = seq - _preRecvSeq;
			if (diffseq != 1) {
				alw("*** recv seq error, cur_seq=%d, pre_seq=%d", seq, _preRecvSeq);
			}
			_preRecvSeq = seq;
		}
	});

	_send.init();
	_send.setRemoteIpV4Addr("127.0.0.1", 9090);

	_timer.init();
	char tmp[1472];
	memset(tmp, 0, sizeof(tmp));
	_timer.start(1, 1, [&](){
		auto rawh = (uv_udp_t*)_send.getRawHandle();
		uint16_t* pseq = (uint16_t*)tmp;
		++_sendSeq;
		*pseq = htons(_sendSeq);
		ret = _send.send(tmp, sizeof(tmp));
		if(ret) {
			alw("*** send error, ret=%d", ret);
		}
		++_sendCnt;
		if(_sendCnt >= 10000) {
			_timer.stop();
			_timer.init();
			_timer.start(100, 0, [&](){
				_timer.stop();
				_send.close();
				_recv.close();
				ali("last send_seq=%d, recv_seq=%d", _sendSeq, _preRecvSeq);
			});
		}
	});

	UvContext::run();
	UvContext::close();
}

#if 0
TEST(perf, recvburst) {
	uint16_t _preRecvSeq=0;
	UvContext::open();
	UvUdp _recv;
	_recv.init();
	_recv.bind("0.0.0.0", 9090);
	_recv.recvStart([&](upReadBuffer upbuf, const sockaddr*, unsigned) {
		if(upbuf->size>0) {
			uint16_t seq = ntohs(*(uint16_t *) upbuf->buffer);
			int16_t diffseq = seq - _preRecvSeq;
			if (diffseq != 1) {
				alw("*** recv seq error, cur_seq=%d, pre_seq=%d", seq, _preRecvSeq);
			}
			_preRecvSeq = seq;
		}
	});

	UvTty tty;
	tty.init(STDIN_FILENO, 1);
	tty.readStart([&](upReadBuffer upbuf) {
		_recv.close();;
		tty.close();
		ali("last recv_seq=%d", _preRecvSeq);
	});
	UvContext::run();
	UvContext::close();
}

TEST(perf, sendburst) {
	uint16_t _preRecvSeq=0;
	UvContext::open();
	UvUdp _send;
	UvTimer _timer;
	int ret;
	int _sendCnt=0;
	uint16_t _sendSeq=0;
	_send.init();
	_send.setRemoteIpV4Addr("127.0.0.1", 9090);

	_timer.init();
	char tmp[1472];
	memset(tmp, 0, sizeof(tmp));
	_timer.start(1, 1, [&](){
		auto rawh = (uv_udp_t*)_send.getRawHandle();
		ald("send qcnt=%d", rawh->send_queue_count);
		uint16_t* pseq = (uint16_t*)tmp;
		++_sendSeq;
		*pseq = htons(_sendSeq);
		ret = _send.send(tmp, sizeof(tmp));
		if(ret) {
			alw("*** send error, ret=%d", ret);
		}
		++_sendCnt;
		if(_sendCnt >= 10000) {
			_timer.stop();
			_timer.init();
			_timer.start(100, 0, [&](){
				_timer.stop();
				_send.close();
				ali("last send_seq=%d, recv_seq=%d", _sendSeq, _preRecvSeq);
			});
		}
	});

	UvContext::run();
	UvContext::close();
}
#endif
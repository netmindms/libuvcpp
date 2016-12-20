//
// Created by netmind on 16. 12. 20.
//

#include <unistd.h>
#include "StreamIpc.h"

namespace uvcpp {
	StreamIpc::StreamIpc() {
		_fds[0] = _fds[1] = -1;
	}

	StreamIpc::~StreamIpc() {

	}

	int StreamIpc::open(std::function<void()> lis) {
		_lis = lis;
		socketpair(AF_UNIX, SOCK_STREAM, 0, _fds);
		_recvPipe.init(1);
		_recvPipe.open(_fds[0]);
		_recvPipe.readStart([this](upReadBuffer upbuf) {
			if(_recvPipe.pendingCount()>0) {
				_lis();
			}
		});
		return 0;
	}

	int StreamIpc::accept(UvStream *tcp) {
		return _recvPipe.accept(tcp);
	}

	void StreamIpc::disconnectIpc() {
		_sendPipe.close();
	}

	int StreamIpc::connectIpc() {
		if(_fds[1]>0) {
			_sendPipe.init(1);
			_sendPipe.open(_fds[1]);
			return 0;
		} else {
			return -1;
		}
	}

	void StreamIpc::close() {
		_recvPipe.close();
		::close(_fds[0]);
		::close(_fds[1]);
		_fds[0] = _fds[1] = -1;
	}

	int StreamIpc::sendUvStream(UvStream *send_strm) {
		auto ret = _sendPipe.write2(send_strm);
		if(!ret) {
			send_strm->close();
		}
		return ret;
	}


}
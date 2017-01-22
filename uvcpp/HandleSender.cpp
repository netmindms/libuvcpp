//
// Created by khkim on 2017-01-22.
//

#include "HandleSender.h"
#include "uvcpplog.h"

namespace uvcpp {
	int HandleSender::connect(const char *name) {
		_pipe.init(1);
		_status = 0;
		_pipe.connect(name, [this](int status) {
			if(!status) {
				ali("handler sender connected");
				_status = status;
			}
		});
		return 0;
	}

	int HandleSender::sendHandle(UvStream *strm) {
		return _pipe.write2(strm);
	}

	void HandleSender::close() {
		_pipe.close();
	}

	int HandleSender::connect(const std::string &name) {
		return connect(name.c_str());
	}
}
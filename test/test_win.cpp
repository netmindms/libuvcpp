//
// Created by khkim on 2017-01-22.
//

#ifdef _WIN32
#include <gtest/gtest.h>
#include "../uvcpp/uvcpp.h"
#include "tlog.h"

using namespace uvcpp;

TEST(win, pipe) {

	UvContext::open();
	UvPipe _svrPipe;
	UvPipe _childPipe;
	UvPipe _client;
	const char _pipename[100] = R"(//./pipe/mypipe)";
//	const char _pipename[100] = R"(\\.\pipe\mypipe)";
	_svrPipe.init(0);
	auto ret = _svrPipe.bind(_pipename);
	_svrPipe.listen([&]() {
		ali("incoming event");
		_childPipe.init(0);
		_svrPipe.accept(&_childPipe);
		_childPipe.readStart([&](upReadBuffer upbuf) {
			std::string rs(upbuf->buffer, upbuf->size);
			ali("recv str: %s", rs);
			_childPipe.write(rs);
		});
	});
	ASSERT_EQ(0, ret);
	_client.init(0);
	_client.connect(_pipename, [&](int status) {
		if(!status) {
			ali("connected");
			_client.readStart([&](upReadBuffer upbuf) {
				_client.close();
				_childPipe.close();
				_svrPipe.close();
			});
			_client.write("1234", 4);
		} else {
			ali("disconnected");
			ASSERT_TRUE(0);
		}
	});

	UvContext::run();
	UvContext::close();
}
#endif
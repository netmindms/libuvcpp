//
// Created by netmind on 16. 12. 13.
//

#include <gtest/gtest.h>
#include "../uvcpp/UvContext.h"
#include "../uvcpp/UvTcp.h"
#include "tlog.h"
#include "../uvcpp/UvTimer.h"
#include "../uvcpp/UvPoll.h"

using namespace uvcpp;

TEST(except, connect) {
	UvContext::open();
	UvTcp tcp;
	int callbackCnt=0;
	tcp.init();
	tcp.connect("111.111.111.111", 9090, [&](int status) {
		callbackCnt++;
		if(!status) {
			ald("connected");
		} else {
			ald("disconnected");
			tcp.close();
		}
	});
	tcp.close();
	UvContext::run();
	UvContext::close();
	ASSERT_EQ(0, callbackCnt);
}

TEST(except, listen) {
	UvContext::open();
	UvTcp tcp;
	int callbackCnt=0;
	tcp.init();
	tcp.listen([&]() {
		callbackCnt++;
	}, 9090);
	tcp.close();
	UvContext::run();
	UvContext::close();
	ASSERT_EQ(0, callbackCnt);
}

TEST(except, timer) {
	UvContext::open();
	UvTimer timer;
	timer.init();
	int callbackCnt=0;
	timer.start(100, 100, [&]() {
		callbackCnt++;
	});
	timer.stop();
	UvContext::run();
	UvContext::close();
	ASSERT_EQ(0, callbackCnt);
}

TEST(except, timerrep) {
	UvContext::open();
	UvTimer timer;
	timer.init();
	int callbackCnt=0;
	timer.start(100, 100, [&]() {
		ald("expired");
		callbackCnt++;
		timer.setRepeat(50);
		if(callbackCnt>=3) {
			timer.stop();
		}
	});
	UvContext::run();
	UvContext::close();
}

#ifdef __linux
#include <sys/inotify.h>
TEST(except, inotify) {
	FILE* st = fopen("test.txt", "wb");
	fclose(st);
	UvContext::open();
	UvPoll ipoll;
	int fd = inotify_init();
	inotify_add_watch(fd, "test.txt", IN_CLOSE_WRITE);
	ipoll.init(fd);
	ipoll.start(UV_READABLE, [&](int event) {
		inotify_event evt;
		::read(ipoll.getFd(), &evt, sizeof(evt));
		ald("file closed on write");
		ipoll.stop();
		::close(fd);
	});
	st = fopen("test.txt", "a+b");
	fwrite("1234", 1, 4, st);
	UvTimer timer;
	timer.init();
	timer.start(1000, 1000, [&]() {
		ald("timer stop");
		timer.stop();
		fclose(st);
	});
	UvContext::run();
	UvContext::close();
}
#endif
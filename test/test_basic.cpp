//
// Created by netmind on 16. 11. 28.
//

#include <chrono>
#include <thread>
#include <gtest/gtest.h>
#include "tlog.h"
#include "../uvcpp/UvContext.h"
#include "../uvcpp/UvTimer.h"
#include "../uvcpp/UvUdp.h"

#include "../uvcpp/UvCheck.h"
#include "../uvcpp/UvPrepare.h"
#include "../uvcpp/UvAsync.h"
#include "../uvcpp/UvTcp.h"
#include "../uvcpp/UvStreamEvent.h"
#include "../uvcpp/UvFdTimer.h"
#include "../uvcpp/UvIdle.h"
#include "../uvcpp/UvPoll.h"
#include "../uvcpp/UvTty.h"

#include <cstdarg>
using namespace std;
using namespace uvcpp;

static void test_close_cb(uv_handle_t* handle) {
	ald("test close callback");
}

TEST(testuv, handle) {

	uv_idle_t handle;
	uv_loop_t* loop = uv_default_loop();
	uv_idle_init(loop, &handle);
	uv_close((uv_handle_t*)&handle, test_close_cb);
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	uv_loop_close(loop);
}

TEST(basic, timer) {
	int expire_cnt=0;
	bool bexit=false;
	std::thread thr = thread([&]() {
		UvContext ctx;
		ctx.open(uv_default_loop());
		UvTimer* timer;
		timer = UvTimer::init();
		timer->timerStart(100, 100, [&]() {
			ald("timer expired");
			expire_cnt++;
			if (bexit) {
				timer->timerStop();
				timer->close();
			}
		});
		uv_run(uv_default_loop(), UV_RUN_DEFAULT);
		uv_loop_close(uv_default_loop());
	});

	std::this_thread::sleep_for(chrono::milliseconds(600));
	ASSERT_EQ(5, expire_cnt);
	bexit = true;
	thr.join();
}

#ifdef __linux
TEST(basic, fdtimer) {
	uint64_t expire_cnt=0;
	bool bexit=false;
	std::thread thr = thread([&]() {
		UvContext ctx;
		ctx.openWithDefaultLoop();
		UvFdTimer timer;
		timer.set(100, 100, [&]() {
			ald("timer expired");
			expire_cnt += timer.getFireCount();
			if(bexit) {
				timer.kill();
			}
		});
		uv_run(uv_default_loop(), UV_RUN_DEFAULT);
		uv_loop_close(ctx.getLoop());
	});

	std::this_thread::sleep_for(chrono::milliseconds(590));
	ASSERT_EQ(5, expire_cnt);
	bexit = true;
	if(thr.joinable()) {
		thr.join();
	}
}
#endif

TEST(basic, tcp) {
	UvContext ctx;
	UvTcp* client;
	UvTcp* server;
	UvTcp* child;
	std::string teststr="test string";
	std::string recvstr;

	ctx.openWithDefaultLoop();
	int ret;
	server = UvTcp::init();

	ret = server->bind(9090, "127.0.0.1", 0);
	assert(!ret);
	ret = server->listen(10, [&](int event) {
		if(event == UvStreamEvent::INCOMING) {
			child = UvTcp::init();
			server->accept(child);

			child->setOnCnnLis([&](int event) {
				if(event == UvStreamEvent::DISCONNECTED) {
					ald("child cnn disconnected");
					child->close();
					server->close();
				}
			});

			child->readStart([&](upUvReadBuffer upbuf) {
				ald("child on read");
				child->write(upbuf->buffer, upbuf->size);
			});
		}
	});
	assert(!ret);


	client = UvTcp::init();
	client->connect("127.0.0.1", 9090, [&](int event) {
		if(event == UvStreamEvent::CONNECTED) {
			ald("connected");
			client->readStart([&](upUvReadBuffer upbuf) {
				recvstr.assign(upbuf->buffer, upbuf->size);
				client->close();
			});

			client->write(teststr.data());
		} else if(event == UvStreamEvent::DISCONNECTED) {
			ald("disconnected");
		}
	});

	ctx.run();
	uv_loop_close(ctx.getLoop());
	ASSERT_STREQ(teststr.c_str(), recvstr.c_str());
}

TEST(basic, udp) {
	bool bexit=false;
	string recvstr;
	string testmsg = "test message";

	UvContext ctx;
	UvUdp* senderUdp;
	UvUdp* recvUdp;
	int ret;
	sockaddr_in inaddr;
	ctx.openWithDefaultLoop();

	recvUdp = UvUdp::init();
	uv_ip4_addr("127.0.0.1", 17000, &inaddr);
	ret = recvUdp->bind((sockaddr*)&inaddr);
	assert(!ret);
	ret = recvUdp->recvStart([&](const sockaddr* padr, upUvReadBuffer upbuf) {
		string ts(upbuf->buffer, upbuf->size);
		ald("recv str: %s", ts);
		recvstr = ts;
		senderUdp->close();
		recvUdp->close();
	});
	assert(!ret);

	senderUdp = UvUdp::init();
	senderUdp->setRemoteIpV4Addr("127.0.0.1", 17000);
	ald("senderUdp open ret=%d", ret);
	ret = senderUdp->recvStart([&](const sockaddr* paddr, upUvReadBuffer upbuf) {
		assert(0);
	});
	senderUdp->send(testmsg.data(), testmsg.size());


	ctx.run();
	ctx.close();
	uv_loop_close(ctx.getLoop());

	ASSERT_STREQ(testmsg.c_str(), recvstr.c_str());
}

TEST(basic, check) {
	UvContext ctx;
	ctx.openWithDefaultLoop();
	UvCheck* chk;
	UvTimer* timer;
	int ret;
	chk = UvCheck::init();
	ret = chk->start([&](){
		ald("check call back");
		chk->stop();
		chk->close([&]() {
			ald("check closed");
		});
	});
	assert(!ret);
	timer = UvTimer::init();
	timer->timerStart(1000, 1000, [&]() {
		ald("timer expired");
		timer->timerStop();
	});
	ctx.run();
	uv_loop_close(ctx.getLoop());
	ald("test end");
}

TEST(basic, prepare) {
	UvContext ctx;
	ctx.openWithDefaultLoop();

	UvPrepare* prepare;
	prepare = UvPrepare::init();
	prepare->start([&]() {
		ald("prepare callback");
		prepare->stop();
		prepare->close();
	});
	ctx.run();
	uv_loop_close(ctx.getLoop());
}

TEST(basic, async) {
	UvContext ctx;
	ctx.openWithDefaultLoop();
	int cnt=0;
	UvAsync* async;
	async = UvAsync::init([&]() {
		ald("async callback");
		cnt++;
//		async->close([&]() {
//			ald("async closed");
//		});
		async->close();
	});
	auto ret = async->send();
	assert(!ret);
	ctx.run();
	uv_loop_close(UvContext::getLoop());
	ctx.close();
	ASSERT_EQ(1, cnt);
}

TEST(basic, idle) {
	UvContext ctx;
	ctx.openWithDefaultLoop();
	int cnt=0;
	UvIdle* idle;
	UvTimer* timer;
	timer = UvTimer::init();
	idle = UvIdle::init();
	auto ret = idle->start([&]() {
		ald("idle callback");
		cnt++;
		idle->stop();
		idle->close([&]() {
			ald("idle closed");
		});
	});
//	timer->timerStart(100, 100, [&]() {
//		ald("timer call");
//		timer->timerStop();
//	});
	timer->timerStop();
	timer->close(nullptr);
	ctx.run();
	uv_loop_close(ctx.getLoop());
	ASSERT_EQ(1, cnt);
}

#if 0
TEST(basic, tty) {
	UvContext ctx;
	UvTty* intty;
	UvTty* outty;
	std::string inputstr;
	std::string expectedStr="test";

	ctx.openWithDefaultLoop();

	outty = UvTty::init(0, 1);
	outty->write(std::string("Input '" + expectedStr + "' : "));

	intty = UvTty::init(1, 0);
	intty->readStart([&](upUvReadBuffer upbuf) {
		inputstr.assign(upbuf->buffer, expectedStr.size());
		intty->close();
		outty->write(inputstr);
		outty->close();
	});

	ctx.run();
	uv_loop_close(ctx.getLoop());
	ASSERT_STREQ(expectedStr.c_str(), inputstr.c_str());
}
#endif

TEST(basic, closecb) {
	UvContext ctx;
	ctx.openWithDefaultLoop();
	int cnt=0;
	UvTimer* timer;
	timer->timerStart(100, 100, [&]() {
		cnt++;
		timer->timerStop();
		timer->close([&]() {
			ald("timer closeHandle callback");
			timer->close(nullptr);
		});
	});
	ctx.run();
	uv_loop_close(ctx.getLoop());
	ASSERT_EQ(1, cnt);
}


#ifdef __linux
TEST(basic, poll) {

	class PollFdTimer: public UvPoll {
	public:

		static PollFdTimer* init(int fd) {
			std::unique_ptr<PollFdTimer> ptr(new PollFdTimer());
			auto upptr = UvPoll::init(fd, std::move(ptr));
			return (PollFdTimer*)upptr;
		}
		int set() {
//			int fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
			if(getFd()>0) {
				pollStart(UV_READABLE, [this](int events) {
					if(events|UV_READABLE) {
						uint64_t cnt;
						auto rcnt = ::read(getFd(), &cnt, sizeof(cnt));
						if(rcnt>0) {
							ald("timer cnt=%llu", rcnt);
							_fireCnt++;
							kill();
						}
					}
				});
				struct itimerspec mTimerSpec;
				mTimerSpec.it_interval.tv_sec = 1;
				mTimerSpec.it_interval.tv_nsec = 0;
				mTimerSpec.it_value.tv_sec = 1;
				mTimerSpec.it_value.tv_nsec = 0;
				timerfd_settime(getFd(), 0, &mTimerSpec, NULL);
			} else {
				return -1;
			}
		};
		void kill() {
			close(nullptr);
		};

		int _fireCnt;
	public:
		PollFdTimer() {
			_fireCnt = 0;
		}

		virtual ~PollFdTimer() {

		}

	};

	UvContext ctx;
	ctx.openWithDefaultLoop();
	int fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
	PollFdTimer* fdTimer = PollFdTimer::init(fd);
//	UvPoll::init(fd, fdTimer);
//	fdTimer.set();
	ctx.run();
	uv_loop_close(UvContext::getLoop());
//	ASSERT_EQ(1, fdTimer._fireCnt);


}
#endif
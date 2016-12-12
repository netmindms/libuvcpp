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
#include "../uvcpp/UvFdTimer.h"
#include "../uvcpp/UvIdle.h"
#include "../uvcpp/UvPoll.h"
#include "../uvcpp/UvTty.h"
#include "../uvcpp/UvPipe.h"
#include "../uvcpp/PeriodicTimer.h"

using namespace std;
using namespace uvcpp;
using namespace chrono;

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
		std::chrono::system_clock::time_point prefire;
		ctx.open(uv_default_loop());
		UvTimer timer;
		timer.init();
		prefire = std::chrono::system_clock::now();
		timer.start(1000, 1000, [&]() {
			ald("timer expired");
			expire_cnt++;
			if (bexit) {
				timer.stop();
			}
			auto ct = system_clock::now();
			auto dur = (ct - prefire);
			auto msec = (dur.count()/1000000 - 1000);
			ald("timer diff=%lld", msec);
			timer.setRepeat(1000 - msec);
			prefire += system_clock::duration(1000000000);

		});
		uv_run(uv_default_loop(), UV_RUN_DEFAULT);
		uv_loop_close(uv_default_loop());
	});

	std::this_thread::sleep_for(chrono::milliseconds(105000));
	ASSERT_EQ(10, expire_cnt);
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
	UvTcp client;
	UvTcp server;
	UvTcp child;
	std::string teststr="test string";
	std::string recvstr;

	ctx.openWithDefaultLoop();
	int ret;
	server.init();
	server.bindAndListen(9090, "127.0.0.1", [&]() {
		ald("new incoming connection");
		child.init();
		server.accept(&child);
		child.readStart([&](upUvReadBuffer upbuf) {
			ald("child on read");
			child.write(upbuf->buffer, upbuf->size);
		});
		child.setOnCnnLis([&](int status) {
			if(status) {
				ald("child cnn disconnected");
				child.close(nullptr);
			}
		});
	}, 10);

	ret = client.init();
	client.connect("127.0.0.1", 9090, [&](int status) {
		if(!status) {
			ald("connected");
			client.readStart([&](upUvReadBuffer upbuf) {
				recvstr.assign(upbuf->buffer, upbuf->size);
				client.close();
				server.close();
			});
			client.write(teststr);
		} else {
			ald("disconnected");
			client.close(nullptr);
		}
	});
//	client.close();
	ctx.run();
	uv_loop_close(ctx.getLoop());
	ASSERT_STREQ(teststr.c_str(), recvstr.c_str());
}

TEST(basic, udp) {
	bool bexit=false;
	string recvstr;
	string testmsg = "test message";

	UvContext ctx;
	UvUdp senderUdp;
	UvUdp recvUdp;
	int ret;
	sockaddr_in inaddr;
	ctx.openWithDefaultLoop();
	recvUdp.init();
	uv_ip4_addr("127.0.0.1", 17000, &inaddr);
	recvUdp.bind((sockaddr*)&inaddr);
	ret = recvUdp.recvStart([&](const sockaddr* padr, upUvReadBuffer upbuf) {
		string ts(upbuf->buffer, upbuf->size);
		ald("recv str: %s", ts);
		recvstr = ts;
		senderUdp.close();
		recvUdp.close();
	});
	ald("recvUdp open ret=%d", ret);

	ret = senderUdp.init();
	senderUdp.setRemoteIpV4Addr("127.0.0.1", 17000);
	ald("senderUdp open ret=%d", ret);
	senderUdp.send(testmsg.data(), testmsg.size());
	senderUdp.recvStart([&](const sockaddr* paddr, upUvReadBuffer upbuf) {
		assert(0);
	});

	ctx.run();
//	ctx.close();
	uv_loop_close(ctx.getLoop());

	ASSERT_STREQ(testmsg.c_str(), recvstr.c_str());
}

TEST(basic, check) {
	UvContext ctx;
	ctx.openWithDefaultLoop();
	UvCheck chk;
	UvTimer timer;
	int ret;
	timer.init();
	ret = chk.init();
	chk.start([&](){
		ald("check call back");
		chk.stop();
	});
	assert(!ret);
	timer.start(1000, 1000, [&]() {
		ald("timer expired");
		timer.stop();
	});
	ctx.run();
	uv_loop_close(ctx.getLoop());
	ald("test end");
}

TEST(basic, prepare) {
	UvContext ctx;
	ctx.openWithDefaultLoop();

	UvPrepare prepare;
	prepare.init();
	prepare.start([&]() {
		ald("prepare callback");
		prepare.stop();
	});
	ctx.run();
	uv_loop_close(ctx.getLoop());
}

TEST(basic, async) {
	UvContext ctx;
	ctx.openWithDefaultLoop();
	int cnt=0;
	UvAsync async;
	async.init([&]() {
		ald("async callback");
		cnt++;
		async.close(nullptr);
	});
	auto ret = async.send();
	assert(!ret);
	ctx.run();
	uv_loop_close(ctx.getLoop());
	ASSERT_EQ(1, cnt);
}

TEST(basic, idle) {
	UvContext ctx;
	ctx.openWithDefaultLoop();
	int cnt=0;
	UvIdle idle;
	idle.init();
	idle.start([&]() {
		ald("idle callback");
		cnt++;
		idle.stop();
	});
	ctx.run();
	uv_loop_close(ctx.getLoop());
	ASSERT_EQ(1, cnt);
}

#if 0
TEST(basic, tty) {
	UvContext ctx;
	UvTty intty;
	UvTty outty;
	std::string inputstr;
	std::string expectedStr="test";

	ctx.openWithDefaultLoop();
	intty.init(0, 1);
	intty.readStart([&](upUvReadBuffer upbuf) {
		inputstr.assign(upbuf->buffer, expectedStr.size());
		intty.close();
		outty.close();
	});
	outty.init(1, 0);
	outty.write(std::string("Input '" + expectedStr + "' : "));
	ctx.run();
	uv_loop_close(ctx.getLoop());
	ASSERT_STREQ(expectedStr.c_str(), inputstr.c_str());
}
#endif

TEST(basic, closecb) {
	UvContext ctx;
	ctx.openWithDefaultLoop();
	int cnt=0;
	UvTimer timer;
	timer.start(100, 100, [&]() {
		cnt++;
		timer.stop();
	});
	ctx.run();
	uv_loop_close(ctx.getLoop());
	ASSERT_EQ(1, cnt);
}


#ifdef __linux
TEST(basic, poll) {

	class PollFdTimer: public UvPoll {
	public:
		PollFdTimer() {
			_fireCnt = 0;
		}

		virtual ~PollFdTimer() {

		}
		int set() {
			int fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
			if(fd>0) {
				init(fd);
				struct itimerspec mTimerSpec;
				mTimerSpec.it_interval.tv_sec = 1;
				mTimerSpec.it_interval.tv_nsec = 0;
				mTimerSpec.it_value.tv_sec = 1;
				mTimerSpec.it_value.tv_nsec = 0;
				timerfd_settime(_fd, 0, &mTimerSpec, NULL);
				start(UV_READABLE, [this](int events) {
					if(events | UV_READABLE) {
						uint64_t cnt;
						auto rcnt = ::read(_fd, &cnt, sizeof(cnt));
						if(rcnt>0) {
							ald("timer cnt=%llu", rcnt);
							_fireCnt++;
							kill();
						}
					}
				});
			} else {
				return -1;
			}
		};
		void kill() {
			auto fd = getFd();
			stop();
			::close(fd);
		};

		int _fireCnt;
	};

	UvContext ctx;
	ctx.openWithDefaultLoop();
	PollFdTimer fdTimer;
	fdTimer.set();
	ctx.run();
	uv_loop_close(ctx.getLoop());
	ASSERT_EQ(1, fdTimer._fireCnt);


}

#endif

TEST(basic, pipe) {
	std::string teststr="test";
	std::string recvstr="test";

	unlink("pipec");
	unlink("pipes");

	UvContext ctx;
	ctx.openWithDefaultLoop();

	int ret;
	UvPipe pc;
	UvPipe ps;
	UvPipe child;

	ps.init(0);
	ps.bind("pipes");
	ret = ps.listen([&]() {
		child.init(0);
		auto accret = ps.accept(&child);
		assert(!accret);
		child.setOnCnnLis([&](int status) {
			ald("child status=%d", status);
			if(status) {
				ald("  child disconnected");
				child.close();
				ps.close();
			}
		});
		child.readStart([&](upUvReadBuffer upbuf) {
			recvstr.assign(upbuf->buffer, upbuf->size);
			ald("recv : %s", recvstr);
			child.write(recvstr);
		});
	});
	assert(!ret);

	pc.init(0);
	pc.bind("pipec");
	pc.connect("pipes", [&](int status) {
		ald("pipe connect event status=%d", status);
		if(!status) {
			pc.readStart([&](upUvReadBuffer upbuf) {
				ald("client received");
				pc.close();
			});
			pc.write(teststr);
		} else {
			pc.close();

		}
	});


	ctx.run();
	ctx.close();

	uv_loop_close(ctx.getLoop());

}


TEST(basic, periodictimer) {
	int fireCnt=0;
	bool bexit=false;
	std::thread thr = thread([&]() {
		UvContext ctx;
		ctx.openWithDefaultLoop();
		PeriodicTimer timer;
		int ret = timer.start(100, [&]() {
			ald("timer expired...");
			if(bexit) {
				timer.stop();
			} else {
				fireCnt++;
			}
		});
		ASSERT_EQ(0, ret);
		ctx.run();
		ctx.close();
		uv_loop_close(uv_default_loop());
	});
	std::this_thread::sleep_for(chrono::milliseconds(5080));
	bexit = true;
	thr.join();
	ASSERT_EQ(50, fireCnt);
}
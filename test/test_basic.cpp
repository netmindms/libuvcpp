//
// Created by netmind on 16. 11. 28.
//

#include <chrono>
#include <thread>
#include <chrono>
#include <gtest/gtest.h>
#include "tlog.h"
#include "../uvcpp/UvContext.h"
#include "../uvcpp/UvTimer.h"
#include "../uvcpp/UvUdp.h"

#include "../uvcpp/UvHandle.h"
#include "../uvcpp/UvCheck.h"
#include "../uvcpp/UvPrepare.h"
#include "../uvcpp/UvAsync.h"

using namespace std;
using namespace uvcpp;

TEST(basic, timer) {
	int expire_cnt=0;
	bool bexit=false;
	std::thread thr = thread([&]() {
		UvContext ctx;
		ctx.open(uv_default_loop());
		UvTimer timer;
		timer.set(100, 100, [&]() {
			ald("timer expired");
			expire_cnt++;
			if(bexit) {
				timer.kill();
			}
		});
		uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	});

	std::this_thread::sleep_for(chrono::milliseconds(550));
	ASSERT_EQ(5, expire_cnt);
	bexit = true;
	thr.join();
}

TEST(basic, udp) {
	bool bexit=false;
	string recvstr;
	string testmsg = "test message";

	std::thread thr = thread([&]() {
		UvContext ctx;
		UvUdp senderUdp;
		UvUdp recvUdp;
		int ret;
		sockaddr_in inaddr;
		ctx.openWithDefaultLoop();
		ret = recvUdp.open([&](const sockaddr* padr, upUvReadBuffer upbuf) {
			string ts(upbuf->buffer, upbuf->size);
			ald("recv str: %s", ts);
			recvstr = ts;
			senderUdp.close();
			recvUdp.close();
		});
		ald("recvUdp open ret=%d", ret);
		uv_ip4_addr("127.0.0.1", 17000, &inaddr);
		recvUdp.bind((sockaddr*)&inaddr);
		recvUdp.readStart();

		ret = senderUdp.open([&](const sockaddr* paddr, upUvReadBuffer upbuf) {
			assert(0);
		});
		senderUdp.setRemoteIpV4Addr("127.0.0.1", 17000);
		ald("senderUdp open ret=%d", ret);
		senderUdp.send(testmsg.data(), testmsg.size());
		senderUdp.readStart();

		ctx.run();
		ctx.close();
		uv_loop_close(ctx.getLoop());
	});
	thr.join();

	ASSERT_STREQ(testmsg.c_str(), recvstr.c_str());
}

TEST(basic, check) {
	UvContext ctx;
	ctx.openWithDefaultLoop();
	UvCheck chk;
	UvTimer timer;
	int ret;
	ret = chk.open([&](){
		ald("check call back");
		chk.close();
	});
	assert(!ret);
	timer.set(1000, 1000, [&]() {
		ald("timer expired");
		timer.kill();
	});
	ctx.run();
	uv_loop_close(ctx.getLoop());
	ald("test end");
}

TEST(basic, prepare) {
	UvContext ctx;
	ctx.openWithDefaultLoop();

	UvPrepare prepare;
	prepare.open([&]() {
		ald("prepare callback");
		prepare.close();
	});
	ctx.run();
	uv_loop_close(ctx.getLoop());
}

TEST(basic, async) {
	UvContext ctx;
	ctx.openWithDefaultLoop();
	int cnt=0;
	UvAsync async;
	async.open([&]() {
		ald("async callback");
		cnt++;
		async.close();
	});
	auto ret = async.send();
	assert(!ret);
	ctx.run();
	uv_loop_close(ctx.getLoop());
	ASSERT_EQ(1, cnt);
}

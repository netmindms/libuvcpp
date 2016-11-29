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
	std::thread thr = thread([&]() {
		UvContext ctx;
		UvUdp udp;
		int ret;
		ctx.open(uv_default_loop());
		ret = udp.open([&](const sockaddr* paddr, upUvReadBuffer upbuf) {
			string ts(upbuf->buffer, upbuf->size);
			ald("recv str: %s", ts);
			udp.close();
			recvstr = ts;
		});
		ald("udp open ret=%d", ret);
		sockaddr_in inaddr;
		uv_ip4_addr("127.0.0.1", 16000, &inaddr);
		udp.bind((sockaddr*)&inaddr, UV_UDP_REUSEADDR);
		udp.readStart();
		uv_run(uv_default_loop(), UV_RUN_DEFAULT);
		ctx.close();
		ald("thread end");
	});
	thr.join();
	ald("teset end");
}
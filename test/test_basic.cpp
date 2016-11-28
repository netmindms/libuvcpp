//
// Created by netmind on 16. 11. 28.
//

#include <thread>
#include <chrono>
#include <gtest/gtest.h>
#include "tlog.h"
#include "../uvcpp/UvContext.h"
#include "../uvcpp/UvTimer.h"

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
	std::thread thr = thread([&]() {
		UvContext ctx;
		ctx.open(uv_default_loop());
		uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	});
}
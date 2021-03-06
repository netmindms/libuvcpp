//
// Created by netmind on 16. 11. 28.
//

#define LOG_LEVEL_DEBUG

#include <chrono>
#include <thread>
#include <gtest/gtest.h>
#include <fstream>
#include <uv.h>
#include "tlog.h"
#include "../uvcpp/UvContext.h"
#include "../uvcpp/UvTimer.h"
#include "../uvcpp/UvUdp.h"

#include "../uvcpp/UvCheck.h"
#include "../uvcpp/UvPrepare.h"
#include "../uvcpp/UvAsync.h"
#include "../uvcpp/UvTcp.h"
#ifdef __linux
#include "../uvcpp/FdTimer.h"
#endif
#include "../uvcpp/UvIdle.h"
#include "../uvcpp/UvPoll.h"
#include "../uvcpp/UvTty.h"
#include "../uvcpp/UvPipe.h"
#include "../uvcpp/UvFsEvent.h"
#include "../uvcpp/UvFsPoll.h"
#include "../uvcpp/UvSignal.h"
#include "../uvcpp/UvSpawn.h"
#include "../uvcpp/UvWork.h"
#include "../uvcpp/Immediate.h"
#include "../uvcpp/ImmediateWrapper.h"

using namespace std;
using namespace uvcpp;
using namespace chrono;

#define logflush() fflush(stdout)

static void test_close_cb(uv_handle_t* handle) {
	uld("test close callback");
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
	UvContext::open();
	UvTimer timer;
	auto ret = timer.init();
	assert(!ret);
	uli("timer starting...");
	timer.start(100, 0, [&]() {
		uli("timer expired");
		logflush();
		expire_cnt++;
		timer.stop();
	});
	UvContext::run();
	UvContext::close();
	ASSERT_EQ(1, expire_cnt);
}

#ifdef __linux
TEST(basic, fdtimer) {
	UvContext::open();
	FdTimer timer;
	timer.init();
	timer.start(100, 0, [&]() {
		uli("on fd timer");
		timer.close();
	});
	UvContext::run();
	UvContext::close();
}
#endif

TEST(basic, tcp) {
	UvTcp client;
	UvTcp server;
	UvTcp child;
	std::string teststr="test string";
	std::string recvstr;

	UvContext::open();
	int ret;
	server.init();
	server.bindAndListen(9090, "127.0.0.1", [&]() {
		uli("new incoming connection");
		child.init();
		server.accept(&child);
		child.readStart([&](upReadBuffer upbuf) {
			uld("child on read");
			child.write(upbuf->buffer, upbuf->size);
		});
		child.setOnCnnLis([&](int status) {
			if(status) {
				uld("child cnn disconnected");
				child.close();
			}
		});
	});

	ret = client.init();
	client.connect("127.0.0.1", 9090, [&](int status) {
		if(!status) {
			uld("connected");
			client.readStart([&](upReadBuffer upbuf) {
				uld("client on read");
				recvstr.assign(upbuf->buffer, upbuf->size);
				client.close();
				server.close();
			});
			client.write(teststr);
		} else {
			uld("disconnected");
			client.close();
			server.close();
		}
	});
	client.setOnWriteLis([&](int status) {
		uli("client on write..., status=%d", status);
	});

	UvContext::run();
	UvContext::close();
	ASSERT_STREQ(teststr.c_str(), recvstr.c_str());
}

TEST(basic, tcpwritelis) {
	UvTcp client;
	UvTcp server;
	UvTcp child;
	std::string teststr="test string";
	std::string recvstr;

	FILE* _st = fopen("/home/netmind/temp/test.pcm", "rb");
	FILE* _wst = fopen("/tmp/test.pcm", "wb");

	UvContext::open();
	int ret;
	server.init();
	server.bindAndListen(9090, "127.0.0.1", [&]() {
		uli("new incoming connection");
		child.init();
		server.accept(&child);
		child.readStart([&](upReadBuffer upbuf) {
//			ald("child on read");
			fwrite(upbuf->buffer, 1, upbuf->size, _wst);
		});
		child.setOnCnnLis([&](int status) {
			if(status) {
				uld("child cnn disconnected");
				child.close();
				server.close();
				fclose(_wst);
			}
		});
	});

	ret = client.init();

	client.connect("127.0.0.1", 9090, [&](int status) {
		if(!status) {
			uld("connected");
			client.readStart([&](upReadBuffer upbuf) {
				uld("client on read");
			});
			char temp[1024];
//			auto rcnt = fread(temp, 1, 1024, _st);
//			if(rcnt>0) {
//				client.write(temp, rcnt);
//			}
			client.write(temp, 0);
		} else {
			uld("disconnected");
			client.close();
		}
	});
	client.setOnWriteLis([&](int status) {
//		ali("client on write..., status=%d", status);
		char temp[1024];
		auto rcnt = fread(temp, 1, 1024, _st);
		if(rcnt>0) {
			client.write(temp, rcnt);
		} else {
			client.close();
			fclose(_st);
		}
	});

	UvContext::run();
	UvContext::close();
}

TEST(basic, shutdown) {
	UvContext::open();
	UvTcp tcp;
	UvTcp server;
	UvTcp child;
	server.init();
	server.bindAndListen(9090, "127.0.0.1", [&]() {
		child.init();
		server.accept(&child);
	});

	tcp.init();
	tcp.connect("127.0.0.1", 9090, [&](int status) {
		ASSERT_EQ(0, status);
		char tmp[1024];
		memset(tmp, 0, sizeof(tmp));
		for(int i=0;i<1024;i++) {
			tcp.write(tmp, 1024);
		}
		tcp.shutDown([&](int status) {
			uli("shutdown callback");
		});
		tcp.close([&](UvHandle*){
			uli("close callback");
			child.close();
			server.close();
		});
	});


	UvContext::run();
	UvContext::close();
}


TEST(basic, udp) {
	bool bexit=false;
	string recvstr;
	string testmsg = "test message";

	UvUdp senderUdp;
	UvUdp recvUdp;
	int ret;
	sockaddr_in inaddr;
	UvContext::open();
	recvUdp.init();
	uv_ip4_addr("127.0.0.1", 17000, &inaddr);
	recvUdp.bind((sockaddr*)&inaddr);
	ret = recvUdp.recvStart([&](upReadBuffer upbuf, const sockaddr* padr, unsigned) {
		string ts(upbuf->buffer, upbuf->size);
		uld("recv str: %s", ts);
		recvstr = ts;
		senderUdp.close();
		recvUdp.close();
	});
	uld("recvUdp open ret=%d", ret);

	ret = senderUdp.init();
	senderUdp.setOnSendLis([&](int status) {
		uld("send callback, status=%d", status);
	});
	senderUdp.setRemoteIpV4Addr("127.0.0.1", 17000);
	uld("senderUdp open ret=%d", ret);
	senderUdp.send(testmsg.data(), testmsg.size());
	senderUdp.recvStart([&](upReadBuffer upbuf, const sockaddr* paddr, unsigned) {
		assert(0);
	});

	UvContext::run();
	UvContext::close();

	ASSERT_STREQ(testmsg.c_str(), recvstr.c_str());
}

TEST(basic, check) {
	UvContext::open();
	UvCheck chk;
	UvTimer timer;
	int ret;
	timer.init();
	ret = chk.init();
	chk.start([&](){
		uld("check call back");
		chk.stop();
	});
	assert(!ret);
	timer.start(1000, 1000, [&]() {
		uld("timer expired");
		timer.stop();
	});
	UvContext::run();
	UvContext::close();
	uld("test end");
}

TEST(basic, prepare) {
	UvContext::open();

	UvPrepare prepare;
	prepare.init();
	prepare.start([&]() {
		uld("prepare callback");
		prepare.stop();
	});
	UvContext::run();
	UvContext::close();
}

TEST(basic, async) {
	UvContext::open();
	int cnt=0;
	UvAsync async;
	async.init([&]() {
		uld("async callback");
		cnt++;
		async.close(nullptr);
	});
	auto ret = async.send();
	assert(!ret);
	UvContext::run();
	UvContext::close();
	ASSERT_EQ(1, cnt);
}

TEST(basic, idle) {
	UvContext::open();
	int cnt=0;
	UvIdle idle;
	idle.init();
	idle.start([&]() {
		uld("idle callback");
		cnt++;
		idle.stop();
	});
	UvContext::run();
	UvContext::close();
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
	UvContext::open();
	int cnt=0;
	int closeCnt=0;
	UvTimer* timer = new UvTimer;
	timer->init();
	timer->start(100, 100, [&]() {
		cnt++;
		timer->stop(false);
		timer->close([&](UvHandle* uvh){
			uld("timer closed");
			closeCnt++;
			delete uvh;
		});
	});
	UvContext::run();
	UvContext::close();
	ASSERT_EQ(1, cnt);
	ASSERT_EQ(1, closeCnt);
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
							uld("timer cnt=%llu", rcnt);
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

	UvContext::open();

	PollFdTimer fdTimer;
	fdTimer.set();
	UvContext::run();
	UvContext::close();
	ASSERT_EQ(1, fdTimer._fireCnt);


}

#endif

TEST(basic, pipe) {
	std::string teststr="test";
	std::string recvstr="test";
#ifdef __linux
	std::string svr_pipename="svr_pipe";
	std::string client_pipename="client_pipe";
#else
	std::string svr_pipename="//./pipe/svr";
	std::string client_pipename="//./pipe/client";
#endif

#ifdef __linux
	unlink(svr_pipename.c_str());
	unlink(client_pipename.c_str());
#endif
	UvContext::open();

	int ret;
	UvPipe pc;
	UvPipe ps;
	UvPipe child;

	ps.init(0);
	ps.bind(svr_pipename.c_str());
	ret = ps.listen([&]() {
		child.init(0);
		auto accret = ps.accept(&child);
		assert(!accret);
		child.setOnCnnLis([&](int status) {
			uld("child status=%d", status);
			if(status) {
				uld("  child disconnected");
				child.close();
				ps.close();
			}
		});
		child.readStart([&](upReadBuffer upbuf) {
			recvstr.assign(upbuf->buffer, upbuf->size);
			uld("recv : %s", recvstr);
			child.write(recvstr);
		});
	});
	ASSERT_EQ(0, ret);

	pc.init(0);
	ret = pc.bind(client_pipename.c_str());
	ASSERT_EQ(0, ret);
	pc.connect(svr_pipename.c_str(), [&](int status) {
		uld("pipe connect event status=%d", status);
		if(!status) {
			pc.readStart([&](upReadBuffer upbuf) {
				uld("client received"); logflush();
				pc.close();
				child.close();
				ps.close();
			});
			pc.write(teststr);
		} else {
			pc.close();

		}
	});
	UvContext::run();
	UvContext::close();
}


#if 0
TEST(basic, fspoll) {
	uint64_t chgsize=0;
	std::ofstream ofs;
	ofs.open ("test.txt");
	UvContext::open();
	UvFsPoll fspoll;
	fspoll.init();
	fspoll.start("test.txt", 1000 , [&](int status, const uv_stat_t *prev, const uv_stat_t* curr) {
		chgsize = curr->st_size;
		fspoll.stop();
	});
	ofs << "1234";
	ofs.flush();
	UvContext::run();
	UvContext::close();
	ofs.close();
	ASSERT_EQ(4, chgsize);
}
#endif

#ifdef __linux
TEST(basic, spawn) {
	UvContext::open();
	UvSpawn sph;
	UvPipe pipe;
	UvPipe in;
	int ret;

	pipe.init(0);
	uv_process_options_t options;
	uv_stdio_container_t child_stdios[3];
	memset(&options, 0, sizeof(options));
	child_stdios[0].flags = UV_IGNORE;
	child_stdios[1].flags = (uv_stdio_flags)(UV_CREATE_PIPE | UV_WRITABLE_PIPE); // child process's perspective
	child_stdios[1].data.stream = (uv_stream_t*)pipe.getRawHandle();
	char cmdpath[128]= "/bin/uname";
	char* args[3];
	args[0] = cmdpath;
	args[1] = NULL;
	args[2] = NULL;
	options.args = args;
	options.file = "/bin/uname";
	options.stdio = child_stdios;
	options.stdio_count = 2;

	ret = sph.spawn(&options, [&](int64_t exit_status, int term_signal) {
		ASSERT_EQ(0, exit_status);
		sph.close();
	});
	ASSERT_EQ(0, ret);
	ret = pipe.readStart([&](upReadBuffer upbuf) {
		std::string ts(upbuf->buffer, upbuf->size);
		ASSERT_STREQ("Linux\n", ts.c_str());
		pipe.close();

	});
	ASSERT_EQ(0, ret);
	UvContext::run();
	UvContext::close();
}
#endif

#if 0
TEST(basic, signal) {
	UvContext::open();
	UvSignal sig;
	int ret;
	ret = sig.init();
	ASSERT_EQ(0, ret);
	ret = sig.start(SIGINT, [&](){
		ali("sigint event");
		sig.stop();
	});
	ASSERT_EQ(0, ret);
	UvContext::run();
	UvContext::close();
}
#endif

#if 0
TEST(basic, fsevent) {
	std::ofstream ofs;
	ofs.open ("test.txt");
	UvContext::open();

	UvFsEvent fse;
	fse.init();
	fse.start("test.txt", UV_FS_EVENT_RECURSIVE ,  [&](const char* fname, int event, int status) {

	});
	UvContext::run();
	UvContext::close();
	ofs.close();
}
#endif

TEST(basic, commonhandle) {
	UvContext::open();
	auto id = UvContext::newCommonHandle();
	ASSERT_EQ(1, id);
	id = UvContext::newCommonHandle();
	ASSERT_EQ(2, id);
	UvContext::run();
	UvContext::close();
}

TEST(basic, work) {
	UvContext::open();
	UvWork work;
	UvTimer tm;
	int cnt=0;
	work.setWork([&]() {
		for(int i=0;i<10;i++) {
			this_thread::sleep_for(milliseconds(100));
			uli("count");
			cnt++;
		}
	});
	work.setOnListener([&](int status) {
		uli("complete, tid=%x", this_thread::get_id());
		tm.close();
	});
	tm.init();
	tm.start(100, 100, []{
		uli("expired");
	});
	work.work();
	UvContext::run();
	UvContext::close();
}

TEST(basic, immediate) {
	UvContext::open();
	Immediate imd;
	int id1=100;
	int id2=200;
	int id3=300;

	imd.setImmediate([id1](uint32_t handle) {
		uld("on imd=%d", id1);
	});

	auto handle = imd.setImmediate([id2](uint32_t handle) {
		uld("on imd=%d", id2);
	});

	imd.setImmediate([id3, &imd](uint32_t handle) {
		uld("on imd=%d", id3);
		imd.close();
	});

	imd.abort(handle);

//	UvTimer endtimer;
//	endtimer.init();
//	endtimer.start(1000, 0, [&]() {
//		endtimer.close();
//	});

	UvContext::run();
	UvContext::close();
}

TEST(basic, immdwrap) {
	UvContext::open();

	int id1=100;
	int id2=200;
	int id3=300;
	uint32_t h1,h2,h3;
	unique_ptr<ImmediateWrapper> upimmd = UvContext::createImmediate();
	h1 = upimmd->setImmediate([&](uint32_t handle) {
		uld("h1 on immd, handle=%u", handle);
	});

	h2 = upimmd->setImmediate([&](uint32_t handle) {
		uld("h2 on immd, handle=%u", handle);
		upimmd->abort(h3);
	});

	h3 = upimmd->setImmediate([&](uint32_t handle) {
		uld("h3 on immd, handle=%u", handle);
	});

//	uint32_t c1, c2;
//	c1 = UvContext::setImmediate([&](uint32_t handle) {
//		ald("c1 on immd");
//	});

	UvTimer endtimer;
	endtimer.init();
	endtimer.start(1000, 0, [&]() {
		endtimer.close();
		upimmd->setImmediate([&](uint32_t handle) {
			uld("end immd");
			upimmd->abortAll();
		});
	});

	UvContext::run();
	UvContext::close();
	uld("after context close");
}
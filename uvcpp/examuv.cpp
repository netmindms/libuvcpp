//============================================================================
// Name        : examuv.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#if 0


#include <iostream>

#include <uv.h>
#include <nmdutil/nmdlog.h>

#include "UvEvent.h"
#include "UvTcp.h"
#include "UvContext.h"
#include "UvFdTimer.h"
#include "UvTcp.h"
#include "UvTimer.h"
#include "UvTimer.h"
using namespace std;
using namespace nmdu;


void on_connect(uv_connect_t* pcnn , int status) {
	ali("on connect, status=%d", status);
	std::string* puserdata = (std::string*)pcnn->data;
	ali("user data : %s", *puserdata);
	if(status == -1) {
		ale("### connection error");
		return;
	}

	getchar();

}
int main() {
	ali("start main");
	auto uvver = uv_version_string();
	ali("uv version: %s", uvver);
	/*
	auto loop = uv_loop_new();
	uv_tcp_t tcphandle;
	auto ret = uv_tcp_init(loop, &tcphandle);
//	uv_tcp_open(&tcphandle, )
	uv_connect_t connect_req;
	struct sockaddr_in req_addr = uv_ip4_addr("127.0.0.1", 7000);

	std::string s="this is userdata";//
//#ifndef LOG_LEVEL
//#define LOG_LEVEL LOG_INFO
//#endif
	connect_req.data = (void*)&s;
	uv_tcp_connect(&connect_req, &tcphandle, req_addr, on_connect);
	uv_run(loop, UV_RUN_DEFAULT);

	uv_loop_delete(loop);
	ali("end main");
	*/

	UvContext ctx;
	ctx.openUv(uv_default_loop());

//	UvTimer timer;
//	timer.set(1000, 1000, [&](){
//		ali("test timer expired");
////		timer.kill();
//	});
//	UvFdTimer fdtimer;
//	fdtimer.set(1000, 1000, [&]() {
//		ali("timer expired");
//		fdtimer.kill();
//	});
	ali("log level=%d", LOG_LEVEL);

	alv("exam main...............");
	UvTcp tcp;

	tcp.open([&](int event) {
		ali("tcp event=%d", event);
		if(event == UvEvent::CONNECTED) {
			std::string ts = "1234\n";
			tcp.write(ts.data(), ts.size());
			tcp.write(ts.data(), ts.size());
			tcp.write(ts.data(), ts.size());
			tcp.write(ts.data(), ts.size());
			tcp.write(ts.data(), ts.size());
			tcp.write(ts.data(), ts.size());
			tcp.write(ts.data(), ts.size());

		} else if(event == UvEvent::DISCONNECTED) {
			ald("disconnected...");
			tcp.close();
		}
	}, [&](std::unique_ptr<UvReadBuffer> upbuf) {
		ald("read event, readcnt=%d", upbuf->size);
		std::string ts(upbuf->buffer, upbuf->size);
		ald("   recv str=%s", ts);
	});
	tcp.connect("127.0.0.1", 7000);
	//tcp.close();

	UvTcp svr, child;
	svr.open([&](int event){
		if(event == UvEvent::INCOMING) {
			ali("incoming connection");
			svr.accept(&child);
			child.setOnCnnLis([&](int event) {
				if (event == UvEvent::DISCONNECTED) {
					ali("child: disconnected");
					child.close();
					svr.close();
					ctx.close();
				}
			});
			child.setOnReadLis([&](upUvReadBuffer upbuf) {
				std::string ts(upbuf->buffer, upbuf->size);
				ali("child: recv %s", ts);
			});
		}
	} ,nullptr);
	svr.listen(9090);
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	uv_loop_close(uv_default_loop());
#if 0
//	ctx.open(uv_default_loop());
	UvTcp sck;
	size_t tbuf_size = 5*1024*1024;
	char *tbuf = new char[tbuf_size];
	memset(tbuf, 0x38, tbuf_size);
	size_t sendCnt = 0;
	UvTimer timer;
	sck.open([&](int event) {
		if(event == UvBaseTcp::SOCK_EVENT_CONNECTED) {
			ali("connected");
			string ts = "me connected";

//			sck.send(tbuf, tbuf_size);
			sck.send(ts.data(), ts.size());
//			sck.close();
			timer.set(1000, 1000, [&]() {
				ali("timer expired....");
				timer.kill();
				sck.close();
			});
		} else if(event == UvBaseTcp::SOCK_EVENT_DISCONNECTED) {
			ali("disconnected");
			sck.close();
		} else if(event == UvBaseTcp::SOCK_EVENT_READ) {
			auto upbuf = sck.fetchReadBuf();
			string ts;
			ts.assign(upbuf.second.get(), upbuf.first);
			ali("read cnt=%d, str=%s", ts.size(), ts);
		} else if(event == UvBaseTcp::SOCK_EVENT_WRITE) {
			size_t sc = tbuf_size - sendCnt;
			auto wcnt = sck.send(tbuf, sc);
			if(wcnt >0 ) {
				sendCnt += wcnt;
			}

			if(wcnt < (int)sc) {
			} else {
//				sck.disableWriteEvent();
			}
			ali("wcnt = %d", wcnt);
		}
	});
	sck.connect("127.0.0.1", 7000);

	uv_run(ctx.getLoop(), UV_RUN_DEFAULT);

//	getchar();
	ctx.close();
#endif
	ali("main end");
	return 0;
}

#endif
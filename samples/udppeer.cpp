//
// Created by netmind on 18. 3. 3.
//


#include <cstdio>
#include "../uvcpp/uvcpp.h"
#include "../uvcpp/uvcpplog.h"

using namespace std;
using namespace uvcpp;

int main(int argc, char* argv[]) {
	UvContext::open();
//	SetLogLevel(UVCLOG_INFO);
	UvUdp udp;
	UvTty tty;
	udp.init();
	udp.bind("0.0.0.0", 4040);
	udp.setRemoteIpV4Addr("127.0.0.1", 4041);
	tty.init(STDIN_FILENO, 1);
	tty.readStart([&](upReadBuffer upbuf) {
		//tcp.write(upbuf->buffer, upbuf->size);
		string ts(upbuf->buffer, upbuf->size);
		if(ts == "q\n") {
			udp.close();
			tty.close();
		} else {
			udp.send(upbuf->buffer, upbuf->size);
		}
	});



	UvContext::run();
	UvContext::close();
}
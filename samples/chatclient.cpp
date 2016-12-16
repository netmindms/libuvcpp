//
// Created by netmind on 16. 12. 16.
//

#include <cstdio>
#include "../uvcpp/uvcpp.h"
#include "../uvcpp/uvcpplog.h"

using namespace std;
using namespace uvcpp;

int main(int argc, char* argv[]) {
	UvContext::open();
	UvTcp tcp;
	UvTty tty;
	tty.init(STDIN_FILENO, 1);
	tty.readStart([&](upReadBuffer upbuf) {
		tcp.write(upbuf->buffer, upbuf->size);
	});

	tcp.init();
	tcp.connect("127.0.0.1", 9090, [&](int status) {
		if(!status) {
			// connected
			tcp.readStart([&](upReadBuffer upbuf) {
				std::string msg(upbuf->buffer, upbuf->size);
				printf("msg: %s", msg.c_str());
//				tcp.recycleReadBuffer(move(upbuf)); // recycling read buffer
			});
		} else {
			// disconnected
			tty.close();
			tcp.close();
		}
	});

	UvContext::run();
	UvContext::close();
}
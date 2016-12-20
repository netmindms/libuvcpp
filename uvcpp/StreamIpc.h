//
// Created by netmind on 16. 12. 20.
//

#ifndef UVCPPPRJ_STREAMIPC_H
#define UVCPPPRJ_STREAMIPC_H

#include "UvPipe.h"
#include "UvTcp.h"

#ifndef __linux
#error "This StreamIpc is not supported in this OS"
#endif

namespace uvcpp {
	class StreamIpc {
	public:
		StreamIpc();

		virtual ~StreamIpc();

		int open(std::function<void()> lis); // call in receive thread
		void close();
		int accept(UvStream* tcp);

		int connectIpc(); // call in send thread
		void disconnectIpc(); // call in send thread
		int sendUvStream(UvStream* strm);
	private:
		std::function<void()> _lis;
		int _fds[2];
		UvPipe _sendPipe;
		UvPipe _recvPipe;
	};
}

#endif //UVCPPPRJ_STREAMIPC_H

//
// Created by netmind on 16. 12. 7.
//

#ifndef UVCPPPRJ_UVPIPE_H
#define UVCPPPRJ_UVPIPE_H


#include "UvStream.h"

namespace uvcpp {
	class UvPipe: public UvStream {
	public:

		virtual ~UvPipe();

		UvPipe();

		int init(int ipc);

		int bind(const char* name);

		void connect(const char* name, UvStream::CnnLis lis);
	private:
		uv_connect_t _cnnReq;
	};
}

#endif //UVCPPPRJ_UVPIPE_H

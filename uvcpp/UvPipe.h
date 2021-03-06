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

		int open(uv_file file);

		int bind(const char* name);

		void connect(const char* name, UvStream::CnnLis lis);

		int getSockName(char* buffer, size_t* size);

		int getPeerName(char* buffer, size_t* size);

		void pendingInstances(int count);

		int pendingCount();

		uv_handle_type pendingType();
	};
}

#endif //UVCPPPRJ_UVPIPE_H

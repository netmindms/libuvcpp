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

		int open(uv_file fd);
	};
}

#endif //UVCPPPRJ_UVPIPE_H

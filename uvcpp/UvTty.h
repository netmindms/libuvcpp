//
// Created by netmind on 16. 12. 7.
//

#ifndef UVCPPPRJ_UVTTY_H
#define UVCPPPRJ_UVTTY_H

#include "UvStream.h"

namespace uvcpp {

	class UvTty : public UvStream {
	public:
		UvTty();

		virtual ~UvTty();

		int open(int readable, uv_file fd);

	};

}

#endif //UVCPPPRJ_UVTTY_H

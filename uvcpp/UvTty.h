//
// Created by netmind on 16. 12. 7.
//

#ifndef UVCPPPRJ_UVTTY_H
#define UVCPPPRJ_UVTTY_H

#include "UvStream.h"

namespace uvcpp {

	class UvTty : public UvStream {
	public:
		static UvTty* init(uv_file fd, int readable);
	private:
		UvTty();

		virtual ~UvTty();

//		int open(int readable, uv_file fd);


	};

}

#endif //UVCPPPRJ_UVTTY_H

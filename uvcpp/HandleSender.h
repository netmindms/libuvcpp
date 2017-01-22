//
// Created by khkim on 2017-01-22.
//

#ifndef UVCPPPRJ_HANDLESENDER_H
#define UVCPPPRJ_HANDLESENDER_H

#include "UvPipe.h"

namespace uvcpp {

	class HandleSender {
	public:
		int connect(const char* name);

		int connect(const std::string& name);

		void close();

		int sendHandle(UvStream* strm);

	private:
		UvPipe _pipe;
		int _status;
	};

}
#endif //UVCPPPRJ_HANDLESENDER_H

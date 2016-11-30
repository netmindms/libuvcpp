//
// Created by netmind on 16. 11. 30.
//

#ifndef UVCPPPRJ_UVHANDLEOWNER_H
#define UVCPPPRJ_UVHANDLEOWNER_H

#include "UvHandle.h"

namespace uvcpp {
	class UvHandleOwner {
	public:
		void closeAsync(UvHandle::CloseLis lis);
	protected:
		UvHandle* _ohandle;
	};
}

#endif //UVCPPPRJ_UVHANDLEOWNER_H

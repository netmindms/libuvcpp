//
// Created by netmind on 16. 11. 30.
//

#include "UvHandleOwner.h"

namespace uvcpp {

	void UvHandleOwner::closeAsync(UvHandle::CloseLis lis) {
		if(_ohandle) {
			_ohandle->setOnCloseListener(lis);
			_ohandle->close(); _ohandle = nullptr;
		}
	}
}

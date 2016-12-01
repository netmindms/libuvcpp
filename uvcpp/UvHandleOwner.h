//
// Created by netmind on 16. 11. 30.
//

#ifndef UVCPPPRJ_UVHANDLEOWNER_H
#define UVCPPPRJ_UVHANDLEOWNER_H

#include "uvcppdef.h"
#include "UvHandle.h"

namespace uvcpp {
	class UvHandleOwner {
	public:
		UvHandleOwner();

		virtual ~UvHandleOwner();

		void closeAsync(UvHandle::CloseLis lis);
		void close();
		template<typename T> static T* getHandleOwner(uv_handle_t* rawh) {
			ASSERT_RAW_UVHANDLE(rawh);
			return GET_UVHANDLE_OWNER(T, rawh);
		}
	protected:
		uv_handle_t* createHandle();
		UvHandle* _ohandle;
		UvContext* _ctx;
	};
}

#endif //UVCPPPRJ_UVHANDLEOWNER_H

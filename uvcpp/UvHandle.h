//
// Created by netmind on 16. 11. 30.
//

#ifndef UVCPPPRJ_UVHANDLE_H
#define UVCPPPRJ_UVHANDLE_H

#include <cassert>
#include <functional>
#include "UvContext.h"

namespace uvcpp {

	class UvContext;
	class UvHandle {
		friend class UvContext;
	public:
		typedef std::function<void()> CloseLis;
		UvHandle();
		virtual ~UvHandle();

		void close();

		uv_handle_t* getRawHandle() {
			return &_rawHandle.handle;
		}

		void* getUserData();
		void setOnCloseListener(CloseLis lis);
	private:
		CloseLis _clis;
		uv_any_handle _rawHandle;
		UvHandle *_next;
		UvHandle *_prev;
		UvContext *_ctx;
		void* _userData;
		int _status;

		int init(void* user_data);
	};
}
#endif //UVCPPPRJ_UVHANDLE_H

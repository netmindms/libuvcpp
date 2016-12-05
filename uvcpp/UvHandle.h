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
		friend class UvHandleOwner;
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
#ifndef NDEBUG
		// for debugging
		std::string _handleName;
#endif
		void* _userData;
		int _status;
		int init(void* user_data);
	};
}

#ifdef NDEBUG
#define GET_UV_HANDLE_NAME(UVH)
#define SET_UV_HANDLE_NAME(UVH, NAME)
#else
#define GET_UV_HANDLE_NAME(UVH) ((const std::string&)std::ref(UVH->_handleName))
#define SET_UV_HANDLE_NAME(UVH, NAME, ID) { char buf[20]; snprintf(buf, UVH->_handleId, "%lu", ID); UVH->_handleName = std::string(NAME) +"_" + buf; }
#endif

#endif //UVCPPPRJ_UVHANDLE_H

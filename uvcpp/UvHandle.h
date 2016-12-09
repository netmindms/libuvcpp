//
// Created by netmind on 16. 11. 30.
//

#ifndef UVCPPPRJ_UVHANDLE_H
#define UVCPPPRJ_UVHANDLE_H


#include <cassert>
#include <functional>
#include <uv.h>

#include "HandleHoder.h"
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

		void close(CloseLis lis=nullptr);

		uv_handle_t* getRawHandle() {
			return &_handleHolder->rawh.handle;
		}


		void* getUserData();
		void setOnCloseListener(CloseLis lis);
		uv_loop_t* getLoop();

		std::unique_ptr<UvWriteInfo> allocWrite();
		int write(const char *buf, size_t len);
		int write(const std::string& msg);
	private:
		CloseLis _clis;
		uv_any_handle _rawHandle;
		UvHandle *_next;
		UvHandle *_prev;
		UvContext *_ctx;
		HandleHolder* _handleHolder;
#ifndef NDEBUG
		// for debugging
		std::string _handleName;
#endif
		void* _userData;
		int _status;
//		int init(void* user_data);
	protected:
		int initHandle();
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

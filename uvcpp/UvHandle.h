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
	public:
		typedef std::function<void()> CloseLis;

		UvHandle();
		virtual ~UvHandle();

		void close(CloseLis lis=nullptr);

		uv_handle_t* getRawHandle();


		uv_loop_t* getLoop();

		int write(const char *buf, size_t len);
		int write(const std::string& msg);
		int send(const char* buf, size_t len, const struct sockaddr* addr);
	private:
		CloseLis _clis;
		UvContext *_ctx;

	protected:
		HandleHolder* _handleHolder;
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

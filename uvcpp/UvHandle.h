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
		enum HANDLE_STATUS {
			IDLE,
			INITIALIZED,
			CLOSING,
		};
		typedef std::function<void()> CloseLis;

		UvHandle();
		virtual ~UvHandle();

		virtual void close(CloseLis lis=nullptr);

		uv_handle_t* getRawHandle();


		uv_loop_t* getLoop();

	private:

		UvContext *_ctx;
	protected:
		HANDLE_STATUS _status;
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

/*
 * UvBaseHandle.h
 *
 *  Created on: Nov 22, 2016
 *      Author: netmind
 */

#ifndef UVBASEHANDLE_H_
#define UVBASEHANDLE_H_

#include <cstdint>
#include <string>
#include <functional>
#include <utility>
#include <memory>
#include <uv.h>

#include "UvContext.h"

namespace uvcpp {

	class UvBaseHandle {
		friend class UvContext;

	public:
		/*
		 * close()를 호출하면 context의 close callback이 반드시 호출되어야 한다.
		 * context의 close callback에서 handle 메모리를 해제 한다.
		 */
		void close();

	protected:
		UvBaseHandle();

		virtual ~UvBaseHandle();

		virtual uv_handle_t *getRawHandle()=0;

		virtual void OnHandleClosed();

	protected:
		UvContext *_ctx;
	private:
		int init();

		UvBaseHandle *_next;
		UvBaseHandle *_prev;
		uv_handle_t *_rawhandle;
	};

}
#endif /* UVBASEHANDLE_H_ */

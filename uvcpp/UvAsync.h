//
// Created by netmind on 16. 11. 30.
//

#ifndef UVCPPPRJ_UVASYNC_H
#define UVCPPPRJ_UVASYNC_H

#include "UvHandle.h"

namespace uvcpp {

	class UvAsync {
	public:
		typedef std::function<void()> Lis;
		UvAsync();

		virtual ~UvAsync();

		int open(Lis lis);

		void close();

		int send();
	private:
		UvHandle* _ohandle;
		Lis _lis;
		static void async_cb(uv_async_t* handle);
	};

}
#endif //UVCPPPRJ_UVASYNC_H

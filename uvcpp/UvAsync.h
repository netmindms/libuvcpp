//
// Created by netmind on 16. 11. 30.
//

#ifndef UVCPPPRJ_UVASYNC_H
#define UVCPPPRJ_UVASYNC_H

#include "UvHandle.h"
#include "UvHandleOwner.h"

namespace uvcpp {

	class UvAsync : public UvHandleOwner {
	public:
		typedef std::function<void()> Lis;
		UvAsync();

		virtual ~UvAsync();

		int open(Lis lis);

		int send();

		void asyncStop();
	private:
		Lis _lis;
		static void async_cb(uv_async_t* handle);
	};

}
#endif //UVCPPPRJ_UVASYNC_H

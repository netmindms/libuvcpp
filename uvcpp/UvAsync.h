//
// Created by netmind on 16. 11. 30.
//

#ifndef UVCPPPRJ_UVASYNC_H
#define UVCPPPRJ_UVASYNC_H

#include "UvHandle.h"
#include "UvHandleOwner.h"

namespace uvcpp {

	class UvAsync : public UvHandle {
	public:
		typedef std::function<void()> Lis;
		static UvAsync* init(Lis lis);
		int send();

	private:
		Lis _lis;

		UvAsync();
		virtual ~UvAsync();
		static void async_cb(uv_async_t* handle);
	};

}
#endif //UVCPPPRJ_UVASYNC_H

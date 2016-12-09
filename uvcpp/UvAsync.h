//
// Created by netmind on 16. 11. 30.
//

#ifndef UVCPPPRJ_UVASYNC_H
#define UVCPPPRJ_UVASYNC_H

#include "UvHandle.h"

namespace uvcpp {

	class UvAsync : public UvHandle {
	public:
		typedef std::function<void()> Lis;
		UvAsync();

		virtual ~UvAsync();

		int init(Lis lis);

		int send();

	private:
		Lis _lis;
		static void async_cb(uv_async_t* handle);
	};

}
#endif //UVCPPPRJ_UVASYNC_H

//
// Created by netmind on 16. 11. 30.
//

#ifndef UVCPPPRJ_UVCHECK_H
#define UVCPPPRJ_UVCHECK_H

#include <functional>
#include "UvHandle.h"
#include "UvHandleOwner.h"

namespace uvcpp {
	class UvCheck : public UvHandleOwner  {
	public:
		typedef std::function<void()> Lis;
		UvCheck();

		virtual ~UvCheck();
		int open(Lis lis);

		void close(UvHandle::CloseLis lis) override;

	private:
		Lis _lis;
		static void check_cb(uv_check_t* handle);
	};
}

#endif //UVCPPPRJ_UVCHECK_H

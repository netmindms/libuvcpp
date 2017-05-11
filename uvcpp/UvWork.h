//
// Created by netmind on 17. 5. 10.
//

#ifndef UVCPPPRJ_UVWORK_H
#define UVCPPPRJ_UVWORK_H

#include <uv.h>
#include <functional>

namespace uvcpp {

	class UvWork {
	public:
		UvWork();

		void setWork(std::function<void()> workcb);

		void setOnListener(std::function<void(int)> completecb);

		int work();

		void cancel();

	private:
		std::function<void()> _work;

		std::function<void(int)> _completeLis;

		uv_work_t _work_req;

		static void work_cb(uv_work_t* req);

		static void after_work_cb(uv_work_t* req, int status);


	};

}
#endif //UVCPPPRJ_UVWORK_H

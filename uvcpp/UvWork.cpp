//
// Created by netmind on 17. 5. 10.
//

#include "UvWork.h"
#include "UvHandle.h"

namespace uvcpp {
	UvWork::UvWork() {
		_work_req.data = nullptr;
	}

	int UvWork::work() {
		if(!_work_req.data) {
			_work_req.data = this;
			return uv_queue_work(UvContext::getContext()->getLoop(), &_work_req, work_cb, after_work_cb);
		} else {
			return -1;
		}
	}

	void UvWork::work_cb(uv_work_t *req) {
		auto workobj = (UvWork*)req->data;
		workobj->_work();
	}

	void UvWork::after_work_cb(uv_work_t *req, int status) {
		auto workobj = (UvWork*)req->data;
		workobj->_completeLis(status);
	}

	void UvWork::setWork(std::function<void()> workcb) {
		_work = workcb;
	}

	void UvWork::setOnListener(std::function<void(int)> complete_lis) {
		_completeLis = complete_lis;
	}

	void UvWork::cancel() {
		if(_work_req.data) {
			uv_cancel((uv_req_t*)&_work_req);
		}
	}


}
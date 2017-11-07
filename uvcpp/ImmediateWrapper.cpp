//
// Created by netmind on 17. 11. 3.
//

#include "ImmediateWrapper.h"
#include "uvcpplog.h"

namespace uvcpp {

	ImmediateWrapper::ImmediateWrapper(Immediate* immd) {
		_immd = immd;
	}

	ImmediateWrapper::~ImmediateWrapper() {
		close();
	}

	void ImmediateWrapper::close() {
		if(_immd) {
			for (; _handles.size();) {
				auto handle = _handles.front();
				_handles.pop_front();
				_immd->abort(handle);
			}
		}
		_immd = nullptr;
	}

	uint32_t ImmediateWrapper::setImmediate(std::function<void(uint32_t)> lis) {
		auto handle = _immd->setImmediate([this, lis](uint32_t th) {
			for(auto itr=_handles.begin(); itr != _handles.end(); itr++) {
				if(*itr == th) {
					_handles.erase(itr);
					break;
				}
			}
			lis(th);
		});
		_handles.push_back(handle);
		if(_handles.size()==0) {
			_immd = nullptr;
		}
		return handle;
	}

	void ImmediateWrapper::abort(uint32_t handle) {
		if(_immd) {
			for(auto itr = _handles.begin();itr != _handles.end(); itr++) {
				if(*itr == handle) {
					_handles.erase(itr);
					break;
				}
			}
			_immd->abort(handle);
			if(_handles.size()==0) {
				_immd = nullptr;
			}
		}
	}

	std::unique_ptr<ImmediateWrapper> ImmediateWrapper::create(Immediate *immd) {
		ImmediateWrapper* wrp = new ImmediateWrapper(immd);
		return std::unique_ptr<ImmediateWrapper>(wrp);
	}
}
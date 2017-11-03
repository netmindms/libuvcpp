//
// Created by netmind on 17. 10. 31.
//

#include "Immediate.h"
#include "uvcpplog.h"

using namespace std;

namespace uvcpp {

	uint32_t Immediate::setImmediate(std::function<void(uint32_t)> lis) {
		if(!_async.getRawHandle()) {
			_async.init([this]() {
				for(;_msgList.size();) {
					auto upmsg = move(_msgList.front());
					_msgList.pop_front();
					upmsg->lis(upmsg->handle);
				}
			});
		}
		if(++_handleSeed == 0) ++_handleSeed;
		unique_ptr<Msg> upmsg(new Msg(_handleSeed, lis));
		_msgList.emplace_back(move(upmsg));
		_async.send();
		return _handleSeed;
	}

	void Immediate::abort(uint32_t handle) {
		for(auto itr=_msgList.begin(); itr != _msgList.end(); itr++) {
			if( (*itr)->handle == handle) {
				_msgList.erase(itr);
				break;
			}
		}
	}

	void Immediate::close() {
		_msgList.clear();
		_async.close();
	}

	Immediate::Immediate() {
		_handleSeed = 0;
	}

	Immediate::Msg::Msg(uint32_t handle, std::function<void(uint32_t)> cb) {
		this->handle = handle;
		this->lis =  cb;
	}
}
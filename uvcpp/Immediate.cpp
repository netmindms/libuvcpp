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
				_async.close();
				// listener callback에서 immediate를 설정하는 경우 _msgList에  새로운 event가 추가 되어
				// loop를 계속 돌게 되므로 새로 등록한 event는 다음 event loop에서
				// 실행 되도록 하기위해 _msgList를 임시 list로 옮기고 임시 list에서 event를 꺼내와
				// callback을 실행 시킨다.
				std::list<std::unique_ptr<Msg>> tmplist = move(_msgList);
				for(;tmplist.size();) {
					auto upmsg = move(tmplist.front());
					tmplist.pop_front();
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
		if(_msgList.size()==0) {
			_async.close();
		}
	}

	void Immediate::close() {
		_msgList.clear();
		_async.close();
	}

	Immediate::Immediate() {
		_handleSeed = 0;
	}

	Immediate::~Immediate() {
	}

	Immediate::Msg::Msg(uint32_t handle, std::function<void(uint32_t)> cb) {
		this->handle = handle;
		this->lis =  cb;
	}
}
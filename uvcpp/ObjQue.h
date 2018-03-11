/*
 * BufferQue.h
 *
 *  Created on: Apr 22, 2016
 *      Author: netmind
 */

#ifndef UVCPPPRJ_OBJQUE_H_
#define UVCPPPRJ_OBJQUE_H_

#include <list>
#include <memory>
#include <mutex>
#include <string>

namespace uvcpp {
template<typename T>
class ObjQue {
public:
	ObjQue() {
		_allocCnt = 0;
	};
	virtual ~ObjQue() {
	};


	void clear() {
		_que.clear();
		_containerStore.clear();
		_recycleStore.clear();
		_allocCnt = 0;
	}

	void push_back(std::unique_ptr<T> obj) {
		if(_containerStore.empty()) {
			// container 가 없으면 하나 생성한다.
			_containerStore.emplace_back();
		}

		// container에 obj 를 담는다.
		auto &pobj = _containerStore.front();
		pobj = std::move(obj);

		// 그리고 나서 que로 옮긴다.
		_que.splice(_que.end(), _containerStore, _containerStore.begin());
	};

	std::unique_ptr<T> pop_front() {
		auto itr = _que.begin();
		if(itr != _que.end()) {
			// que에 있는 pointer를 가져온다.(move)
			auto up = std::move(*itr);

			// 비어 있는 상태가 된 que의 container를 container pool로 옮긴다.
			_containerStore.splice(_containerStore.end(), _que, _que.begin());
			return up;
		} else {
			return nullptr;
		}
	};


	std::unique_ptr<T> allocObj() {
		std::unique_ptr<T> ret;
		if(_recycleStore.size()) {
			ret = std::move(_recycleStore.front());
			_containerStore.splice(_containerStore.begin(), _recycleStore, _recycleStore.begin());
		} else {
			_allocCnt++;
			ret.reset(new T);
		}
		return ret;
	}

	size_t size() {
		return _que.size();
	}

	void recycle(std::unique_ptr<T> upobj) {
		if(upobj==nullptr) return;
		if(_containerStore.empty()) {
			// container 가 없으면 하나 만든다.
			_containerStore.emplace_back();
		}

		// container로 재활용할 obj pointer 로 옮긴다.
		auto &pobj = _containerStore.front();
		pobj = std::move(upobj);

		// mem pool로 옮긴다.
		_recycleStore.splice(_recycleStore.end(), _containerStore, _containerStore.begin());
	}

	void recycleObjs(std::list<std::unique_ptr<T>>& objlist) {
		_recycleStore.splice(_recycleStore.begin(), objlist);
	}


	void push_back_list(std::list<T>& objs) {
		_que.splice(_que.end(), objs);
	}

	void pop_list(std::list<std::unique_ptr<T>>& objs) {
		objs.splice(objs.end(), _que);
	}


	void pushSync(std::unique_ptr<T> obj) {
		lock();
		push_back(std::move(obj));
		unlock();
	}

	std::unique_ptr<T> pop_front_sync() {
		lock();
		auto up = pop_front();
		unlock();
		return up;
	}

	void recycleSync(std::unique_ptr<T> upobj) {
		lock();
		recycleObj(std::move(upobj));
		unlock();
	}


	std::unique_ptr<T> allocObjSync() {
		lock();
		auto up = allocObj();
		unlock();
		return up;
	}


	size_t getMemPoolCnt() {
		return _recycleStore.size();
	}


	T* getFirstObj() {
		if(_que.size()) {
			return _que.front().get();
		} else {
			return nullptr;
		}
	}


	std::string getStatistics() {
		std::string ret;
		ret = "alloc_cnt: " + std::to_string(_allocCnt) + ", ";
		ret += "mem_pool: " + std::to_string(_recycleStore.size()) + ", ";
		ret += "que : " + std::to_string(_que.size()) + ", ";
		ret += "item_pool: " + std::to_string(_containerStore.size()) + ", ";
		return std::move(ret);
	}

	void lock() {
		_mutex.lock();
	}
	void unlock() {
		_mutex.unlock();
	}

private:
	size_t _maxQue;
	std::list<std::unique_ptr<T>> _que;
	std::list<std::unique_ptr<T>> _containerStore;
	std::list<std::unique_ptr<T>> _recycleStore;
	size_t _allocCnt;
	std::mutex _mutex;
};

} // namespace uvcpp

#endif /* UVCPPPRJ_OBJQUE_H_ */

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
		_containerBufs.clear();
		_memPool.clear();
		_allocCnt = 0;
	}

	void push(std::unique_ptr<T> obj) {
		if(_containerBufs.empty()) {
			// container 가 없으면 하나 생성한다.
			_containerBufs.emplace_back();
		}

		// container에 obj 를 담는다.
		auto &pobj = _containerBufs.front();
		pobj = std::move(obj);

		// 그리고 나서 que로 옮긴다.
		_que.splice(_que.end(), _containerBufs, _containerBufs.begin());
	};

	void pushSync(std::unique_ptr<T> obj) {
		lock();
		push(std::move(obj));
		unlock();
	}

	std::unique_ptr<T> pop_front() {
		auto itr = _que.begin();
		if(itr != _que.end()) {
			// que에 있는 pointer를 가져온다.(move)
			auto up = std::move(*itr);

			// 비어 있는 상태가 된 que의 container를 container pool로 옮긴다.
			_containerBufs.splice(_containerBufs.end(), _que, _que.begin());
			return up;
		} else {
			return nullptr;
		}
	};

	std::unique_ptr<T> pop_front_sync() {
		lock();
		auto up = pop_front();
		unlock();
		return up;
	}

	size_t getQueCnt() {
		size_t ret;
		ret = _que.size();
		return ret;
	}

	void recycleObj(std::unique_ptr<T> upobj) {
		if(upobj==nullptr) return;
		if(_containerBufs.empty()) {
			// container 가 없으면 하나 만든다.
			_containerBufs.emplace_back();
		}

		// container로 재활용할 obj pointer 로 옮긴다.
		auto &pobj = _containerBufs.front();
		pobj = std::move(upobj);

		// mem pool로 옮긴다.
		_memPool.splice(_memPool.end(), _containerBufs, _containerBufs.begin());
	}

	void recycleObjSync(std::unique_ptr<T> upobj) {
		lock();
		recycleObj(std::move(upobj));
		unlock();
	}


	std::unique_ptr<T> allocObj() {
		std::unique_ptr<T> ret;
		if(_memPool.size()) {
			ret = std::move(_memPool.front());
			_containerBufs.splice(_containerBufs.begin(), _memPool, _memPool.begin());
		} else {
			_allocCnt++;
			ret.reset(new T);
		}
		return ret;
	}
	std::unique_ptr<T> allocObjSync() {
		lock();
		auto up = allocObj();
		unlock();
		return up;
	}


	size_t getMemPoolCnt() {
		return _memPool.size();
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
		ret += "mem_pool: " + std::to_string(_memPool.size()) + ", ";
		ret += "que : " + std::to_string(_que.size()) + ", ";
		ret += "item_pool: " + std::to_string(_containerBufs.size()) + ", ";
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
	std::list<std::unique_ptr<T>> _containerBufs;
	std::list<std::unique_ptr<T>> _memPool;
	size_t _allocCnt;
	std::mutex _mutex;
};

} // namespace uvcpp

#endif /* UVCPPPRJ_OBJQUE_H_ */

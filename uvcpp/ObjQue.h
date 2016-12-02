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
		mMaxQue = 100;
		mAllocCnt = 0;
	};
	virtual ~ObjQue() {
		close();
	};
	int open(size_t maxq) {
		mMaxQue = maxq;
		return 0;
	};

	void close() {
		mQue.clear();
		mPool.clear();
		mMemPool.clear();
		mAllocCnt = 0;
	}

	void clear() {
		mQue.clear();
		mPool.clear();
		mMemPool.clear();
		mAllocCnt = 0;
	}

	void push(std::unique_ptr<T> obj) {
		if(mPool.empty()) {
			mPool.emplace_back();
		}
		auto &pobj = mPool.front();
		pobj = std::move(obj);
		mQue.splice(mQue.end(), mPool, mPool.begin());
	};

	void pushSync(std::unique_ptr<T> obj) {
		lock();
		push(std::move(obj));
		unlock();
	}

	std::unique_ptr<T> pop() {
		auto itr = mQue.begin();
		if(itr != mQue.end()) {
			auto up = std::move(*itr);
			mPool.splice(mPool.end(), mQue, mQue.begin());
			return up;
		} else {
			return nullptr;
		}
	};

	std::unique_ptr<T> popSync() {
		lock();
		auto up = pop();
		unlock();
		return up;
	}

	size_t getQueCnt() {
		size_t ret;
		ret = mQue.size();
		return ret;
	}

	void recycleObj(std::unique_ptr<T> upobj) {
		if(upobj==nullptr) return;
		if(mPool.empty()) {
			mPool.emplace_back();
		}
		auto &pobj = mPool.front();
		pobj = std::move(upobj);
		mMemPool.splice(mMemPool.end(), mPool, mPool.begin());
	}

	void recycleObjSync(std::unique_ptr<T> upobj) {
		lock();
		recycleObj(std::move(upobj));
		unlock();
	}


	std::unique_ptr<T> allocObj() {
		std::unique_ptr<T> ret;
		if(mMemPool.size()) {
			ret = std::move(mMemPool.front());
			mPool.splice(mPool.begin(), mMemPool, mMemPool.begin());
		} else {
			mAllocCnt++;
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
		return mMemPool.size();
	}


	T* getFirstObj() {
		if(mQue.size()) {
			return mQue.front().get();
		} else {
			return nullptr;
		}
	}


	std::string getStatistics() {
		std::string ret;
		ret = "alloc_cnt: " + std::to_string(mAllocCnt) + ", ";
		ret += "mem_pool: " + std::to_string(mMemPool.size()) + ", ";
		ret += "que : " + std::to_string(mQue.size()) + ", ";
		ret += "item_pool: " + std::to_string(mPool.size()) + ", ";
		return std::move(ret);
	}

	void lock() {
		_mutex.lock();
	}
	void unlock() {
		_mutex.unlock();
	}

private:
	size_t mMaxQue;
	std::list<std::unique_ptr<T>> mQue;
	std::list<std::unique_ptr<T>> mPool;
	std::list<std::unique_ptr<T>> mMemPool;
	size_t mAllocCnt;
	std::mutex _mutex;
};

} // namespace uvcpp

#endif /* UVCPPPRJ_OBJQUE_H_ */

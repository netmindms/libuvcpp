//
// Created by netmind on 17. 6. 12.
//

#if 0
#include <string>
#include <gtest/gtest.h>
#include <functional>
#include <memory>
#include <uvcpp/UvAsync.h>

using namespace uvcpp;

class PromiseResult {
public:
	PromiseResult(){};
	virtual ~PromiseResult(){};
};

class IntPromseResult: public PromiseResult {
public:
	int val;
};

typedef std::unique_ptr<PromiseResult> upPromiseResult;
typedef std::function<void (upPromiseResult)> ResultCallback;
typedef std::function<void (std::string)> ErrorCallback;
class EventSerial {
public:
	EventSerial() {
		_resolve = [this](upPromiseResult res) {
			_resolveResult = move(res);
			_async.send();
		};

		_reject = [this](std::string err) {

		};
	}
	void resolve(upPromiseResult result) {
		_resolveResult = move(result);
		_async.send();
	}

	void reject(std::string err) {
		_errMsg = err;
		_async.send();
	}

	EventSerial* then(std::function<void(upPromiseResult)> cb) {
		_thenCb = cb;
		_next.reset(new EventSerial);
		return _next.get();
	}

	void doFunc(std::function<void(ResultCallback rcb, ErrorCallback rjb)> cb) {
		_async.init([this]() {
			if(_resolveResult) {
				_thenCb(move(_resolveResult));
			} else {
				_errCb(_errMsg);
			}
		});
		cb(_resolve, _reject);
	}


private:
	upPromiseResult _resolveResult;
	std::string _errMsg;
	ResultCallback _resolve;
	ErrorCallback _reject;
	uvcpp::UvAsync _async;
	std::unique_ptr<EventSerial> _next;

	std::function<void(ResultCallback rcb, ErrorCallback rjb)> _doFunc;

	std::function<void (upPromiseResult)> _thenCb;
	std::function<void(std::string)> _errCb;
};

TEST(eventserial, basic) {
	UvContext::open();

	EventSerial* evs;
	evs->doFunc([](ResultCallback res_cb, ErrorCallback err_cb) {

	});
	evs->then([](upPromiseResult res) {
		//...

	})
	->then([](upPromiseResult res) {

	})
	;




//	evs->then([](upPromiseResult res) {
//
//	})->then([](upPromiseResult res) {
//
//	});


	UvContext::run();
	UvContext::close();

}
#endif
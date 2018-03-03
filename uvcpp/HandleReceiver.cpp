//
// Created by khkim on 2017-01-22.
//

#include <cassert>
#include "HandleReceiver.h"
#include "uvcpplog.h"

using namespace std;

namespace uvcpp {

	int HandleReceiver::open(const std::string& pipename, Lis lis) {
		int ret;
		_lis = lis;
		_svr.init(0);
		_pipeName = pipename;
		ret = _svr.bind(pipename.c_str());
		if(ret) {
			_svr.close();
			assert(0);
			return -1;
		}
		_svr.listen([this]() {
			uld("incoming handle pass connection");
			_pipeList.emplace_back();
			auto &pipe = _pipeList.back();
			pipe.itr = --_pipeList.end();
			pipe.init(1);
			int ret = _svr.accept(&pipe);
			assert(0==ret);
			pipe.setOnCnnLis([this, &pipe](int status) {
				if(status) {
					uld("handle pipe disconnected");
					pipe.close();
					_pipeList.erase(pipe.itr);
				}
			});
			ret = pipe.readStart([this, &pipe](upReadBuffer upbuf) {
				uld("pipe read event");
				if(pipe.pendingCount()>0) {
					uli("  pipe pendingcount=%d", pipe.pendingCount());
					uv_handle_type type = pipe.pendingType();
					_lis(&pipe, type);
					uld("  accept after pendingcount=%d", pipe.pendingCount());
				}
			});
			assert(ret==0);
		});
		return 0;
	}

	void HandleReceiver::close() {
		_svr.close();
		for(auto &pipe : _pipeList) {
			pipe.close();
		}
		_pipeList.clear();
#ifdef __linux
		unlink(_pipeName.c_str());
#endif
	}


}
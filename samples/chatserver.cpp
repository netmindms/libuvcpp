//
// Created by netmind on 16. 12. 16.
//

#include <unordered_map>
#include "../uvcpp/uvcpp.h"
#include "../uvcpp/uvcpplog.h"

using namespace std;

using namespace uvcpp;

namespace {


	class Server;

	class ChildCnn {
	public:
		int _id;
		UvTcp _tcp;

		ChildCnn();

		void connectionInit(int id, Server *psvr);

		void recvOtherMsg(const std::string& msg);

		virtual ~ChildCnn();

		Server *_server;
	};



	class Server {
	public:
		int init() {
			initDummyCleaner();
			_idSeed = 0;
			_serverTcp.init();
			auto ret = _serverTcp.bindAndListen(9090, "0.0.0.0", [this]() {
				ChildCnn *pcnn = new ChildCnn();
				pcnn->_tcp.init();
				_serverTcp.accept(&(pcnn->_tcp));
				auto id = _idSeed++;
				pcnn->connectionInit(id, this);
				_clientMap[id].reset(pcnn);
				printf("new connection id=%d, total client=%lu\n", id, _clientMap.size());
			});
			if(!ret) {
				printf("server start on tcp port=%d\n", 9090);
			} else {
				printf("### listen or bind error\n");

			}
			return ret;
		}

		void initDummyCleaner() {
			_dummyCheck.init();
			_dummyCheck.start([this]() {
				if(_dummyChild.size()) {
					printf("dummy clients = %lu\n", _dummyChild.size());
					_dummyChild.clear();
				}
			});
		}

		void leaveChat(int id) {
			auto itr = _clientMap.find(id);
			if(itr != _clientMap.end()) {
				_dummyChild.emplace_back(move(itr->second));
				_clientMap.erase(itr);
				printf("client id=%d leaved, total_client=%lu\n", id, _clientMap.size());
			}
		}

		void sendOthers(int i, char *string, size_t len) {
			std::string msg(string, len);
			for(auto &c : _clientMap) {
				if(c.first != i) {
					c.second->recvOtherMsg(msg);
				}
			}
		}

		void close() {
			_dummyChild.clear();
			_clientMap.clear();
			_dummyCheck.close();
			_serverTcp.close();
		}

		int _idSeed;
		UvTcp _serverTcp;
		UvCheck _dummyCheck;

		unordered_map<int, std::unique_ptr<ChildCnn>> _clientMap;
		list<unique_ptr<ChildCnn>> _dummyChild;
	};

	ChildCnn::ChildCnn() {
		_id = -1;
	}

	ChildCnn::~ChildCnn() {
		_tcp.close();
	}

	void ChildCnn::connectionInit(int id, Server *psvr) {
		_server = psvr;
		_id = id;
		_tcp.setOnCnnLis([this](int status) {
			if(status) {
				// disconnected
				_tcp.close();
				_server->leaveChat(_id);
			}

		});
		_tcp.readStart([this](upReadBuffer upbuf) {
			std::string msg(upbuf->buffer, upbuf->size);
			printf("recv msg, msg=%s, len=%u\n", msg.c_str(), msg.size());
			_server->sendOthers(_id, upbuf->buffer, upbuf->size);
		});

	}

	void ChildCnn::recvOtherMsg(const std::string& msg) {
		_tcp.write(msg);
	}
}




int main(int argc, char* argv[]) {
	UvContext::open();
//	uvcpp::SetLogLevel(LOG_VERBOSE);
	Server chatsvr;
	auto ret = chatsvr.init();
	if(ret) {
		chatsvr.close();
	}
	UvContext::run();
	UvContext::close();
}
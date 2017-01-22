//
// Created by khkim on 2017-01-22.
//

#ifndef UVCPPPRJ_HANDLERECEIVER_H
#define UVCPPPRJ_HANDLERECEIVER_H
#include "UvPipe.h"

namespace uvcpp {
	class HandleReceiver {
	public:
		typedef std::function<void(UvPipe* pipe, uv_handle_type)> Lis;

		/*
		 *  In Windows, name must be started with //./pipe/ . ex) //./pipe/mypipename
		 *
		 */
		int open(const char* name, Lis lis);

		int open(const std::string& name, Lis lis);

		void close();

		void accept(UvPipe *pipe, UvHandle* handle);

	private:
		Lis _lis;
		class HandlePipe : public uvcpp::UvPipe {
			friend class HandleReceiver;
			std::list<HandlePipe>::iterator itr;
		};
		uvcpp::UvPipe _svr;
		std::list<HandlePipe> _pipeList;
	};
}

#endif //UVCPPPRJ_HANDLERECEIVER_H

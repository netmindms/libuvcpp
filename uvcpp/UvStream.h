//
// Created by netmind on 16. 12. 7.
//

#ifndef UVCPPPRJ_UVSTREAM_H
#define UVCPPPRJ_UVSTREAM_H

#include "UvWriteInfo.h"
#include "UvReadBuffer.h"
#include "ObjQue.h"
#include "UvHandle.h"

namespace uvcpp {
	class UvStream : public UvHandle {
		friend class UvContext;
	public:
		typedef std::function<void(std::unique_ptr<UvReadBuffer>)> ReadLis;
		typedef std::function<void()> ListenLis;
		typedef std::function<void(int)> CnnLis;
		UvStream();

		virtual ~UvStream();

		int readStart(ReadLis lis);

		int readStop();

		int listen(ListenLis lis, int backlogs=128);

		int accept(UvStream* newtcp);

		int write(const char *buf, size_t len);

		int write(const std::string &msg);

		void setOnCnnLis(CnnLis lis);

	protected:
		uv_connect_t* setConnectionReq(CnnLis lis);

	private:
		ReadLis _readLis;
		CnnLis _cnnLis;
		ListenLis _listenLis;

		void procReadCallback(upUvReadBuffer upbuf);
		void procConnectCallback(int status);
		void procListenCallback(int status);
	};
}

#endif //UVCPPPRJ_UVSTREAM_H

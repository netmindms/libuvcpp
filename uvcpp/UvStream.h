//
// Created by netmind on 16. 12. 7.
//

#ifndef UVCPPPRJ_UVSTREAM_H
#define UVCPPPRJ_UVSTREAM_H

#include "UvWriteInfo.h"
#include "ReadBuffer.h"
#include "ObjQue.h"
#include "UvHandle.h"

namespace uvcpp {
	class UvStream : public UvHandle {
		friend class UvContext;
	public:
		typedef std::function<void(std::unique_ptr<ReadBuffer>)> ReadLis;
		typedef std::function<void()> ListenLis;
		typedef std::function<void(int)> CnnLis;
		typedef std::function<void(int)> ShutdownLis;

		UvStream();

		virtual ~UvStream();

		int readStart(ReadLis lis=nullptr);

		int readStop();

		int listen(ListenLis lis, int backlogs=128);

		int accept(UvStream* newtcp);

		int write(const char *buf, size_t len);

		int write(const std::string &msg);

		int write2(UvStream* send_handle);

		int tryWrite(const char *buf, size_t len);

		int shutDown(ShutdownLis lis);

		int isReadable();

		int isWritable();

		int setBlocking(int blocking);

		void recycleReadBuffer(upReadBuffer upbuf);

		void setOnCnnLis(CnnLis lis);

		void setOnReadLis(ReadLis lis);

		uint32_t getWriteQueCnt();

		uint32_t getSendQueCnt();

	protected:
		uv_connect_t* setConnectionReq(CnnLis lis);

	private:
		ReadLis _readLis;
		CnnLis _cnnLis;
		ListenLis _listenLis;
		ShutdownLis _shutdownLis;

		void procReadCallback(upReadBuffer upbuf);
		void procConnectCallback(int status);
		void procListenCallback(int status);
		void procShutdownCallback(int status);
	};
}

#endif //UVCPPPRJ_UVSTREAM_H

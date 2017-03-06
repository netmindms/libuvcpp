//
// Created by netmind on 16. 11. 28.
//

#ifndef UVCPPPRJ_UVUDP_H
#define UVCPPPRJ_UVUDP_H

#include <cstdint>
#include <functional>
#include "ObjQue.h"
#include "ReadBuffer.h"
#include "UvUdpWriteInfo.h"
#include "UvHandle.h"

namespace uvcpp {

	class UvUdp : public UvHandle {
		friend class UvContext;
	public:
		typedef std::function<void(std::unique_ptr<ReadBuffer>, const struct sockaddr *, unsigned)> RecvLis;
		typedef std::function<void(int)> SendLis;
		UvUdp();
		virtual ~UvUdp();
		int init();
		int initEx(unsigned int flags);
		int bind(const struct sockaddr *addr, unsigned int flags=UV_UDP_REUSEADDR);
		int bind(const char *ipaddr, uint16_t port, unsigned int flags=UV_UDP_REUSEADDR);
		int send(const char* buf, size_t len, const sockaddr* addr);
		int send(const char* buf, size_t len);
		int send(const std::string& msg);
		int send(const std::string& msg, const sockaddr* addr);
		void setRemoteIpV4Addr(const char* ipaddr, uint16_t port);
		int recvStart(RecvLis lis=nullptr);
		int recvStop();
		void setOnRecvLis(RecvLis lis);
		void setOnSendLis(SendLis lis);
		int trySend(const char* bufs, size_t len, const struct sockaddr* addr);
		int getsSockName(struct sockaddr* name, int* namelen);
		int setMemberShip(const char* multicast_addr, const char* interface_addr, uv_membership membership);
		int setMulticastLoop(int on);
		int setMulticastTtl(int ttl);
		int setMulticastInterface(const char* interface_addr);
		int setBroadcast(int on);
		int setTtl(int ttl);
	private:
		sockaddr* _remoteAddr;
		RecvLis _recvLis;
		SendLis _sendLis;

		void procRecvCallback(upReadBuffer upbuf, const struct sockaddr *addr, unsigned flags);
		void procSendCallback(int status);
	};

}

#endif //UVCPPPRJ_UVUDP_H

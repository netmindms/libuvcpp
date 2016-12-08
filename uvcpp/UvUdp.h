//
// Created by netmind on 16. 11. 28.
//

#ifndef UVCPPPRJ_UVUDP_H
#define UVCPPPRJ_UVUDP_H

#include <cstdint>
#include <functional>
#include "ObjQue.h"
#include "UvReadBuffer.h"
#include "UvHandleOwner.h"
#include "UvUdpWriteInfo.h"

namespace uvcpp {

	class UvUdp : public UvHandle {
	public:
		typedef std::function<void(const struct sockaddr *, std::unique_ptr<UvReadBuffer>)> Lis;

		static UvUdp* init();
		int bind(const struct sockaddr *addr, unsigned int flags=UV_UDP_REUSEADDR);
		int bind(const char *ipaddr, uint16_t port, unsigned int flags=UV_UDP_REUSEADDR);
		int send(const char* buf, size_t len, const sockaddr* addr);
		int send(const char* buf, size_t len);
		int send(const std::string& msg);
		int send(const std::string& msg, const sockaddr* addr);
		void setRemoteIpV4Addr(const char* ipaddr, uint16_t port);

		int recvStart(Lis lis);
		void recvStop();
	private:
		sockaddr* _remoteAddr;
		Lis _readLis;
		ObjQue<UvUdpWriteInfo> _writeReqQue;
		ObjQue<UvReadBuffer> _readBufQue;

		static void alloc_cb(uv_handle_t *handle, size_t suggesited_size, uv_buf_t *puvbuf);
		static void recv_cb(uv_udp_t *handle, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr, unsigned flags);
		static void send_cb(uv_udp_send_t *req, int status);

		UvUdp();
		virtual ~UvUdp();
	};

}

#endif //UVCPPPRJ_UVUDP_H

/*
 * UvEvent.h
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#ifndef UVCPPPRJ_UVEVENT_H_
#define UVCPPPRJ_UVEVENT_H_

namespace uvcpp {

	enum UvStreamEvent {
		DISCONNECTED = 0,
		CONNECTED = 1,
		INCOMING = 4,
		READABLE = 2,
		WRITABLE = 3,
	};

}

#endif /* UVCPPPRJ_UVEVENT_H_ */

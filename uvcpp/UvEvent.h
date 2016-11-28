/*
 * UvEvent.h
 *
 *  Created on: Nov 25, 2016
 *      Author: netmind
 */

#ifndef UVEVENT_H_
#define UVEVENT_H_

namespace uvcpp {

	enum UvEvent {
		DISCONNECTED = 0,
		CONNECTED = 1,
		INCOMING = 4,
		READABLE = 2,
		WRITABLE = 3,
	};

}

#endif /* UVEVENT_H_ */

//
// Created by netmind on 16. 12. 1.
//

#ifndef UVCPPPRJ_UVOBJ_H
#define UVCPPPRJ_UVOBJ_H


#include <memory>

namespace uvcpp {
	class UvObj {
	public:
		UvObj();

		virtual ~UvObj();
	};

	typedef std::unique_ptr<UvObj> upUvObj;
}

#endif //UVCPPPRJ_UVOBJ_H

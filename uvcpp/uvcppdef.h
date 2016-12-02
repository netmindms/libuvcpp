//
// Created by netmind on 16. 11. 30.
//

#ifndef UVCPPPRJ_UVCPPDEF_H
#define UVCPPPRJ_UVCPPDEF_H

#include <cassert>
#define GET_RAWH(RAWTYPE, UVHANDLE)  ( (RAWTYPE*)UVHANDLE->getRawHandle() )

#define ASSERT_RAW_UVHANDLE(RAW_HANDLE) {assert(RAW_HANDLE->data); assert(  ((UvHandle*)RAW_HANDLE->data)->getUserData());  }
#define GET_UVHANDLE_OWNER(OWNER_TYPE, RAW_HANDLE)  ((OWNER_TYPE*)((UvHandle*)RAW_HANDLE->data)->getUserData())

#endif //UVCPPPRJ_UVCPPDEF_H

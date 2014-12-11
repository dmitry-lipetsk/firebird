////////////////////////////////////////////////////////////////////////////////
#ifndef _COMMON_UTILITIES__FB_DELETE_AND_SET_NULL_H_
#define _COMMON_UTILITIES__FB_DELETE_AND_SET_NULL_H_

#include "../../common/classes/fb_null_ptr.h"

namespace Firebird{
////////////////////////////////////////////////////////////////////////////////

template<typename T>
void FB_DeletePtrAndSetNull(T*& ptr)
{
 T* const tmp=ptr;

 ptr=Firebird::null_ptr;

 delete tmp;
}//FB_DeletePtrAndSetNull

////////////////////////////////////////////////////////////////////////////////

template<typename T>
void FB_DeleteArrayAndSetNull(T*& ptr)
{
 T* const tmp=ptr;

 ptr=Firebird::null_ptr;

 delete[] tmp;
}//FB_DeleteArrayAndSetNull

////////////////////////////////////////////////////////////////////////////////
}/*nms Firebird*/
#endif

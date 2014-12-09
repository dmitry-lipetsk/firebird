////////////////////////////////////////////////////////////////////////////////
#ifndef COMMON_CLASSES__FB_NULL_PTR_H
#define COMMON_CLASSES__FB_NULL_PTR_H

#include "../fb_compiler_cfg.h"

namespace Firebird{
////////////////////////////////////////////////////////////////////////////////
//class FB_NullPtr

#if(FB_COMPILER_CONF__SUPPORT_CPP_NULLPTR!=0)

typedef std::nullptr_t FB_NullPtr;

#else

class FB_NullPtr
{
 public:
  FB_NullPtr()
  {;}

  template<typename T>
  operator T* ()const
  {
   return reinterpret_cast<T*>(0);
  }
 };//class FB_NullPtr

#endif

//------------------------------------------------------------------------
static const FB_NullPtr null_ptr;

////////////////////////////////////////////////////////////////////////////////
}/*nms Firebird*/
#endif
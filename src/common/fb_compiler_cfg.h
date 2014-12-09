////////////////////////////////////////////////////////////////////////////////
#ifndef _COMMON__FB_COMPILER_CFG_H_
#define _COMMON__FB_COMPILER_CFG_H_

////////////////////////////////////////////////////////////////////////////////
//Configurations

//FB_COMPILER_CONF__SUPPORT_CPP_NULLPTR
// 0 - compiler not supports the std::nullptr
// 1 - compiler support the std::nullptr

////////////////////////////////////////////////////////////////////////////////
#define __FB_COMPILER_ID_UNKNOWN                            0

//#define __FB_COMPILER_ID_VISUALC_V60                        0x40000060
//#define __FB_COMPILER_ID_VISUALC_V70                        0x40000070
#define __FB_COMPILER_ID_VISUALC_V80                        0x40000080
#define __FB_COMPILER_ID_VISUALC_V90                        0x40000090
#define __FB_COMPILER_ID_VISUALC_V100                       0x400000A0
#define __FB_COMPILER_ID_VISUALC_V110                       0x400000B0
#define __FB_COMPILER_ID_VISUALC_V120                       0x400000C0

////////////////////////////////////////////////////////////////////////////////
//Detect compiler ID

#if !defined(__FB_COMPILER_ID_CURRENT)

# if defined(_MSC_VER)

#  if   (_MSC_VER>=1800)
#    define __FB_COMPILER_ID_CURRENT                   __FB_COMPILER_ID_VISUALC_V120
#  elif  (_MSC_VER>=1700)
#    define __FB_COMPILER_ID_CURRENT                   __FB_COMPILER_ID_VISUALC_V110
#  elif (_MSC_VER>=1600)
#    define __FB_COMPILER_ID_CURRENT                   __FB_COMPILER_ID_VISUALC_V100
#  elif (_MSC_VER>=150)
#    define __FB_COMPILER_ID_CURRENT                   __FB_COMPILER_ID_VISUALC_V90
#  elif   (_MSC_VER>=1400)
#    define __FB_COMPILER_ID_CURRENT                   __FB_COMPILER_ID_VISUALC_V80
#  elif (_MSC_VER>=1300)
//#    define __FB_COMPILER_ID_CURRENT                   __FB_COMPILER_ID_VISUALC_V70
#  elif (_MSC_VER>=1200)
//#    define __FB_COMPILER_ID_CURRENT                   __FB_COMPILER_ID_VISUALC_V60
#  endif

# endif

# if !defined(__FB_COMPILER_ID_CURRENT)
#  __FB_COMPILER_ID_CURRENT                            __FB_COMPILER_ID_UNKNOWN
# endif

#endif

////////////////////////////////////////////////////////////////////////////////
//Configuration

#if   (__FB_COMPILER_ID_CURRENT==__FB_COMPILER_ID_VISUALC_V120)
# include "../common/compiler_cfg/fb_compiler_cfg__msvc12.h"
#elif (__FB_COMPILER_ID_CURRENT==__FB_COMPILER_ID_VISUALC_V110)
# include "../common/compiler_cfg/fb_compiler_cfg__msvc11.h"
#elif (__FB_COMPILER_ID_CURRENT==__FB_COMPILER_ID_VISUALC_V100)
# include "../common/compiler_cfg/fb_compiler_cfg__msvc10.h"
#elif (__FB_COMPILER_ID_CURRENT==__FB_COMPILER_ID_VISUALC_V90)
# include "../common/compiler_cfg/fb_compiler_cfg__msvc9.h"
#elif (__FB_COMPILER_ID_CURRENT==__FB_COMPILER_ID_VISUALC_V80)
# include "../common/compiler_cfg/fb_compiler_cfg__msvc8.h"
#else
# include "../common/compiler_cfg/fb_compiler_cfg__unknown.h"
#endif

////////////////////////////////////////////////////////////////////////////////
#endif

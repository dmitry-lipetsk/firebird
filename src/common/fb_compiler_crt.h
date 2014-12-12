////////////////////////////////////////////////////////////////////////////////
#ifndef _COMMON__FB_COMPILER_CRT_H_
#define _COMMON__FB_COMPILER_CRT_H_

#include "fb_compiler_cfg.h"

////////////////////////////////////////////////////////////////////////////////
//
// __FB_COMPILER_CRT__open
//  - open / _open
//
// __FB_COMPILER_CRT__read
//  - read / _read
//
// __FB_COMPILER_CRT__close
//  - close / _close
//
// __FB_COMPILER_CRT__setmode
//  - setmode / _setmode
//
// __FB_COMPILER_CRT__setmode
//  - fileno / _fileno
//
////////////////////////////////////////////////////////////////////////////////

#define __FB_COMPILER_CRT_ID_UNKNOWN                            0

//#define __FB_COMPILER_CRT_ID_VISUALC_V60                        0x40000060
//#define __FB_COMPILER_CRT_ID_VISUALC_V70                        0x40000070
#define __FB_COMPILER_CRT_ID_VISUALC_V80                        0x40000080
#define __FB_COMPILER_CRT_ID_VISUALC_V90                        0x40000090
#define __FB_COMPILER_CRT_ID_VISUALC_V100                       0x400000A0
#define __FB_COMPILER_CRT_ID_VISUALC_V110                       0x400000B0
#define __FB_COMPILER_CRT_ID_VISUALC_V120                       0x400000C0

////////////////////////////////////////////////////////////////////////////////
//Detect crt id through compiler id

#if !defined(__FB_COMPILER_CRT_ID_CURRENT)

#if   (__FB_COMPILER_ID_CURRENT==__FB_COMPILER_ID_VISUALC_V120)
# define __FB_COMPILER_CRT_ID_CURRENT       __FB_COMPILER_CRT_ID_VISUALC_V120
#elif (__FB_COMPILER_ID_CURRENT==__FB_COMPILER_ID_VISUALC_V110)
# define __FB_COMPILER_CRT_ID_CURRENT       __FB_COMPILER_CRT_ID_VISUALC_V110
#elif (__FB_COMPILER_ID_CURRENT==__FB_COMPILER_ID_VISUALC_V100)
# define __FB_COMPILER_CRT_ID_CURRENT       __FB_COMPILER_CRT_ID_VISUALC_V100
#elif (__FB_COMPILER_ID_CURRENT==__FB_COMPILER_ID_VISUALC_V90)
# define __FB_COMPILER_CRT_ID_CURRENT       __FB_COMPILER_CRT_ID_VISUALC_V90
#elif (__FB_COMPILER_ID_CURRENT==__FB_COMPILER_ID_VISUALC_V80)
# define __FB_COMPILER_CRT_ID_CURRENT       __FB_COMPILER_CRT_ID_VISUALC_V80
#elif (__FB_COMPILER_ID_CURRENT==__FB_COMPILER_ID_UNKNOWN)
# define __FB_COMPILER_CRT_ID_CURRENT       __FB_COMPILER_CRT_ID_UNKNOWN
#else
# error Can't detect the current crt ID. Unexpected compiler ID.
#endif

#endif

////////////////////////////////////////////////////////////////////////////////
//Configuration

#if   (__FB_COMPILER_CRT_ID_CURRENT==__FB_COMPILER_CRT_ID_VISUALC_V120)
# include "../common/compiler_crt/fb_compiler_crt__msvc12.h"
#elif (__FB_COMPILER_CRT_ID_CURRENT==__FB_COMPILER_CRT_ID_VISUALC_V110)
# include "../common/compiler_crt/fb_compiler_crt__msvc11.h"
#elif (__FB_COMPILER_CRT_ID_CURRENT==__FB_COMPILER_CRT_ID_VISUALC_V100)
# include "../common/compiler_crt/fb_compiler_crt__msvc10.h"
#elif (__FB_COMPILER_CRT_ID_CURRENT==__FB_COMPILER_CRT_ID_VISUALC_V90)
# include "../common/compiler_crt/fb_compiler_crt__msvc9.h"
#elif (__FB_COMPILER_CRT_ID_CURRENT==__FB_COMPILER_CRT_ID_VISUALC_V80)
# include "../common/compiler_crt/fb_compiler_crt__msvc8.h"
#elif (__FB_COMPILER_CRT_ID_CURRENT==__FB_COMPILER_CRT_ID_UNKNOWN)
# include "../common/compiler_crt/fb_compiler_crt__unknown.h"
#else
# error Unexpected crt ID
#endif

////////////////////////////////////////////////////////////////////////////////
#endif
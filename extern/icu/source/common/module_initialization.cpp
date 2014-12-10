////////////////////////////////////////////////////////////////////////////////
//ICU DLL Initialization

#ifdef WIN32
# define STRICT
# define WIN32_LEAN_AND_MEAN
# define NOGDI
# define NOUSER
# define NOSERVICE
# define NOIME
# define NOMCX
# include <windows.h>
#endif

#include "unicode/uclean.h"
#include "umutex.h"   //umtx_init
#include "ucln_cmn.h" //umtx_cleanup
#include <assert.h>

////////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(HINSTANCE const /*hInstance*/,
                    DWORD     const dwReason,
                    LPVOID    const /*lpReserved*/)
{
 BOOL bResult=TRUE;

 try
 {
  switch(dwReason)
  {
   case DLL_PROCESS_ATTACH:
   {
    //[2011-08-22] This is introduces the unexpected problems at ICU compile time
    //UErrorCode err_code=U_ZERO_ERROR;
    //u_init(&err_code);    
    //bResult=(err_code==U_ZERO_ERROR);

    umtx_init(NULL);
   
    break;
   }//DLL_PROCESS_ATTACH
  
   case DLL_PROCESS_DETACH:
   {
    u_cleanup();
    //umtx_cleanup();
    
    break;
   }//DLL_PROCESS_DETACH
  }//switch
 }
 catch(...)
 {
  assert(false && "ICU DllMain");
  
  bResult=FALSE;
 }
 
 return bResult;
}//DllMain

////////////////////////////////////////////////////////////////////////////////

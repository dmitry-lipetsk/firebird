@echo off
set FB_DBG=
set FB_CFG_NAME=Release
set FB_CLEAN=/build
set FB_VC10CRT_DIR=%FB_PROCESSOR_ARCHITECTURE%

for %%v in ( %* )  do (
  ( if /I "%%v"=="DEBUG" ( (set FB_DBG=TRUE) && (set FB_CFG_NAME=Debug) ) )
  ( if /I "%%v"=="CLEAN" (set FB_CLEAN=/rebuild) )
)

set FB_OBJ_DIR=%FB_TARGET_PLATFORM%\%FB_CFG_NAME%

if %MSVC_VERSION% == 10 ( if %FB_VC10CRT_DIR% == AMD64 ( set FB_VC10CRT_DIR=x64))

@set FB_TEMP_DIR2=%FB_ROOT_PATH%\temp\%VS_VER%\%FB_OBJ_DIR%\
@echo FB_TEMP_DIR2=[%FB_TEMP_DIR2%]

@set FB_COMPILE_LOG_DIR=%FB_TEMP_DIR2%log\

@set FB_ICU_SOURCE_BIN2=%FB_TEMP_DIR2%icu\bin\
@echo FB_ICU_SOURCE_BIN2=[%FB_ICU_SOURCE_BIN2%]

@set FB_OUTPUT_DIR=%FB_ROOT_PATH%\output\%VS_VER%_%FB_TARGET_PLATFORM%_%FB_CFG_NAME%

@echo    Executed %0
@echo.


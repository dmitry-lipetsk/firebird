:: This batch file sets the environment values
:: FB_ROOT_PATH dos format path of the main directory
:: FB_DB_PATH unix format path of the main directory
:: (This is used by gpre and preprocess.bat)
:: VS_VER VisualStudio version (msvc7|msvc8|msvc9)
:: SERVER_NAME server needed to connect to firebird (could include port)
::   Example : localhost/3051
:: (Note - SERVER_NAME is almost deprecated - it is only used by
::   make_examples.bat
::
:: Note for MSVC8/9 and 64-bit build environment: The assumption is that a
:: 64-bit host is available and builds are targeted at either the native
:: 64-bit environment or the 32-bit WOW64 environment. Cross-compiling 64-bit
:: from a 32-bit host is not currently supported from this set of batch files.
::

@echo off

::===============================
@set FB_BUILD__COMPILER=
@set FB_CFG_NAME=Release
@set FB_CLEAN=/build

for %%v in ( %* ) do (
  ( if /I "%%v"=="MSVC14XP" (set FB_BUILD__COMPILER=msvc14xp) )
  ( if /I "%%v"=="MSVC12XP" (set FB_BUILD__COMPILER=msvc12xp) )
  ( if /I "%%v"=="MSVC11XP" (set FB_BUILD__COMPILER=msvc11xp) )
  ( if /I "%%v"=="MSVC10"   (set FB_BUILD__COMPILER=msvc10) )
  ( if /I "%%v"=="MSVC9"    (set FB_BUILD__COMPILER=msvc9) )
  ( if /I "%%v"=="MSVC8"    (set FB_BUILD__COMPILER=msvc8) )
  ( if /I "%%v"=="MSVC7"    (set FB_BUILD__COMPILER=msvc7) )

  ( if /I "%%v"=="AMD64"    (set FB_PROCESSOR_ARCHITECTURE=AMD64) )
  ( if /I "%%v"=="x86"      (set FB_PROCESSOR_ARCHITECTURE=x86) )

  ( if /I "%%v"=="DEBUG"    (set FB_CFG_NAME=Debug) )
  ( if /I "%%v"=="CLEAN"    (set FB_CLEAN=/rebuild) )
)

if "%FB_BUILD__COMPILER%"=="msvc14xp" if NOT DEFINED VS140COMNTOOLS goto :ERROR__NO_VS140COMNTOOLS
if "%FB_BUILD__COMPILER%"=="msvc12xp" if NOT DEFINED VS120COMNTOOLS goto :ERROR__NO_VS120COMNTOOLS
if "%FB_BUILD__COMPILER%"=="msvc11xp" if NOT DEFINED VS110COMNTOOLS goto :ERROR__NO_VS110COMNTOOLS
if "%FB_BUILD__COMPILER%"=="msvc10"   if NOT DEFINED VS100COMNTOOLS goto :ERROR__NO_VS100COMNTOOLS
if "%FB_BUILD__COMPILER%"=="msvc9"    if NOT DEFINED VS90COMNTOOLS  goto :ERROR__NO_VS90COMNTOOLS
if "%FB_BUILD__COMPILER%"=="msvc8"    if NOT DEFINED VS80COMNTOOLS  goto :ERROR__NO_VS80COMNTOOLS
if "%FB_BUILD__COMPILER%"=="msvc7"    if NOT DEFINED VS71COMNTOOLS  goto :ERROR__NO_VS71COMNTOOLS

if "%FB_BUILD__COMPILER%"=="" if DEFINED VS140COMNTOOLS set FB_BUILD__COMPILER=msvc14xp
if "%FB_BUILD__COMPILER%"=="" if DEFINED VS120COMNTOOLS set FB_BUILD__COMPILER=msvc12xp
if "%FB_BUILD__COMPILER%"=="" if DEFINED VS110COMNTOOLS set FB_BUILD__COMPILER=msvc11xp
if "%FB_BUILD__COMPILER%"=="" if DEFINED VS100COMNTOOLS set FB_BUILD__COMPILER=msvc10
if "%FB_BUILD__COMPILER%"=="" if DEFINED VS90COMNTOOLS  set FB_BUILD__COMPILER=msvc9
if "%FB_BUILD__COMPILER%"=="" if DEFINED VS80COMNTOOLS  set FB_BUILD__COMPILER=msvc8
if "%FB_BUILD__COMPILER%"=="" if DEFINED VS71COMNTOOLS  set FB_BUILD__COMPILER=msvc7

if %FB_BUILD__COMPILER%=="" goto :HELP

:: Default target CPU architecture is the native environment
if NOT DEFINED FB_PROCESSOR_ARCHITECTURE (
set FB_PROCESSOR_ARCHITECTURE=%PROCESSOR_ARCHITECTURE%
)

::echo FB_PROCESSOR_ARCHITECTURE : [%FB_PROCESSOR_ARCHITECTURE%]
::echo FB_BUILD__COMPILER        : [%FB_BUILD__COMPILER%]
::echo FB_CFG__NAME              : [%FB_CFG_NAME%]

::===============================
if "%FB_BUILD__COMPILER%"=="msvc14xp" set MSVC_VERSION=14
if "%FB_BUILD__COMPILER%"=="msvc12xp" set MSVC_VERSION=12
if "%FB_BUILD__COMPILER%"=="msvc11xp" set MSVC_VERSION=11
if "%FB_BUILD__COMPILER%"=="msvc10"   set MSVC_VERSION=10
if "%FB_BUILD__COMPILER%"=="msvc9"    set MSVC_VERSION=9
if "%FB_BUILD__COMPILER%"=="msvc8"    set MSVC_VERSION=8
if "%FB_BUILD__COMPILER%"=="msvc7"    set MSVC_VERSION=7

if %MSVC_VERSION%=="" goto :HELP

@set VS_VER=%FB_BUILD__COMPILER%

::===============================
::Set up the compiler environment

if "%FB_BUILD__COMPILER%"=="msvc14xp" (
@devenv /? >nul 2>nul
@if errorlevel 9009 (call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" %FB_PROCESSOR_ARCHITECTURE%) else ( echo    The file: & @echo      "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" %FB_PROCESSOR_ARCHITECTURE% & echo    has already been executed.)
)

if "%FB_BUILD__COMPILER%"=="msvc12xp" (
@devenv /? >nul 2>nul
@if errorlevel 9009 (call "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat" %FB_PROCESSOR_ARCHITECTURE%) else ( echo    The file: & @echo      "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat" %FB_PROCESSOR_ARCHITECTURE% & echo    has already been executed.)
)

if "%FB_BUILD__COMPILER%"=="msvc11xp" (
@devenv /? >nul 2>nul
@if errorlevel 9009 (call "%VS110COMNTOOLS%\..\..\VC\vcvarsall.bat" %FB_PROCESSOR_ARCHITECTURE%) else ( echo    The file: & @echo      "%VS110COMNTOOLS%\..\..\VC\vcvarsall.bat" %FB_PROCESSOR_ARCHITECTURE% & echo    has already been executed.)
)

if "%FB_BUILD__COMPILER%"=="msvc10" (
@devenv /? >nul 2>nul
@if errorlevel 9009 (call "%VS100COMNTOOLS%\..\..\VC\vcvarsall.bat" %FB_PROCESSOR_ARCHITECTURE%) else ( echo    The file: & @echo      "%VS100COMNTOOLS%\..\..\VC\vcvarsall.bat" %FB_PROCESSOR_ARCHITECTURE% & echo    has already been executed.)
)

if "%FB_BUILD__COMPILER%"=="msvc9" (
@devenv /? >nul 2>nul
@if errorlevel 9009 (call "%VS90COMNTOOLS%\..\..\VC\vcvarsall.bat" %FB_PROCESSOR_ARCHITECTURE%) else ( echo    The file: & @echo      "%VS90COMNTOOLS%\..\..\VC\vcvarsall.bat" %FB_PROCESSOR_ARCHITECTURE% & echo    has already been executed.)
)

if "%FB_BUILD__COMPILER%"=="msvc8" (
@devenv /? >nul 2>nul
@if errorlevel 9009 (call "%VS80COMNTOOLS%\..\..\VC\vcvarsall.bat" %FB_PROCESSOR_ARCHITECTURE%) else ( echo    The file: & @echo      "%VS80COMNTOOLS%\..\..\VC\vcvarsall.bat" %FB_PROCESSOR_ARCHITECTURE% & echo    has already been executed.)
)

if "%FB_BUILD__COMPILER%"=="msvc7" (
@devenv /? >nul 2>nul
@if errorlevel 9009 (call "%VS71COMNTOOLS%vsvars32.bat") else ( echo    The file: & echo      "%VS71COMNTOOLS%vsvars32.bat" & echo    has already been executed.)
)

@echo.

::=================
:SET_DB_DIR

@SET SERVER_NAME=localhost

@cd ..\..
@for /f "delims=" %%a in ('@cd') do (set FB_ROOT_PATH=%%a)
@cd %~dp0
@for /f "tokens=*" %%a in ('@echo %FB_ROOT_PATH:\=/%') do (set FB_DB_PATH=%%a)

::=================
rem :SET_MSVC_VER
rem 
rem @if "%VS_VER_EXPRESS%" == "" (
rem   @vcexpress /? >nul 2>nul
rem   @if not errorlevel 9009 (
rem     @if DEFINED VS80COMNTOOLS ((set MSVC_VERSION=8) & (set VS_VER=msvc8))
rem     @if DEFINED VS90COMNTOOLS ((set MSVC_VERSION=9) & (set VS_VER=msvc9))
rem     @if DEFINED VS100COMNTOOLS ((set MSVC_VERSION=10) & (set VS_VER=msvc10))
rem 
rem     set VS_VER_EXPRESS=1
rem     goto :SET_FB_TARGET_PLATFORM
rem   )
rem 
rem   @set VS_VER_EXPRESS=
rem )
rem 
rem @for /f "delims=." %%a in ('@devenv /?') do (
rem   @for /f "tokens=6" %%b in ("%%a") do ((set MSVC_VERSION=%%b) & (set VS_VER=msvc%%b) & (goto :SET_FB_TARGET_PLATFORM))
rem )
rem 
rem @if not defined MSVC_VERSION goto :HELP

::===========
:SET_FB_TARGET_PLATFORM

@set FB_TARGET_PLATFORM=Win32
:: If MSVC >= 8 then we can test for processor architecture
:: We certainly don't want to try and set platform=x64 if MSVC7 is installed
@if %MSVC_VERSION% GEQ 8 (
@if "%FB_PROCESSOR_ARCHITECTURE%"=="x86" (set FB_TARGET_PLATFORM=Win32)
@if "%FB_PROCESSOR_ARCHITECTURE%"=="AMD64" (set FB_TARGET_PLATFORM=x64)
)

@set FB_INSTALL_SCRIPTS=%FB_ROOT_PATH%\builds\install\arch-specific\win32
@set FB_GEN_DIR=%FB_ROOT_PATH%\gen\%FB_BUILD__COMPILER%_%FB_TARGET_PLATFORM%_%FB_CFG_NAME%\fb
@set FB_GEN_DB_DIR=%FB_DB_PATH%/gen/%FB_BUILD__COMPILER%_%FB_TARGET_PLATFORM%_%FB_CFG_NAME%/fb

::===========
@set FB_OBJ_DIR=%FB_TARGET_PLATFORM%\%FB_CFG_NAME%

@set FB_BUILD__ROOT_TEMP_DIR=%FB_ROOT_PATH%\temp\%FB_BUILD__COMPILER%_%FB_TARGET_PLATFORM%_%FB_CFG_NAME%\

@set FB_BUILD__COMPILE_LOG_DIR=%FB_BUILD__ROOT_TEMP_DIR%log\

@set FB_BUILD__ICU_SOURCE_BIN=%FB_BUILD__ROOT_TEMP_DIR%icu\bin\

@set FB_BUILD__TEMP_DIR=%FB_BUILD__ROOT_TEMP_DIR%fb_tmp\

@set FB_BUILD__OUT_DIR=%FB_BUILD__ROOT_TEMP_DIR%fb_out\

@set FB_OUTPUT_DIR=%FB_ROOT_PATH%\output\%FB_BUILD__COMPILER%_%FB_TARGET_PLATFORM%_%FB_CFG_NAME%

goto :END


::===========
:ERROR__NO_VS140COMNTOOLS
@echo "ERROR: VS2015 not installed"
@exit /B 1

::===========
:ERROR__NO_VS120COMNTOOLS
@echo "ERROR: VS2013 not installed"
@exit /B 1

::===========
:ERROR__NO_VS110COMNTOOLS
@echo "ERROR: VS2012 not installed"
@exit /B 1

::===========
:ERROR__NO_VS100COMNTOOLS
@echo "ERROR: VS2010 not installed"
@exit /B 1

::===========
:ERROR__NO_VS90COMNTOOLS
@echo "ERROR: VS2008 not installed"
@exit /B 1

::===========
:ERROR__NO_VS80COMNTOOLS
@echo "ERROR: VS2005 not installed"
@exit /B 1

::===========
:ERROR__NO_VS71COMNTOOLS
@echo "ERROR: VS2003 not installed"
@exit /B 1

::===========
:HELP
@echo.
@echo    ERROR:
@echo    A working version of Visual Studio cannot be found
@echo    on your current path.
@echo.
@echo    You need MS Visual Studio 7 or newer to build Firebird
@echo    from these batch files.
@echo.
@echo    A properly installed version of Visual Studio will set
@echo    an environment variable such as %%VS71COMNTOOLS%% or
@echo    %%VS80COMNTOOLS%%. We use that variable to run the
@echo    appropriate batch file to set up the build environment.
@echo.
:: set errorlevel
@exit /B 1

:END
@echo.
@echo    Setting Environment Variables thus...
@echo.
@echo    vs_ver             =%VS_VER%
if defined VS_VER_EXPRESS (
@echo    vs_ver_express     =%VS_VER_EXPRESS%
)
@echo    platform           =%FB_TARGET_PLATFORM%
@echo    msvc_version       =%MSVC_VERSION%
@echo    configuration      =%FB_CFG_NAME%
@echo    db_path            =%FB_DB_PATH%
@echo    root_path          =%FB_ROOT_PATH%
@echo    server_name        =%SERVER_NAME%
@echo.
@echo    FB_BUILD__ROOT_TEMP_DIR  =[%FB_BUILD__ROOT_TEMP_DIR%]
@echo    FB_BUILD__ICU_SOURCE_BIN =[%FB_BUILD__ICU_SOURCE_BIN%]
@echo    FB_BUILD__TEMP_DIR       =[%FB_BUILD__TEMP_DIR%]
@echo    FB_BUILD__OUT_DIR        =[%FB_BUILD__OUT_DIR%]
@echo    FB_OUTPUT_DIR            =[%FB_OUTPUT_DIR%]
@echo    FB_GEN_DIR               =[%FB_GEN_DIR%]
@echo.
@echo    (End of %0)
@echo.

@exit /B 0

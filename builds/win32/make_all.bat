@echo off
set ERRLEV=0

:: Set env vars
@call helper__setenvvar.bat %*

@if errorlevel 1 (call :ERROR Executing setenvvar.bat failed & goto :EOF)

:: verify that boot was run before

@if not exist %FB_GEN_DIR%\gpre_boot.exe (goto :HELP_BOOT & goto :EOF)

::==========
:: MAIN

@echo Building %FB_OBJ_DIR%

call helper__compile.bat %FB_ROOT_PATH%\builds\win32\%VS_VER%\Firebird2 make_all.log
if errorlevel 1 call :ERROR build failed - see make_all_%FB_TARGET_PLATFORM%.log for details

@if "%ERRLEV%"=="1" (
  @goto :EOF
) else (
  @call :MOVE
)
@goto :EOF

::===========
:MOVE
@echo Copying files to output [%FB_OUTPUT_DIR%]
@del %FB_BUILD__OUT_DIR%bin\*.exp 2>nul
@del %FB_BUILD__OUT_DIR%bin\*.lib 2>nul
@rmdir /q /s %FB_OUTPUT_DIR% 2>nul

@mkdir %FB_OUTPUT_DIR% 2>nul
@mkdir %FB_OUTPUT_DIR%\bin 2>nul
@mkdir %FB_OUTPUT_DIR%\intl 2>nul
@mkdir %FB_OUTPUT_DIR%\udf 2>nul
@mkdir %FB_OUTPUT_DIR%\help 2>nul
@mkdir %FB_OUTPUT_DIR%\doc 2>nul
@mkdir %FB_OUTPUT_DIR%\doc\sql.extensions 2>nul
@mkdir %FB_OUTPUT_DIR%\include 2>nul
@mkdir %FB_OUTPUT_DIR%\lib 2>nul
@mkdir %FB_OUTPUT_DIR%\system32 2>nul
@mkdir %FB_OUTPUT_DIR%\plugins 2>nul

for %%v in ( icuuc30 icudt30 icuin30 ) do (
@copy %FB_BUILD__ICU_SOURCE_BIN%\%%v.dll %FB_OUTPUT_DIR%\bin >nul
)

@copy %FB_BUILD__OUT_DIR%bin\* %FB_OUTPUT_DIR%\bin >nul
@copy %FB_BUILD__OUT_DIR%intl\* %FB_OUTPUT_DIR%\intl >nul
@copy %FB_BUILD__OUT_DIR%udf\* %FB_OUTPUT_DIR%\udf >nul
@copy %FB_BUILD__OUT_DIR%system32\* %FB_OUTPUT_DIR%\system32 >nul
@copy %FB_BUILD__OUT_DIR%plugins\fbtrace.dll %FB_OUTPUT_DIR%\plugins\fbtrace.dll >nul
@copy %FB_BUILD__TEMP_DIR%fbclient\fbclient.lib %FB_OUTPUT_DIR%\lib\fbclient_ms.lib >nul
@copy %FB_BUILD__TEMP_DIR%ib_util\ib_util.lib %FB_OUTPUT_DIR%\lib\ib_util_ms.lib >nul

for %%v in ( btyacc gbak_embed gpre_boot gpre_embed isql_embed ) do (
@del %FB_OUTPUT_DIR%\bin\%%v.exe >nul
)

:: Firebird.conf, etc
@copy %FB_GEN_DIR%\firebird.msg %FB_OUTPUT_DIR% > nul
:: The line @UDF_COMMENT@ should be deleted from the target file.
findstr /V "@UDF_COMMENT@" %FB_ROOT_PATH%\builds\install\misc\firebird.conf.in > %FB_OUTPUT_DIR%\firebird.conf
@copy %FB_ROOT_PATH%\builds\install\misc\fbintl.conf %FB_OUTPUT_DIR%\intl >nul
@copy %FB_ROOT_PATH%\src\utilities\ntrace\fbtrace.conf %FB_OUTPUT_DIR% >nul
@copy %FB_ROOT_PATH%\builds\install\misc\IPLicense.txt %FB_OUTPUT_DIR% >nul
@copy %FB_ROOT_PATH%\builds\install\misc\IDPLicense.txt %FB_OUTPUT_DIR% >nul

:: DATABASES
@copy %FB_GEN_DIR%\dbs\SECURITY2.FDB %FB_OUTPUT_DIR%\security2.fdb >nul
@copy %FB_GEN_DIR%\dbs\HELP.fdb %FB_OUTPUT_DIR%\help\help.fdb >nul

:: DOCS
@copy %FB_ROOT_PATH%\ChangeLog %FB_OUTPUT_DIR%\doc\ChangeLog.txt >nul
@copy %FB_ROOT_PATH%\doc\WhatsNew %FB_OUTPUT_DIR%\doc\WhatsNew.txt >nul

:: READMES
@copy %FB_ROOT_PATH%\doc\README.* %FB_OUTPUT_DIR%\doc >nul
@copy %FB_ROOT_PATH%\doc\sql.extensions\README.* %FB_OUTPUT_DIR%\doc\sql.extensions >nul


:: HEADERS
:: Don't use this ibase.h unless you have to - we build it better in BuildExecutableInstall.bat
:: This variation doesn't clean up the license templates, and processes the component files in
:: a different order to that used in the production version. However, this version doesn't
:: have a dependancy upon sed while the production one does.
echo #pragma message("Non-production version of ibase.h.") > %FB_OUTPUT_DIR%\include\ibase.tmp
echo #pragma message("Using raw, unprocessed concatenation of header files.") >> %FB_OUTPUT_DIR%\include\ibase.tmp
type %FB_ROOT_PATH%\src\misc\ibase_header.txt >> %FB_OUTPUT_DIR%\include\ibase.tmp
type %FB_ROOT_PATH%\src\include\types_pub.h >> %FB_OUTPUT_DIR%\include\ibase.tmp
type %FB_ROOT_PATH%\src\jrd\dsc_pub.h >> %FB_OUTPUT_DIR%\include\ibase.tmp
type %FB_ROOT_PATH%\src\dsql\sqlda_pub.h >> %FB_OUTPUT_DIR%\include\ibase.tmp
type %FB_ROOT_PATH%\src\jrd\ibase.h >> %FB_OUTPUT_DIR%\include\ibase.tmp
type %FB_ROOT_PATH%\src\jrd\inf_pub.h >> %FB_OUTPUT_DIR%\include\ibase.tmp
type %FB_ROOT_PATH%\src\include\consts_pub.h >> %FB_OUTPUT_DIR%\include\ibase.tmp
type %FB_ROOT_PATH%\src\jrd\blr.h >> %FB_OUTPUT_DIR%\include\ibase.tmp
type %FB_ROOT_PATH%\src\include\gen\iberror.h >> %FB_OUTPUT_DIR%\include\ibase.tmp
sed -f %FB_ROOT_PATH%\src\misc\headers.sed < %FB_OUTPUT_DIR%\include\ibase.tmp > %FB_OUTPUT_DIR%\include\ibase.h
del %FB_OUTPUT_DIR%\include\ibase.tmp > nul

:: Additional headers
copy %FB_ROOT_PATH%\src\extlib\ib_util.h %FB_OUTPUT_DIR%\include > nul
copy %FB_ROOT_PATH%\src\jrd\perf.h %FB_OUTPUT_DIR%\include >nul
copy %FB_ROOT_PATH%\src\include\gen\iberror.h %FB_OUTPUT_DIR%\include > nul

:: UDF
copy %FB_ROOT_PATH%\src\extlib\ib_udf.sql %FB_OUTPUT_DIR%\udf > nul
copy %FB_ROOT_PATH%\src\extlib\ib_udf2.sql %FB_OUTPUT_DIR%\udf > nul
copy %FB_ROOT_PATH%\src\extlib\fbudf\fbudf.sql %FB_OUTPUT_DIR%\udf > nul

:: Installers
@copy %FB_INSTALL_SCRIPTS%\install_super.bat %FB_OUTPUT_DIR%\bin >nul
@copy %FB_INSTALL_SCRIPTS%\install_classic.bat %FB_OUTPUT_DIR%\bin >nul
@copy %FB_INSTALL_SCRIPTS%\install_superclassic.bat %FB_OUTPUT_DIR%\bin >nul
@copy %FB_INSTALL_SCRIPTS%\uninstall.bat %FB_OUTPUT_DIR%\bin >nul

:: ------------------------------------------ MSVC runtime 2013
if %MSVC_VERSION% == 12 if %FB_PROCESSOR_ARCHITECTURE%==AMD64 if %FB_CFG_NAME%==Release (
@copy "%VS120COMNTOOLS%..\..\VC\redist\x64\Microsoft.VC120.CRT\msvcr120.dll" %FB_OUTPUT_DIR%\bin >nul
)

if %MSVC_VERSION% == 12 if %FB_PROCESSOR_ARCHITECTURE%==AMD64 if %FB_CFG_NAME%==Debug (
@copy "%VS120COMNTOOLS%..\..\VC\redist\Debug_NonRedist\x64\Microsoft.VC120.DebugCRT\msvcr120d.dll" %FB_OUTPUT_DIR%\bin >nul
)

if %MSVC_VERSION% == 12 if %FB_PROCESSOR_ARCHITECTURE%==x86 if %FB_CFG_NAME%==Release (
@copy "%VS120COMNTOOLS%..\..\VC\redist\x86\Microsoft.VC120.CRT\msvcr120.dll" %FB_OUTPUT_DIR%\bin >nul
)

if %MSVC_VERSION% == 12 if %FB_PROCESSOR_ARCHITECTURE%==x86 if %FB_CFG_NAME%==Debug (
@copy "%VS120COMNTOOLS%..\..\VC\redist\Debug_NonRedist\x86\Microsoft.VC120.DebugCRT\msvcr120d.dll" %FB_OUTPUT_DIR%\bin >nul
)

:: ------------------------------------------ MSVC runtime 2012
if %MSVC_VERSION% == 11 if %FB_PROCESSOR_ARCHITECTURE%==AMD64 if %FB_CFG_NAME%==Release (
@copy "%VS110COMNTOOLS%..\..\VC\redist\x64\Microsoft.VC110.CRT\msvcr110.dll" %FB_OUTPUT_DIR%\bin >nul
)

if %MSVC_VERSION% == 11 if %FB_PROCESSOR_ARCHITECTURE%==AMD64 if %FB_CFG_NAME%==Debug (
@copy "%VS110COMNTOOLS%..\..\VC\redist\Debug_NonRedist\x64\Microsoft.VC110.DebugCRT\msvcr110d.dll" %FB_OUTPUT_DIR%\bin >nul
)

if %MSVC_VERSION% == 11 if %FB_PROCESSOR_ARCHITECTURE%==x86 if %FB_CFG_NAME%==Release (
@copy "%VS110COMNTOOLS%..\..\VC\redist\x86\Microsoft.VC110.CRT\msvcr110.dll" %FB_OUTPUT_DIR%\bin >nul
)

if %MSVC_VERSION% == 11 if %FB_PROCESSOR_ARCHITECTURE%==x86 if %FB_CFG_NAME%==Debug (
@copy "%VS110COMNTOOLS%..\..\VC\redist\Debug_NonRedist\x86\Microsoft.VC110.DebugCRT\msvcr110d.dll" %FB_OUTPUT_DIR%\bin >nul
)

:: ------------------------------------------ MSVC runtime 2010
if %MSVC_VERSION% == 10 if %FB_PROCESSOR_ARCHITECTURE%==AMD64 if %FB_CFG_NAME%==Release (
@copy "%VS100COMNTOOLS%..\..\VC\redist\x64\Microsoft.VC100.CRT\msvcr100.dll" %FB_OUTPUT_DIR%\bin >nul
)

if %MSVC_VERSION% == 10 if %FB_PROCESSOR_ARCHITECTURE%==AMD64 if %FB_CFG_NAME%==Debug (
@copy "%VS100COMNTOOLS%..\..\VC\redist\Debug_NonRedist\x64\Microsoft.VC100.DebugCRT\msvcr100d.dll" %FB_OUTPUT_DIR%\bin >nul
)

if %MSVC_VERSION% == 10 if %FB_PROCESSOR_ARCHITECTURE%==x86 if %FB_CFG_NAME%==Release (
@copy "%VS100COMNTOOLS%..\..\VC\redist\x86\Microsoft.VC100.CRT\msvcr100.dll" %FB_OUTPUT_DIR%\bin >nul
)

if %MSVC_VERSION% == 10 if %FB_PROCESSOR_ARCHITECTURE%==x86 if %FB_CFG_NAME%==Debug (
@copy "%VS100COMNTOOLS%..\..\VC\redist\Debug_NonRedist\x86\Microsoft.VC100.DebugCRT\msvcr100d.dll" %FB_OUTPUT_DIR%\bin >nul
)

:: ------------------------------------------ MSVC runtime 2008
if %MSVC_VERSION% == 9 if %FB_CFG_NAME%==Release (
@copy "%VS90COMNTOOLS%\..\..\VC\redist\%FB_PROCESSOR_ARCHITECTURE%\Microsoft.VC90.CRT\msvcr90.dll" %FB_OUTPUT_DIR%\bin >nul
@copy "%VS90COMNTOOLS%\..\..\VC\redist\%FB_PROCESSOR_ARCHITECTURE%\Microsoft.VC90.CRT\msvcp90.dll" %FB_OUTPUT_DIR%\bin >nul
@copy "%VS90COMNTOOLS%\..\..\VC\redist\%FB_PROCESSOR_ARCHITECTURE%\Microsoft.VC90.CRT\Microsoft.VC90.CRT.manifest" %FB_OUTPUT_DIR%\bin >nul
)

if %MSVC_VERSION% == 9 if %FB_CFG_NAME%==Debug (
@copy "%VS90COMNTOOLS%\..\..\VC\redist\Debug_NonRedist\%FB_PROCESSOR_ARCHITECTURE%\Microsoft.VC90.DebugCRT\msvcr90d.dll" %FB_OUTPUT_DIR%\bin >nul
@copy "%VS90COMNTOOLS%\..\..\VC\redist\Debug_NonRedist\%FB_PROCESSOR_ARCHITECTURE%\Microsoft.VC90.DebugCRT\msvcp90d.dll" %FB_OUTPUT_DIR%\bin >nul
@copy "%VS90COMNTOOLS%\..\..\VC\redist\Debug_NonRedist\%FB_PROCESSOR_ARCHITECTURE%\Microsoft.VC90.DebugCRT\Microsoft.VC90.DebugCRT.manifest" %FB_OUTPUT_DIR%\bin >nul
)

:: ------------------------------------------ MSVC runtime 2005
if %MSVC_VERSION% == 8 if %FB_CFG_NAME%==Release (
@copy "%VS80COMNTOOLS%\..\..\VC\redist\%FB_PROCESSOR_ARCHITECTURE%\Microsoft.VC80.CRT\msvcr80.dll" %FB_OUTPUT_DIR%\bin >nul
@copy "%VS80COMNTOOLS%\..\..\VC\redist\%FB_PROCESSOR_ARCHITECTURE%\Microsoft.VC80.CRT\msvcp80.dll" %FB_OUTPUT_DIR%\bin >nul
@copy "%VS80COMNTOOLS%\..\..\VC\redist\%FB_PROCESSOR_ARCHITECTURE%\Microsoft.VC80.CRT\Microsoft.VC80.CRT.manifest" %FB_OUTPUT_DIR%\bin >nul
)

if %MSVC_VERSION% == 8 if %FB_CFG_NAME%==Debug (
@copy "%VS80COMNTOOLS%\..\..\VC\redist\Debug_NonRedist\%FB_PROCESSOR_ARCHITECTURE%\Microsoft.VC80.DebugCRT\msvcr80d.dll" %FB_OUTPUT_DIR%\bin >nul
@copy "%VS80COMNTOOLS%\..\..\VC\redist\Debug_NonRedist\%FB_PROCESSOR_ARCHITECTURE%\Microsoft.VC80.DebugCRT\msvcp80d.dll" %FB_OUTPUT_DIR%\bin >nul
@copy "%VS80COMNTOOLS%\..\..\VC\redist\Debug_NonRedist\%FB_PROCESSOR_ARCHITECTURE%\Microsoft.VC80.DebugCRT\Microsoft.VC80.DebugCRT.manifest" %FB_OUTPUT_DIR%\bin >nul
)

@goto :EOF

::==============
:HELP_BOOT
@echo.
@echo    You must run make_boot.bat before running this script
@echo.
@goto :EOF

:ERROR
::====
@echo.
@echo   An error occurred while running make_all.bat -
@echo     %*
@echo.
set ERRLEV=1
cancel_script > nul 2>&1
::End of ERROR
::------------
@goto :EOF

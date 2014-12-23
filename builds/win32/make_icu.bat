@echo off


:: Set env vars
@call helper__setenvvar.bat %*
@if errorlevel 1 (goto :EOF)

::==========
:: MAIN

@echo Building ICU %FB_OBJ_DIR% for %FB_TARGET_PLATFORM%

@call helper__compile.bat %FB_ROOT_PATH%\extern\icu\builds\win32\%VS_VER%\allinone make_icu.log

if errorlevel 1 call :ERROR build failed - see make_icu_%FB_TARGET_PLATFORM%.log for details
@goto :EOF



:ERROR
::====
@echo.
@echo   An error occurred while running make_icu.bat -
@echo     %*
@echo.
set ERRLEV=1
cancel_script > nul 2>&1
::End of ERROR
::------------
@goto :EOF





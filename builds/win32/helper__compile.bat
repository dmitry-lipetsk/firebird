@echo off
::==============
:: compile.bat solution, output, [projects...]
::   Note: MSVC7/8 don't accept more than one project
::
::   Note2: Our MSVC8/9 projects create object files in temp/$platform/$config
::     but we call devenv with $config|$platform (note variable in reverse order
::      and odd syntax.) This extended syntax for devenv does not seem to be
::      supported in MSVC7 (despite documentation to the contrary.)

setlocal
set solution=%1
set output=%2
set projects=

if %MSVC_VERSION% GEQ 8 (set config="%FB_CFG_NAME%|%FB_TARGET_PLATFORM%") ELSE (set config="%FB_CFG_NAME%")

if "%VS_VER_EXPRESS%"=="1" (
	set exec=vcexpress
) else (
	set exec=devenv
)

shift
shift

:loop_start

if "%1" == "" goto loop_end

set projects=%projects% /project %1

shift
goto loop_start

:loop_end

set output=%FB_BUILD__COMPILE_LOG_DIR%%output%

@echo LOG: [%output%]

@if NOT EXIST %FB_BUILD__COMPILE_LOG_DIR% @md %FB_BUILD__COMPILE_LOG_DIR%

%exec% %solution%.sln %projects% %FB_CLEAN% %config% /OUT %output%

endlocal

goto :EOF

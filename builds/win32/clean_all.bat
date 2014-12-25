@echo off

@setlocal

@call helper__setenvvar.bat %*
@if errorlevel 1 (goto :END)

@echo Cleaning temporary directories...
@rmdir /S /Q %FB_OUTPUT_DIR% 2>nul
@rmdir /S /Q %FB_BUILD__ROOT_TEMP_DIR% 2>nul

@echo Cleaning gen...
@rmdir /S /Q %FB_GEN_DIR% 2>nul

if defined FB_CLEAN_SHARED (
@echo Cleaning shared gen and dsql files...
@del %FB_ROOT_PATH%\src\include\gen\autoconfig.h 2>nul
@del %FB_ROOT_PATH%\src\include\gen\blrtable.h 2>nul
@del %FB_ROOT_PATH%\src\dsql\dsql.tab.h 2>nul
@del %FB_ROOT_PATH%\src\dsql\parse.cpp 2>nul
)

@rmdir /s /q %FB_ROOT_PATH%\builds\win32\install_image 2>nul

@echo Completed executing %0
@echo.

:END
@endlocal
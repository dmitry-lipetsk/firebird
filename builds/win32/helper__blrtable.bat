@echo off

@if NOT EXIST %FB_COMPILE_LOG_DIR% @md %FB_COMPILE_LOG_DIR%

@cl -Ob2iytp -Gs -DWIN95 -W3 -G4 -Gd -MD -I..\..\src\include -I..\..\src\include\gen -DSUPERSERVER -DNOMSG -D_X86_=1 -DWIN32 -DI386 ..\..\src\misc\blrtable.cpp  /Fo%FB_TEMP_DIR2%blrtable.obj /Fe%FB_TEMP_DIR2%blrtable.exe > %FB_COMPILE_LOG_DIR%blrtable.log 2>&1
@if errorlevel 1 goto :ERROR
@%FB_TEMP_DIR2%blrtable.exe > %FB_ROOT_PATH%\src\include\gen\blrtable.h

goto :EOF

:ERROR
@echo.
@echo A compiler error occurred.
@echo.
goto :EOF


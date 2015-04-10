@SETLOCAL

call build_fb.bat %* x86 Debug
@IF NOT %ERRORLEVEL% == 0 GOTO :ERROR

call build_fb.bat %* x86 Release
@IF NOT %ERRORLEVEL% == 0 GOTO :ERROR

call build_fb.bat %* AMD64 Debug
@IF NOT %ERRORLEVEL% == 0 GOTO :ERROR

call build_fb.bat %* AMD64 Release
@IF NOT %ERRORLEVEL% == 0 GOTO :ERROR

@goto :EXIT

@rem -----------------------------------------------------
:ERROR
@goto :EXIT_BY_ERROR

@rem ------------------------------------------- EXIT
:EXIT
@ENDLOCAL

@EXIT /B

@rem ------------------------------------------- EXIT_BY_ERROR
:EXIT_BY_ERROR

@ECHO EXIT_BY_ERROR [fb_build_all]

@ENDLOCAL

@EXIT /B 999

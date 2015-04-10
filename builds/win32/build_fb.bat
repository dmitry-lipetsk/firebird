@SETLOCAL

call make_icu.bat %*
@IF NOT %ERRORLEVEL% == 0 GOTO :ERROR

call make_boot.bat %*
@IF NOT %ERRORLEVEL% == 0 GOTO :ERROR

call make_all.bat %*
@IF NOT %ERRORLEVEL% == 0 GOTO :ERROR

@rem -----------------------------------------------------
@ECHO [ALL IS OK]
@GOTO :EXIT

@rem -----------------------------------------------------
:ERROR
@ECHO [THE ERROR WAS OCCURRED]

@goto :EXIT_BY_ERROR

@rem ------------------------------------------- EXIT
:EXIT
@ENDLOCAL

@EXIT /B

@rem ------------------------------------------- EXIT_BY_ERROR
:EXIT_BY_ERROR

@ECHO EXIT_BY_ERROR

@ENDLOCAL

@EXIT /B 999

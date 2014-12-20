#**********************************************************************
#* Copyright (C) 1999-2003, International Business Machines Corporation
#* and others.  All Rights Reserved.
#**********************************************************************
# nmake file for creating data files on win32
# invoke with
# nmake /f makedata.mak icup=<path_to_icu_instalation> [Debug|Release]
#
#	12/10/1999	weiv	Created

!MESSAGE ICUTOOLS path is [$(ICUTOOLS)]
!IF "$(ICUTOOLS)"==""
!ERROR ICUTOOLS not defined!
!ENDIF

!MESSAGE ICU_ROOT_OUT path is [$(ICU_ROOT_OUT)]
!IF "$(ICU_ROOT_OUT)"==""
!ERROR ICU_ROOT_OUT not defined!
!ENDIF

!MESSAGE DLL_OUTPUT is [$(DLL_OUTPUT)]
!IF "$(DLL_OUTPUT)"==""
!ERROR DLL_OUTPUT not defined!
!ENDIF

ICUOUT=$(ICU_ROOT_OUT)\uconv
!MESSAGE ICUOUT is [$(ICUOUT)]

#Let's see if user has given us a path to ICU
#This could be found according to the path to makefile, but for now it is this way
!IF "$(ICUP)"==""
!ERROR Can't find path!
!ENDIF

!MESSAGE ICUP is [$(ICUP)]

RESNAME=uconvmsg
RESFILES=$(ICUP)\source\extra\uconv\resfiles.mk
#ICUDATA=$(ICUP)\data

# set the following to 'static' or 'dll' depending
PKGMODE=static

# Suffixes for data files
.SUFFIXES : .ucm .cnv .dll .dat .res .txt .c

!IF EXISTS("$(RESFILES)")
!INCLUDE "$(RESFILES)"
!ELSE
!ERROR ERROR: cannot find "$(RESFILES)"
!ENDIF
RB_FILES = $(RESSRC:.txt=.res)

# This target should build all the data files
!IF "$(PKGMODE)" == "dll"
OUTPUT = "$(DLL_OUTPUT)$(RESNAME).dll"
!ELSE
OUTPUT = "$(DLL_OUTPUT)$(RESNAME).lib"
!ENDIF

!MESSAGE OUTPUT is [$(OUTPUT)]

ALL : $(OUTPUT)
	@echo All targets are up to date (mode $(PKGMODE))


# invoke pkgdata - static
"$(DLL_OUTPUT)$(RESNAME).lib" : $(RB_FILES) $(RESFILES)
	@echo Building $(RESNAME).lib
    -md "$(ICUOUT)"
	"$(ICUTOOLS)pkgdata.exe" -f -v -m static -c -p $(RESNAME) -d "$(ICUOUT)" -s "$(ICUOUT)" <<$(ICUOUT)\pkgdatain.txt
$(RB_FILES:.res =.res
)
<<KEEP
 copy "$(ICUOUT)\$(RESNAME).lib" "$(DLL_OUTPUT)\"

# This is to remove all the data files
CLEAN :
	-erase "$(RESDIR)\uconvmsg*.*"
	-erase "$(DLL_OUTPUT)*uconvmsg*.*"
    -rd /S /Q "$(ICUOUT)"

# Inference rule for creating resource bundles
.txt.res:
	@echo Making Resource Bundle files
    -md "$(ICUOUT)"
	@echo EXEC: "$(ICUTOOLS)genrb.exe" -t -p $(RESNAME) -s resources -d $(ICUOUT) $(?F)
    "$(ICUTOOLS)genrb.exe" -t -p $(RESNAME) -s $(ICUP)\source\extra\uconv\resources -d $(ICUOUT) $(?F)

$(RESSRC) : "$(ICUTOOLS)genrb.exe"


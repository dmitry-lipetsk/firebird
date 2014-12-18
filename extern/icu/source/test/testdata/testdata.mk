#**********************************************************************
#* Copyright (C) 1999-2004, International Business Machines Corporation
#* and others.  All Rights Reserved.
#**********************************************************************
#
#   03/19/2001  weiv, schererm  Created

.SUFFIXES : .res .txt

!MESSAGE ------------------------------ testdata.mk
!MESSAGE ICUTOOLS is [$(ICUTOOLS)]
!MESSAGE TESTDATAOUT is [$(TESTDATAOUT)]
!MESSAGE TESTDATABLD is [$(TESTDATABLD)]
!MESSAGE ICUPBIN is [$(ICUPBIN)]

TESTPKG=testdata
TESTDT=$(TESTPKG)_

ICUTOOLS__GENRB_EXE=$(ICUTOOLS)genrb.exe
!MESSAGE ICUTOOLS__GENRB_EXE = $(ICUTOOLS__GENRB_EXE)

ICUTOOLS__MAKECONV_EXE=$(ICUTOOLS)makeconv.exe
!MESSAGE ICUTOOLS__MAKECONV_EXE = $(ICUTOOLS__MAKECONV_EXE)

ICUTOOLS__GENTEST_EXE=$(ICUTOOLS)gentest.exe
!MESSAGE ICUTOOLS__GENTEST_EXE = $(ICUTOOLS__GENTEST_EXE)

ICUTOOLS__GENSPREP_EXE=$(ICUTOOLS)gensprep.exe
!MESSAGE ICUTOOLS__GENSPREP_EXE = $(ICUTOOLS__GENSPREP_EXE)

ALL : "$(TESTDATAOUT)\testdata.dat"
	@echo Test data is built.

# icu26_testtypes.res is there for cintltst/udatatst.c/TestSwapData()
# I generated it with an ICU 2.6.1 build on Windows after removing
# testincludeUTF (which made it large, unnecessarily for this test)
# and CollationElements (which will not work with a newer swapper)
# markus 2003nov19

# icu26e_testtypes.res is the same, but icuswapped to big-endian EBCDIC
# markus 2003nov21

"$(TESTDATAOUT)\testdata.dat" : "$(TESTDATABLD)\casing.res" "$(TESTDATABLD)\conversion.res" "$(TESTDATABLD)\icuio.res" "$(TESTDATABLD)\mc.res" "$(TESTDATABLD)\root.res" "$(TESTDATABLD)\te.res" "$(TESTDATABLD)\te_IN.res" "$(TESTDATABLD)\testaliases.res" "$(TESTDATABLD)\testtypes.res" "$(TESTDATABLD)\testempty.res" "$(TESTDATABLD)\$(TESTDT)iscii.res" "$(TESTDATABLD)\$(TESTDT)idna_rules.res" "$(TESTDATABLD)\DataDrivenCollationTest.res" "$(TESTDATABLD)\$(TESTDT)test.icu" "$(TESTDATABLD)\$(TESTDT)testtable32.res" "$(TESTDATABLD)\$(TESTDT)test1.cnv" "$(TESTDATABLD)\$(TESTDT)test3.cnv" "$(TESTDATABLD)\$(TESTDT)test4.cnv" "$(TESTDATABLD)\$(TESTDT)test4x.cnv" "$(TESTDATABLD)\$(TESTDT)ibm9027.cnv" "$(TESTDATABLD)\$(TESTDT)nfscsi.spp" "$(TESTDATABLD)\$(TESTDT)nfscss.spp" "$(TESTDATABLD)\$(TESTDT)nfscis.spp" "$(TESTDATABLD)\$(TESTDT)nfsmxs.spp" "$(TESTDATABLD)\$(TESTDT)nfsmxp.spp"
	@echo Building test data
	@copy "$(TESTDATABLD)\$(TESTDT)te.res" "$(TESTDATAOUT)\$(TESTDT)nam.typ"
	@copy "$(TESTDATA)\$(TESTDT)icu26_testtypes.res" "$(TESTDATABLD)"
	@copy "$(TESTDATA)\$(TESTDT)icu26e_testtypes.res" "$(TESTDATABLD)"
	@"$(ICUPBIN)pkgdata" -f -v -m common -c -p"$(TESTPKG)" -d "$(TESTDATAOUT)" -T "$(TESTDATABLD)" -s "$(TESTDATABLD)" <<
$(TESTDT)casing.res
$(TESTDT)conversion.res
$(TESTDT)mc.res
$(TESTDT)root.res
$(TESTDT)testtable32.res
$(TESTDT)te.res
$(TESTDT)te_IN.res
$(TESTDT)testtypes.res
$(TESTDT)icu26_testtypes.res
$(TESTDT)icu26e_testtypes.res
$(TESTDT)testempty.res
$(TESTDT)testaliases.res
$(TESTDT)icuio.res
$(TESTDT)iscii.res
$(TESTDT)DataDrivenCollationTest.res
$(TESTDT)test.icu
$(TESTDT)test1.cnv
$(TESTDT)test3.cnv
$(TESTDT)test4.cnv
$(TESTDT)test4x.cnv
$(TESTDT)ibm9027.cnv
$(TESTDT)idna_rules.res
$(TESTDT)nfscsi.spp
$(TESTDT)nfscss.spp
$(TESTDT)nfscis.spp
$(TESTDT)nfsmxs.spp
$(TESTDT)nfsmxp.spp
<<


# Inference rule for creating resource bundles
# Some test data resource bundles are known to have warnings and bad data.
# The -q option is there on purpose, so we don't see it normally.
{$(TESTDATA)}.txt.res::
	@echo Making Test Resource Bundle files $<
	@"$(ICUTOOLS__GENRB_EXE)" -t -p"$(TESTPKG)" -q -s"$(TESTDATA)" -d"$(TESTDATABLD)" $<

"$(TESTDATABLD)\$(TESTDT)iscii.res": "$(TESTDATA)\iscii.bin"
	@echo Making Test Resource Bundle file with encoding ISCII,version=0
	@"$(ICUTOOLS__GENRB_EXE)" -p"$(TESTPKG)" -q -s"$(TESTDATA)" -eISCII,version=0 -d"$(TESTDATABLD)" iscii.bin

"$(TESTDATABLD)\$(TESTDT)idna_rules.res": "$(TESTDATA)\idna_rules.txt"
	@echo Making Test Resource Bundle file for IDNA reference implementation
	@"$(ICUTOOLS__GENRB_EXE)" -p"$(TESTPKG)" -q -s"$(TESTDATA)" -d"$(TESTDATABLD)" idna_rules.txt

# [Dmitry Kovalenko] critical changes:
# - was: .... : {"$(ICUTOOLS)\gentest\$(CFG)"}gentest.exe
"$(TESTDATABLD)\$(TESTDT)test.icu" : "$(ICUTOOLS__GENTEST_EXE)"
	"$(ICUTOOLS__GENTEST_EXE)" -d"$(TESTDATABLD)"

# testtable32 resource file
# [Dmitry Kovalenko] critical changes:
# - was: .... : {"$(ICUTOOLS)\gentest\$(CFG)"}gentest.exe
"$(TESTDATABLD)\testtable32.txt" : "$(ICUTOOLS__GENTEST_EXE)"
	"$(ICUTOOLS__GENTEST_EXE)" -r -d"$(TESTDATABLD)"

"$(TESTDATABLD)\$(TESTDT)testtable32.res": "$(TESTDATABLD)\testtable32.txt"
	@echo Making Test Resource Bundle file for IDNA reference implementation
	@"$(ICUTOOLS__GENRB_EXE)" -p"$(TESTPKG)" -q -s"$(TESTDATABLD)" -d"$(TESTDATABLD)" testtable32.txt

# Targets for nfscsi.spp
# [Dmitry Kovalenko] critical changes:
#  - was: .... : {"$(ICUTOOLS)\gensprep\$(CFG)"}gensprep.exe .....
"$(TESTDATABLD)\$(TESTDT)nfscsi.spp" : "$(ICUTOOLS__GENSPREP_EXE)" "$(TESTDATA)\nfs4_cs_prep_ci.txt"
	@echo Building $@
	@"$(ICUTOOLS__GENSPREP_EXE)" -s "$(TESTDATA)" -d "$(TESTDATABLD)\\" -b nfscsi -p "$(TESTPKG)" -u 3.2.0 nfs4_cs_prep_ci.txt

# Targets for nfscss.spp
# [Dmitry Kovalenko] critical changes:
#  - was: .... : {"$(ICUTOOLS)\gensprep\$(CFG)"}gensprep.exe .....
"$(TESTDATABLD)\$(TESTDT)nfscss.spp" : "$(ICUTOOLS__GENSPREP_EXE)" "$(TESTDATA)\nfs4_cs_prep_cs.txt"
	@echo Building $@
	@"$(ICUTOOLS__GENSPREP_EXE)" -s "$(TESTDATA)" -d "$(TESTDATABLD)\\" -b nfscss -p "$(TESTPKG)" -u 3.2.0 nfs4_cs_prep_cs.txt

# Targets for nfscis.spp
# [Dmitry Kovalenko] critical changes:
#  - was: .... : {"$(ICUTOOLS)\gensprep\$(CFG)"}gensprep.exe .....
"$(TESTDATABLD)\$(TESTDT)nfscis.spp" : "$(ICUTOOLS__GENSPREP_EXE)" "$(TESTDATA)\nfs4_cis_prep.txt"
	@echo Building $@
	@"$(ICUTOOLS__GENSPREP_EXE)" -s "$(TESTDATA)" -d "$(TESTDATABLD)\\" -b nfscis -p "$(TESTPKG)" -u 3.2.0 -k -n "$(ICUDATA)\unidata" nfs4_cis_prep.txt

# Targets for nfsmxs.spp
# [Dmitry Kovalenko] critical changes:
#  - was: .... : {"$(ICUTOOLS)\gensprep\$(CFG)"}gensprep.exe .....
"$(TESTDATABLD)\$(TESTDT)nfsmxs.spp" : "$(ICUTOOLS__GENSPREP_EXE)" "$(TESTDATA)\nfs4_mixed_prep_s.txt"
	@echo Building $@
	@"$(ICUTOOLS__GENSPREP_EXE)" -s "$(TESTDATA)" -d "$(TESTDATABLD)\\" -b nfsmxs -p "$(TESTPKG)" -u 3.2.0 -k -n "$(ICUDATA)\unidata" nfs4_mixed_prep_s.txt

# Targets for nfsmxp.spp
# [Dmitry Kovalenko] critical changes:
#  - was: .... : {"$(ICUTOOLS)\gensprep\$(CFG)"}gensprep.exe .....
"$(TESTDATABLD)\$(TESTDT)nfsmxp.spp" : "$(ICUTOOLS__GENSPREP_EXE)" "$(TESTDATA)\nfs4_mixed_prep_p.txt"
	@echo Building $@
	@"$(ICUTOOLS__GENSPREP_EXE)" -s "$(TESTDATA)" -d "$(TESTDATABLD)\\" -b nfsmxp -p "$(TESTPKG)" -u 3.2.0 -k -n "$(ICUDATA)\unidata" nfs4_mixed_prep_p.txt


# Targets for test converter data
"$(TESTDATABLD)\$(TESTDT)test1.cnv": "$(TESTDATA)\test1.ucm"
	@echo Building $@
	@"$(ICUTOOLS__MAKECONV_EXE)" -d"$(TESTDATABLD)" -p $(TESTPKG) $**

"$(TESTDATABLD)\$(TESTDT)test3.cnv": "$(TESTDATA)\test3.ucm"
	@echo Building $@
	@"$(ICUTOOLS__MAKECONV_EXE)" -d"$(TESTDATABLD)" -p $(TESTPKG) $**

"$(TESTDATABLD)\$(TESTDT)test4.cnv": "$(TESTDATA)\test4.ucm"
	@echo Building $@
	@"$(ICUTOOLS__MAKECONV_EXE)" -d"$(TESTDATABLD)" -p $(TESTPKG) $**

"$(TESTDATABLD)\$(TESTDT)test4x.cnv": "$(TESTDATA)\test4x.ucm"
	@echo Building $@
	@"$(ICUTOOLS__MAKECONV_EXE)" -d"$(TESTDATABLD)" -p $(TESTPKG) $**

"$(TESTDATABLD)\$(TESTDT)ibm9027.cnv": "$(TESTDATA)\ibm9027.ucm"
	@echo Building $@
	@"$(ICUTOOLS__MAKECONV_EXE)" -d"$(TESTDATABLD)" -p $(TESTPKG) $**


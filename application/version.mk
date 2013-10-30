#
# Patterned after Mike Ferrara's g2edk.mk 
#    A makefile for array filling,
#    from Centi-days of yore, 
#    when life was harsh 
#    and cursed by gore,
#    but all the women were willing  ;-]
#
# create a set of global QA variables based on Git, time, host, etc.
#   and generate a header file which can be included by any other file

# $Id$
# $Author$

# GIT_SERIAL:=$(shell git rev-list HEAD | wc -l)
GIT_SERIAL:=$(shell git rev-list master...HittiteDemo | wc -l)
GIT_SERIAL:=$(shell printf "%04d" $(GIT_SERIAL))
GIT_HASH_CLEAN:=$(shell git rev-list --abbrev-commit --abbrev=7 HEAD | head -1)
GIT_HASH_DIRTY:=$(shell git rev-list --abbrev-commit --abbrev=6 HEAD | head -1)
GIT_DIRTY:=$(shell if git diff-files --quiet --ignore-submodules --;then echo "clean";else echo "dirty";fi)
ifeq (dirty, $(GIT_DIRTY))
GIT_HASH_DIRTY:=+$(GIT_HASH_DIRTY)
GIT_HASH=$(GIT_HASH_DIRTY)
else
GIT_HASH:=$(GIT_HASH_CLEAN)
endif
CHANGESET:=$(GIT_SERIAL)_$(GIT_HASH)
BUILD_DATE:=$(shell date +%Y%m%d)
BUILD_TIME:=$(shell date +%H%M%S)

# Get the branch name from the environment or command line, else compute it from local repo
ifeq ($(origin BUILDBRANCH), undefined)
BUILD_BRANCH:=$(shell git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/\1/')
endif
# export BUILDBRANCH

# Make the version file dependent on _every_ C and C++ source file
VFILE:=version.h
$(VFILE):	USER:=$(shell id -u -n)
$(VFILE):	$(CSRC) $(CPPSRC)
	@echo make $(VFILE)
	@rm -f $(VFILE)
	@echo "//////////////////////////////////////////////////////////////////////////////" >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//           Copyright (C) 2012-2013 Tektronix, all rights reserved"            >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//                     3841 Brickway Blvd. Suite 210"                           >> $(VFILE)
	@echo "//                     Santa Rosa, CA 95403"                                    >> $(VFILE)
	@echo "//                     Tel:(707)595-4770"                                       >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//    Filename:        version.h"                                               >> $(VFILE)
	@echo "//    For product:     LE320/160"                                               >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//    This is an auto-generated header file which will be overwritten by"       >> $(VFILE)
	@echo "//    the sub-makefile, version.mk, at the next compile."                       >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//    This header defines four (4) global strings useful for QA    "            >> $(VFILE)
	@echo "//    when displayed in user-facing or remote interfaces           "            >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//          NAME              Example                              "            >> $(VFILE)
	@echo "//          -----------       ----------------------------------   "            >> $(VFILE)
	@echo "//          build_info        \"built at 9:53:22 May 16 2013...\'  "            >> $(VFILE)
	@echo "//          build_sha1        \"$(CHANGESET)\"                     "            >> $(VFILE)
	@echo "//          build_branch      \"RC_BUGFIX_LSI_07\"                 "            >> $(VFILE)
	@echo "//          build_rls_state   \"DEBUG\" or \"RELEASE\"             "            >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//    Auto-generated:  date: $(BUILD_DATE), time: $(BUILD_TIME)"                >> $(VFILE)
	@echo "//    by user:         $(USER)"                                                 >> $(VFILE)
	@echo "//    on host machine: $(HOSTNAME)"                                             >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//"                                                                             >> $(VFILE)
	@echo "//////////////////////////////////////////////////////////////////////////////" >> $(VFILE)
	@echo "#ifndef _VERSION_INCLUDED"                                                >> $(VFILE)
	@echo "#define _VERSION_INCLUDED"                                                >> $(VFILE)
	@echo "#ifdef GLOBAL_VERSION"                                                    >> $(VFILE)
	@echo "#define VERSIONGLOBAL"                                                    >> $(VFILE)
	@echo "#define VERSIONPRESET(A) = (A)"                                           >> $(VFILE)
	@echo "#else"                                                                    >> $(VFILE)
	@echo "#define VERSIONPRESET(A)"                                                 >> $(VFILE)
	@echo "#ifdef __cplusplus"                                                       >> $(VFILE)
	@echo "#define VERSIONGLOBAL extern \"C\""                                       >> $(VFILE)
	@echo "#else"                                                                    >> $(VFILE)
	@echo "#define VERSIONGLOBAL extern"                                             >> $(VFILE)
	@echo "#endif     /*__cplusplus*/"                                               >> $(VFILE)
	@echo "#endif     /* end GLOBAL_VERSION */"                                      >> $(VFILE)
	@echo ""                                                                         >> $(VFILE)
	@echo "#include \"instr_debug.h\"  /* this controls RELEASE_STATE */ "           >> $(VFILE)
	@echo ""                                                                         >> $(VFILE)
	@echo "#ifdef RELEASE"                                                           >> $(VFILE)
	@echo "#define BUILD_INFO \"built $(BUILD_DATE) at $(BUILD_TIME), changeset $(CHANGESET) of branch $(BUILD_BRANCH), by $(USER) on $(HOSTNAME)\""  >> $(VFILE)
	@echo "#define RELEASE_STATE \"RELEASE\""                                        >> $(VFILE)
	@echo "#else"                                                                    >> $(VFILE)
	@echo "#define BUILD_INFO \"DEBUG, built $(BUILD_DATE) at $(BUILD_TIME), changeset $(CHANGESET) of branch $(BUILD_BRANCH), by $(USER) on $(HOSTNAME)\""  >> $(VFILE)
	@echo "#define RELEASE_STATE \"DEBUG\""                                          >> $(VFILE)
	@echo "#endif      /* end RELEASE_STATE */"                                      >> $(VFILE)
	@echo "#define BUILD_SHA1   \"$(CHANGESET)\""                                    >> $(VFILE)
	@echo "#define BUILD_BRANCH \"$(BUILDBRANCH)\""                                  >> $(VFILE)
	@echo "VERSIONGLOBAL const char build_info     [] VERSIONPRESET(BUILD_INFO);"    >> $(VFILE)
	@echo "VERSIONGLOBAL const char build_sha1     [] VERSIONPRESET(BUILD_SHA1);"    >> $(VFILE)
	@echo "VERSIONGLOBAL const char build_branch   [] VERSIONPRESET(BUILD_BRANCH);"  >> $(VFILE)
	@echo "VERSIONGLOBAL const char build_rls_state[] VERSIONPRESET(RELEASE_STATE);" >> $(VFILE)
	@echo "  "                                                                       >> $(VFILE)
	@echo "#define SZ_BUILD_CHANGESET 12"                                            >> $(VFILE)
	@echo "#define SZ_BUILD_SHA1 7"                                                  >> $(VFILE)
	@echo "#endif    // end _VERSION_INCLUDED"                                       >> $(VFILE)

version:	$(VFILE)
# end version.mk


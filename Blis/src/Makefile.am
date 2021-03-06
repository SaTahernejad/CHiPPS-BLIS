#==========================================================================#
# This file is part of the BiCePS Linear Integer Solver (BLIS).            #
#                                                                          #
# BLIS is distributed under the Eclipse Public License as part of the      #
# COIN-OR repository (http://www.coin-or.org).                             #
#                                                                          #
# Authors:                                                                 #
#                                                                          #
#          Yan Xu, Lehigh University                                       #
#          Ted Ralphs, Lehigh University                                   #
#                                                                          #
# Conceptual Design:                                                       #
#                                                                          #
#          Yan Xu, Lehigh University                                       #
#          Ted Ralphs, Lehigh University                                   #
#          Laszlo Ladanyi, IBM T.J. Watson Research Center                 #
#          Matthew Saltzman, Clemson University                            #
#                                                                          #
#                                                                          #
# Copyright (C) 2006-2013, Lehigh University, Yan Xu, and Ted Ralphs.      #
# All Rights Reserved.                                                     #
#==========================================================================#

## $Id$

AUTOMAKE_OPTIONS = foreign

########################################################################
#                               libBlis                                #
########################################################################

# Name of the library compiled in this directory.  We want it to be installed
# in the 'lib' directory
lib_LTLIBRARIES = libBlis.la

# List all source files for this library, including headers
libBlis_la_SOURCES = \
	BlisBranchObjectInt.cpp \
	BlisBranchObjectInt.h \
	BlisBranchObjectBilevel.cpp \
	BlisBranchObjectBilevel.h \
	BlisBranchStrategyMaxInf.cpp \
	BlisBranchStrategyMaxInf.h \
	BlisBranchStrategyPseudo.cpp \
	BlisBranchStrategyPseudo.h \
	BlisBranchStrategyRel.cpp \
	BlisBranchStrategyRel.h \
	BlisBranchStrategyStrong.cpp \
	BlisBranchStrategyStrong.h \
	BlisBranchStrategyBilevel.cpp \
	BlisBranchStrategyBilevel.h \
	BlisConGenerator.cpp \
	BlisConGenerator.h \
	BlisConstraint.cpp \
	BlisConstraint.h \
	Blis.h \
	BlisHelp.cpp \
	BlisHelp.h \
	BlisHeuristic.h \
	BlisHeurRound.cpp \
	BlisHeurRound.h \
	BlisLicense.h \
	BlisMessage.cpp \
	BlisMessage.h \
	BlisModel.cpp \
	BlisModel.h \
	BlisNodeDesc.h \
	BlisObjectInt.cpp \
	BlisObjectInt.h \
	BlisParams.cpp \
	BlisParams.h \
	BlisPresolve.h \
	BlisPresolve.cpp \
	BlisPseudo.cpp \
	BlisPseudo.h \
	BlisSolution.h \
	BlisSubTree.h \
	BlisTreeNode.cpp \
	BlisTreeNode.h \
	BlisVariable.h

# List all additionally required libraries
if DEPENDENCY_LINKING
libBlis_la_LIBADD = $(BLISLIB_LIBS)
endif

# This is for libtool (on Windows)
libBlis_la_LDFLAGS = $(LT_LDFLAGS)

########################################################################
#                        blis binary                                   #
########################################################################

bin_PROGRAMS = blis
blis_SOURCES = BlisMain.cpp 
blis_LDADD = libBlis.la $(BLISLIB_LIBS)
blis_DEPENDENCIES = libBlis.la $(BLISLIB_DEPENDENCIES)

########################################################################
#                            Additional flags                          #
########################################################################

# Here list all include flags, relative to this "srcdir" directory.  This
# "cygpath" stuff is necessary to compile with native compilers on Windows.
AM_CPPFLAGS = $(BLISLIB_CFLAGS)

# This line is necessary to allow VPATH compilation
DEFAULT_INCLUDES = -I. -I`$(CYGPATH_W) $(srcdir)`

########################################################################
#                Headers that need to be installed                     #
########################################################################

# Here list all the header files that are required by a user of the library,
# and that therefore should be installed in 'install/coin'
includecoindir = $(includedir)/coin
includecoin_HEADERS = \
	BlisBranchObjectInt.h \
	BlisBranchStrategyMaxInf.h \
	BlisBranchStrategyPseudo.h \
	BlisBranchStrategyRel.h \
	BlisBranchStrategyStrong.h \
	BlisConGenerator.h \
	BlisConstraint.h \
	Blis.h \
	BlisHelp.h \
	BlisHeuristic.h \
	BlisHeurRound.h \
	BlisLicense.h \
	BlisMessage.h \
	BlisModel.h \
	BlisNodeDesc.h \
	BlisObjectInt.h \
	BlisParams.h \
	BlisPresolve.h \
	BlisPseudo.h \
	BlisSolution.h \
	BlisSubTree.h \
	BlisTreeNode.h \
	BlisVariable.h

########################################################################
#                     Installing manifest (MSVC++)                     #
########################################################################

if COIN_CXX_IS_CL
install-exec-hook:
	test -s blis.exe.manifest && \
	mt -manifest blis.exe.manifest -outputresource:blis.exe;\
	cp blis.exe $(bindir)/blis.exe
endif

#############################################################################
# Create the Config.h file that always defines HAVE_CONFIG_H and install it #
#############################################################################

install-exec-local:
	$(install_sh_DATA) config_blis.h $(DESTDIR)$(includecoindir)/BlisConfig.h

uninstall-local:
	rm -f $(DESTDIR)$(includecoindir)/BlisConfig.h

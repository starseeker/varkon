#########################################
#
#   Makefile for Varkon version 1.19C
#   ---------------------------------
#
#   2008-02-20 J.Kjellander
#
##########################################

#=========================================================
# Build options:
#=========================================================

#Uncomment the platform you are running on
PLATFORM=linux
#PLATFORM=FreeBSD
#PLATFORM=MacOSX

# Comment the line below to exclude support for TIFF
# output.
DV3TIFF=TRUE
export DV3TIFF

# Uncomment the line below to include ODBC support
# see also README_ODBC
# DV3ODBC=TRUE
export DV3ODBC

# Uncomment the line below to include routines for Robot
# Application Protocol (RAP). Only on Linux platform.
# DV3RAP=TRUE
export DV3RAP

# Uncomment the line below to include routines specific
# for Örebro University.
# DV3ORU=TRUE
export DV3ORU

#=========================================================
# End build options
#=========================================================

.PHONY: distr debug

distr:   BUILDTARGET=distr
debug:   BUILDTARGET=debug

distr debug:

# create directories if needed
	mkdir -p ./IG/lib
	mkdir -p ./PM/lib
	mkdir -p ./EX/lib
	mkdir -p ./DB/lib
	mkdir -p ./WP/lib
	mkdir -p ./GE/lib
	mkdir -p ./AN/lib
	mkdir -p ./PL/lib
	mkdir -p ../bin
	mkdir -p ../tmp
	mkdir -p ../man

# If SVN is used, update includefile with current SVNVERSION.
	./update_versionfile

# Make xvarkon
	$(MAKE) -C IG -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	$(MAKE) -C PM -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	$(MAKE) -C EX -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	$(MAKE) -C DB -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	$(MAKE) -C WP -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	$(MAKE) -C GE -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	$(MAKE) -f Makefile.$(PLATFORM)

# Make mbsc
	$(MAKE) -C AN -C src -f Makefile.$(PLATFORM)

# Make plotter programs
	$(MAKE) -C PL -C src -f Makefile.$(PLATFORM) PLLIB
	$(MAKE) -C PL -C src -f Makefile.$(PLATFORM) POSTSCRIPT
	$(MAKE) -C PL -C src -f Makefile.$(PLATFORM) HPCL5
	$(MAKE) -C PL -C src -f Makefile.$(PLATFORM) HPGL2
	$(MAKE) -C PL -C src -f Makefile.$(PLATFORM) HP7475

# Make vlib
	$(MAKE) -C Vlib -f Makefile

# Make man
	$(MAKE) -C MAN -C src -f Makefile

clean:
	$(MAKE) -C IG -C src -f Makefile.$(PLATFORM) clean
	$(MAKE) -C PM -C src -f Makefile.$(PLATFORM) clean
	$(MAKE) -C EX -C src -f Makefile.$(PLATFORM) clean
	$(MAKE) -C DB -C src -f Makefile.$(PLATFORM) clean
	$(MAKE) -C WP -C src -f Makefile.$(PLATFORM) clean
	$(MAKE) -C GE -C src -f Makefile.$(PLATFORM) clean
	$(MAKE) -C AN -C src -f Makefile.$(PLATFORM) clean
	$(MAKE) -C PL -C src -f Makefile.$(PLATFORM) clean
	$(MAKE) -C MAN -C src -f Makefile clean
	$(MAKE) -C Vlib -f Makefile clean
	rm -f -r ../bin

##########################################################

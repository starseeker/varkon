##########################################################
#
#   Makefile for building xvarkon,mbsc and varkon lib
#   -------------------------------------------------
#
#   Sören Larsson 2004-04-27
#
##########################################################



#=========================================================
# Build options:
#=========================================================

#Uncomment the platform you are running on
PLATFORM=linux
#PLATFORM=crayT3E
#PLATFORM=sgi
#PLATFORM=aix
#PLATFORM=hpux
#PLATFORM=sun
#PLATFORM=FreeBSD

# Uncomment the line below to include routines for Robot
# Application Protocol (RAP). Only on Linux platform.
# DV3RAP=TRUE

# Uncomment the line below to include routines specific
# for Örebro University.
# DV3ORU=TRUE



#=========================================================
# End build options
#=========================================================

export DV3RAP
export DV3ORU

.PHONY: distr debug kdebug

distr:   BUILDTARGET=distr
debug:   BUILDTARGET=debug

distr debug:

# create lib directories if needed
	mkdir -p ./IG/lib
	mkdir -p ./PM/lib
	mkdir -p ./EX/lib
	mkdir -p ./GP/lib
	mkdir -p ./DB/lib
	mkdir -p ./WP/lib
	mkdir -p ./GE/lib
	mkdir -p ./AN/lib
	mkdir -p ./PL/lib
    
#make xvarkon
	@make -C IG -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	@make -C PM -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	@make -C EX -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	@make -C GP -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	@make -C DB -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	@make -C WP -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	@make -C GE -C src -f Makefile.$(PLATFORM) $(BUILDTARGET)
	@make -f Makefile.$(PLATFORM)

#make mbsc
	@make -C AN -C src -f Makefile.$(PLATFORM)

#make plotter programs
	@make -C PL -C src -f Makefile.$(PLATFORM) all

#make vlib
	@make -C Vlib -f Makefile
           
clean:
	@make -C IG -C src -f Makefile.$(PLATFORM) clean
	@make -C PM -C src -f Makefile.$(PLATFORM) clean
	@make -C EX -C src -f Makefile.$(PLATFORM) clean
	@make -C GP -C src -f Makefile.$(PLATFORM) clean
	@make -C DB -C src -f Makefile.$(PLATFORM) clean
	@make -C WP -C src -f Makefile.$(PLATFORM) clean
	@make -C GE -C src -f Makefile.$(PLATFORM) clean
	@make -C AN -C src -f Makefile.$(PLATFORM) clean

##########################################################

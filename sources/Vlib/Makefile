#####################################################################
#
#  Compile the MBS source modules and install the
#  MBO-files in $VARKON_ROOT/lib
#
#  2008-02-27 J.Kjellander
#
#####################################################################

.PHONY:distr

distr :
	mkdir -p ../../lib
	$(MAKE) -C analysis -f Makefile
	$(MAKE) -C dxf      -f Makefile
	$(MAKE) -C edit     -f Makefile
	$(MAKE) -C fonts    -C fedit  -f Makefile
	$(MAKE) -C iges     -f Makefile
	$(MAKE) -C macro    -f Makefile
	$(MAKE) -C stl      -f Makefile

clean :
	rm -f -r ../../lib
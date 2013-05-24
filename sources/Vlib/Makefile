#####################################################################
#
#  Compile the MBS source modules and install the
#  MBO-files in $VARKON_ROOT/lib
#
#  2004 Sören Larsson, Örebro University
#
#####################################################################

.PHONY:distr

distr :
	mkdir -p ../../lib
	@make -C analysis   -f Makefile
	@make -C dxf   -f Makefile
	@make -C edit   -f Makefile
	@make -C fonts  -C fedit  -f Makefile
	@make -C iges   -f Makefile
	@make -C macro   -f Makefile
	@make -C plott   -f Makefile
	@make -C stl   -f Makefile


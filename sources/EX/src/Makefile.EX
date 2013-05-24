##########################################################
#
#   Makefile.EX
#   -----------
#
#  Make rules for the EX module
#
#  (C)2007 J.Kjellander, Sören Larsson
#
##########################################################

INCFILES = ../include/EX.h              \
           ../../GE/include/GE.h        \
           ../../DB/include/DB.h

OBJECTS = ../lib/EXlib.a(exarc.o)      \
          ../lib/EXlib.a(exbplane.o)   \
          ../lib/EXlib.a(excsy.o)      \
          ../lib/EXlib.a(excugl.o)     \
          ../lib/EXlib.a(excur1.o)     \
          ../lib/EXlib.a(excur2.o)     \
          ../lib/EXlib.a(exdia1.o)     \
          ../lib/EXlib.a(exdia2.o)     \
          ../lib/EXlib.a(exdim.o)      \
          ../lib/EXlib.a(exdraw.o)     \
          ../lib/EXlib.a(exgatt.o)     \
          ../lib/EXlib.a(exgeofun1.o)  \
          ../lib/EXlib.a(exgeofun2.o)  \
          ../lib/EXlib.a(exget1.o)     \
          ../lib/EXlib.a(exget2.o)     \
          ../lib/EXlib.a(exgroup.o)    \
          ../lib/EXlib.a(exline.o)     \
          ../lib/EXlib.a(exmesh.o)     \
          ../lib/EXlib.a(exmove.o)     \
          ../lib/EXlib.a(exodbc.o)     \
          ../lib/EXlib.a(exoru.o)      \
          ../lib/EXlib.a(exos.o)       \
          ../lib/EXlib.a(expart.o)     \
          ../lib/EXlib.a(expoint.o)    \
          ../lib/EXlib.a(exrap.o)      \
          ../lib/EXlib.a(exref.o)      \
          ../lib/EXlib.a(exjob.o)      \
          ../lib/EXlib.a(exsugl.o)     \
          ../lib/EXlib.a(exsur1.o)     \
          ../lib/EXlib.a(exsur2.o)     \
          ../lib/EXlib.a(exsur3.o)     \
          ../lib/EXlib.a(exsurcur.o)   \
          ../lib/EXlib.a(exsurexd.o)   \
          ../lib/EXlib.a(exsurtrim.o)  \
          ../lib/EXlib.a(exsurusr.o)   \
          ../lib/EXlib.a(exsymb.o)     \
          ../lib/EXlib.a(extcp.o)      \
          ../lib/EXlib.a(extext.o)     \
          ../lib/EXlib.a(extform.o)    \
          ../lib/EXlib.a(extime.o)     \
          ../lib/EXlib.a(exview.o)     \
          ../lib/EXlib.a(exlevel.o)    \
          ../lib/EXlib.a(exxhatch.o)   \
          ../lib/EXlib.a(rap_clnt.o)   \
          ../lib/EXlib.a(rap_xdr.o


ifeq ($(DV3RAP),TRUE)
RAPOBJECTS = ../lib/EXlib.a(rap_clnt.o) \
             ../lib/EXlib.a(rap_xdr.o)
endif  
            
# EXlib.a depends on the objects. 

distr : $(OBJECTS) $(RAPOBJECTS)

debug : $(OBJECTS) $(RAPOBJECTS)

clean:
	rm -f ../lib/EXlib.a


# Objects depend on sources implicitly and sources depend
# on the following include-files.

$(OBJECTS) : $(INCFILES)
 

# Special rules needed for RAP-RPC
#-----------------------------------
ifeq ($(DV3RAP),TRUE)

rap.h rap_clnt.c rap_xdr.c: rap.x
	$(RPCGENERATE) rap.x
	rm -f rap_svc.c

../lib/EXlib.a(rap_clnt.o): rap_clnt.c rap.h
	$(CC) -c $(RAPCFLAGS) rap_clnt.c
	ar r ../lib/EXlib.a rap_clnt.o
	$(RM) rap_clnt.o

../lib/EXlib.a(rap_xdr.o): rap_xdr.c rap.h
	$(CC) -c $(RAPCFLAGS)  rap_xdr.c
	ar r ../lib/EXlib.a rap_xdr.o
	$(RM) rap_xdr.o

../lib/EXlib.a(exrap.o): exrap.c rap.h $(INCFILES)
	$(CC) -c $(RAPCFLAGS) -DV3RAP exrap.c
	ar r ../lib/EXlib.a exrap.o
	$(RM) exrap.o

endif



# Special rules for ORU specific routines
#------------------------------------------
        
ifeq ($(DV3ORU),TRUE)

../lib/EXlib.a(exoru.o): exoru.c $(INCFILES)
	$(CC) -c $(CFLAGS) -DV3ORU exoru.c
	ar r ../lib/EXlib.a exoru.o
	$(RM) exoru.o
 
endif  




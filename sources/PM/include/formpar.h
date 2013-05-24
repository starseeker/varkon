/*!******************************************************************
*  File: formpar.h 
*  ===============   
* 
*  This file is part of the Program Module Library. 
*    
*  This library is free software; you can redistribute it and/or 
*  modify it under the terms of the GNU Library General Public 
*  License as published by the Free Software Foundation; either 
*  version 2 of the License, or (at your option) any later 
*  version. 
* 
*  This library is distributed in the hope that it will be 
*  useful, but WITHOUT ANY WARRANTY; without even the implied  
*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
*  PURPOSE.  See the GNU Library General Public License for more  
*  details.   
*   
*  You should have received a copy of the GNU Library General 
*  Public License along with this library; if not, write to the 
*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge, 
*  MA 02139, USA. 
*       
*  (C) J.Kjellander, Örebro university
*   
********************************************************************/
/*******************************************************************
*
*        formpar.h
*        =========
*
*   Definition of all MBS-routine parameter lists.
*
*   Simple types:
*                  v = vector = stvecp
*                  f = float  = stflop
*                  i = integer = stintp
*                  s = string  = ststrp
*                  r = reference = strefp
*                  d = file ( device ) = stfilp
*                  a = anytype = NULL
*                  - = no type - return type for procedures
*   Array types:
*                  j = 4*4 float matrix           = st_j_p
*                  k = conformant array of st_j_p = st_k_p
*                  l = conformant array of float  = st_l_p
*                  m = conformant array of refs.  = st_m_p
*                  n = conformant array of vector = st_n_p
*                  o = conformant array of string = st_o_p
*                  p = conformant array of int    = st_p_p
*
*   Classes:          
*                  e = expresssion = ST_EXPCL
*                  v = variable = ST_VARCL
*                  d = default = ST_DEFCL
*
*   Geometry:       
*                  5 = 2D and 3D
*                  2 = only 2D
*                  3 = only 3D
*                              
********************************************************************/
/*
***Parameter lists for Varkon MBS functions and procedures.
*/

#ifdef VARKON

/*
***       = 0,  not defined
*/
     "",
/*
***VPOI = 1,  POI_FREE(-,<vec>)
*/
     "-5:ve",
/*
***VPOIP = 2,  POI_PROJ(-,<vec>)
*/
     "-3:ve",
/*
***VLINF = 3,  LIN_FREE<-,<vec>,<vec>
*/
     "-5:ve.ve",
/*
***VLINP = 4,  LIN_PROJ(-,<vec>,<vec>)
*/
     "-3:ve.ve",
/*
***VLINA = 5,  LIN_ANG(-,<vec>,<flo>,<flo>) 
*/
     "-5:ve.fe.fe",
/*
***VLINO = 6,  LIN_OFFS(-,<ref>,<flo>) 
*/
     "-5:re.fe",
/*
***VLINT1 = 7,  LIN_TAN1(-,<vec>,<ref>,<int>)
*/
     "-5:ve.re.ie",
/*
***VLINT2 = 8,  LIN_TAN2(-,<ref>,<ref>,<int>)
*/
     "-5:re.re.ie",
/*
***VARCF = 9,  ARC_1POS(-,<vec>,<flo>,<flo>,<flo>)
*/
     "-5:ve.fe.1(fd<0>).1(fd<360>)",
/*
***VARC2P = 10, ARC_2POS(-,<vec>,<vec>,<flo>)
*/
     "-5:ve.ve.fe",
/*
***VARC3P = 11, ARC_3POS(-,<vec>,<vec>,<vec>)
*/
     "-5:ve.ve.ve",
/*
***VARCO = 12, ARC_OFFS(-,<ref>,<flo>)
*/
     "-5:re.fe",
/*
***VARCFI = 13, ARC_FIL(-,<ref>,<ref>,<flo>,<int>)
*/
     "-5:re.re.fe.ie",
/*
***VCURF = 14, CUR_FREE(-,[<vec>,<vec>])
*/
     "-5:ve.vd<0,0,0>.ve.1(vd<0,0,0>.24(ve.vd<0,0,0>))",
/*
***VCURP = 15, CUR_PROJ(-,[<vec>,<vec>])
*/
     "-3:ve.vd<0,0,0>.ve.1(vd<0,0,0>.24(ve.vd<0,0,0>))",
/*
***VCURO = 16, CUR_OFFS(-,<ref>,<flo>)
*/
     "-5:re.fe",
/*
***VCOMP = 17, CUR_COMP(-,<ref>[,<ref>])
*/
     "-5:re.24(re)",
/*
***VXHT = 18, XHATCH(-,<flo>,<flo>,<ref>[,<ref>])
*/
     "-2:fe.fe.re.49(re)",
/*
***VPART = 19, PART(-,-,<ref>)
*/
     "-5:1(re)",
/*
***VIONSUR = 20, INV_ON_SUR(ref,p,kod,nï¿½start_uvï¿½
*/
     "v3:re.ve.ie.ie.1(ve)",
/*
***VTEXT = 21, TEXT(id,pos,angle,string);
*/
     "-5:ve.fe.se",
/*
***VPINC = 22, status:=POS_IN_CONE(p1,p2,r1,r2,npts,pts,pstat);
*/
     "i5:ve.ve.fe.fe.ie.nv.1(pv)",
/*
***VCSYS3P = 23, CSYS_3P(id,namn,origo,xaxel[,yaxel]);
***Y-Axis optional 1996-05-28 JK.
*/
     "-5:se.ve.ve.1(ve)",
/*
***VGROUP = 24, GROUP(---,<str>,<ref>[,<ref>])
*/
     "-5:se.re.99(re)",
/*
***VLDIM = 25, LDIM(-,<vec>,<vec>,<vec>,<int>)
*/
     "-2:ve.ve.ve.ie",
/*
***VCDIM = 26, CDIM(-,<ref>,<pos>,<int>) 
*/
     "-2:re.ve.ie",
/*
***VRDIM = 27, RDIM(-,<ref>,<vec>,<vec>)
*/
     "-2:re.ve.ve",
/*
***VADIM = 28, ADIM(-,<ref>,<ref>,<vec>,<int>)
*/
     "-2:re.re.ve.ie",
/*
***VACOS = 29, ARCCOS(<float>) = <float>
*/
     "f5:fe",
/*
***VASIN = 30, ARCSIN(<float>) = <float>
*/
     "f5:fe",
/*
***VATAN = 31, ARCTAN(<float>) = <float>
*/
     "f5:fe",
/*
***VCOS = 32, COS(<float>) = <float>
*/
     "f5:fe",
/*
***VSIN = 33, SIN(<float>) = <float>
*/
     "f5:fe",
/*
***VTAN = 34, TAN(<float>) = <float>
*/
     "f5:fe",
/*
***VLN = 35, LN(<float>) = <float>
*/
     "f5:fe",
/*
***VLOG = 36, LOG(<float>) = <float>
*/
     "f5:fe",
/*
***VSQRT = 37, SQRT(<float>) = <float>
*/
     "f5:fe",
/*
***VABS = 38, ABS(<float> | <int>) = <float> | <int>
*/
     "f5:fe",
/*
***VFRAC = 39, FRAC(<float>) = <float>
*/
     "f5:fe",
/*
***VROUND = 40, ROUND(<float>) = <int>
*/
     "i5:fe",
/*
***VTRUNC = 41, TRUNC(<float>) = <int>
*/
     "i5:fe",
/*
***VGTREF = 42, GETREF(<ref>,<int>)
*/
     "r5:re.ie",
/*
***VLDGM = 43, LOAD_GM(<str>)
*/
     "-5:se",
/*
***VSVGM = 44, SAVE_GM(<str>)
*/
     "-5:se",
/*
***VBLEV = 45, BLANK_LEV(). Replaced by 386=BLANK_LEVEL()
*/
     "-5:ie.1(id<0>)",
/*
***VUBLEV = 46, UNBLANK_LEV(). Replaced by 387=UNBLANK_LEVEL()
*/
     "-5:ie.1(id<0>)",
/*
***VGTLEV = 47, GET_LEV(). Replaced by 388=GET_LEVEL()
*/
     "-5:ie.iv.sv",
/*
***VACLEV = 48, ACT_LEV(). Replaced by 389=ACT_LEVEL()
*/
     "i5:",
/*
***VNCURI = 49, N_CUR_INT(ref1,ref2)
*/
     "i3:re.re",
/*
***VEVAL = 50, EVAL(ref,case,inval)
*/
     "v5:re.se.ve",
/*
***VFINDS = 51, FINDS(<str>,<str>) = <int>
*/
    "i5:se.se",
/*
***VARCL = 52, ARCL(<ref>) = <flo>
*/
    "f5:re",
/*
***VTANG = 53, TANG(<ref>ï¿½<flo>ï¿½ = <vec>
*/
    "v5:re.1(fe)",
/*
***VCENTRE = 54, p:=CENTRE(ref[,parval])
*/
    "v5:re.1(fe)",
/*
***VINTERS = 55, p:=INTERSECT(ref1,ref2[,ord[,utdata]])
*/
    "v5:re.re.1(id<1>).1(id<0>)",
/*
***VON = 56, p:=ON(ref[,float/vector]);
*/
     "v5:re.1(ae)",
/*
***VVECP = 57, p:=VECP(radie,vinkel[,z]);
*/
     "v5:fe.fe.1(fe<0.0>)",
/*
***VIDENT = 58, ref:=IDENT(typmask[,end,side]);
*/
     "r5:ie.2(iv)",
/*
***VPOS = 59 pos:=POS([tkn]);
*/
     "v5:1(sv)",
/*
***VSCREEN = 60, pos:=SCREEN[win_id]);
*/
     "v5:1(iv)",
/*
***VBPLAN = 61  B_PLANE(<vec>,<vec>,<vec>,<vec>)
*/
     "-5:ve.ve.ve.ve",
/*
***VMODLO = 62, MODE_LOCAL(<ref>)
*/
     "-5:re",
/*
***VMODGL = 63, MODE_GLOBAL()
*/
     "-5:",
/*
***VVEC = 64, pos:=VEC(<flo>,<flo>[,<flo>])
*/
     "v5:fe.fe.1(fe<0.0>)",
/*
***VSET = 65, SET()
*/
     "-5:",
/*
***VOS = 66, OS(<str>,[<mode>])
*/
     "-5:se.1(ie)",
/*
***VVECL = 67, VECL(<vec>) = <flo>
*/
     "f5:ve",
/*
***VECN = 68, VECN(<vec>) = <vec>
*/
     "v5:ve",

/*
***VANGLE = 69, ANGLE(<flo>,<flo>) = <flo>
*/
     "f5:fe.fe",
/*
***VASCII = 70, ASCII(<str>) = <int>
*/
     "i5:se",
/*
***VSTR = 71, str(<flo>ï¿½<int>ï¿½<int>ï¿½) = <str>
*/
     "s5:fe.1(ie<20>.1(ie<-2>))",
/*
***VCHR = 72, string:=CHR(ascii_integer);
*/
     "s5:ie",
/*
***VVAL = 73, float_value:=VAL(string);
*/
     "f5:se",
/*
***VLENGTH = 74, l:=LENGTH(string);
*/
     "i5:se",
/*
***VSUBSTR = 75, str:=SUBSTR(string,start[,num])
*/
     "s5:se.ie.1(ie<0>)",
/*
***VOPEN = 76, OPEN(fil,mode,name);
*/
     "-5:de.se.se",
/*
***VSEEK = 77, SEEK(fil,int);
*/
     "-5:de.ie",
/*
***VOUTINT = 78, OUTINT(fil,int[,int]);
*/
     "-5:de.ie.1(ie<12>)",
/*
***VOUTFLT = 79, OUTFLT(fil,float[,int[,int]]);
*/
     "-5:de.fe.1(ie<20>.1(ie<-1>))",
/*
***VOUTSTR = 80, OUTSTR(fil,str[,width]);
*/
     "-5:de.se.1(ie<0>)",
/*
***VOUTLIN = 81, OUTLIN(fil);
*/
     "-5:de",
/*
***VOUTBIN = 82, OUTBIN(fil,uttryck);
*/
     "-5:de.ae",
/*
***VININT = 83, int:=ININT(fil[,ntkn]);
*/
     "i5:de.1(ie<0>)", 
/*
***VINFLT = 84, float:=INFLT(fil[,ntkn]);
*/
     "f5:de.1(ie<0>)",
/*
***VINSTR = 85, s:=INSTR(fil[,ntkn]);
*/
     "s5:de.1(ie<0>)",
/*
***VINLIN = 86, s:=INLIN(fil);
*/
     "s5:de",
/*
***VINBIN = 87, INBIN(fil,variabel[,nbytes])
*/
     "-5:de.av.1(ie)",
/*
***VFPOD = 88, pos:=FPOS(fil);
*/
     "i5:de",
/*
***VCLOSE = 89, CLOSE([fil]);
*/
     "-5:1(de)",
/*
***VIOSTAT = 90, status:=IOSTAT(filvariabel);
*/
     "i5:de",
/*
***VGTID = 91, GETID.
*/
     "r5:se",
/*
***VGTHDR = 92, GETHDR.
*/
     "-5:re.iv.iv.iv.iv.iv.mv",
/*
***VGTPOI = 93, GETPOI.
*/
     "-5:re.vv",
/*
***VGTLIN  = 94, GETLIN.
*/
     "-5:re.iv.fv.vv.vv",
/*
***VGTARC = 95, GETARC.
*/
     "-5:re.iv.fv.vv.fv.fv.fv.iv.kv",     
/*
***VGTCUR = 96, GETCUR.
*/
     "-5:re.iv.fv.iv.kv.lv.iv.jv",
/*
***VGTTRF = 97, GETTRF.
*/
     "-5:re.jv",
/*
***VGTTXT = 98, GETTXT.
*/
     "-5:re.vv.iv.fv.fv.fv.fv.sv",
/*
***VGTXHT = 99, GETXHT.
*/
     "-5:re.iv.fv.fv.fv.iv.lv",
/*
***VGTLDM = 100, GETLDM.
*/
     "-5:re.vv.vv.vv.iv.fv.fv.iv.iv",
/*
***VGTCDM = 101, GETCDM.
*/
     "-5:re.vv.vv.vv.iv.fv.fv.iv.iv",
/*
***VGTRDM = 102, GETRDM.
*/
     "-5:re.vv.vv.vv.fv.fv.fv.iv.iv",
/*
***VGTADM = 103, GETADM.
*/
     "-5:re.vv.fv.fv.fv.fv.fv.fv.fv.fv.iv.iv",
/*
***VGTGRP = 104, GETGRP.
*/
     "-5:re.sv.iv.mv",
/*
***VGTCSY = 105, GETCSY.
*/
     "-5:re.sv.jv",
/*
***VGTSYM = 106, GETSYM. Obsolete !!!!!!!!!!!!!!!!!!!!!!
*/
     "-5:re.sv.vv.fv.fv.iv.lv",
/*
***VGTPRT = 107, GETPRT.
*/
     "-5:re.sv.iv.iv.rv.iv.iv.mv",
/*
***VGTTYP = 108, GETTYP.
*/
     "i5:re.ie",
/*
***VGTINT = 109, GETINT.
*/
     "i5:re.ie",
/*
***VGTFLT = 110, GETFLT.
*/
     "f5:re.ie",
/*
***VGTSTR = 111, GETSTR.
*/
     "s5:re.ie",
/*
***VGTVEC = 112, GETVEC.
*/
     "v5:re.ie",
/*
***VSYMB = 113, SYMB.
*/
     "-2:se.ve.1(fe<1.0>.1(fe<0.0>))",
/*
***VTRIM = 114, TRIM.
*/
     "-2:re.ie.re.1(ie<1>)",
/*
***VDEL = 115, DEL.
*/
     "-5:re",
/*
***VCLGM = 116, CLEAR_GM.
*/
     "-5:",
/*
***VBLNK = 117, BLANK.
*/
     "-5:re",
/*
***VUBLNK = 118, UNBLANK.
*/
     "-5:re",
/*
***VCREVI = 119, CRE_VIEW.
*/
     "-5:se.ae",
/*
***VACTVI = 120, ACT_VIEW.
*/
     "-5:se.1(id<0>)",
/*
***VSCLVI = 121, SCL_VIEW.
*/
     "-5:se.fe",
/*
***VCENVI = 122, CEN_VIEW.
*/
     "-5:se.ve",
/*
***VERAVI = 123, ERASE_VIEW.
*/
     "-5:1(id<0>)",
/*
***VREPVI = 124, REP_VIEW.
*/
     "-5:2(ie)",
/*
***VCACVI = 125, CACC_VIEW.
*/
     "-5:fe",
/*
***VPLTVI = 126, PLOT_VIEW, Removed !!!.
*/
     "-5:se.se",
/*
***VTIM = 127, TIME.
*/
     "-5:iv.iv.iv.iv.iv.iv",
/*
***VRSTR = 128, RSTR.
*/
     "s5:re.1(ie<0>)",
/*
***VRVAL = 129, RVAL.
*/
     "r5:se",
/*
***VREFC = 130, REFC.
*/
     "r5:ie.1(id<1>).9(ie.id<1>)",
/*
***VLSIN = 131, LST_INI.
*/
     "-5:se",
/*
***VLSEX = 132, LST_EXI.
*/
     "-5:",
/*
***VLSLN = 133, LST_LIN.
*/
     "-5:se",
/*
***VACBLK = 134, ACT_BLANK
*/
     "i5:",
/*
***VACVIN = 135, ACT_VNAM
*/
     "s5:",
/*
***VACPEN = 136, ACT_PEN
*/
     "i5:",
/*
***VACSCL = 137, ACT_SCL
*/
     "f5:",
/*
***VACDSCL = 138, ACT_DSCL
*/
     "f5:",
/*
***VACCAC = 139, ACT_CACC
*/
     "f5:",
/*
***VACGRX = 140, ACT_GRIDX
*/
     "f5:",
/*
***VACGRY = 141, ACT_GRIDY
*/
     "f5:",
/*
***VACGRDX = 142, ACT_GRIDDX
*/
     "f5:",
/*
***VACGRDY = 143, ACT_GRIDDY
*/
     "f5:",
/*
***VACGRID = 144, ACT_GRID
*/
     "i5:",
/*
***VACJOBN = 145, ACT_JOBNAM
*/
     "s5:",
/*
***VACMTYP = 146, ACT_MTYPE
*/
     "i5:",
/*
***VACMATR = 147, ACT_MATTR
*/
     "i5:",
/*
***VACJOBD = 148, ACT_JOBDIR
*/
     "s5:",
/*
***VACLFNT = 149, ACT_LFONT
*/
     "i5:",
/*
***VACAFNT = 150, ACT_AFONT
*/
     "i5:",
/*
***VACXFNT = 151, ACT_XFONT
*/
     "i5:",
/*
***VACLDSL = 152, ACT_LDASHL
*/
     "f5:",
/*
***VACADSL = 153, ACT_ADASHL
*/
     "f5:",
/*
***VACXDSL = 154, ACT_XDASHL
*/
     "f5:",
/*
***VACTSIZ = 155, ACT_TSIZE
*/
     "f5:",
/*
***VACTWID = 156, ACT_TWIDTH
*/
     "f5:",
/*
***VACTSLT = 157, ACT_TSLANT
*/
     "f5:",
/*
***VACDTSZ = 158, ACT_DTSIZ
*/
     "f5:",
/*
***VACDASZ = 159, ACT_DASIZ
*/
     "f5:",
/*
***VACDNDG = 160, ACT_DNDIG
*/
     "i5:",
/*
***VACDAUT = 161, ACT_DAUTO
*/
     "i5:",
/*
***VGLOREF = 162, GLOBAL_REF
*/
     "r5:re.1(ie<0>)",
/*
***VPSPMT  = 163, PSH_PMT
*/
     "-5:se",
/*
***VPOPMT  = 164, POP_PMT
*/
     "-5:",
/*
***VCRTSTR = 165, CRE_TSTR
*/
     "-5:ie.se",
/*
***VGTTSTR = 166, GET_TSTR
*/
     "s5:ie",
/*
***VCRMEN  = 167, CRE_MEN
*/
     "-5:ie.se.20(se.se)",
/*
***VPSMEN  = 168, PSH_MEN
*/
     "-5:ie",
/*
***VPOMEN  = 169, POP_MEN
*/
     "-5:",
/*
***VGTALT  = 170, GET_ALT
*/
     "s5:se",
/*
***VGTMEN  = 171, GET_MEN
*/
     "-5:ie.sv",
/*
***VVPROD  = 172, VPROD
*/
     "v5:ve.ve",
/*
***VLINPER = 173, LIN_PERP
*/
     "-5:ve.re.fe",
/*
***VDBOP = 174, DB_OPEN
*/
     "i5:se",
/*
***VDBBG = 175, DB_BEGIN
*/
     "i5:se",
/*
***VDBSL = 176, DB_SELECT
*/
     "i5:se.se.ae.se.ae.20(se.ie)",
/*
***VDBUP = 177, DB_UPDATE
*/
     "i5:se.se.ae.se.ae.20(se.ae)",
/*
***VDBIN = 178, DB_INSERT
*/
     "i5:se.20(se.ae)",
/*
***VDBDL = 179, DB_DELETE
*/
     "i5:se.se.ae.se.ae",
/*
***VDBEN = 180, DB_END
*/
     "i5:",
/*
***VDBRL = 181, DB_ROLLBACK
*/
     "i5:",
/*
***VDBCL = 182, DB_CLOSE
*/
     "i5:",
/*
***VDBFI = 183, DB_FETCHI
*/
     "i5:ie.iv",
/*
***VDBFF = 184, DB_FETCHF
*/
     "i5:ie.fv",
/*
***VDBFS = 185, DB_FETCHS
*/
     "i5:ie.sv",
/*
***VDBNX = 186, DB_NEXT
*/
     "i5:",
/*
***VGTBPL = 187, GETBPL.
*/
     "-5:re.vv.vv.vv.vv",
/*
***VCSYS1P = 188, CSYS_1P.
*/
     "-5:se.ve.3(fd<0.0>)",
/*
***VPLTWI = 189, PLOT_WIN.
*/
     "-5:ie.ve.ve.se.1(ve)",
/*
***VUPHDR  = 190, UPDHDR.
*/
     "-5:re.ie.ie.ie.ie",
/*
***VLDJOB  = 191, LOAD_JOB.
*/
     "-5:se.ie.48(ie)",
/*
***VSVJOB  = 192, SAVE_JOB.
*/
     "-5:se",
/*
***VINPMT  = 193, INPMT.   OBS OBS ie borde vï¿½ vara id ????
***ï¿½en se kan vara default.
*/
     "s5:se.se.1(ie<0>)",
/*
***VHIDVI  = 194, HIDE_VIEW.
*/
     "-3:se.ie.1(sd<\042\042>).1(ve)",
/*
***VPRPVI  = 195, PERP_VIEW.
*/
     "-3:se.fe",
/*
***VTEXTL  = 196, TEXTL.
*/
     "f5:se",
/*
***VEXIT  = 197, EXIT.
*/
     "-5:1(se)",
/*
***VEXEST = 198, EXESTAT.
*/
     "i5:",
/*
***VAREA  = 199, AREA.
*/
     "f2:fe.re.49(re)",
/*
***VCGRAV = 200, CGRAV.
*/
     "v2:fe.re.49(re)",
/*
***VSETB = 201, SET_GLOBAL
*/
     "-5:",
/*
***VCURC = 202, CUR_CONIC
*/
     "-5:se.ve.ve.ae.ve.ve.24(ae.ve.ve)",
/*
***VITANG = 203, INV_TANG
*/
     "f5:re.ve.1(id<1>)",
/*
***VCURS = 204, CUR_SPLINE
*/
     "-5:se.ve.vd<0,0,0>.ve.1(vd<0,0,0>.50(ve.vd<0,0,0>))",
/*
***VION = 205, t:=INV_ON(ref,pos,onr);
*/
     "f5:re.ve.1(id<1>)",
/*
***VCURT = 206, CUR_TRIM
*/
     "-5:re.fe.fe",
/*
***VCURU = 207, CUR_USRDEF
*/
     "-5:ie.kv.1(re)",
/*
***VCURV = 208, CURV
*/
     "f5:re.fe",
/*
***VICURV = 209, INV_CURV
*/
     "f5:re.fe.1(id<1>)",
/*
***VIARCL = 210, INV_ARCL
*/
     "f5:re.fe",
/*
***VACCFNT = 211, ACT_CFONT
*/
     "i5:",
/*
***VACCDSL = 212, ACT_CDASHL
*/
     "f5:",
/*
***VADDMBS = 213, ADD_MBS
*/
     "r5:se.49(se)",
/*
***VNINTER = 214, N_INTERSECT
*/
     "i5:re.re.1(id<1>)",
/*
***VGPMBS = 215, GETP_MBS
*/
     "s5:re.ie",
/*
***VUPMBS = 216, UPDP_MBS
*/
     "-5:re.ie.se.1(id<0>)",
/*
***VSUREX = 217, SUR_EXDEF
*/
     "-3:se.se.se.4(fd<0.0>).id<0>",
/*
***VCURSIL = 218, CUR_SIL(id,yta,riktn,typ,gren);
*/
     "-3:re.ve.ie.1(id<1>)",
/*
***VCURINT = 219, CUR_INT(id,yta,yta,onr);
*/
     "-3:re.re.1(id<1>)",
/*
***VCURISO = 220, CUR_ISO(id,yta,U/V,iso_val,start,slut);
*/
     "-3:re.se.fe.fe.fe",
/*
***VNORM = 221, NORM(ref,t eller u+v);
*/
     "v5:re.1(ae)",
/*
***VTFMOV = 222, TFORM_MOVE(id,p1,p2,sx,sy,sz);
*/
     "-5:ve.ve.3(fe)",
/*
***VTFROTP = 223, TFORM_ROTP(id,pos,ang,sx,sy,sz);
*/
     "-5:ve.fe.3(fe)",
/*
***VTFROTL = 224, TFORM_ROTL(id,p1,p2,ang,sx,sy,sz);
*/
     "-3:ve.ve.fe.3(fe)",
/*
***VTFMIRR = 225, TFORM_MIRR(id,p1,p2,p3);
*/
     "-5:ve.ve.1(ve)",
/*
***VTFCOMP = 226, TFORM_COMP(id,p1,p2,p3,p4,p5,p6);
*/
     "-3:ve.ve.ve.ve.ve.ve",
/*
***VTFU = 227, TFORM_USRDEF(id,matris);
*/
     "-5:jv",
/*
***VTCOPY = 228, TCOPY(id,copref,trfref,code);
*/
     "-5:re.re.1(id<0>)",
/*
***VCURGL = 229, CUR_GL
*/
     "-3:re.se",
/*
***VMSIZE = 230, MSIZE_VIEW(pos1,pos2);
*/
     "-5:vv.vv",
/*
***VCRWIN = 231, win_id:=CRE_WIN(pos,dx,dy,labelï¿½typï¿½;
*/
     "i5:ve.fe.fe.se.1(id<0>)",
/*
***VCREDI = 232, sub_id:=CRE_EDIT(win_id,pos,dx,dy,bw,default,ntkn);
*/
     "i5:ie.ve.fe.fe.fe.se.ie",
/*
***VCRICO = 233, sub_id:=CRE_ICON(win_id,pos,bw,ikon-filnamn,
*                                          col1,col2);
*/
     "i5:ie.ve.fe.se.2(ie)",
/*
***VCRBUT = 234, sub_id:=CRE_BUTTON(win_id,pos,dx,dy,bw,
*                                          ,off_str,on_str,fnt_str,
*                                          col1,col2);
*/
     "i5:ie.ve.fe.fe.fe.se.se.1(se.2(ie))",
/*
***VSHWIN = 235, SHOW_WIN(win_id);
*/
     "-5:ie",
/*
***VWTWIN = 236, sub_id:=WAIT_WIN(win_id);
*/
     "i5:ie",
/*
***VGTEDI = 237, string:=GET_EDIT(win_id,sub_id);
*/
     "s5:ie.ie",
/*
***VUPEDI = 238, status:=UPD_EDIT(win_id,sub_id,string);
*/
     "-5:ie.ie.se",
/*
***VGTBUT = 239, status:=GET_BUTTON(win_id,sub_id);
*/
     "i5:ie.ie",
/*
***VDLWIN = 240, DEL_WIN(win_idï¿½subwin_idï¿½;
*/
     "-5:ie.1(ie)",
/*
***VGTCUH = 241, GETCURH(id,font,sl,ns_geo,ns_gra);
*/
     "-5:re.iv.fv.iv.iv",
/*
***VGTSEG = 242, GETSEG(id,num,rep,sdat,off,typ,subtyp,id1,id2);
*/
     "-5:re.ie.ie.jv.fv.iv.1(iv.1(rv.1(rv)))",
/*
***VGTSUH = 243, GETSURH(id,kod,utdata);
*/
     "-5:re.se.av",
/*
***VUPPMBS = 244, UPDPP_MBS(part_id,parnum,newval,code);
*/
     "-5:re.ie.se.1(id<0>)",
/*
***VSURCON = 245, SUR_CONIC. Fï¿½ antal strippar se MXLSTP i V3.h.
*/
     "-3:re.re.re.se.re.re.re.24(se.re.re.re)",
/*
***VSUROF = 246, SUR_OFFS(id,ref,dist);
*/
     "-3:re.fe",
/*
***VGTVIEW = 247, GET_VIEW(id,namn,skala,xmin,ymin,xmax,ymax,trf,persp);
*/
     "-5:ie.sv.fv.fv.fv.fv.fv.jv.fv",
/*
***VDELMBS = 248, DEL_MBS(ref);
*/
     "-5:re",
/*
***VACTCSY = 249, ref:=ACT_CSY();
*/
     "r5:",
/*
***VMODBA = 250, MODE_BASIC();
*/
     "-5:",
/*
***VSURSA = 251, SUR_SPLARR(id,metod,nu,nv,pos,tan_u,tan_v,twist);
*/
     "-3:se.ie.ie.av.3(av)",
/*
***VSURCA = 252, SUR_CONARR(id,metod,ncur,spine,limarr,tanarr,mparr,curarr);
*/
     "-3:se.ie.re.mv.mv.ov.mv",
/*
***VSURAP = 253, SUR_APPROX(id,ref,typ,metod,tol1,tol2);
*/
     "-3:re.se.ie.fe.1(fe)",
/*
***VSURCO = 254, SUR_COMP(id,ref1,ref2...refn);
*/
     "-3:25(re)",
/*
***VSURT = 255, SUR_TRIM(id,ref,u1,u2,v1,v2,fall);
*/
     "-3:re.fe.fe.fe.fe.1(ie)",
/*
***VGTTOP = 256, GETTOPP(id,iu,iv,typ,u1,u2,v1,v2);
*/
     "-5:re.ie.ie.iv.fv.fv.fv.fv",
/*
***VGTCUP = 257, GETCUBP(id,iu,iv,data,offset);
*/
     "-5:re.ie.ie.lv.fv",
/*
***VGTRAP = 258, GETRATP(id,iu,iv,data,offset);
*/
     "-5:re.ie.ie.lv.fv",
/*
***VCURSA = 259, CUR_SPLARR(id,type,npos,pos,tan);
*/
     "-5:se.ie.nv.1(nv)",
/*
***VCURCA = 260, CUR_CONARR(id,type,npos,pos,tan,metod,pval,mpos);
*/
     "-5:se.ie.nv.nv.ov.lv.nv",
/*
***VCURFA = 261, CUR_FANGA(id,ref,riktn1,riktn2,strip1,strip2,u1,u2);
*/
     "-5:re.ve.ve.1(ie.ie.fe.fe)",
/*
***VCURGD = 262, CUR_GEODESIC(id,ref,metod,pos1,pos2);
*/
     "-5:re.se.ve.ve",
/*
***VCURAP = 263, CUR_APPROX(id,ref,typ,metod,tol1,tol2);
*/
     "-5:re.se.ie.fe.1(fe)",
/*
***VRUNMBS = 264, RUN_MBS();
*/
     "-5:",
/*
***VAPID = 265, string:=ACT_PID();
*/
     "s5:",
/*
***VAAVVERS = 266, string:=ACT_VARKON_VERSION();
*/
     "s5:",
/*
***VAVSER = 267, int:=ACT_VARKON_SERIAL();
*/
     "i5:",
/*
***VAOSTYP = 268, string:=ACT_OSTYPE();
*/
     "s5:",
/*
***VAHOST = 269, string:=ACT_HOST();
*/
     "s5:",
/*
***VUFNAM = 270, string:=UNIQUE_FILENAME();
*/
     "s5:",
/*
***VRAND = 271, int:=RANDOM(seed);
*/
     "f5:1(fe)",
/*
***VALLILS = 272, ALL_INT_LIN_SUR(ref,pos,riktn,sort,start,antal,uv,xyz);
*/
     "-3:re.ve.ve.id<1>.vd<-1,-1,-1>.iv.1(nv.1(nv))",
/*
***VALLIOS = 273, ALL_INV_ON_SUR(ref,pos,sort,antal,uv,xyz);
*/
     "-3:re.ve.id<1>.iv.1(nv.1(nv))",
/*
***VAPPFIL = 274, APPEND_FILE(fil1,fil2);
*/
     "-5:se.se",
/*
***VDELFIL = 275, DELETE_FILE(fil);
*/
     "-5:se",
/*
***VMOVFIL = 276, MOVE_FILE(fil1,fil2);
*/
     "-5:se.se",
/*
***VCOPFIL = 277, COPY_FILE(fil1,fil2);
*/
     "-5:se.se",
/*
***VTSTFIL = 278, status:=TEST_FILE(fil,mode);
*/
     "i5:se.se",
/*
***VGETENV = 279, string:=GET_ENVIRONMENT(env_namn);
*/
     "s5:se",
/*
***VGRID = 280, GRID_VIEW(0/1);
*/
     "-2:ie",
/*
***VGRIDX = 281, GRIDX_VIEW(x_val);
*/
     "-2:fe",
/*
***VGRIDY = 282, GRIDY_VIEW(y_val);
*/
     "-2:fe",
/*
***VGRIDDX = 283, GRIDDX_VIEW(dx_val);
*/
     "-2:fe",
/*
***VGRIDDY = 284, GRIDDY_VIEW(dy_val);
*/
     "-2:fe",
/*
***VSRTGM = 285, SET_ROOT_GM([id]);
*/
     "-5:1(re)",
/*
***VGNXGM = 286, GET_NEXT_GM(mode,typ,namn,grupp);
*/
     "r5:1(id<0>.1(id<65535>.1(sd<*>.1(sd<*>))))",
/*
***VCRFBUT = 287, CRE_FBUTTON(main_id,pos,b,h,str,akod,anum);
*/
     "i5:ie.ve.fe.fe.se.se.ie",
/*
***VCRFICO = 288, CRE_FICON(main_id,pos,filnam,akod,anum);
*/
     "i5:ie.ve.se.se.ie",
/*
***VGETWIN = 289, GET_WIN(id,pos,b,h[,typ,rubrik]);
*/
     "-5:ie.vv.fv.fv.1(iv.1(sv))",
/*
***VTXTLW = 290, TEXTL_WIN(str,font_nam);
*/
     "i5:se.1(se)",
/*
***VTXTHW = 291, TEXTH_WIN(font_nam);
*/
     "i5:1(se)",
/*
***VLDMDF = 292, LOAD_MDF(filnam);
*/
     "-5:se",
/*
***VBOX1P = 293, BOX_1P(id,pos,b,h,l);
*/
     "-3:ve.fe.fe.fe",
/*
***VBOXEN = 294, BOX_ENCLOSE(id,ref);
*/
     "-3:re",
/*
***VSURROT = 295, SUR_ROT(id,curve,p1,p2,v1,v2,dir,reverse);
*/
     "-3:re.ve.ve.fe.fe.1(id<1>).1(id<1>)",
/*
***VCRDBUT = 296, sub_id:=CRE_DBUTTON(win_id,pos,dx,dy,bw,
*                                          ,str,fnt_str,col1,col2);
*/
     "i5:ie.ve.fe.fe.fe.se.1(se.2(ie))",
/*
***VEND = 297, ENDP(id);
*/
     "v5:re",
/*
***VSTART = 298, STARTP(id);
*/
     "v5:re",
/*
***VLDDLL = 299, status:=LOAD_DLL(path);
*/
     "i5:se",
/*
***VCLDLL = 300, CALL_DLL_FUNCTION(path,name[,instrings]);
*/
     "-5:se.se.3(se)",
/*
***VULDLL = 301, UNLOAD_DLL(path);
*/
     "-5:se",
/*
***VMTVIEW = 302, MATERIAL_VIEW(pen,ar,ag,ab,dr,dg,db,sr,sg,sb,er,eg,eb,s);
*/
     "-3:ie.fe.fe.fe.fe.fe.fe.fe.fe.fe.fe.fe.fe.fe",
/*
***VLTVIEW = 303, LIGHT_VIEW(num,pos[,ang,dir]);
*/
     "-3:ie.ve.1(ve.fe.fe)",
/*
***VGTCOL = 304, GET_COLOR(pen,red,green,blue);
*/
     "-5:ie.iv.iv.iv",
/*
***VCRCOL = 305, CRE_COLOR(pen,red,green,blue);
*/
     "-5:ie.ie.ie.ie",
/*
***VLTON = 306, LIGHT_ON(light,intensity);
*/
     "-3:ie.1(fd<100.0>)",
/*
***VLTOFF = 307, LIGHT_OFF(light);
*/
     "-3:ie",
/*
***VSURUSD = 308, SUR_USRDEF(id,root,head,topo,geo);
*/
     "-3:re.se.se.se",
/*
***VSURCYL = 309, SUR_CYL(id,curve,distance,reverse,dir);
*/
     "-3:re.fe.1(id<1>).1(vd<0,0,0>)",
/*
***VCLPM = 310, CLEAR_PM();
*/
     "-5:",
/*
***VSURSWP = 311, SUR_SWEEP(id,curve,spine,reverse,dir);
*/
     "-3:re.re.1(id<1>).1(vd<0,0,0>)",
/*
***VSURRUL = 312, SUR_RULED(id,curve1,curve2,reverse,par);
*/
     "-3:re.re.1(id<1>).1(id<1>)",
/*
***VPOSMBS = 313, str:=POS_MBS(promt); 
*/
     "s5:se",
/*
***VGTFAP = 314, GETFACP(id,iu,iv,p1,p2,p3,p4);
*/
     "-5:re.ie.ie.vv.vv.vv.vv",
/*
***VSURNA = 315, SUR_NURBSARR(id,ordu,ordv,nku,nkv,cpts[,uk,vk,w]);
*/
     "-3:ie.ie.ie.ie.av.1(2(lv).av)",
/*
***VSUAREA = 316, area:=SURFACE_AREA(id,tol);
*/
     "f3:re.1(fd<0.0>)",
/*
***VSEAREA = 317, area:=SECTION_AREA(id,tol);
*/
     "f3:re.1(fd<0.0>)",
/*
***VGETWDT = 318, width:=GETWIDTH(id);
*/
     "f5:re",
/*
***VACTWDT = 319, width:=ACT_WIDTH();
*/
     "f5:",
/*
***VSCR_2 = 320, SCREEN_2(win_id,p1,p2[,mode]);
*/
     "-5:iv.vv.vv.1(id<1>)",
/*
***VPOS_2 = 321, POS_2(p1,p2[,mode]);
*/
     "-5:vv.vv.1(id<1>)",
/*
***VIDNT_2 = 322, IDENT_2(typmask,mode,maxrefs,refarr,nref);
*/
     "-5:ie.ie.ie.mv.iv",
/*
***VHIGHL = 323, HIGHLIGHT(id,mode);
*/
     "-5:ie.1(re)",
/*
***VGTPTR = 324, pos:=GET_POINTER(0/1);
*/
     "v5:1(id<0>)",
/*
***VINFNM = 325, filename:=INFNAME(promt,default,path,pattern);
*/
     "s5:1(sd<\0>.1(sd<\0>.1(sd<\0>.1(sd<*>))))",
/*
***VLOADPM = 326, LOAD_PM(filename);
*/
     "-5:se",
/*
***VALLILM = 327, ALL_INT_LIN_MSUR(refarr,nref,pos,rikt,sort,ant,uv,xyz,ytid);
*/
     "-3:mv.ie.ve.ve.id<1>.iv.1(nv.1(nv.1(mv)))",
/*
***VCALEXT = 328, CALL_EXTERN(proc_name,nflts,ftlarr);
*/
     "-5:se.ie.lv",
/*
***VIVAL = 329, val:=IVAL(str,status,nchar);
*/
     "i5:se.1(iv.1(iv))",
/*
***VRVAL = 330, val:=RVAL(str,status,nchar);
*/
     "f5:se.1(iv.1(iv))",
/*
***VODBCCO = 331, status:=ODBC_Connect(source,user,passw);
*/
     "i5:se.se.se",
/*
***VODBCED = 332, status:=ODBC_Execdirekt(sql_string);
*/
     "i5:se.1(se)",
/*
***VODBCFE = 333, status:=ODBC_Fetch();
*/
     "i5:",
/*
***VODBCGS = 334, status:=ODBC_Getstring(str_variable);
*/
     "i5:sv",
/*
***VODBCDC = 335, status:=ODBC_Disconnect();
*/
     "i5:",
/*
***VODBCER = 336, errormessage:=ODBC_Error();
*/
     "s5:",
/*
***VPUTDAT = 337, status:=PUTDAT_GM(key,count,array);
*/
     "i5:se.ie.av",
/*
***VGETDAT = 338, count:=GETDAT_GM(key,array);
*/
     "i5:se.av",
/*
***VDELDAT = 339, status:=DELDAT_GM(key);
*/
     "i5:se",
/*
***VCRAYNPES = 340, num_pes:=CRAY_NUM_PES();
*/
     "i5:",
/*
***VCRAYMYPE = 341, my_pe:=CRAY_MY_PE();
*/
     "i5:",
/*
***VSECGRAV = 342, cgrav:=SECTION_CGRAV(id,tol);
*/
     "v3:re.1(fd<0.0>)",
/*
***VSURCURV = 343, sur_curves(id,nu,nv,idcurves,twistmethod);
*/
     "-3:ie.ie.mv.se",
/*
***VCURNA = 344, CUR_NURBSARR(id,order,ncpts,cpts,[w,k]);
*/
     "-5:ie.ie.nv.1(lv.1(lv))",
/*
***VTCPCO = 345, status:=TCP_CONNECT(server, port, localport, socket);
*/
     "i5:se.ie.ie.iv",
/*
***VTCPLI = 346, status:=TCP_LISTEN(port, timeout, socket);
*/
     "i5:ie.fe.iv",
/*
***VTCPCL = 347, status:=TCP_CLOSE(socket);
*/
     "i5:ie",
/*
***VTCPSI = 348, status:=TCP_OUTINT(socket,bytecount,byteorder,timeout,data);
*/
     "i5:ie.ie.ie.fe.ie",
/*
***VTCPRI = 349, status:=TCP_ININT(socket,bytecount,byteorder,timeout,data);
*/
     "i5:ie.ie.ie.fe.iv",
/*
***VTCPSF = 350, status:=TCP_OUTFLOAT(socket,bytecount,byteorder,timeout,data);
*/
     "i5:ie.ie.ie.fe.fe",
/*
***VTCPRF = 351, status:=TCP_INFLOAT(socket,bytecount,byteorder,timeout,data);
*/
     "i5:ie.ie.ie.fe.fv",
/*
***VTCPSF = 352, status:=TCP_OUTCHAR(socket,timeout,data);
*/
     "i5:ie.fe.fe",
/*
***VTCPRF = 353, status:=TCP_INCHAR(socket,timeout,data);
*/
     "i5:ie.fe.fv",
/*
***VTCPRF = 354, status:=TCP_GETLOCAL(socket,name,port);
*/
     "i5:ie.sv.iv",
/*
***VTCPRF = 355, status:=TCP_GETREMOTE(socket,name,port);
*/
     "i5:ie.sv.iv",
/*
***VTCPSF = 356, status:=TCP_OUTVEC(socket,bytecount,byteorder,timeout,data);
*/
     "i5:ie.ie.ie.fe.ve",
/*
***VTCPRF = 357, status:=TCP_INVEC(socket,bytecount,byteorder,timeout,data);
*/
     "i5:ie.ie.ie.fe.vv",   
/*
***VRPCCC = 358, status:=RPC_CLIENTCREATE(servername,programnumber,versionnumber,client);
*/
     "i5:se.ie.ie.iv",
/*
***VRPCCD = 359, status:=RPC_CLIENTDESTROY(client);
*/
     "i5:ie",
/*
***VRAPOF = 360, status:=RAP_OUTFLOAT(client,var_name,data);
*/
     "i5:ie.se.fe",
/*
***VRAPIF = 361, status:=RAP_INFLOAT(client,var_name,data);
*/
     "i5:ie.se.fv",
/*
***VRAPOF = 362, status:=RAP_OUTINT(client,var_name,data);
*/
     "i5:ie.se.ie",
/*
***VRAPIF = 363, status:=RAP_ININT(client,var_name,data);
*/
     "i5:ie.se.iv",     
/*
***VRAPORT= 364, status:=RAP_OUTROBTARGET(client,var_name,trans,rot,conf,extax);
*/
     "i5:ie.se.ve.lv.pv.lv",     
/*
***VRAPIRT= 365, status:=RAP_INROBTARGET(client,var_name,trans,rot,conf,extax);
*/
     "i5:ie.se.vv.lv.pv.lv",                          
/*
***VRAPOS= 366, status:=RAP_OUTSPEED(client,var_name,speeddata);
*/
     "i5:ie.se.lv",
/*
***VRAPIS= 367, status:=RAP_INSPEED(client,var_name,speeddata);
*/
     "i5:ie.se.lv",                          
/*
***VRAPORT= 368, status:=RAP_OUTROBTARR(client,var_name,first,last,trans,rot,conf,extax);
*/
     "i5:ie.se.ie.ie.nv.lv.pv.lv",
/*
***VRAPIRT= 369, status:=RAP_INROBTARR(client,var_name,first,last,trans,rot,conf,extax);
*/
     "i5:ie.se.ie.ie.nv.lv.pv.lv",                          
/*
***VORURSTD= 370, status:=ORU_RSTIMEDIFF(rclient,ssocket,diff,robtime);
*/
     "i5:ie.ie.fv.fv",
/*
***VRAPORT= 371, status:=RAP_OUTFLOATARR(client,var_name,first,last,data);
*/
     "i5:ie.se.ie.ie.lv",
/*
***VRAPIRT= 372, status:=RAP_INFLOATARR(client,var_name,first,last,data);
*/
     "i5:ie.se.ie.ie.lv",       
/*
***VMSHARR = 373, MESH_ARR(id,nv,pts,eh,nh,ev,oh,nh,f,nf,h(,nx,xf));
*/
     "-3:ie.nv.pv.ie.pv.pv.pv.pv.ie.pv.1(id<0>).1(lv))",
/*
***VGTMSHH = 374, GETMSHH(id,nv,nh,nf,minp,maxp,font);
*/
     "-5:re.iv.iv.iv.vv.vv.iv",
/*
***VGTVRTX = 375, GETVERTEX(id,n,p,eh);
*/
     "-5:re.ie.vv.iv",
/*
***VGTHEDG = 376, GETHEDGE(id,n,ev,oh,nh,pf);
*/
     "-5:re.ie.iv.iv.iv.iv",
/*
***VGTFACE = 377, GETFACE(id,n,h);
*/
     "-5:re.ie.iv",
/*
***VSRTMSH = 378, SORT_MESH(id,"mode",sorted_face_list,min,max);
*/
     "-3:re.se.pv.lv.lv",
/*
***VGTXFLT = 379, GETXFLOAT(id,n,f);
*/
     "-5:re.ie.fv",

/*
***VTMULT = 380, TFORM_MULT(tmat,tmat,tmat);
*/
     "-5:jv.jv.jv",
     
/*
***VTINV = 381, TFORM_INV(tmat,tmat);
*/
     "-5:jv.jv",
     
/*
***VCCMPARR = 382, CUR_COMPARR(id,nref,ref,surid);
*/
     "-3:ie.mv.1(re)",
     
/*
***VSURTUSRD = 383, SUR_TRIM_USRDEF(id,surid,nloops,looparr);
*/
     "-3:re.ie.mv",
/*
***VACPFNT = 384, ACT_PFONT
*/
     "i5:",
/*
***VACPSIZ = 385, ACT_PSIZE
*/
     "f5:",
/*
***VBLLEVEL = 386, BLANK_LEVEL()
*/
     "-5:ie.1(id<0>)",
/*
***VUBLEVEL = 387, UNBLANK_LEVEL()
*/
     "-5:ie.1(id<0>)",
/*
***VGTLEVEL = 388, GET_LEVEL()
*/
     "-5:ie.ie.iv.1(sv)",
/*
***VACLEVEL = 389, ACT_LEVEL()
*/
     "i5:",
/*
***VNMLEVEL = 390, NAME_LEVEL()
*/
     "-5:ie.se"
     
/*
***Remember, no comma after the last parameter list definition.
*/

/*
***Custom parameter lists are defined by including the
***following file instead.
*/
#else
#include "my_formpar.h"
#endif


/************************************************************/

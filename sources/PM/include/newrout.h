/********************************************************************/
/*  File: newrout.h                                                 */
/*  ===============                                                 */
/*                                                                  */
/*  This file includes the definition of the last part of the       */
/*  system global symbol table.                                     */
/*                                                                  */
/*  With VARKON defined the standard Varkon routines will be used.  */
/*  If you want to build your own version of the system see notes   */
/*  at the end of this file.                                        */
/*                                                                  */
/*  This file is part of the VARKON Pmpac Library.                  */
/*                                                                  */
/*  This library is free software; you can redistribute it and/or   */
/*  modify it under the terms of the GNU Library General Public     */
/*  License as published by the Free Software Foundation; either    */
/*  version 2 of the License, or (at your option) any later         */
/*  version.                                                        */
/*                                                                  */
/*  This library is distributed in the hope that it will be         */
/*  useful, but WITHOUT ANY WARRANTY; without even the implied      */
/*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR         */
/*  PURPOSE.  See the GNU Library General Public License for more   */
/*  details.                                                        */
/*                                                                  */
/*  You should have received a copy of the GNU Library General      */
/*  Public License along with this library; if not, write to the    */
/*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge,        */
/*  MA 02139, USA.                                                  */
/*                                                                  */
/********************************************************************/

#ifdef VARKON

{"CRE_MATERIAL",      ST_PROC,     FALSE,  ST_ORD,    VCRMAT},
{"CRE_LIGHT",         ST_PROC,     FALSE,  ST_ORD,    VLTVIEW},
{"GET_COLOR",         ST_FUNC,     FALSE,  ST_ORD,    VGTCOL},
{"CRE_COLOR",         ST_PROC,     FALSE,  ST_ORD,    VCRCOL},
{"LIGHT_ON",          ST_PROC,     FALSE,  ST_ORD,    VLTON},
{"LIGHT_OFF",         ST_PROC,     FALSE,  ST_ORD,    VLTOFF},
{"SUR_USRDEF",        ST_PROC,     FALSE,  ST_GEO,    VSURUSD},
{"SUR_CYL",           ST_PROC,     FALSE,  ST_GEO,    VSURCYL},
{"CLEAR_PM",          ST_PROC,     FALSE,  ST_ORD,    VCLPM},
{"SUR_SWEEP",         ST_PROC,     FALSE,  ST_GEO,    VSURSWP},
{"SUR_RULED",         ST_PROC,     FALSE,  ST_GEO,    VSURRUL},
{"POS_MBS",           ST_FUNC,     FALSE,  ST_ORD,    VPOSMBS},
{"GETFACP",           ST_PROC,     FALSE,  ST_ORD,    VGTFAP},
{"SUR_NURBSARR",      ST_PROC,     FALSE,  ST_GEO,    VSURNA},
{"SURFACE_AREA",      ST_FUNC,     FALSE,  ST_ORD,    VSUAREA},
{"SECTION_AREA",      ST_FUNC,     FALSE,  ST_ORD,    VSEAREA},
{"GETWIDTH",          ST_FUNC,     FALSE,  ST_ORD,    VGETWDT},
{"ACT_WIDTH",         ST_FUNC,     FALSE,  ST_ORD,    VACTWDT},
{"SCREEN_2",          ST_PROC,     FALSE,  ST_ORD,    VSCR_2},
{"POS_2",             ST_PROC,     FALSE,  ST_ORD,    VPOS_2},
{"IDENT_2",           ST_PROC,     FALSE,  ST_ORD,    VIDNT_2},
{"HIGHLIGHT",         ST_PROC,     FALSE,  ST_ORD,    VHIGHL},
{"GET_POINTER",       ST_FUNC,     FALSE,  ST_ORD,    VGTPTR},
{"INFNAME",           ST_FUNC,     FALSE,  ST_ORD,    VINFNM},
{"LOAD_PM",           ST_PROC,     FALSE,  ST_ORD,    VLOADPM},
{"ALL_INT_LIN_MSUR",  ST_PROC,     FALSE,  ST_ORD,    VALLILM},
{"CALL_EXTERN",       ST_PROC,     FALSE,  ST_ORD,    VCALEXT},
{"IVAL",              ST_FUNC,     FALSE,  ST_ORD,    VIVAL},
{"FVAL",              ST_FUNC,     FALSE,  ST_ORD,    VFVAL},
{"ODBC_CONNECT",      ST_FUNC,     FALSE,  ST_ORD,    VODBCCO},
{"ODBC_EXECDIRECT",   ST_FUNC,     FALSE,  ST_ORD,    VODBCED},
{"ODBC_FETCH",        ST_FUNC,     FALSE,  ST_ORD,    VODBCFE},
{"ODBC_GETSTRING",    ST_FUNC,     FALSE,  ST_ORD,    VODBCGS},
{"ODBC_DISCONNECT",   ST_FUNC,     FALSE,  ST_ORD,    VODBCDC},
{"ODBC_ERROR",        ST_FUNC,     FALSE,  ST_ORD,    VODBCER},
{"PUTDAT_GM",         ST_FUNC,     FALSE,  ST_ORD,    VPUTDAT},
{"GETDAT_GM",         ST_FUNC,     FALSE,  ST_ORD,    VGETDAT},
{"DELDAT_GM",         ST_FUNC,     FALSE,  ST_ORD,    VDELDAT},
{"CRAY_NUM_PES",      ST_FUNC,     FALSE,  ST_ORD,    VCRAYNPES},
{"CRAY_MY_PE",        ST_FUNC,     FALSE,  ST_ORD,    VCRAYMYPE},
{"SECTION_CGRAV",     ST_FUNC,     FALSE,  ST_ORD,    VSECGRAV},
{"SUR_CURVES",        ST_PROC,     FALSE,  ST_GEO,    VSURCURV},
{"CUR_NURBSARR",      ST_PROC,     FALSE,  ST_GEO,    VCURNA},
{"TCP_CONNECT",       ST_FUNC,     FALSE,  ST_ORD,    VTCPCO},
{"TCP_LISTEN",        ST_FUNC,     FALSE,  ST_ORD,    VTCPLI},
{"TCP_CLOSE",         ST_FUNC,     FALSE,  ST_ORD,    VTCPCL},
{"TCP_OUTINT",        ST_FUNC,     FALSE,  ST_ORD,    VTCPOI},
{"TCP_ININT",         ST_FUNC,     FALSE,  ST_ORD,    VTCPII},
{"TCP_OUTFLOAT",      ST_FUNC,     FALSE,  ST_ORD,    VTCPOF},
{"TCP_INFLOAT",       ST_FUNC,     FALSE,  ST_ORD,    VTCPIF},
{"TCP_OUTCHAR",       ST_FUNC,     FALSE,  ST_ORD,    VTCPOC},
{"TCP_INCHAR",        ST_FUNC,     FALSE,  ST_ORD,    VTCPIC},
{"TCP_GETLOCAL",      ST_FUNC,     FALSE,  ST_ORD,    VTCPGL},
{"TCP_GETREMOTE",     ST_FUNC,     FALSE,  ST_ORD,    VTCPGR},
{"TCP_OUTVEC",        ST_FUNC,     FALSE,  ST_ORD,    VTCPOV},
{"TCP_INVEC",         ST_FUNC,     FALSE,  ST_ORD,    VTCPIV},
{"RPC_CLIENTCREATE",  ST_FUNC,     FALSE,  ST_ORD,    VRPCCC},
{"RPC_CLIENTDESTROY", ST_FUNC,     FALSE,  ST_ORD,    VRPCCD},
{"RAP_OUTFLOAT",      ST_FUNC,     FALSE,  ST_ORD,    VRAPOF},
{"RAP_INFLOAT",       ST_FUNC,     FALSE,  ST_ORD,    VRAPIF},
{"RAP_OUTINT",        ST_FUNC,     FALSE,  ST_ORD,    VRAPOI},
{"RAP_ININT",         ST_FUNC,     FALSE,  ST_ORD,    VRAPII},
{"RAP_OUTROBTARGET",  ST_FUNC,     FALSE,  ST_ORD,    VRAPORT},
{"RAP_INROBTARGET",   ST_FUNC,     FALSE,  ST_ORD,    VRAPIRT},
{"RAP_OUTSPEED",      ST_FUNC,     FALSE,  ST_ORD,    VRAPOS},
{"RAP_INSPEED",       ST_FUNC,     FALSE,  ST_ORD,    VRAPIS},
{"RAP_OUTROBTARR",    ST_FUNC,     FALSE,  ST_ORD,    VRAPORTA},
{"RAP_INROBTARR",     ST_FUNC,     FALSE,  ST_ORD,    VRAPIRTA},
{"ORU_RSTIMEDIFF",    ST_FUNC,     FALSE,  ST_ORD,    VORURSTD},
{"RAP_OUTFLOATARR",   ST_FUNC,     FALSE,  ST_ORD,    VRAPOFA},
{"RAP_INFLOATARR",    ST_FUNC,     FALSE,  ST_ORD,    VRAPIFA},
{"MESH_ARR",          ST_PROC,     FALSE,  ST_GEO,    VMSHARR},
{"GETMSHH",           ST_PROC,     FALSE,  ST_ORD,    VGTMSHH},
{"GETVERTEX",         ST_PROC,     FALSE,  ST_ORD,    VGTVRTX},
{"GETHEDGE",          ST_PROC,     FALSE,  ST_ORD,    VGTHEDG},
{"GETFACE",           ST_PROC,     FALSE,  ST_ORD,    VGTFACE},
{"SORT_MESH",         ST_PROC,     FALSE,  ST_ORD,    VSRTMSH},
{"GETXFLOAT",         ST_PROC,     FALSE,  ST_ORD,    VGTXFLT},
{"POS_IN_CONE",       ST_FUNC,     FALSE,  ST_ORD,    VPINC},
{"TFORM_MULT",        ST_PROC,     FALSE,  ST_ORD,    VTMULT},
{"TFORM_INV",         ST_PROC,     FALSE,  ST_ORD,    VTINV},
{"CUR_COMPARR",       ST_PROC,     FALSE,  ST_GEO,    VCCMPARR},
{"SUR_TRIM_USRDEF",   ST_PROC,     FALSE,  ST_GEO,    VSURTUSRD},
{"ACT_PFONT",         ST_FUNC,     FALSE,  ST_ORD,    VACPFNT},
{"ACT_PSIZE",         ST_FUNC,     FALSE,  ST_ORD,    VACPSIZ},
{"BLANK_LEVEL",       ST_PROC,     FALSE,  ST_ORD,    VBLLEVEL},
{"UNBLANK_LEVEL",     ST_PROC,     FALSE,  ST_ORD,    VUBLEVEL},
{"GET_LEVEL",         ST_PROC,     FALSE,  ST_ORD,    VGTLEVEL},
{"ACT_LEVEL",         ST_FUNC,     FALSE,  ST_ORD,    VACLEVEL},
{"NAME_LEVEL",        ST_PROC,     FALSE,  ST_ORD,    VNMLEVEL},
{"ACT_SVN_VERS",      ST_FUNC,     FALSE,  ST_ORD,    VASVNVERS},
{"GET_MATERIAL",      ST_FUNC,     FALSE,  ST_ORD,    VGETMAT},
{"GET_LIGHT",         ST_FUNC,     FALSE,  ST_ORD,    VGETLIGHT},
{"RGB_TO_HSV",        ST_PROC,     FALSE,  ST_ORD,    VRGBHSV},
{"HSV_TO_RGB",        ST_PROC,     FALSE,  ST_ORD,    VHSVRGB},
{"CSYS_USRDEF",       ST_PROC,     FALSE,  ST_GEO,    VCSYSUD},
{"PCA",               ST_FUNC,     FALSE,  ST_ORD,    VPCATMAT},
{"EIGASYM",           ST_FUNC,     FALSE,  ST_ORD,    VEIGASYM},
/*
***The global symbol table ends here.
*/
{"",           ST_UNDEF,    FALSE,  ST_ORD,   ST_UNDEF}
/*
***If you want to build your own version of the system,
***undefine VARKON and include your own file here.
*/
#else
#include "my_newrout.h"
#endif

/******************************************************/

/*!******************************************************************
*  File: evfuncs.h 
*  ===============   
* 
*  The table of evaluate functions. This file compiles
*  for Varkon if VARKON is defined. If not, include
*  your own file, my_evfuncs.h. See note at end of this file.
*
*  This file is part of the Program Module (PM) Library. 
* 
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
*  (C) J.Kjellander, �ebro university
*   
******************************************************************/

/*
***The following table defines the entrypoints for
***the C-functions that evaluates each MBS routine.
*/
#ifdef VARKON
static short (*functab[])() =

{
/*
***When building mbsc only a few routines need to be included.
*/
#ifdef ANALYZER
     NULL,      NULL,      NULL,      NULL,      NULL, /* 04 */
     NULL,      NULL,      NULL,      NULL,      NULL, /* 09 */
     NULL,      NULL,      NULL,      NULL,      NULL, /* 14 */
     NULL,      NULL,      NULL,      NULL,      NULL, /* 19 */
     NULL,      NULL,      NULL,      NULL,      NULL, /* 24 */
     NULL,      NULL,      NULL,      NULL,    evacos, /* 29 */
   evasin,    evatan,
    evcos,     evsin,     evtan,    evnlog,    evlogn,
   evsqrt,     evabs,    evfrac,    evroud,    evtrnc,
     NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 49 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,    evvecp,      NULL,      NULL, /* 59 */
     NULL,      NULL,      NULL,      NULL,     evvec,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 69 */
   evasci,     evstr,     evchr,     evval,    evleng,
   evsubs,
/*
***When building xvarkon all routines are included.
*/
#else
     NULL,    evpofr,    evpopr,    evlifr,    evlipr,  /* 04 */
   evlian,    evliof,    evlit1,    evlit2,    evarfr,  /* 09 */
   evar2p,    evar3p,    evarof,    evarfl,    evcufr,  /* 14 */
   evcupr,    evcuro,    evcomp,    evxht,     NULL,    /* 19 */
   evions,    evtext,    evpinc,    evcs3p,    evgrp,   /* 24 */
   evldim,    evcdim,    evrdim,    evadim,    evacos,  /* 29 */
   evasin,    evatan,
   evcos,     evsin,     evtan,     evnlog,    evlogn,
   evsqrt,    evabs,     evfrac,    evroud,    evtrnc,
   evgref,    evldgm,    evsvgm,
   inmiss,    inmiss,    inmiss,    inmiss,    evncui,  /* 49 */
   eveval,    evfins,    evarcl,    evtang,    evcen,
   evsect,    evon,      evvecp,    evidnt,    evpos,   /* 59 */
   evscr,     evbpln,    evmolo,    evmogl,    evvec,
   evset,     evos,      evvecl,    evvecn,    evangl,  /* 69 */
   evasci,    evstr,     evchr,     evval,     evleng,
   evsubs,    evopen,    evseek,    evouti,    evoutf,  /* 79 */
   evouts,    evoutl,    evoutb,    evinii,    evinif,
   evinis,    evinl,     evinb,     evfpos,    evclos,  /* 89 */
   eviost,    evgtid,    evghdr,    evgpoi,    evglin,
   evgarc,    evgcur,    evgtrf,    evgtxt,    evgxht,  /* 99 */
   evgldm,    evgcdm,    evgrdm,    evgadm,    evggrp, 
   evgcsy,    NULL,      evgprt,    evgtyp,    evgint,  /* 109 */
   evgflt,    evgstr,    evgvec,    evsymb,    evtrim,
   evdel,     evclgm,    evblk,     evublk,    evcrvi,  /* 119 */
   evacvi,    evscvi,    evcevi,    evervi,    evrpvi,
   evcavi,    inmiss,    evtime,    evrstr,    evrval,  /* 129 */
   evrefc,    evlsin,    evlsex,    evlsln,    evablk,
   evavin,    evapen,    evascl,    evadsc,    evacac,  /* 139 */
   evagrx,    evagry,    evagdx,    evagdy,    evagrd,
   evajbn,    evamtp,    evamat,    evajbd,    evalft,  /* 149 */
   evaaft,    evaxft,    evaldl,    evaadl,    evaxdl,
   evatsi,    evatwi,    evatsl,    evadts,    evadas,  /* 159 */
   evadnd,    evadau,    evglor,    evpspt,    evpopt,
   evcrts,    evgtts,    evcrmy,    evpsmy,    evpomy,  /* 169 */
   evgtal,    evgtmy,    evcrpr,    evlipe,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL,  /* 179 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,    evgbpl,    evcs1p,    evplwi,  /* 189 */
   evuhdr,    evldjb,    evsvjb,    evinpt,    evhdvi,
   evprvi,    evtxtl,    evexit,    evexst,    evarea,  /* 199 */
   evcgra,    evsetb,    evcurc,    evitan,    evcurs,
   evion,     evcurt,    evcuru,    evcurv,    evicur,  /* 209 */
   eviarc,    evacft,    evacdl,    evaddm,    evnsec,
   evgpm,     evupm,     evsexd,    evcsil,    evcint,  /* 219 */
   evciso,    evnorm,    evtrmv,    evtrrp,    evtrrl,
   evtrmi,    evtrco,    evtrus,    evtcop,    evcugl,  /* 229 */
   evmsiz,    evcrwi,    evcred,    evcric,    evcrbu,
   evshwi,    evwtwi,    evgted,    evuped,    evgtbu,  /* 239 */
   evdlwi,    evgcuh,    evgseg,    evgsuh,    evuppm,
   evscon,    evsoff,    evgtvi,    evdelm,    evacsy,  /* 249 */
   evmoba,    evssar,    evscar,    evsapp,    evscom,
   evstrm,    evgtop,    evgcub,    evgrat,    evcusa,  /* 259 */
   evcuca,    evcfan,    evcged,    evcapp,    evrunm,
   evapid,    evavvr,    evavsr,    evaost,    evahst,  /* 269 */
   evufnm,    evrand,    evails,    evaios,    evappf,
   evdelf,    evmovf,    evcopf,    evtstf,    evgenv,  /* 279 */
   evgrid,    evgrix,    evgriy,    evgrdx,    evgrdy,
   evsrgm,    evgngm,    evcrfb,    evcrfi,    evgtwi,  /* 289 */
   evgttl,    evgtth,    evlmdf,    NULL,      NULL,  
   evsrot,    evcrdb,    evend,     evstrt,    evlddl,  /* 299 */
   evcldl,    evuldl,    evcrmt,    evltvi,    evgtco,
   evcrco,    evlton,    evltof,    evsusr,    evscyl,  /* 309 */
   evclpm,    evsswp,    evsrul,    evposm,    evgfac,
   evsnar,    evsuar,    evsear,    evgwdt,    evawdt,  /* 319 */
   evscr2,    evpos2,    evidn2,    evhigh,    evgptr,
   evinfn,    evldpm,    evailm,    evcaxt,    evival,  /* 329 */
   evfval,    evodco,    evoded,    evodfe,    evodgs,
   evoddc,    evoder,    evpdat,    evgdat,    evddat,  /* 339 */
   evcrnp,    evcrmp,    evsecg,    evscur,    evcuna,
   evtcco,    evtcli,    evtccl,    evtcoi,    evtcii,  /* 349 */
   evtcof,    evtcif,    evtcoc,    evtcic,    evtcgl,
   evtcgr,    evtcov,    evtciv,    evrpcc,    evrpcd,  /* 359 */
   evrpof,    evrpif,    evrpoi,    evrpii,    evrport,
   evrpirt,   evrpos,    evrpis,    evrpora,   evrpira, /* 369 */
   evorrstd,  evrpofa,   evrpifa,   evmsar,    evgmsh,
   evgvrt,    evghdg,    evgface,   evsrms,    evgxflt, /* 379 */
   evtmult,   evtinv,    evccmparr, evstusr,   evapft,
   evapsi,    evbllv,    evublv,    evgtlv,    evalev,  /* 389 */
   evnmlv
#endif
};


/*
***If you are building your own application,
***write your own file, name it "my_evfuncs.h"
***compile the system without VARKON defined.
*/
#else
#include "my_evfuncs.h"
#endif

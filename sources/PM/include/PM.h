/*!******************************************************************/
/*  File: PM.h                                                      */
/*  ==========                                                      */
/*                                                                  */
/*  Common 'typedef' and '#define' declarations for PM's interface. */
/*                                                                  */
/*  This file includes the definition of the last part of the       */
/*  system global symbol table.                                     */
/*                                                                  */
/*  This file is part of the VARKON Pmpac Library.                  */
/*  URL:  http://www.varkon.com                                     */
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
/* Names on structures and elements are constructed from the          */
/* following abbreviations.                                           */
/*              AT = ATTRIBUTE                                        */
/*              CO = CONDITION                                        */
/*              EX = EXPRESSION                                       */
/*              LI = LIST                                             */
/*              MD = MODIFIER                                         */
/*              MO = MODULE                                           */
/*              NO = NODE                                             */
/*              NP = NAMED PARAMETER                                  */
/*              PA = PARAMETER                                        */
/*              PM = Program Memory                                   */
/*              SB = SYMBOL TABLE                                     */
/*              ST = STATEMENT                                        */
/*              TY = TYPE                                             */

#define MIN_BLKS (long)8 /* Minimum number of bytes to allocate for data in   */
                         /* PM in order to assure alignment of all datatypes  */

typedef char *string;    /* character pointer for names and string references */
                         /* the string ends with the NULL-character.          */

typedef DBseqnum pmseqn; /*PM sequence number in PM                           */

typedef short pmroutn;   /* Type for MBS routine identifier                   */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*      pmint  -  definition       (INT_VAL)                                 */

typedef v2int pmint;     /* integer value type                               */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*      pmfloat  -  definition         (FLOAT_VAL)                           */

typedef v2float pmfloat; /* float value type                                 */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef v2vaty pmvaty;  /* Type for literal value structures with values     */
                        /* defined by constants.                             */
 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef char pmfieldt;  /* Type for VECTOR component fields.                 */

#define PM_X   1        /* constant for X-field                              */
#define PM_Y   2        /*    - " -     Y-field                              */
#define PM_Z   3        /*    - " -     Z-field                              */
	
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef char pmopty;    /* Type for binary operator.                         */

#define PM_GT     1     /* constant for  >      operator                     */
#define PM_GE     2     /*   - " -       >=      - " -                       */
#define PM_EQ     3     /*   - " -       =       - " -                       */ 
#define PM_NE     4     /*   - " -       <>      - " -                       */
#define PM_LT     5     /*   - " -       <       - " -                       */
#define PM_LE     6     /*   - " -       <=      - " -                       */
#define PM_NOT    7     /*   - " -      NOT      - " -                       */
#define PM_AND    8     /*   - " -      AND      - " -                       */
#define PM_OR     9     /*   - " -      OR       - " -                       */
#define PM_PLUS  10     /*   - " -       +       - " -                       */
#define PM_MINUS 11     /*   - " -       -       - " -                       */
#define PM_DIV   12     /*   - " -       /       - " -                       */
#define PM_MUL   13     /*   - " -       *       - " -                       */
#define PM_EXP   14     /*   - " -       **      - " -                       */

/*---------------------------------------------------------------------------*/
typedef char pmmoty;    /* Module type, from 1.19B always _3D                */
#define _2D   2         /* value for 2D to be used with pmmoty               */
#define _3D   3         /* value for 3D        - " -                         */
/*---------------------------------------------------------------------------*/
typedef char pmmoat;    /* Module attribute, default = GLOBAL                */
                        /*                                                   */
#define LOCAL  1        /* value for LOCAL to be used with pmmoat - type     */
#define GLOBAL 2        /* value for GLOBAL      - " -                       */
#define BASIC  3        /* value for BASIC       - " -                       */
#define MACRO  4        /* value for MACRO       -"-            92/02/13/JK  */
/*---------------------------------------------------------------------------*/
/* PMMODULE - interface structure for module header */

typedef struct pmmodule
    {
    char    mname[ V2SYNLEN ]; /* module name */
    pmmoty  mtype;      /* module type 2D eller 3D */
    pmmoat  mattri;     /* module attribute */
    pm_ptr  parlist;    /* parameter list */
    pm_ptr  stlist;     /* statement list */
    pmseqn  idmax;      /* maximal sequens number */
    DBint   ldsize;     /* local data area size */
    V3MDAT  system;     /* system data structure */
    } PMMODULE;

/*--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*      Literal value struct  -  definition                                  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*    The type for the following value structures are defined they represent */
/*    the different MBS literal types.                                       */
/*              pmint  -  INT_VAL      integer value  defined above          */
/*              pmfloat  -  FLOAT_VAL  float value       - " -               */
/*              string  -  STR_VAL     string value       - " -              */
/*              PMVECVA  -  VEC_VAL    vector value                          */
/*              PMREFVA  -  REF_VAL    reference value                       */
/*    It also contains value tages for address and boolean values            */
/*    internaly.                                                             */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*      PMVECVA  -  definition           (VEC_VAL)                           */

typedef V2VECVA PMVECVA; /* vector value structure                           */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*      PMREFVA  -  definition            (REF_VAL)                          */

typedef V2REFVA PMREFVA; /* reference value structure                        */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*     PMLITVA    Compond LITERAL struct - definition                        */

typedef V2LITVA PMLITVA; /* literal value structure                          */

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* PMPARVA - structure for parameter value and type */

typedef struct pmparva
    {
    PMLITVA  par_va;    /* parameter value */
    pm_ptr   par_ty;    /* parameter type */
    } PMPARVA;
/*--------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* PMPATLOG - structure for log of module parameter type in GM */

typedef V2PATLOG PMPATLOG;

/*--------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* PMREFL - structure for reference-lists used in reference-analysis 920506JK*/

typedef struct pmrefl
  {
  DBseqnum snr;      /* Storhetens sekvensnummer */
  pm_ptr   statla ;  /* Satsens pm-adress */
  char     typ;      /* Typ av sats, Geo-Part-Proc-If etc. */
  } PMREFL;
/*--------------------------------------------------------------------------*/

/*
***Named parameters (attributes).
*/
typedef char pmmdid;  /* type for named parameters */

#define PMPEN    1    /* Constant for  PEN     */
#define PMLEVEL  2    /*    - " -      LEVEL   */
#define PMLFONT  3    /*    - " -      LFONT   */
#define PMAFONT  4    /*    - " -      AFONT   */
#define PMLDASHL 5    /*    - " -      LDASHL  */
#define PMADASHL 6    /*    - " -      ADASHL  */
#define PMTSIZE  7    /*    - " -      TSIZE   */
#define PMTWIDTH 8    /*    - " -      TWIDTH  */
#define PMTSLANT 9    /*    - " -      TSLANT  */
#define PMDTSIZE 10   /*    - " -      DTSIZE  */
#define PMDASIZE 11   /*    - " -      DASIZE  */
#define PMDNDIG  12   /*    - " -      DNDIG   */
#define PMDAUTO  13   /*    - " -      DAUTO   */
#define PMXFONT  14   /*    - " -      XFONT   */
#define PMXDASHL 15   /*    - " -      XDASHL  */
#define PMBLANK  16   /*    - " -      BLANK   */
#define PMHIT    17   /*    - " -      HIT     */
#define PMSAVE   18   /*    - " -      SAVE    */
#define PMCFONT  19   /*    - " -      CFONT   */
#define PMCDASHL 20   /*    - " -      CDASHL  */
#define PMTFONT  21   /*    - " -      TFONT   */
#define PMSFONT  22   /*    - " -      SFONT   */
#define PMSDASHL 23   /*    - " -      SDASHL  */
#define PMNULIN  24   /*    - " -      NULINES */
#define PMNVLIN  25   /*    - " -      NVLINES */
#define PMWIDTH  26   /*    - " -      WIDTH   */
#define PMTPMODE 27   /*    - " -      TPMODE  */
#define PMMFONT  28   /*    - " -      MFONT   */
#define PMPFONT  29   /*    - " -      PFONT   */
#define PMPSIZE  30   /*    - " -      PSIZE   */

/*
***Definitions for PM internal structs.
*/
#include "../include/pmstru.h"
#include "../include/stdef.h"
#include "../include/ppdef.h"


/*
***Function prototypes for the PM API in Varkon.
*/
#ifdef __cplusplus
extern "C" {
#endif
/*
***evact.c
*/
short evalev(void);
short evablk(void);
short evavin(void);
short evapen(void);
short evawdt(void);
short evascl(void);
short evacac(void);
short evagrx(void);
short evagry(void);
short evagdx(void);
short evagdy(void);
short evagrd(void);
short evajbn(void);
short evamat(void);
short evajbd(void);
short evalft(void);
short evapft(void);
short evaaft(void);
short evacft(void);
short evaxft(void);
short evapsi(void);
short evaldl(void);
short evaadl(void);
short evacdl(void);
short evaxdl(void);
short evatsi(void);
short evatwi(void);
short evatsl(void);
short evadts(void);
short evadas(void);
short evadnd(void);
short evadau(void);
short evacsy(void);
short evavvr(void);
short evasvr(void);
short evaost(void);
short evahst(void);

/*
***evarc.c
*/
short evarfr(void);
short evar2p(void);
short evar3p(void);
short evarof(void);
short evarfl(void);

/*
***evarea.c
*/
short evarea(void);
short evsuar(void);
short evsear(void);
short evsecg(void);
short evcgra(void);

/*
***evbpln.c
*/
short evbpln(void);

/*
***evcray.c
*/
short evcrnp(void);
short evcrmp(void);

/*
***evcsy.c
*/
short evcs3p(void);
short evcs1p(void);
short evcsud(void);
short evpcatm(void);

/*
***evcsys.c
*/
short evmolo(void);
short evmogl(void);
short evmoba(void);
short evset(void);
short evsetb(void);

/*
***evcur.c
*/
short evcurs(void);
short evcusa(void);
short evcuna(void);
short evcurc(void);
short evcuca(void);
short evcuro(void);
short evcomp(void);
short evcurt(void);
short evcuru(void);

short evcsil(void);
short evcint(void);
short evciso(void);
short evcfan(void);
short evcged(void);
short evcapp(void);
short evcugl(void);
short evccmparr(void);

/*
***evdim.c
*/
short evldim(void);
short evcdim(void);
short evrdim(void);
short evadim(void);

/*
***evfile.c
*/
short evopen(void);
short evclos(void);
short evoutl(void);
bool  v3isop(char *path);

short evappf(void);
short evdelf(void);
short evmovf(void);
short evcopf(void);
short evtstf(void);

short evseek(void);
short evouti(void);
short evoutf(void);
short evouts(void);
short evoutb(void);
short evinii(void); 
short evinif(void);
short evinis(void);
short evinl(void);
short evinb(void);
short evfpos(void);
short eviost(void);
short evufnm(void);

/*
***evdll.c
*/
short evlddl(void);
short evcldl(void);
short evuldl(void);

/*
***evget.c
*/
short evsrgm(void);
short evgngm(void);
short evgtyp(void);
short evgint(void);
short evgflt(void);
short evgstr(void);
short evgvec(void);
short evgref(void);
short evghdr(void);
short evuhdr(void);
short evgwdt(void);
short evgpoi(void);
short evglin(void);
short evgarc(void);
short evgcuh(void);
short evgseg(void);
short evgsuh(void);
short evgtop(void);
short evgcub(void);
short evgfac(void);
short evgrat(void);
short evgtrf(void);
short evgtxt(void);
short evgxht(void);
short evgldm(void);
short evgcdm(void);
short evgrdm(void);
short evgadm(void);
short evggrp(void);
short evgcsy(void);
short evgbpl(void);
short evgprt(void);
short evpdat(void);
short evgdat(void);
short evddat(void);
short evdel(void);
short evclgm(void);

/*
***evgfuncs.c
*/
#ifndef ANALYZER
short evarcl(void); 
short eviarc(void);
short evtang(void);
short evitan(void);
short evcurv(void);
short evicur(void);
short evcen(void);
short evnorm(void);
short evstrt(void);
short evend(void);
short evon(void);
short evion(void);
short evsect(void);
short evnsec(void);
#endif
short evrefc(void);
short evglor(void);
short evvecl(void);
short evvecn(void);
short evcrpr(void);
short evangl(void);
short evions(void);
short eveval(void);
short evncui(void);

/*
***evvec.c
*/
short evvec(void);
short evvecp(void);

/*
***evgprocs.c
*/
short evtrim(void);
short evblk(void);
short evublk(void);
short evails(void);
short evailm(void);
short evaios(void);

/*
***evgrid.c
*/
short evgrid(void);
short evgrix(void);
short evgriy(void);
short evgrdx(void);
short evgrdy(void);

/*
***evgrp.c
*/
short evgrp(void);

/*
***evlev.c
*/
short evbllv(void);
short evublv(void);
short evgtlv(void);
short evnmlv(void);

/*
***evlin.c
*/
short evlifr(void);
short evlipr(void);
short evliof(void);
short evlian(void);
short evlit1(void);
short evlit2(void);
short evlipe(void);

/*
***evmath.c
*/
short evacos(void);
short evasin(void);
short evatan(void);
short evcos(void);
short evsin(void);
short evtan(void);
short evnlog(void);
short evlogn(void);
short evsqrt(void);
short evabs(void);
short evfrac(void);
short evroud(void);
short evtrnc(void);

#ifndef ANALYZER
short evrand(void);
#endif

/*
***evmatrix.c
*/
short eveigasym(void);

/*
***evmbs.c
*/
short evaddm(void);
short evdelm(void);
short evrunm(void);
short evgpm(void);
short evupm(void);
short evuppm(void);
short evclpm(void);
short evposm(void);

/*
***evmesh.c
*/
short evmsar(void);
short evgmsh(void);
short evgvrt(void);
short evghdg(void);
short evgface(void);
short evgxflt(void);
short evsrms(void);
/*
***evos.c, evexit.c, evtime.c and evexst.c
*/
short evos(void);
short evexit(void);
short evtime(void);
short evexst(void);

/*
***evodbc.c
*/
short evodco(void);
short evoded(void);
short evodfe(void);
short evodgs(void);
short evoddc(void);
short evoder(void);

/*
***evoru.c
*/
short evorrstd(void);


/*
***evpart.c
*/
short evpart(V2REFVA *ident, PMPAST *partp);

/*
***evplot.c
*/
short evplwi(void);

/*
***evpoi.c
*/
short evpofr(void);
short evpopr(void);

/*
***evpinc.c
*/
short evpinc(void);

/*
***evrap.c
*/
short evrpcc(void);
short evrpcd(void);
short evrpof(void);
short evrpif(void);
short evrpoi(void);
short evrpii(void);
short evrport(void);
short evrpirt(void);
short evrpos(void);
short evrpis(void);
short evrpora(void);
short evrpira(void);
short evrpofa(void);
short evrpifa(void);

/*
***evcolor.c
*/
short evltvi(void);
short evlton(void);
short evltof(void);
short evgtlt(void);
short evcrco(void);
short evgtco(void);
short evrgbhsv(void);
short evhsvrgb(void);
short evcrmt(void);
short evgtmt(void);

/*
***evsur.c
*/
short evsexd(void);
short evscon(void);
short evsoff(void);
short evssar(void);
short evscar(void);
short evsnar(void);
short evsapp(void);
short evscom(void);
short evstrm(void);
short evsrot(void);
short evsusr(void);
short evscyl(void);
short evsswp(void);
short evsrul(void);
short evscur(void);
short evstusr(void);

/*
***evstr.c
*/
short evrstr(void);
short evrval(void);
short evival(void);
short evfval(void);
short evfins(void);
short evchr(void);
short evasci(void);
short evstr(void);
short evleng(void);
short evsubs(void);

/*
***evsymb.c
*/
short evsymb(void);

/*
***evtcp.c
*/
short evtcco(void);
short evtcli(void);
short evtccl(void);
short evtcoi(void);
short evtcii(void);
short evtcof(void);
short evtcif(void);
short evtcoc(void);
short evtcic(void);
short evtcgl(void);
short evtcgr(void);
short evtcov(void);
short evtciv(void);

/*
***evtext.c
*/
short evtext(void);

/*
***evtform.c
*/
short evtrmv(void);
short evtrrp(void);
short evtrrl(void);
short evtrmi(void);
short evtrco(void);
short evtrus(void);
short evtcop(void);
short evtmult(void);
short evtinv(void);

/*
***evui.c
*/
short evlsin(void);
short evlsex(void);
short evlsln(void);
short evpspt(void);
short evpopt(void);
short evcrts(void);
short evcrmy(void);
short evpsmy(void);
short evpomy(void);
short evgtmy(void);
short evgtts(void);
short evgtal(void);
short evinpt(void);
short evinfn(void);
short evgptr(void);
short evscr(void);
short evscr2(void);
short evpos(void);
short evpos2(void);
short evidnt(void);
short evidn2(void);
short evhigh(void);

/*
***evvfile.c
*/
short evldgm(void);
short evsvgm(void);
short evldjb(void);
short evsvjb(void);
short evlmdf(void);
short evldpm(void);
short evgenv(void);

/*
***evview.c
*/
short evgtvi(void);
short evcrvi(void);
short evacvi(void);
short evscvi(void);
short evcevi(void);
short evhdvi(void);
short evprvi(void);
short evervi(void);
short evrpvi(void);
short evcavi(void);
short evmsiz(void);

/*
***evwin.c
*/
short evcrwi(void);
short evcred(void);
short evcrbu(void);
short evcrdb(void);
short evcrfb(void);
short evcric(void);
short evcrfi(void);
short evgted(void);
short evuped(void);
short evgtbu(void);
short evgtwi(void);
short evshwi(void);
short evwtwi(void);
short evdlwi(void);
short evgttl(void);
short evgtth(void);

/*
***evxht.c
*/
short evxht(void);

/*
***eval1.c
*/
short evwval(PMLITVA litval[], DBint nval, PMPARVA *pv);
short evwseg(DBint nseg, GMSEG seg[], DBint pn, PMPARVA *pv);
short evwfvk(gmflt fvk[], DBint nval, DBint pn, PMPARVA *pv);
short evwvec(DBVector vvk[], DBint nval, DBint pn, PMPARVA *pv);
short evwrve(PMREFVA idvek[], DBint i, DBint pn, PMPARVA *pv);
short evwivk(DBint ivk[],DBint  nval,DBint pn,  PMPARVA *pv);

/*
***inac10.c
*/
short inevex(pm_ptr exprla, PMLITVA *valp, pm_ptr *tylap);
short inevev(pm_ptr exprla, PMLITVA *valp, pm_ptr *tylap);

/*
***inac11.c
*/
#ifndef ANALYZER
short evtxtl(void);
#endif

/*
***inac13.c
*/
short inevnl(pm_ptr nplist, V2NAPA *npblockp);
short insetb(pm_ptr nplist);
short inpunp(V2NAPA *newdefnp);
short inponp(void);

/*
***inac17.c
*/
short evequa(PMLITVA *val1p, pm_ptr tyla1, PMLITVA *val2p, pm_ptr tyla2,
             PMLITVA *valp, pm_ptr *tylap);
short evgreq(PMLITVA *val1p, pm_ptr tyla1, PMLITVA *val2p, pm_ptr tyla2,
             PMLITVA *valp, pm_ptr *tylap);
short evleeq(PMLITVA *val1p, pm_ptr tyla1, PMLITVA *val2p, pm_ptr tyla2,
             PMLITVA *valp, pm_ptr *tylap);
short evand(pm_ptr rexpr, PMLITVA *val1p, pm_ptr tyla1,
            PMLITVA *valp, pm_ptr *tylap);
short evor(pm_ptr rexpr, PMLITVA *val1p, pm_ptr tyla1,
           PMLITVA *valp, pm_ptr *tylap);

/*
***inac19.c
*/
short mkform(pmvaty typ, int form1, int form2, char *formstr);

/*
***inac20.c
*/
short inwvar(pm_ptr acttyla, DBint rtsaddr, short dim,
             DBint indarr[], PMLITVA *valp);
short ingpva(pm_ptr palist, char valarr[], PMPATLOG typarr[], int *par_nr);

/*
***inacc.c
*/
short inmod(pm_ptr mola);
short inssta(pm_ptr statla);
short ininit(void);
short inevst(pm_ptr statla);
short inevsl(pm_ptr stlist);
short inevel(pm_ptr exlist);
short inevpa(pm_ptr pala, PMLITVA *valp, STTYTBL *in_tp, DBint cfsize);
short inevpl(pm_ptr palist);
short inevmo(pm_ptr mola);
short inevcl(pm_ptr colist);
short inevco(pm_ptr condla);
short inev(pm_ptr a);
short ineqty(pm_ptr tyla1, pm_ptr tyla2);
short incoty(PMLITVA *avalp, pm_ptr *atylap, pm_ptr ctyla);
short inmiss(void);

/*
***infunc.c
*/
short inevfu(pm_ptr routla, pm_ptr arglist, PMLITVA *valp, pm_ptr *tylap);

/*
***ingeop.c
*/
short inevgp(V2REFVA *identp, pm_ptr routla, pm_ptr arglist, V2NAPA *npblockp);

/*
***ingpar.c
*/
short inapar(pm_ptr actplist, pm_ptr routla, STPROC *rout,
             PMPARVA **pv, short *nopar);
void  inrpar(void);
void  inclpp(void);

/*
***innpar.c
*/
short inrdnp(void);
short inevnp(pm_ptr napala, V2NAPA *npblockp);

/*
***inproc.c
*/
short inevpr(pm_ptr routla, pm_ptr arglist);

/*
***inrts.c
*/
short inpfrh(char *mnamp, DBint datasz);
short ingfrh(char *mnamp);
short inpdfr(DBint datasize);
short inpval(DBint rtsofs, pm_ptr tyla, bool addrflg, PMLITVA *litvap);
short ingval(DBint rtsofs, pm_ptr tyla, bool addrflg, PMLITVA *litvap);
void  ingref(DBint rtsofs, PMLITVA *litvap);
short inirts(void);
short incrts(void);
short inallo(DBint size, char **chp, DBint *rtsofs);
char *incrsa(DBint rtsofs);
short insrtb(char *rbasp);
short insrsp(char *rsp);
char *ingrtb(void);
char *inggrb(void);
char *inglrb(void);
char *ingrsp(void);
short inslrb(void);

/*
***pmac1.c
*/
short pminit(void);
short pmcmod(char *name, PMMODULE *module, pm_ptr *retla);
short pmload(char  *modname, pm_ptr *retla);
short pmsave(pm_ptr mola);
short pmlmst(pm_ptr modptr, pm_ptr statptr);
short pmlmpa(pm_ptr modptr, pm_ptr parptr);
short pmumod(PMMODULE *module);
short pmrmod(PMMODULE *module);
short pmcpar(pm_ptr symbp, char *prompt, pm_ptr *retla);
short pmcass(pm_ptr lvalue, pm_ptr rvalue, pm_ptr *retla);
short pmcifs(pm_ptr condl, pm_ptr statl, pm_ptr *retla);
short pmcfos(pm_ptr loopvar, pm_ptr startex, pm_ptr toex,
             pm_ptr stepex, pm_ptr statl, pm_ptr *retla);
short pmcgos(pm_ptr label, pm_ptr *retla);
short pmclas(pm_ptr label, pm_ptr stat, pm_ptr *retla);
short pmcprs(pm_ptr nameid, pm_ptr valparam, pm_ptr *retla);
short pmcpas(pmseqn geid, char  *mname, pm_ptr mnexpr, pm_ptr mpara,
             pm_ptr ppara, pm_ptr nampara, pm_ptr *retla);
short pmcges(pm_ptr nameid, pmseqn geid, pm_ptr valparam,
             pm_ptr namparam, pm_ptr *retla);
short pmcune(pmopty opid, pm_ptr expr, pm_ptr *retla);
short pmcbie(pmopty opid, pm_ptr lexpr, pm_ptr rexpr, pm_ptr *retla);
short pmclie(PMLITVA *lit_p, pm_ptr *retla);
short pmcide(pm_ptr id_p, pm_ptr *retla);
short pmcine(pm_ptr indid_p, pm_ptr experl, pm_ptr *retla);
short pmccoe(pm_ptr comvar, pmfieldt field, pm_ptr *retla);
short pmcfue(pm_ptr fid, pm_ptr arglist, pm_ptr *retla);
short pmcnpa(pmmdid paname, pm_ptr valuep, pm_ptr *retla);
short pmccon(pm_ptr cond, pm_ptr statl, pm_ptr *retla);
short pmtcon(pm_ptr gennop, pm_ptr listp, pm_ptr *retla, pm_ptr *lnodep);
short pmcstr(char *str, pm_ptr *retla);
short pmcref(PMREFVA *refp, pm_ptr *retla);
void  pmstpr(pm_ptr mbase);

/*
***pmac2.c
*/
short pmgmod(pm_ptr pmla, PMMONO **retp);
short pmgpar(pm_ptr pmla, PMPANO **retp);
short pmgsta(pm_ptr pmla, PMSTNO **retp);
short pmgexp(pm_ptr pmla, PMEXNO **retp);
short pmgnpa(pm_ptr pmla, PMNPNO **retp);
short pmgcon(pm_ptr pmla, PMCONO **retp);
short pmglin(pm_ptr pmla, pm_ptr *nextla, pm_ptr *genola);
short pmgfli(pm_ptr pmla, pm_ptr *retla);
short pmgstr(pm_ptr pmla, char **retp);
short pmgvec(pm_ptr pmla, PMVECVA **retp);
short pmgref(pm_ptr pmla, PMREFEX **retp);

/*
***pmac3.c
*/
bool pmargs(pm_ptr statla);
bool pmarex(PMREFVA *idvek, pm_ptr exprla);
bool pmamir(PMREFVA *id);

/*
***pmac4.c
*/
short pmrpap(pm_ptr modptr);
short pmgpad(pm_ptr *panola);
short pmrpar(pm_ptr panola, char *name, char *prompt, PMLITVA *litval);
short pmupar(pm_ptr panola, PMLITVA *litval);

/*
***pmac5.c
*/
short pmdges(V2REFVA *ref);
short pmglst(V2REFVA *ref);
short pmdlst(void);
short pmchpa(V2REFVA *ref, short nr, pm_ptr new_expr, pm_ptr *retla);
short pmchnp(V2REFVA *ref, pmmdid npid, pm_ptr new_expr, pm_ptr *retla);
short pmlges(V2REFVA *ref, pm_ptr  *stllap, pm_ptr  *retla);
short pmrgps(pm_ptr  lstla, pm_ptr  newsla);
short pmgpla(V2REFVA *id, short *pant, pm_ptr pla[]);

/*
***pmac6.c
*/
short pmwrme(PMREFVA *id, PMREFL **prtabp, int *rant);

/*
***pmallo.c
*/
short  pmibuf(void);
short  pmallo(DBint blsize, pm_ptr *retla);
short  pmgeba(char *moname, pm_ptr *retla);
short  pmclea(void);
char  *pmgadr(pm_ptr pmla);
short  pmsbla(pm_ptr basla);
pm_ptr pmgbla(void);
short  pmsaba(pm_ptr basla);
pm_ptr pmgaba(void);
short  pmssta(void);
short  pmlsta(char *moname, pm_ptr *retla);
void   pmmark(void);
void   pmrele(void);
void   pmgstp(pm_ptr *pstack);
void   pmsstp(pm_ptr stkptr);
void   pmstat(short  *pnm, char namvek[][JNLGTH+1], pm_ptr basvek[],
             pm_ptr sizvek[], pm_ptr *pstack, pm_ptr *pheap);
short  clheap(void);

/*
***pmlist.c
*/
short pmprmo(pm_ptr mola);

/*
**pretty.c
*/
short pprsts(pm_ptr statla, char *str, int ntkn);
short pprst(pm_ptr statla);
short pprexs(pm_ptr exprla, short geotyp, char *str, int ntkn);
short pprex(pm_ptr exprla, ppopri inpri);
short pprmo(char mode, FILE *filpek);

/*
***calext.c
*/
short evcaxt(void);



#ifdef __cplusplus
}
#endif

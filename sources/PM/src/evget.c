/**********************************************************************
*
*    evget.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*
*    This file includes the following routines:
*
*    evsrgm();      Evaluates SET_ROOT_GM
*    evgngm();      Evaluates GET_NEXT_GM
*    evgtid();      Evaluates GETID
*    evgtyp();      Evaluates GETTYP
*    evgint();      Evaluates GETINT
*    evgflt();      Evaluates GETFLT
*    evgstr();      Evaluates GETSTR
*    evgvec();      Evaluates GETVEC
*    evgref();      Evaluates GETREF
*
*    evghdr();      Evaluates GETHDR
*    evuhdr();      Evaluates UPDHDR
*    evgwdt();      Evaluates GETWIDTH
*    evgpoi();      Evaluates GETPOI 
*    evglin();      Evaluates GETLIN 
*    evgarc();      Evaluates GETARC 
*    evgcur();      Evaluates GETCUR 
*    evgcuh();      Evaluates GETCURH
*    evgseg();      Evaluates GETSEG
*    evgsuh();      Evaluates GETSURH
*    evgtop();      Evaluates GETTOPP
*    evgcub();      Evaluates GETCUBP
*    evgfac();      Evaluates GETFACP
*    evgrat();      Evaluates GETRATP
*    evgtrf();      Evaluates GETTRF
*    evgtxt();      Evaluates GETTXT
*    evgxht();      Evaluates GETXHT
*    evgldm();      Evaluates GETLDM
*    evgcdm();      Evaluates GETCDM
*    evgrdm();      Evaluates GETRDM
*    evgadm();      Evaluates GETADM
*    evggrp();      Evaluates GETGRP
*    evgcsy();      Evaluates GETCSY
*    evgbpl();      Evaluates GETBPL
*    evgprt();      Evaluates GETPRT
*
*    evpdat();      Evaluates PUTDAT_GM
*    evgdat();      Evaluates GETDAT_GM
*    evddat();      Evaluates DELDAT_GM 
*    evdel();       Evaluates DEL
*
*    This library is free software; you can redistribute it and/or
*    modify it under the terms of the GNU Library General Public
*    License as published by the Free Software Foundation; either
*    version 2 of the License, or (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    Library General Public License for more details.
*
*    You should have received a copy of the GNU Library General Public
*    License along with this library; if not, write to the Free
*    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../DB/include/DBintern.h"
#include "../../IG/include/IG.h"
#include "../../EX/include/EX.h"
#include <ctype.h>
#include <string.h>
#include <memory.h>

extern PMPARVA *proc_pv;  /* inproc.c *pv      Access structure for MBS routines */
extern short    proc_pc;  /* inproc.c parcount Number of actual parameters */

extern PMPARVA *func_pv;   /* Access structure for MBS routines */
extern short    func_pc;   /* Number of actual parameters */
extern PMLITVA *func_vp;   /* Pekare till resultat. */

/*!******************************************************/

        short evsrgm()

/*      Evaluerar proceduren SET_ROOT_GM().
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *          extern proc_pc => Antal parametrar.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 1996-02-14 J. Kjellander
 *
 *      2001-02-22 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    PMREFVA id;

/*
***Här skulle det egentligen gått med en optionell parameter
***med defaultvärde = #0.1 men systemet (formpar.h) klarar inte
***defaultvärden på parametrar av typen REF så vi fixar det så
***här istället.
*/
    if ( proc_pc == 1 )
      {
      return(DBset_root_id(proc_pv[1].par_va.lit.ref_va));
      }
    else
      {
      id.seq_val = 0;
      id.ord_val = 1;
      id.p_nextre = NULL;
      return(DBset_root_id(&id));
      }
  }

/********************************************************/
/*!******************************************************/

        short evgngm()

/*      Evaluerar funktionen GET_NEXT_GM().
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  ID.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1996-02-14 J. Kjellander
 *
 *      2001-03-05 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   short   status;
   DBptr   la;
   char   *namn,*grupp;

/*
***Finns namn och/eller grupp med ? Isåfall byter vi ut ev. ""
***mot NULL.
*/
   namn = func_pv[3].par_va.lit.str_va;
   if ( *namn == '*'  &&  *(namn+1) == '\0' ) namn = NULL;

   grupp = func_pv[4].par_va.lit.str_va;
   if ( *grupp == '*' &&  *(grupp+1) == '\0'  ) grupp = NULL;
/*
***Hämta LA till nästa storhet från GM.
*/   
   status = DBget_next_ptr(func_pv[1].par_va.lit.int_va,
                  (DBetype)func_pv[2].par_va.lit.int_va,
                   namn,
                   grupp,
                  &la);
   if ( status < 0 ) return(status);
/*
***Översätt LA till ID. LA = DBNULL betyder slut på storheter.
*/
   if ( la == DBNULL )
     {
     func_vp->lit.ref_va[0].seq_val = 0;
     func_vp->lit.ref_va[0].ord_val = 1;
     func_vp->lit.ref_va[0].p_nextre = NULL;
     }
   else DBget_id(la,func_vp->lit.ref_va);
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short evgtid()

/*      Evaluerar funktionen GETID.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  ID.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 24/3/86 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXgtid(&func_vp->lit.ref_va[0],func_pv[1].par_va.lit.str_va));
  }

/********************************************************/
/*!******************************************************/

        short evgtyp()

/*      Evaluerar funktionen GETTYP.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  ID.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 24/3/86 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXgtyp(  &func_pv[1].par_va.lit.ref_va[0],
              (short)func_pv[2].par_va.lit.int_va,func_vp));
  }

/********************************************************/
/*!******************************************************/

        short evgint()

/*      Evaluerar funktionen GETINT.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  ID.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 24/3/86 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    return(EXgint(  &func_pv[1].par_va.lit.ref_va[0],
              (short)func_pv[2].par_va.lit.int_va,func_vp));

  }

/********************************************************/
/*!******************************************************/

        short evgflt()

/*      Evaluerar funktionen GETFLT.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  ID.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 24/3/86 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    return(EXgflt(  &func_pv[1].par_va.lit.ref_va[0],
              (short)func_pv[2].par_va.lit.int_va,func_vp));

  }

/********************************************************/
/*!******************************************************/

        short evgstr()

/*      Evaluerar funktionen GETSTR.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  ID.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 24/3/86 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    return(EXgstr(   &func_pv[1].par_va.lit.ref_va[0],
               (short)func_pv[2].par_va.lit.int_va,func_vp));

  }

/********************************************************/
/*!******************************************************/

        short evgvec()

/*      Evaluerar funktionen GETVEC.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  ID.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 24/3/86 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    return(EXgvec(   &func_pv[1].par_va.lit.ref_va[0],
               (short)func_pv[2].par_va.lit.int_va,func_vp));

  }

/********************************************************/
/*!******************************************************/

        short evgref()

/*      Evaluerar funktionen GETREF.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  ID.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 29/5/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    return(EXgref(   &func_pv[1].par_va.lit.ref_va[0],
               (short)func_pv[2].par_va.lit.int_va,func_vp));

  }

/********************************************************/
/*!******************************************************/

        short evghdr()

/*      Evaluerar proceduren GETHDR.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      2001-02-13 In-Param changed to Global variables, R Svedin
 *
 * ******************************************************!*/

  {
     short   status,i;
     DBptr   la;
     DBetype typ;
     DBHeader  hdr;
     PMLITVA litval[5];
     PMREFVA idvek[MXINIV];

/*
***Översätt ID till la och kolla om storheten finns.
***Om inte sätts 1:a parametern till -1 och övriga till 0.
***Detta måste göras direkt i PMLITVA:n eftersom headerns
***medlem type är unsigned.
*/
     if ( DBget_pointer('I',&proc_pv[1].par_va.lit.ref_va[0],&la,&typ) < 0 )
       {
       erinit();
       litval[0].lit.int_va = -1;
       hdr.vers  = 0;
       hdr.blank = 0;
       hdr.level = 0;
       hdr.pen = 0;
       hdr.g_ptr[0] = hdr.g_ptr[1] = hdr.g_ptr[2] = DBNULL;
       }
/*
***Storheten finns, hämta huvud från GM. Sätt typ.
*/
     else
       {
       if ( (status=DBread_header(&hdr,la)) < 0 ) return(status);
       litval[0].lit.int_va = hdr.type;/*
***Resten av parametrarna.
*/
     litval[1].lit.int_va = hdr.vers;
     litval[2].lit.int_va = hdr.blank;
     litval[3].lit.int_va = hdr.level;
     litval[4].lit.int_va = hdr.pen;
/*
***Skriv parametervärden 1-5 till motsvarande MBS-variabler.
*/
     evwval(litval, 5, proc_pv);
       }

/*
***Returnera parameter 6 = grupper, max 3 st.
*/
     for ( i=0; i<3; ++i )
        {
        if ( hdr.g_ptr[i] == DBNULL )
          {
          idvek[0].seq_val = 0;
          idvek[0].ord_val = 1;
          idvek[0].p_nextre = NULL;
          }
        else
          {
          DBget_id(hdr.g_ptr[i],idvek);
          }
        status = evwrve(idvek,i,7,proc_pv);
        }
/*
***Slut.
*/
     return(status);
  }

/********************************************************/
/*!******************************************************/

        short evuhdr()

/*      Evaluerar proceduren UPDHDR.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 14/10/88 R. Svedin
 *
 *      15/3/92 refcnt, J. Kjellander
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
/*
***Uppdatera blank,level och pen. refcnt, parameter 2 utgår.
*/
   return(EXuphd(      proc_pv[1].par_va.lit.ref_va,
                (short)proc_pv[3].par_va.lit.int_va,
                (short)proc_pv[4].par_va.lit.int_va,
                (short)proc_pv[5].par_va.lit.int_va));

  }

/********************************************************/
/*!******************************************************/

        short evgwdt()

/*      Evaluerar funktionen GETWIDTH.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  ID.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1998-01-01 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    short status;
    DBfloat width;

    status = EXgwdt(func_pv[1].par_va.lit.ref_va,&width);

    if ( status == 0 ) func_vp->lit.float_va = width;

    return(status);
  }

/********************************************************/
/*!******************************************************/

        short evgpoi()

/*      Evaluerar proceduren GETPOI.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      2001-02-13 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
     short   status;
     DBPoint   punkt;
     PMLITVA litval;

/*
***Hämta punkt från GM.
*/
     if ( (status=EXgtpo(&proc_pv[1].par_va.lit.ref_va[0], &punkt)) < 0 )
       return(status);
/*
***Returnera parameter 1 = pos.
*/
     litval.lit.vec_va.x_val = punkt.crd_p.x_gm;
     litval.lit.vec_va.y_val = punkt.crd_p.y_gm;
     litval.lit.vec_va.z_val = punkt.crd_p.z_gm;
     evwval(&litval, 1, proc_pv);
/*
***Slut.
*/
     return(0);
  }

/********************************************************/
/*!******************************************************/

        short evglin()

/*      Evaluerar proceduren GETLIN.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      2001-02-13 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
     short   status;
     DBLine   linje;
     PMLITVA litval[4];

/*
***Hämta linje från GM.
*/
     if ( (status=EXgtli(&proc_pv[1].par_va.lit.ref_va[0], &linje)) < 0 )
       return(status);
/*
***Kopiera parametervärden till PMLITVA.
*/
     litval[0].lit.int_va = linje.fnt_l;
     litval[1].lit.float_va = linje.lgt_l;
     litval[2].lit.vec_va.x_val = linje.crd1_l.x_gm;
     litval[2].lit.vec_va.y_val = linje.crd1_l.y_gm;
     litval[2].lit.vec_va.z_val = linje.crd1_l.z_gm;
     litval[3].lit.vec_va.x_val = linje.crd2_l.x_gm;
     litval[3].lit.vec_va.y_val = linje.crd2_l.y_gm;
     litval[3].lit.vec_va.z_val = linje.crd2_l.z_gm;
/*
***Skriv parametervärden till motsvarande MBS-variabler.
*/
     evwval(litval, 4, proc_pv);
/*
***Slut.
*/
     return(0);
  }

/********************************************************/
/*!******************************************************/

        short evgarc()

/*      Evaluerar proceduren GETARC.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      2001-02-13 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
     short   status;
     DBArc   arc;
     DBSeg   arcseg[4];
     PMLITVA litval[7];

/*
***Hämta arc från GM.
*/
     if ( (status=EXgtar(&proc_pv[1].par_va.lit.ref_va[0],
                         &arc,arcseg)) < 0 ) return(status);
/*
***Kopiera parametervärden 1-7 till PMLITVA.
*/
     litval[0].lit.int_va = arc.fnt_a;
     litval[1].lit.float_va = arc.lgt_a;
     litval[2].lit.vec_va.x_val = arc.x_a;
     litval[2].lit.vec_va.y_val = arc.y_a;
     litval[2].lit.vec_va.z_val = 0.0;
     litval[3].lit.float_va = arc.r_a;
     litval[4].lit.float_va = arc.v1_a;
     litval[5].lit.float_va = arc.v2_a;
     litval[6].lit.int_va = arc.ns_a;
/*
***Skriv parametervärden 1-7 till motsvarande MBS-variabler.
*/
     evwval(litval, 7, proc_pv);
/*
***Returnera parameter 8 = segment.
*/
     return(evwseg(arc.ns_a,arcseg,9,proc_pv));
  }

/********************************************************/
/*!******************************************************/

        short evgcur()

/*      Evaluerar proceduren GETCUR.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      Felkoder: PM1032 = Kan ej mallokera minne för offset
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      23/11/91 Nytt format på kurvor, J. Kjellander
 *      7/6/93   Dynamisk allokering av minne, J. Kjellander
 *      2001-02-13 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
     short   status;
     DBint   i,j,index[2];
     DBfloat  *offs,*matpek;
     DBCurve   cur;
     DBSeg  *segpek;
     PMLITVA litval[3];

/*
***Hämta cur från GM.
*/
     if ( (status=EXgtcu(&proc_pv[1].par_va.lit.ref_va[0],
                         &cur,NULL,&segpek)) < 0 ) return(status);
/*
***Kopiera font, strlgd och nseg till PMLITVA. Skriv till
***motsvarande MBS-variabler.
*/
     litval[0].lit.int_va   = cur.fnt_cu;
     litval[1].lit.float_va = cur.lgt_cu;
     litval[2].lit.int_va   = cur.ns_cu;
     evwval(litval, 3, proc_pv);
/*
***Returnera segment.
*/
     if ( (status=evwseg(cur.ns_cu,segpek,5,proc_pv)) < 0 ) goto end;
/*
***Returnera offset.
*/
     if ( (offs=(DBfloat *)v3mall(cur.ns_cu*sizeof(gmflt),"evgcur")) == NULL )
       {
       status = erpush("PM1032","");
       goto end;
       }
     for ( i=0; i<cur.ns_cu; ++i ) *(offs+i) = (segpek+i)->ofs;
     status = evwfvk(offs,cur.ns_cu,6,proc_pv);
     v3free(offs,"evgcur");
     if ( status < 0 ) goto end;
/*
***Returnera plank.
*/
     litval[0].lit.int_va = cur.plank_cu;
     inwvar(proc_pv[7].par_ty, proc_pv[7].par_va.lit.adr_va, 
             0, NULL, litval);
/*
***Returnera parameter 8, kurvplan.
***Om inget kurvplan finns, returnera nollor, annars
***blir det problem på VAX:en.
*/
     matpek = (DBfloat *) &cur.csy_cu;

     for ( i=0; i<4; ++i )
        {
        index[0] = i+1;
        for ( j=0; j<4; ++j )
           {
           index[1] = j+1;
           if ( cur.plank_cu == TRUE )
             litval[0].lit.float_va = *matpek;
           else
             litval[0].lit.float_va = 0.0;
           status = inwvar(proc_pv[8].par_ty, proc_pv[8].par_va.lit.adr_va,
                                           2, index, litval);
           ++matpek;
           }
        }
/*
***Lämna tillbaks allokerat minne.
*/
end:
     DBfree_segments(segpek);

     return(status);
  }

/********************************************************/
/*!******************************************************/

        short evgcuh()

/*      Evaluerar proceduren GETCURH.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 10/1/94 J. Kjellander
 *
 *      2001-02-13 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
     short   status;
     DBCurve   cur;
     PMLITVA litval[4];

/*
***Hämta cur från GM.
*/
     if ( (status=EXgtcu(&proc_pv[1].par_va.lit.ref_va[0],
                         &cur,NULL,NULL)) < 0 ) return(status);
/*
***Kopiera font, strlgd och nseg till PMLITVA. Skriv till
***motsvarande MBS-variabler.
*/
     litval[0].lit.int_va   = cur.fnt_cu;
     litval[1].lit.float_va = cur.lgt_cu;
     litval[2].lit.int_va   = cur.ns_cu;
     litval[3].lit.int_va   = cur.nsgr_cu;
     status = evwval(litval, 4, proc_pv);

     return(status);
  }

/********************************************************/
/*!******************************************************/

        short evgseg()

/*      Evaluerar proceduren GETSEG.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *          extern proc_pc => Antal parametrar.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 10/1/94 J. Kjellander
 *
 *      2001-02-13 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
     char    errbuf[80];
     short   status,segnum,segant;
     DBint   i,j,index[2];
     DBfloat  *datpek;
     DBCurve   cur;
     DBSeg  *segpek,*actpek;
     PMLITVA litval;

/*
***Vilken typ av segment önskas ? reptyp = 0 => geometrisk
***                               reptyp = 1 => grafisk
***Hämta kurva med rätt sorts segment från GM.
*/
   if ( proc_pv[3].par_va.lit.int_va == 0 )
     {
     status = EXgtcu(&proc_pv[1].par_va.lit.ref_va[0],&cur,NULL,&segpek);
     segant = cur.ns_cu;
     }
   else
     {
     status = EXgtcu(&proc_pv[1].par_va.lit.ref_va[0],&cur,&segpek,NULL);
     segant = cur.nsgr_cu;
     }

   if ( status < 0 ) return(status);
/*
***Vilket segment önskas ?
*/
   segnum = proc_pv[2].par_va.lit.int_va;

   if ( segnum < 1  ||  segnum > segant )
     {
     sprintf(errbuf,"%d",(int)segnum);
     status = erpush("PM1042",errbuf);
     goto end;
     }
/*
***Fixa en pekare till det önskade segmentet.
*/
   actpek = segpek + (segnum - 1);
   datpek = (DBfloat *)actpek;
/*
***Returnera segment-koefficienter.
*/
   for ( i=0; i<4; ++i )
      {
      index[0] = i+1;
      for ( j=0; j<4; ++j )
         {
         index[1] = j+1;
         litval.lit.float_va = *datpek;
         status = inwvar(proc_pv[4].par_ty, 
			             proc_pv[4].par_va.lit.adr_va,
                         2, index, &litval);
         if ( status < 0 ) goto end;
         ++datpek;
         }
      }
/*
***Kopiera offset mm. till PMLITVA. Skriv till
***motsvarande MBS-variabler.
*/
   litval.lit.float_va = actpek->ofs;
   inwvar(proc_pv[5].par_ty,
	      proc_pv[5].par_va.lit.adr_va,0,NULL,&litval);

   litval.lit.int_va = actpek->typ;
   inwvar(proc_pv[6].par_ty,
	      proc_pv[6].par_va.lit.adr_va,0,NULL,&litval);
/*
***Följande parametrar är optionella. Först subtyp.
*/
   if ( proc_pc > 6 )
     {
     litval.lit.int_va = actpek->subtyp;
     inwvar(proc_pv[7].par_ty,
		    proc_pv[7].par_va.lit.adr_va,0,NULL,&litval);
     }
/*
***ID för yta-1. Gäller bara UV-segment.
*/
   if ( proc_pc > 7 )
     {
     if ( actpek->typ == UV_CUB_SEG  &&  actpek->spek_gm > 0 )
       {
       DBget_id(actpek->spek_gm,&litval.lit.ref_va[0]);
       }
     else
       {
       litval.lit.ref_va[0].seq_val = 0;
       litval.lit.ref_va[0].ord_val = 1;
       litval.lit.ref_va[0].p_nextre = NULL;
       }
     inwvar(proc_pv[8].par_ty,
		    proc_pv[8].par_va.lit.adr_va,0,NULL,&litval);
     }
/*
***ID för yta-2. Gäller bara UV-segment.
*/
   if ( proc_pc > 8 )
     {
     if ( actpek->typ == UV_CUB_SEG  &&  actpek->spek2_gm > 0 )
       {
       DBget_id(actpek->spek2_gm,&litval.lit.ref_va[0]);
       }
     else
       {
       litval.lit.ref_va[0].seq_val = 0;
       litval.lit.ref_va[0].ord_val = 1;
       litval.lit.ref_va[0].p_nextre = NULL;
       }
     inwvar(proc_pv[9].par_ty,
		    proc_pv[9].par_va.lit.adr_va,0,NULL,&litval);
     }
/*
***Lämna tillbaks allokerat minne.
*/
end:
   DBfree_segments(segpek);

   return(status);
  }

/********************************************************/
/*!******************************************************/

        short evgsuh()

/*      Evaluerar proceduren GETSURH.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 10/1/94 J. Kjellander
 *
 *      2001-02-13 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype typ;
    char   *kod;
    DBSurf   sur;
    PMLITVA litval;

/*
***Hämta yta från GM.
*/
   if ( DBget_pointer('I',&proc_pv[1].par_va.lit.ref_va[0],&la,&typ) < 0 )
     return(erpush("EX1402",""));
   DBread_surface(&sur,la);
/*
***Returnera det som efterfrågats.
*/
   kod = proc_pv[2].par_va.lit.str_va;

   if      ( strcmp(kod,"NPATU") == 0 )
     litval.lit.int_va = sur.nu_su;

   else if ( strcmp(kod,"NPATV") == 0 )
     litval.lit.int_va = sur.nv_su;

   else if ( strcmp(kod,"SUR_TYPE") == 0 )
     litval.lit.int_va = sur.typ_su;

   else if ( strcmp(kod,"FONT") == 0 )
     litval.lit.int_va = sur.fnt_su;

   else if ( strcmp(kod,"SDASHL") == 0 )
     litval.lit.float_va = sur.lgt_su;

   else
     return(erpush("IN2122",kod));

   inwvar(proc_pv[3].par_ty,
	      proc_pv[3].par_va.lit.adr_va,0,NULL,&litval);

   return(0);

  }

/********************************************************/
/*!******************************************************/

        short evgtop()

/*      Evaluerar proceduren GETTOPP.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 28/2/95 J. Kjellander
 *
 *      2001-02-13 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   short   status;
   DBPatch   toppat;
   PMLITVA litval;

/*
***Hämta topologisk patch från GM.
*/
   if ( (status=EXgtop(&proc_pv[1].par_va.lit.ref_va[0],
                 (short)proc_pv[2].par_va.lit.int_va,
                 (short)proc_pv[3].par_va.lit.int_va,
                       &toppat)) < 0 ) return(status);
/*
***Kopiera data till PMLITVA och skriv till motsvarande variabler.
*/
   litval.lit.int_va = toppat.styp_pat;
   inwvar(proc_pv[4].par_ty, 
	      proc_pv[4].par_va.lit.adr_va,0,NULL,&litval);

   litval.lit.float_va = toppat.us_pat;
   inwvar(proc_pv[5].par_ty, 
	      proc_pv[5].par_va.lit.adr_va,0,NULL,&litval);

   litval.lit.float_va = toppat.ue_pat;
   inwvar(proc_pv[6].par_ty, 
	      proc_pv[6].par_va.lit.adr_va,0,NULL,&litval);

   litval.lit.float_va = toppat.vs_pat;
   inwvar(proc_pv[7].par_ty, 
	      proc_pv[7].par_va.lit.adr_va,0,NULL,&litval);

   litval.lit.float_va = toppat.ve_pat;
   inwvar(proc_pv[8].par_ty, 
	      proc_pv[8].par_va.lit.adr_va,0,NULL,&litval);

   return(status);
  }

/********************************************************/
/*!******************************************************/

        short evgcub()

/*      Evaluerar proceduren GETCUBP.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 28/2/95 J. Kjellander
 *
 *      2001-02-13 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   short    status;
   DBfloat  data[48];
   DBPatchC cubpat;
   PMLITVA  litval;
/*
***Hämta patchdata från GM.
*/
   if ( (status=EXgcub(&proc_pv[1].par_va.lit.ref_va[0],
                 (short)proc_pv[2].par_va.lit.int_va,
                 (short)proc_pv[3].par_va.lit.int_va,
                       &cubpat)) < 0 ) return(status);
/*
***Skriv patchkoefficienterna till MBS-variabeln.
*/
   data[0]  = cubpat.a00x; data[1]  = cubpat.a00y; data[2]  = cubpat.a00z;
   data[3]  = cubpat.a01x; data[4]  = cubpat.a01y; data[5]  = cubpat.a01z;
   data[6]  = cubpat.a02x; data[7]  = cubpat.a02y; data[8]  = cubpat.a02z;
   data[9]  = cubpat.a03x; data[10] = cubpat.a03y; data[11] = cubpat.a03z;

   data[12] = cubpat.a10x; data[13] = cubpat.a10y; data[14] = cubpat.a10z;
   data[15] = cubpat.a11x; data[16] = cubpat.a11y; data[17] = cubpat.a11z;
   data[18] = cubpat.a12x; data[19] = cubpat.a12y; data[20] = cubpat.a12z;
   data[21] = cubpat.a13x; data[22] = cubpat.a13y; data[23] = cubpat.a13z;

   data[24] = cubpat.a20x; data[25] = cubpat.a20y; data[26] = cubpat.a20z;
   data[27] = cubpat.a21x; data[28] = cubpat.a21y; data[29] = cubpat.a21z;
   data[30] = cubpat.a22x; data[31] = cubpat.a22y; data[32] = cubpat.a22z;
   data[33] = cubpat.a23x; data[34] = cubpat.a23y; data[35] = cubpat.a23z;

   data[36] = cubpat.a30x; data[37] = cubpat.a30y; data[38] = cubpat.a30z;
   data[39] = cubpat.a31x; data[40] = cubpat.a31y; data[41] = cubpat.a31z;
   data[42] = cubpat.a32x; data[43] = cubpat.a32y; data[44] = cubpat.a32z;
   data[45] = cubpat.a33x; data[46] = cubpat.a33y; data[47] = cubpat.a33z;

   status = evwfvk(data,48,4,proc_pv);
/*
***Offset.
*/
   litval.lit.float_va = cubpat.ofs_pat;
   inwvar(proc_pv[5].par_ty,
	      proc_pv[5].par_va.lit.adr_va,0,NULL,&litval);

   return(status);
  }

/********************************************************/
/*!******************************************************/

        short evgfac()

/*      Evaluerar proceduren GETFACP.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 1997-06-06 J. Kjellander
 *  
 *      2001-02-13 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   short    status;
   DBPatchF   facpat;
   PMLITVA  litval[4];

/*
***Hämta patchdata från GM.
*/
   if ( (status=EXgfac(&proc_pv[1].par_va.lit.ref_va[0],
                 (short)proc_pv[2].par_va.lit.int_va,
                 (short)proc_pv[3].par_va.lit.int_va,
                       &facpat)) < 0 ) return(status);
/*
***Kopiera positioner till PMLITVA.
*/
   litval[0].lit.vec_va.x_val = facpat.p1.x_gm;
   litval[0].lit.vec_va.y_val = facpat.p1.y_gm;
   litval[0].lit.vec_va.z_val = facpat.p1.z_gm;
   litval[1].lit.vec_va.x_val = facpat.p2.x_gm;
   litval[1].lit.vec_va.y_val = facpat.p2.y_gm;
   litval[1].lit.vec_va.z_val = facpat.p2.z_gm;
   litval[2].lit.vec_va.x_val = facpat.p3.x_gm;
   litval[2].lit.vec_va.y_val = facpat.p3.y_gm;
   litval[2].lit.vec_va.z_val = facpat.p3.z_gm;
   litval[3].lit.vec_va.x_val = facpat.p4.x_gm;
   litval[3].lit.vec_va.y_val = facpat.p4.y_gm;
   litval[3].lit.vec_va.z_val = facpat.p4.z_gm;
/*
***Skriv till motsvarande MBS-variabler.
*/
   inwvar(proc_pv[4].par_ty,
	      proc_pv[4].par_va.lit.adr_va,0,NULL,&litval[0]);
   inwvar(proc_pv[5].par_ty,
	      proc_pv[5].par_va.lit.adr_va,0,NULL,&litval[1]);
   inwvar(proc_pv[6].par_ty,
	      proc_pv[6].par_va.lit.adr_va,0,NULL,&litval[2]);
   inwvar(proc_pv[7].par_ty,
	      proc_pv[7].par_va.lit.adr_va,0,NULL,&litval[3]);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short evgrat()

/*      Evaluerar proceduren GETRATP.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 28/2/95 J. Kjellander
 *
 *      2001-02-13 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short evgtrf()

/*      Evaluerar proceduren GETTRF.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 15/12/92 J. Kjellander
 *
 *      2001-02-13 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
     short   status;
     DBint   i,j,index[2];
     DBTform   trf;
     PMLITVA litval;
     DBfloat  *matpek;

/*
***Hämta transformation från GM.
*/
     if ( (status=EXgttf(&proc_pv[1].par_va.lit.ref_va[0], &trf)) < 0 )
       return(status);
/*
***Returnera parameter 2, transformationsmatris.
*/
     matpek = (DBfloat *) &trf.mat_tf;

     for ( i=0; i<4; ++i )
        {
        index[0] = i+1;
        for ( j=0; j<4; ++j )
           {
           index[1] = j+1;
           litval.lit.float_va = *matpek;
           status = inwvar(proc_pv[2].par_ty, proc_pv[2].par_va.lit.adr_va,
                                           2, index, &litval);
           ++matpek;
           }
        }
/*
***Slut.
*/
     return(status);
  }

/********************************************************/
/*!******************************************************/

        short evgtxt()

/*      Evaluerar proceduren GETTXT.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      2001-02-13 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
     short   status;
     DBText   text;
     char    str[V3STRLEN+1];
     PMLITVA litval[7];

/*
***Hämta text från GM.
*/
     if ( (status=EXgttx(&proc_pv[1].par_va.lit.ref_va[0], &text, str)) < 0 )
       return(status);
/*
***Kopiera parametervärden till PMLITVA.
*/
     litval[0].lit.vec_va.x_val = text.crd_tx.x_gm;
     litval[0].lit.vec_va.y_val = text.crd_tx.y_gm;
     litval[0].lit.vec_va.z_val = text.crd_tx.z_gm;
     litval[1].lit.int_va = text.fnt_tx;
     litval[2].lit.float_va = text.h_tx;;
     litval[3].lit.float_va = text.b_tx;;
     litval[4].lit.float_va = text.l_tx;;
     litval[5].lit.float_va = text.v_tx;;
     strcpy(litval[6].lit.str_va,str);
/*
***Skriv parametervärden till motsvarande MBS-variabler.
*/
     evwval(litval, 7, proc_pv);
/*
***Slut.
*/
     return(0);
  }

/********************************************************/
/*!******************************************************/

        short evgxht()

/*      Evaluerar proceduren GETXHT.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
     short   status;
     DBHatch xht;
     DBfloat xhtcrd[4*GMXMXL];
     PMLITVA litval[5];

/*
***Hämta snitt från GM.
*/
     if ( (status=EXgtxh(&proc_pv[1].par_va.lit.ref_va[0],
		                 &xht, xhtcrd)) < 0 )
       return(status);
/*
***Kopiera parametervärden 1-5 till PMLITVA.
*/
     litval[0].lit.int_va = xht.fnt_xh;
     litval[1].lit.float_va = xht.lgt_xh;
     litval[2].lit.float_va = xht.dist_xh;
     litval[3].lit.float_va = xht.ang_xh;
     litval[4].lit.int_va = xht.nlin_xh;
/*
***Skriv parametervärden 1-5 till motsvarande MBS-variabler.
*/
     evwval(litval, 5, proc_pv);
/*
***Returnera parameter 6, snittlinjer.
*/
     return(evwfvk(xhtcrd,4*xht.nlin_xh,7,proc_pv));
  }

/********************************************************/
/*!******************************************************/

        short evgldm()

/*      Evaluerar proceduren GETLDM.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
     short   status;
     DBLdim   ldm;
     PMLITVA litval[8];

/*
***Hämta längdmått från GM.
*/
     if ( (status=EXgtld(&proc_pv[1].par_va.lit.ref_va[0], &ldm)) < 0 )
       return(status);
/*
***Kopiera parametervärden till PMLITVA.
*/
     litval[0].lit.vec_va.x_val = ldm.p1_ld.x_gm;
     litval[0].lit.vec_va.y_val = ldm.p1_ld.y_gm;
     litval[0].lit.vec_va.z_val = ldm.p1_ld.z_gm;
     litval[1].lit.vec_va.x_val = ldm.p2_ld.x_gm;
     litval[1].lit.vec_va.y_val = ldm.p2_ld.y_gm;
     litval[1].lit.vec_va.z_val = ldm.p2_ld.z_gm;
     litval[2].lit.vec_va.x_val = ldm.p3_ld.x_gm;
     litval[2].lit.vec_va.y_val = ldm.p3_ld.y_gm;
     litval[2].lit.vec_va.z_val = ldm.p3_ld.z_gm;
     litval[3].lit.int_va = ldm.dtyp_ld;
     litval[4].lit.float_va = ldm.asiz_ld;
     litval[5].lit.float_va = ldm.tsiz_ld;
     litval[6].lit.int_va = ldm.ndig_ld;
     if ( ldm.auto_ld ) litval[7].lit.int_va = 1;
     else               litval[7].lit.int_va = 0;
/*
***Skriv parametervärden till motsvarande MBS-variabler.
*/
     evwval(litval, 8, proc_pv);
/*
***Slut.
*/
     return(0);
  }

/********************************************************/
/*!******************************************************/

        short evgcdm()

/*      Evaluerar proceduren GETCDM.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
     short   status;
     DBCdim   cdm;
     PMLITVA litval[8];

/*
***Hämta diametermått från GM.
*/
     if ( (status=EXgtcd(&proc_pv[1].par_va.lit.ref_va[0], &cdm)) < 0 )
       return(status);
/*
***Kopiera parametervärden till PMLITVA.
*/
     litval[0].lit.vec_va.x_val = cdm.p1_cd.x_gm;
     litval[0].lit.vec_va.y_val = cdm.p1_cd.y_gm;
     litval[0].lit.vec_va.z_val = cdm.p1_cd.z_gm;
     litval[1].lit.vec_va.x_val = cdm.p2_cd.x_gm;
     litval[1].lit.vec_va.y_val = cdm.p2_cd.y_gm;
     litval[1].lit.vec_va.z_val = cdm.p2_cd.z_gm;
     litval[2].lit.vec_va.x_val = cdm.p3_cd.x_gm;
     litval[2].lit.vec_va.y_val = cdm.p3_cd.y_gm;
     litval[2].lit.vec_va.z_val = cdm.p3_cd.z_gm;
     litval[3].lit.int_va = cdm.dtyp_cd;
     litval[4].lit.float_va = cdm.asiz_cd;
     litval[5].lit.float_va = cdm.tsiz_cd;
     litval[6].lit.int_va = cdm.ndig_cd;
     if ( cdm.auto_cd ) litval[7].lit.int_va = 1;
     else               litval[7].lit.int_va = 0;
/*
***Skriv parametervärden till motsvarande MBS-variabler.
*/
     evwval(litval, 8, proc_pv);
/*
***Slut.
*/
     return(0);
  }

/********************************************************/
/*!******************************************************/

        short evgrdm()

/*      Evaluerar proceduren GETRDM.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
     short   status;
     DBRdim   rdm;
     PMLITVA litval[8];

/*
***Hämta radiemått från GM.
*/
     if ( (status=EXgtrd(&proc_pv[1].par_va.lit.ref_va[0], &rdm)) < 0 )
       return(status);
/*
***Kopiera parametervärden till PMLITVA.
*/
     litval[0].lit.vec_va.x_val = rdm.p1_rd.x_gm;
     litval[0].lit.vec_va.y_val = rdm.p1_rd.y_gm;
     litval[0].lit.vec_va.z_val = rdm.p1_rd.z_gm;
     litval[1].lit.vec_va.x_val = rdm.p2_rd.x_gm;
     litval[1].lit.vec_va.y_val = rdm.p2_rd.y_gm;
     litval[1].lit.vec_va.z_val = rdm.p2_rd.z_gm;
     litval[2].lit.vec_va.x_val = rdm.p3_rd.x_gm;
     litval[2].lit.vec_va.y_val = rdm.p3_rd.y_gm;
     litval[2].lit.vec_va.z_val = rdm.p3_rd.z_gm;
     litval[3].lit.float_va = rdm.r_rd;
     litval[4].lit.float_va = rdm.asiz_rd;
     litval[5].lit.float_va = rdm.tsiz_rd;
     litval[6].lit.int_va = rdm.ndig_rd;
     if ( rdm.auto_rd ) litval[7].lit.int_va = 1;
     else               litval[7].lit.int_va = 0;
/*
***Skriv parametervärden till motsvarande MBS-variabler.
*/
     evwval(litval, 8, proc_pv);
/*
***Slut.
*/
     return(0);
  }

/********************************************************/
/*!******************************************************/

        short evgadm()

/*      Evaluerar proceduren GETADM.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
     short   status;
     DBAdim   adm;
     PMLITVA litval[11];

/*
***Hämta vinkelmått från GM.
*/
     if ( (status=EXgtad(&proc_pv[1].par_va.lit.ref_va[0], &adm)) < 0 )
       return(status);
/*
***Kopiera parametervärden till PMLITVA.
*/
     litval[0].lit.vec_va.x_val = adm.pos_ad.x_gm;
     litval[0].lit.vec_va.y_val = adm.pos_ad.y_gm;
     litval[0].lit.vec_va.z_val = adm.pos_ad.z_gm;
     litval[1].lit.float_va = adm.r_ad;
     litval[2].lit.float_va = adm.v1_ad;
     litval[3].lit.float_va = adm.r1_ad;
     litval[4].lit.float_va = adm.v2_ad;
     litval[5].lit.float_va = adm.r2_ad;
     litval[6].lit.float_va = adm.tv_ad;
     litval[7].lit.float_va = adm.asiz_ad;
     litval[8].lit.float_va = adm.tsiz_ad;
     litval[9].lit.int_va = adm.ndig_ad;
     if ( adm.auto_ad ) litval[10].lit.int_va = 1;
     else               litval[10].lit.int_va = 0;
/*
***Skriv parametervärden till motsvarande MBS-variabler.
*/
     evwval(litval, 11, proc_pv);
/*
***Slut.
*/
     return(0);
  }

/********************************************************/
/*!******************************************************/

        short evggrp()

/*      Evaluerar proceduren GETGRP.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
     short   status,i;
     DBptr   lavek[GMMXGP];
     DBGroup   grp;
     PMLITVA litval[2];
     PMREFVA idvek[MXINIV];

/*
***Hämta grupp från GM.
*/
     if ( (status=EXgtgp(&proc_pv[1].par_va.lit.ref_va[0], 
		                 &grp, lavek)) < 0 )
       return(status);
/*
***Kopiera namn och antal till PMLITVA.
*/
     strcpy(litval[0].lit.str_va,grp.name_gp);
     litval[1].lit.int_va = grp.nmbr_gp;
/*
***Skriv parametervärden 1-2 till motsvarande MBS-variabler.
*/
     evwval(litval, 2, proc_pv);
/*
***Returnera parameter 3, medlemmar i gruppen.
*/
     for ( i=0; i<grp.nmbr_gp; ++i )
        {
        DBget_id(lavek[i],idvek);
        status = evwrve(idvek,i,4,proc_pv);
        }
/*
***Slut.
*/
     return(status);
  }

/********************************************************/
/*!******************************************************/

        short evgcsy()

/*      Evaluerar proceduren GETCSY.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
     short   status;
     DBint   i,j,index[2];
     DBCsys   csy;
     PMLITVA litval;
     DBfloat  *matpek;

/*
***Hämta koordinatsystem från GM.
*/
     if ( (status=EXgtcs(&proc_pv[1].par_va.lit.ref_va[0], &csy)) < 0 )
       return(status);
/*
***Parameter 1.
*/
     strcpy(litval.lit.str_va,csy.name_pl);
     evwval(&litval, 1, proc_pv);
/*
***Returnera parameter 2, transformationsmatris.
*/
     matpek = (DBfloat *) &csy.mat_pl;

     for ( i=0; i<4; ++i )
        {
        index[0] = i+1;
        for ( j=0; j<4; ++j )
           {
           index[1] = j+1;
           litval.lit.float_va = *matpek;
           status = inwvar(proc_pv[3].par_ty, 
			               proc_pv[3].par_va.lit.adr_va,
                           2, index, &litval);
           ++matpek;
           }
        }
/*
***Slut.
*/
     return(status);
  }

/********************************************************/
/*!******************************************************/

        short evgbpl()

/*      Evaluerar proceduren GETBPL.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 27/8/87 J. Kjellander
 *
 *      3/2/88   Bugfix 5->4, J. Kjellander
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
     short   status;
     DBBplane   bplan;
     PMLITVA litval[4];

/*
***Hämta B-plan från GM.
*/
     if ( (status=EXgtbp(&proc_pv[1].par_va.lit.ref_va[0], &bplan)) < 0 )
       return(status);
/*
***Kopiera parametervärden till PMLITVA.
*/
     litval[0].lit.vec_va.x_val = bplan.crd1_bp.x_gm;
     litval[0].lit.vec_va.y_val = bplan.crd1_bp.y_gm;
     litval[0].lit.vec_va.z_val = bplan.crd1_bp.z_gm;
     litval[1].lit.vec_va.x_val = bplan.crd2_bp.x_gm;
     litval[1].lit.vec_va.y_val = bplan.crd2_bp.y_gm;
     litval[1].lit.vec_va.z_val = bplan.crd2_bp.z_gm;
     litval[2].lit.vec_va.x_val = bplan.crd3_bp.x_gm;
     litval[2].lit.vec_va.y_val = bplan.crd3_bp.y_gm;
     litval[2].lit.vec_va.z_val = bplan.crd3_bp.z_gm;
     litval[3].lit.vec_va.x_val = bplan.crd4_bp.x_gm;
     litval[3].lit.vec_va.y_val = bplan.crd4_bp.y_gm;
     litval[3].lit.vec_va.z_val = bplan.crd4_bp.z_gm;
/*
***Skriv parametervärden till motsvarande MBS-variabler.
*/
     evwval(litval, 4, proc_pv);
/*
***Slut.
*/
     return(0);
  }

/********************************************************/
/*!******************************************************/

        short evgprt()

/*      Evaluerar proceduren GETPRT.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      9/10/86  Returnerar 0 om array out of bounds,
 *               J. Kjellander
 *      23/10/86 Returnera bara medlemmar som finns.
 *               J. Kjellander
 *      24/10/86 Alla instanser, J. Kjellander
 *      14/11/86 Bug, kordinatsystem, J. Kjellander
 *      2/1/86   goto rest, J. Kjellander
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
     short   status,i,nst;
     DBptr   la_mem;
     DBPart   part;
     DBHeader  hed;
     DBPdat  pdat;
     PMLITVA litval[6];
     PMREFVA idvek[MXINIV];

/*
***Hämta part från GM.
*/
     if ( (status=EXgtpt(&proc_pv[1].par_va.lit.ref_va[0], &part)) < 0 )
       return(status);
/*
***Hämta part-data.
*/
     DBread_part_parameters(&pdat,NULL,NULL,part.dtp_pt,part.dts_pt);
/*
***Returnera parameter 7 först, medlemmar i parten.
***Returnera bara medlemmar som finns.
*/
     for ( i=0, nst=0; i<part.its_pt; ++i )
        {
        la_mem = gmrdid(part.itp_pt,i);
        if ( la_mem > 0 )
          {
/*
***Gå igenom alla instanser.
*/
          do
            {
            DBget_id(la_mem,idvek);
/*
***Skriv till MBS-variabeln. Om den ej dimensionerats
***stor nog att rymma alla medlemmar, avsluta utan fel.
*/
            if ( (status=evwrve(idvek,nst,8,proc_pv)) < 0 )
              {
              if ( erlerr() == 341 )
                {
                erinit();
                goto rest;
                }
              }
            ++nst;
            DBread_header(&hed,la_mem);
            la_mem = hed.n_ptr;
            } while ( la_mem != DBNULL );
          }
        }
/*
***Kopiera parametrar 1 - 6 till PMLITVA.
*/
rest:
     strcpy(litval[0].lit.str_va,part.name_pt);
     litval[1].lit.int_va = pdat.mtyp_pd;
     litval[2].lit.int_va = pdat.matt_pd;
     if ( pdat.matt_pd == LOCAL )
       {
       DBget_id(pdat.csp_pd,&litval[3].lit.ref_va[0]);
       }
     else
       {
       litval[3].lit.ref_va[0].seq_val = 0;
       litval[3].lit.ref_va[0].ord_val = 1;
       litval[3].lit.ref_va[0].p_nextre = NULL;
       }
     litval[4].lit.int_va = pdat.npar_pd;
     litval[5].lit.int_va = nst;
/*
***Skriv parametervärden 1-6 till motsvarande MBS-variabler.
*/
     evwval(litval, 6, proc_pv);
/*
***Slut.
*/
     return(0);
  }

/********************************************************/
/*!******************************************************/

        short evpdat()

/*      Evaluerar funktionen PUTDAT_GM.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 1999-01-20 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *      2004-09-24 VECTOR+REF, J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
   short     status;
   DBint     i,count,dekldim,base_size,dattyp,arradr;
   char     *datpek;
   char      key[V3STRLEN+1],errbuf[V3STRLEN];
   DBint    *intpek;
   DBfloat  *fltpek;
   DBVector *vecpek;
   STTYTBL   typtbl;
   STARRTY   arrtyp;
   PMLITVA   val;
/*
***Nyckel.
*/
   strcpy(key,func_pv[1].par_va.lit.str_va);
   if ( strlen(key) == 0 ) return(erpush("IN5542",""));
/*
***Hur många element skall lagras ?
*/
   count = func_pv[2].par_va.lit.int_va;
/*
***Kolla typ och dimension på MBS-arrayen.
*/
   arradr = func_pv[3].par_va.lit.adr_va;
   strtyp(func_pv[3].par_ty,&typtbl);
   if ( typtbl.arr_ty == (pm_ptr)NULL )
     return(erpush("IN5522","no array"));

   strarr(typtbl.arr_ty,&arrtyp);
   dekldim = arrtyp.up_arr - arrtyp.low_arr + 1;
   if ( dekldim < count )
     return(erpush("IN5522","size too small"));

   strtyp(arrtyp.base_arr,&typtbl);
   if ( typtbl.arr_ty != (pm_ptr)NULL )
     return(erpush("IN5522","not 1-dim array"));

   dattyp = typtbl.kind_ty;

   switch ( dattyp )
     {
     case ST_INT:   dattyp = C_INT_VA; break;
     case ST_FLOAT: dattyp = C_FLO_VA; break;
     case ST_STR:   dattyp = C_STR_VA; break;
     case ST_VEC:   dattyp = C_VEC_VA; break;
     case ST_REF:   dattyp = C_REF_VA; break;
     default: return(erpush("IN5522","wrong type"));
     }
/*
***Hur stor plats på RTS tar varje element ?
*/
   base_size = typtbl.size_ty;
/*
***Allokera minne för data.
*/
   if ( (datpek=(char *)v3mall(count*base_size,"evpdat")) == NULL )
     {
     sprintf(errbuf,"%d",(int)(count));
     return(erpush("IN5532",errbuf));
     }
/*
***Kopiera från RTS till allokerad area.
*/
   switch ( dattyp )
     {
     case C_INT_VA:
     intpek = (DBint *)datpek;
     for ( i=0; i<count; ++i )
       {
       ingval(arradr+i*base_size,arrtyp.base_arr,FALSE,&val);
      *(intpek+i) = val.lit.int_va;
       }
     break;

     case C_FLO_VA:
     fltpek = (DBfloat *)datpek;
     for ( i=0; i<count; ++i )
       {
       ingval(arradr+i*base_size,arrtyp.base_arr,FALSE,&val);
      *(fltpek+i) = val.lit.float_va;
       }
     break;

     case C_STR_VA:
     for ( i=0; i<count; ++i )
       {
       ingval(arradr+i*base_size,arrtyp.base_arr,FALSE,&val);
       strcpy(datpek+i*base_size,val.lit.str_va);
       }
     break;

     case C_VEC_VA:
     vecpek = (DBVector *)datpek;
     for ( i=0; i<count; ++i )
       {
       ingval(arradr+i*base_size,arrtyp.base_arr,FALSE,&val);
       V3MOME(&val.lit.vec_va,vecpek+i,sizeof(DBVector));
       }
     break;

     case C_REF_VA:
     for ( i=0; i<count; ++i )
       {
       ingval(arradr+i*base_size,arrtyp.base_arr,FALSE,&val);
       V3MOME(val.lit.ref_va,datpek+i*base_size,base_size);    }
     break;
     }
/*
***Anropa exe-rutinen.
*/
   status = EXpdat(key,dattyp,base_size,count,datpek);
/*
***Lämna tillbaks allokerat minne.
*/
   v3free(datpek,"evpdat");
/*
***Returnera funktionsvärde.
*/
   func_vp->lit.int_va = status;
/*
***Slut. Status -1 är inget fel.
*/
    if ( status > -2 ) status = 0;
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short evgdat()

/*      Evaluerar funktionen GETDAT_GM.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 1999-01-20 J. Kjellander
 *
 *      1999-03-01 Bug returvärde, J.Kjellander
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *      2004-09-24 VECTOR+REF, J.Kjellander, Örebro university
 *
 ******************************************************!*/

  {
   short     status;
   DBint     index[1];
   DBint     i,j,count,base_size,vartyp,dattyp,dekldim,arradr;
   char     *datpek;
   char      key[V3STRLEN+1];
   DBint    *intpek;
   DBfloat  *fltpek;
   DBVector *vecpek;
   STTYTBL   typtbl;
   STARRTY   arrtyp;
   PMLITVA   litval;

/*
***Nyckel.
*/
   strcpy(key,func_pv[1].par_va.lit.str_va);
   if ( strlen(key) == 0 ) return(erpush("IN5542",""));
/*
***Hämta data från GM.
*/
   status = EXgdat(key,&dattyp,&base_size,&count,&datpek);
   if ( status == -1 )
     {
     func_vp->lit.int_va = status;
     return(0);
    }
   else if ( status < 0 ) return(status);
/*
***Kolla typ och dimension på MBS-arrayen.
*/
   arradr = func_pv[2].par_va.lit.adr_va;
   strtyp(func_pv[2].par_ty,&typtbl);
   if ( typtbl.arr_ty == (pm_ptr)NULL )
     { 
     status = erpush("IN5552","no array");
     goto exit;
     }

   strarr(typtbl.arr_ty,&arrtyp);
   dekldim = arrtyp.up_arr - arrtyp.low_arr + 1;
   if ( dekldim < count )
     {
     status = erpush("IN5552","size too small");
     goto exit;
     }

   strtyp(arrtyp.base_arr,&typtbl);
   if ( typtbl.arr_ty != (pm_ptr)NULL )
     {
     status = erpush("IN5552","not 1-dim array");
     goto exit;
     }

   vartyp = typtbl.kind_ty;

   switch ( vartyp )
     {
     case ST_INT:   vartyp = C_INT_VA; break;
     case ST_FLOAT: vartyp = C_FLO_VA; break;
     case ST_STR:   vartyp = C_STR_VA; break;
     case ST_VEC:   vartyp = C_VEC_VA; break;
     case ST_REF:   vartyp = C_REF_VA; break;
     default:
       {
       status = erpush("IN5552","wrong type");
       goto exit;
       }
     }

   if ( dattyp != vartyp )
      {
      status = erpush("IN5552","wrong type");
      goto exit;
      }
/*
***Skriv data till RTS.
*/
   switch ( dattyp )
     {
     case C_INT_VA:
     intpek = (DBint *)datpek;
     for ( i=0; i<count; ++i )
       {
       index[0] = i+1;
       litval.lit.int_va = *(intpek+i);
       status = inwvar(func_pv[2].par_ty,func_pv[2].par_va.lit.adr_va,1,index,&litval);
       }
     break;

     case C_FLO_VA:
     fltpek = (DBfloat *)datpek;
     for ( i=0; i<count; ++i )
       {
       index[0] = i+1;
       litval.lit.float_va = *(fltpek+i);
       status = inwvar(func_pv[2].par_ty,func_pv[2].par_va.lit.adr_va,1,index,&litval);
       }
     break;

     case C_STR_VA:
     for ( i=0; i<count; ++i )
       {
       index[0] = i+1;
       strcpy(litval.lit.str_va,datpek+i*base_size);
       status = inwvar(func_pv[2].par_ty,func_pv[2].par_va.lit.adr_va,1,index,&litval);
       }
     break;

     case C_VEC_VA:
     vecpek = (DBVector *)datpek;
     for ( i=0; i<count; ++i )
       {
       index[0] = i+1;
       V3MOME(vecpek+i,&litval.lit.vec_va,base_size);
       status = inwvar(func_pv[2].par_ty,func_pv[2].par_va.lit.adr_va,1,index,&litval);
       }
     break;

     case C_REF_VA:
     for ( i=0; i<count; ++i )
       {
       index[0] = i+1;
       V3MOME(datpek+i*base_size,litval.lit.ref_va,base_size);
       j = 0;
       while ( litval.lit.ref_va[j].p_nextre > (DBId *)NULL )
         {
         litval.lit.ref_va[j].p_nextre = &litval.lit.ref_va[j+1];
         j++;
         }
       status = inwvar(func_pv[2].par_ty,func_pv[2].par_va.lit.adr_va,1,index,&litval);
       }
     break;
     }
/*
***Lämna tillbaks allokerat minne.
*/
exit:
   v3free(datpek,"evgdat");
/*
***Returnera funktionsvärde.
*/
   func_vp->lit.int_va = count;
/*
***Slut.
*/
   return(status);
  }

/********************************************************/
/*!******************************************************/

        short evddat()

/*      Evaluerar funktionen DELDAT_GM.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 1999-01-20 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   short status;

/*
***Ta bort.
*/
   status = EXddat(func_pv[1].par_va.lit.str_va);
/*
***Returnera funktionsvärde.
*/
   func_vp->lit.int_va = status;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short evdel()

/*      Evaluerar proceduren DEL.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 14/3/86 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    return( EXdel( &proc_pv[1].par_va.lit.ref_va[0] ) );

  }

/********************************************************/


/**********************************************************************
*
*    evview.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    This file includes the following routines:
*
*    evgtvi();     Evaluerar GET_VIEW
*    evcrvi();     Evaluerar CRE_VIEW
*    evacvi();     Evaluerar ACT_VIEW
*    evscvi();     Evaluerar SCL_VIEW
*    evcevi();     Evaluerar CEN_VIEW
*    evhdvi();     Evaluerar HIDE_VIEW
*    evprvi();     Evaluerar PERP_VIEW
*    evervi();     Evaluerar ERASE_VIEW
*    evrpvi();     Evaluerar REP_VIEW
*    evcavi();     Evaluerar CACC_VIEW
*    evmsiz();     Evaluerar MSIZE_VIEW
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
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../WP/include/WP.h"
#include "../../EX/include/EX.h"

extern PMPARVA *proc_pv;  /* inproc.c *pv      Access structure for MBS routines */
extern short    proc_pc;  /* inproc.c parcount Number of actual parameters */

extern PMLITVA *func_vp;   /* Pekare till resultat. */

/*!******************************************************/

        short evgtvi()

/*      Evaluerar proceduren GET_VIEW.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      Felkod:
 *
 *      (C)microform ab 20/1-95 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   short   status;
   DBint   i,j,index[2];
   char    vynamn[V3STRLEN+1];
   double  skala,xmin,ymin,xmax,ymax,persp;
   DBint   win_id;
   DBTmat  vymat;
   PMLITVA litval[16];

   win_id = proc_pv[1].par_va.lit.int_va;

#ifdef V3_X11
   WPgtvi(win_id,vynamn,&skala,&xmin,&ymin,&xmax,&ymax,&vymat,&persp);
#else
#ifdef WIN32
   msgtvi(win_id,vynamn,&skala,&xmin,&ymin,&xmax,&ymax,&vymat,&persp);
#else
   return(-2);
#endif
#endif
/*
***Vynamn,skala,modellfönster.
*/
   strcpy(litval[0].lit.str_va,vynamn);
   litval[1].lit.float_va = skala;
   litval[2].lit.float_va = xmin;
   litval[3].lit.float_va = ymin;
   litval[4].lit.float_va = xmax;
   litval[5].lit.float_va = ymax;

   evwval(litval,6,proc_pv);
/*
***Transformationsmatris.
*/
   litval[0].lit.float_va  = vymat.g11;
   litval[1].lit.float_va  = vymat.g12;
   litval[2].lit.float_va  = vymat.g13;
   litval[3].lit.float_va  = vymat.g14;

   litval[4].lit.float_va  = vymat.g21;
   litval[5].lit.float_va  = vymat.g22;
   litval[6].lit.float_va  = vymat.g23;
   litval[7].lit.float_va  = vymat.g24;

   litval[8].lit.float_va  = vymat.g31;
   litval[9].lit.float_va  = vymat.g32;
   litval[10].lit.float_va = vymat.g33;
   litval[11].lit.float_va = vymat.g34;

   litval[12].lit.float_va = vymat.g41;
   litval[13].lit.float_va = vymat.g42;
   litval[14].lit.float_va = vymat.g43;
   litval[15].lit.float_va = vymat.g44;

   for ( i=0; i<4; ++i )
     {
     index[0] = i+1;
     for ( j=0; j<4; ++j )
       {
       index[1] = j+1;
       status = inwvar(proc_pv[8].par_ty, proc_pv[8].par_va.lit.adr_va,
                                          2, index, &litval[4*i+j]);
       }
     }
/*
***Perspektiv.
*/
   litval[0].lit.float_va = persp;
   inwvar(proc_pv[9].par_ty,proc_pv[9].par_va.lit.adr_va,
	                        0,NULL,&litval[0]);
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short evcrvi()

/*      Evaluerar proceduren CRE_VIEW.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      Felkoder: IN5062 = Fel typ av indata.
 *
 *      (C)microform ab 30/6/86 R. Svedin
 *
 *      10/10/86 Tagit bort param. för vyfönster, R. Svedin
 *      29/9/92  Matris-vyer, J. Kjellander
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    VYVEC    bpos;

/*
***Betraktelseposition (VECTOR) eller koordinatsystem (REF).
*/
    switch ( proc_pv[2].par_va.lit_type )
      {
      case C_VEC_VA:
/*
***Betraktelseposition.
*/
      bpos.x_vy = proc_pv[2].par_va.lit.vec_va.x_val;
      bpos.y_vy = proc_pv[2].par_va.lit.vec_va.y_val;
      bpos.z_vy = proc_pv[2].par_va.lit.vec_va.z_val;
      return(EXcrvp(proc_pv[1].par_va.lit.str_va,&bpos));
      break;
/*
***Koordinatsystem.
*/
      case C_REF_VA:
      return(EXcrvc( proc_pv[1].par_va.lit.str_va,
		            &proc_pv[2].par_va.lit.ref_va[0]));
/*
***Felaktig typ av indata.
*/
      default:
      return(erpush("IN5062",""));
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evacvi()

/*      Evaluerar proceduren ACT_VIEW.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 1/8/86 R. Svedin
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXacvi(proc_pv[1].par_va.lit.str_va,
                  proc_pv[2].par_va.lit.int_va));
  }

/********************************************************/
/*!******************************************************/

        short evscvi()

/*      Evaluerar proceduren SCL_VIEW.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 1/8/86 R. Svedin
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXscvi(proc_pv[1].par_va.lit.str_va,
                  proc_pv[2].par_va.lit.float_va));
  }

/********************************************************/
/*!******************************************************/

        short evcevi()

/*      Evaluerar proceduren CEN_VIEW.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 4/8/86 R. Svedin
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

   {
    return(EXcevi(proc_pv[1].par_va.lit.str_va,
                  proc_pv[2].par_va.lit.vec_va.x_val,
                  proc_pv[2].par_va.lit.vec_va.y_val));
  }

/********************************************************/
/*!******************************************************/

        short evhdvi()

/*      Evaluerar proceduren HIDE_VIEW.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *          extern proc_pc => 
 *
 *      Ut: Inget.
 *
 *      FV:      0 = OK.
 *          AVBRYT = <CTRL>c i WPhide().
 *
 *      Felkoder: IN2262 = Filnamn saknas.
 *                IN2272 = Felaktig kod.
 *                IN2282 = Kan ej skapa fil %s.
 *
 *      (C)microform ab 21/3/89 J. Kjellander
 *
 *      29/5/91 Placering, J. Kjellander
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

   {
    short  code,status;
    DBVector *origo;
    FILE  *fp;

/*
***Skärm, fil eller båda ?
*/
    code = (short)proc_pv[2].par_va.lit.int_va;
/*
***Nollpunkt. Om ingen getts, ta skärmens nedre vänstra hörn.
*/
    if ( proc_pc == 4 ) origo = (DBVector * )&proc_pv[4].par_va.lit.vec_va;
    else origo = NULL;

    if ( code == 1 )
      return(EXhdvi(proc_pv[1].par_va.lit.str_va,TRUE,FALSE,NULL,NULL));

    else if ( proc_pc < 3 )
      return(erpush("IN2262",""));

    else
      {
      if ( (fp=fopen(proc_pv[3].par_va.lit.str_va,"w+")) == NULL )
        return(erpush("IN2282",proc_pv[3].par_va.lit.str_va));
      else if ( code == 2 )
        status = EXhdvi(proc_pv[1].par_va.lit.str_va,FALSE,TRUE,fp,origo);
      else if ( code == 3 )
        status = EXhdvi(proc_pv[1].par_va.lit.str_va,TRUE,TRUE,fp,origo);
      else
        status = erpush("IN2272","");
      fclose(fp);
      }

    return(status);
  }

/********************************************************/
/*!******************************************************/

        short evprvi()

/*      Evaluerar proceduren PERP_VIEW.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 21/3/89 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

   {
    return(EXprvi(proc_pv[1].par_va.lit.str_va,
                  proc_pv[2].par_va.lit.float_va));
  }

/********************************************************/
/*!******************************************************/

        short evervi()

/*      Evaluerar proceduren ERASE_VIEW.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 30/6/86 R. Svedin
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXervi(proc_pv[1].par_va.lit.int_va));
  }

/********************************************************/
/*!******************************************************/

        short evrpvi()

/*      Evaluerar proceduren REP_VIEW.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *          extern proc_pc   => Antal parametrar 2, 1 eller 0.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 30/6/86 R. Svedin
 *
 *      19/10/93 Autozoom, J. Kjellander
 *      27/12/94 Multifönster, J. Kjellander
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    DBint azoom,win_id;

    if ( proc_pc == 0 )
      {
      azoom = 0;
      win_id = GWIN_MAIN;
      }
    else if ( proc_pc == 1 )
      {
      azoom = proc_pv[1].par_va.lit.int_va;
      win_id = GWIN_MAIN;
      }
    else
      {
      azoom = proc_pv[1].par_va.lit.int_va;
      win_id = proc_pv[2].par_va.lit.int_va;
      }

    return(EXrpvi(azoom,win_id));
  }

/********************************************************/
/*!******************************************************/

        short evcavi()

/*      Evaluerar proceduren CACC_VIEW.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 4/8/86 R. Svedin
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXcavi(proc_pv[1].par_va.lit.float_va));
  }

/********************************************************/
/*!******************************************************/

        short evmsiz()

/*      Evaluerar proceduren MSIZE_VIEW.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 8/8/93
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *      2006-12-28 Removed GP, J.Kjellander
 *
 ******************************************************!*/

  {
   short   status;
   VY      modvy;
   PMLITVA litval;
   WPWIN  *winptr;

/*
***Get a ptr to the main GWIN.
*/
    winptr = WPwgwp((wpw_id)GWIN_MAIN);
/*
***Calculate bounding rectangle of projected model.
*/
    WPmsiz(winptr->ptr,&modvy);
/*
***If there is no model or it has no size, return zeros.
*/
    if ( (modvy.vywin[2] < modvy.vywin[0]) &&
         (modvy.vywin[3] < modvy.vywin[1]) )
      {
      modvy.vywin[0] = modvy.vywin[1] =
      modvy.vywin[2] = modvy.vywin[3] = 0.0;
      }
/*
***Else return 2D view limits in p1 and p2.
*/    
    litval.lit.vec_va.x_val = modvy.vywin[0];
    litval.lit.vec_va.y_val = modvy.vywin[1];
    inwvar(proc_pv[1].par_ty,
		   proc_pv[1].par_va.lit.adr_va,0,NULL,&litval);
    litval.lit.vec_va.x_val = modvy.vywin[2];
    litval.lit.vec_va.y_val = modvy.vywin[3];
    inwvar(proc_pv[2].par_ty,
		   proc_pv[2].par_va.lit.adr_va,0,NULL,&litval);

    return(0);
  }

/********************************************************/

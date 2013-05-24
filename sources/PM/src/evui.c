/**********************************************************************
*
*    evui.c
*    ======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes the following routines:
*
*    evlsin();     evaluate LST_INI
*    evlsex();     evaluate LST_EXI
*    evlsln();     evaluate LST_LIN
*    evpspt();     evaluate PSH_PMT
*    evpopt();     evaluate POP_PMT
*    evcrts();     evaluate CRE_TSTR
*    evcrmy();     evaluate CRE_MEN
*    evpsmy();     evaluate PSH_MEN
*    evpomy();     evaluate POP_MEN
*    evgtmy();     evaluate GET_MEN
*
*    evgtts();     evaluate GET_TSTR
*    evgtal();     evaluate GET_ALT
*    evinpt();     evaluate INPMT
*    evinfn();     evaluate INFNAME
*
*    evgptr();    Evaluerar GET_POINTER
*    evscr();     Evaluerar SCREEN
*    evscr2();    Evaluerar SCREEN_2
*    evpos();     Evaluerar POS
*    evpos2();    Evaluerar POS_2
*    evidnt();    Evaluerar IDENT
*    evidn2();    Evaluerar IDENT_2
*    evhigh();    Evaluerar HIGHLIGHT
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
#include "../../EX/include/EX.h"
#include "../../GE/include/GE.h"
/*#include "../../GP/include/GP.h"*/

#ifdef UNIX
#include <X11/Xlib.h>
#include "../../WP/include/WP.h"
#endif

#include <ctype.h>

#ifdef WIN32
#include "../../WP/include/WP.h"
#endif

#ifdef WIN32
extern int msgtsw(),msgtmp();
#endif

extern PMPARVA *proc_pv;  /* inproc.c *pv      Access structure for MBS routines */
extern short    proc_pc;  /* inproc.c parcount Number of actual parameters */

extern PMPARVA *func_pv;   /* Access structure for MBS routines */
extern short    func_pc;   /* Number of actual parameters */
extern PMLITVA *func_vp;   /* Pekare till resultat. */

/*!******************************************************/

        short evlsin()

/*      Evaluerar proceduren LST_INI.
 *
 *      In: extern proc_pv => Pekare till array med parameterv�rden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 1/10/86 R. Svedin
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXlsin(&proc_pv[1].par_va.lit.str_va[0]));
  }

/********************************************************/
/*!******************************************************/

        short evlsex()

/*      Evaluerar proceduren LST_EXI.
 *
 *      In: extern proc_pv => Pekare till array med parameterv�rden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 1/10/86 R. Svedin
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXlsex());
  }

/********************************************************/
/*!******************************************************/

        short evlsln()

/*      Evaluerar proceduren LST_LIN.
 *
 *      In: extern proc_pv => Pekare till array med parameterv�rden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 1/10/86 R. Svedin
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXlsln(proc_pv[1].par_va.lit.str_va));
  }

/********************************************************/
/*!******************************************************/

        short evpspt()

/*      Evaluates procedure PSH_PMT().
 *
 *      In: extern proc_pv => Ptr to parameter value
 *
 *      (C)microform ab 13/4/87 R. Svedin
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXpspt(proc_pv[1].par_va.lit.str_va));
  }

/********************************************************/
/*!******************************************************/

        short evpopt()

/*      Evaluates procedure POP_PMT().
 *
 *      (C)microform ab 10/4/87 R. Svedin
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXpopt());
  }

/********************************************************/
/*!******************************************************/

        short evcrts()

/*      Evaluerar proceduren CRE_TSTR.
 *
 *      In: extern proc_pv => Pekare till array med parameterv�rden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 13/4/87 R. Svedin
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXcrts((short)proc_pv[1].par_va.lit.int_va,
                         proc_pv[2].par_va.lit.str_va));
  }

/********************************************************/
/*!******************************************************/

        short evcrmy()

/*      Evaluerar proceduren CRE_MEN.
 *
 *      In: extern proc_pv => Pekare till array med parameterv�rden
 *          extern proc_pc => Antal parametrar.
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 13/4/87 R. Svedin
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXcrmy(proc_pv,proc_pc));
  }

/********************************************************/
/*!******************************************************/

        short evpsmy()

/*      Evaluerar proceduren PSH_MEN.
 *
 *      In: extern proc_pv => Pekare till array med parameterv�rden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 13/4/87 R. Svedin
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXpsmy((short)proc_pv[1].par_va.lit.int_va));
  }

/********************************************************/
/*!******************************************************/

        short evpomy()

/*      Evaluates procedure POP_MEN().
 *
 *      In: extern proc_pv => Ptr to parameter value
 *
 *      (C)microform ab 10/4/87 R. Svedin
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXpomy());
  }

/********************************************************/
/*!******************************************************/

        short evgtmy()

/*      Evaluerar proceduren GET_MEN.
 *
 *      In: extern proc_pv => Pekare till array med parameterv�rden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 13/4/87 R. Svedin
 *
 *      24/4/87  Felhantering, R. Svedin
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    short    status;
    PMLITVA  str;

    if ( (status=EXgtmy((short)proc_pv[1].par_va.lit.int_va,
                 str.lit.str_va)) < 0 ) return(status);

/*
***Returnera parameter 1 = rubriktext.
*/
    evwval(&str, 1, proc_pv);

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evgtts()

/*      Evaluerar funktionen GET_TSTR.
 *
 *      In: Global func_pv  => Pekare till array med parameterv�rden.
 *          Global func_vp  => Pekare till resultat.
 *
 *      Ut: Global*func_vp  = Referens.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 13/4/87  R. Svedin.
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    return(EXgtts((short)func_pv[1].par_va.lit.int_va,func_vp));

  }

/********************************************************/
/*!******************************************************/

        short evgtal()

/*      Evaluerar funktionen GET_ALT.
 *
 *      In: Global func_pv  => Pekare till array med parameterv�rden.
 *          Global func_vp  => Pekare till resultat.
 *
 *      Ut: Global*func_vp  = Referens.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 13/4/87  R. Svedin.
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    return(EXgtal(func_pv[1].par_va.lit.str_va,func_vp));
  }

/********************************************************/
/*!******************************************************/

        short evinpt()

/*      Evaluerar funktionen INPMT.
 *
 *      In: Global func_pv  => Pekare till array med parameterv�rden.
 *          Global func_vp  => Pekare till resultat.
 *
 *      Ut: Global*func_vp  = Referens.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 14/10/88 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXinpt(func_pv[1].par_va.lit.str_va,
                  func_pv[2].par_va.lit.str_va,
           (short)func_pv[3].par_va.lit.int_va,
                  func_vp->lit.str_va));

  }

/********************************************************/
/*!******************************************************/

        short evinfn()

/*      Evaluerar funktionen INFNAME.
 *
 *      In: Global func_pv  => Pekare till array med parameterv�rden.
 *          Global func_vp  => Pekare till resultat.
 *
 *      Ut: Global*func_vp  = Referens.
 *
 *      FV: Returnerar ex-rutinens status.
 *
 *      (C)microform ab 1988-04-01 J.Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    return(EXinfn(func_pv[1].par_va.lit.str_va,
                  func_pv[2].par_va.lit.str_va,
                  func_pv[3].par_va.lit.str_va,
                  func_pv[4].par_va.lit.str_va,
                  func_vp->lit.str_va));
  }

/********************************************************/
/*!******************************************************/

        short evgptr()

/*      Evaluerar funktionen GET_POINTER.
 *
 *      In: Global  func_pv  => Parameter value array
 *
 *      Ut: Global *func_vp  => Pointer to result value.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1998-03-23 J. Kjellander
 *
 *      2001-03-05 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   int     intx,inty,main_x,main_y,gra_x,gra_y;
   WPGWIN *gwinpt;
/*
***Get current mouse position in screen coordinates.
*/
#ifdef UNIX
   WPgtmp(&intx,&inty);
#endif
#ifdef WIN32
   int dum1,dum2;
   msgtmp(&intx,&inty);
#endif

/*
***Adjust according to code.
*/
   switch ( func_pv[1].par_va.lit.int_va )
     {
/*
***Relative to screen.
*/
     case 0:
     func_vp->lit.vec_va.x_val = (DBfloat)intx;
     func_vp->lit.vec_va.y_val = (DBfloat)inty;
     func_vp->lit.vec_va.z_val = 0.0;
     return(0);
/*
***Relative to application main window ?
*/
     case 1:
#ifdef UNIX
     gwinpt = (WPGWIN *)wpwtab[GWIN_MAIN].ptr;
     WPgtwp(gwinpt->id.x_id,&main_x,&main_y);
#endif
#ifdef WIN32
     msggeo(ms_main,&main_x,&main_y,&dum1,&dum2,NULL,NULL);
#endif
     func_vp->lit.vec_va.x_val = (DBfloat)(intx - main_x);
     func_vp->lit.vec_va.y_val = (DBfloat)(inty - main_y);
     func_vp->lit.vec_va.z_val = 0.0;
     return(0);
/*
***Relative to graphics window 0 ?
*/
     case 2:
     gwinpt = (WPGWIN *)wpwtab[GWIN_MAIN].ptr;
#ifdef UNIX
     WPgtwp(gwinpt->id.x_id,&gra_x,&gra_y);
     func_vp->lit.vec_va.x_val = (DBfloat)(intx - gra_x);
     func_vp->lit.vec_va.y_val = (DBfloat)(inty - gra_y);
     func_vp->lit.vec_va.z_val = 0.0;
#endif
#ifdef WIN32
     msggeo(ms_main,&main_x,&main_y,&dum1,&dum2,NULL,NULL);
     gra_x = main_x + GetSystemMetrics(SM_CXFRAME);
     gra_y = main_y + GetSystemMetrics(SM_CYFRAME) +
                      GetSystemMetrics(SM_CYMENU) + 
                      GetSystemMetrics(SM_CYCAPTION);
     func_vp->lit.vec_va.x_val = (DBfloat)(intx - gra_x - gwinpt->geo.x);
     func_vp->lit.vec_va.y_val = (DBfloat)(inty - gra_y - gwinpt->geo.y);
     func_vp->lit.vec_va.z_val = 0.0;
#endif
     return(0);
/*
***Model coordinates ?
*/
     case 3:
     gwinpt = (WPGWIN *)wpwtab[GWIN_MAIN].ptr;
#ifdef UNIX
     WPgtwp(gwinpt->id.x_id,&gra_x,&gra_y);
     intx = intx - gra_x;
     inty = gwinpt->geo.dy - (inty - gra_y);
#endif
#ifdef WIN32
     msggeo(ms_main,&main_x,&main_y,&dum1,&dum2,NULL,NULL);
     gra_x = main_x + 2*GetSystemMetrics(SM_CXFRAME);
     gra_y = main_y + 2*GetSystemMetrics(SM_CYFRAME) +
                        GetSystemMetrics(SM_CYMENU) +
                      2*GetSystemMetrics(SM_CYCAPTION);
     intx = intx - gra_x - gwinpt->geo.x;
     inty = gwinpt->geo.dy - (inty - gra_y - gwinpt->geo.y);
#endif
     func_vp->lit.vec_va.x_val = gwinpt->vy.modwin.xmin +
                      (intx - gwinpt->vy.scrwin.xmin)*
                      (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin)/
                      (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
     func_vp->lit.vec_va.y_val = gwinpt->vy.modwin.ymin +
                      (inty - gwinpt->vy.scrwin.ymin)*
                      (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin)/
                      (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);
     func_vp->lit.vec_va.z_val = 0.0;
     return(0);
/*
***Unknown code.
*/
     default: return(erpush("IN5562",""));
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evscr()

/*      Evaluerar funktionen SCREEN.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *          Global  func_pc  => Number of parameters 
 *
 *      Ut: Global *func_vp  => Pointer to result value.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 18/5/92 J. Kjellander
 *
 *      2001-03-05 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   short   intx; 
   short   inty;
   short   status;
   DBint   win_id;
   PMLITVA litval;

   if ( (status=EXscr(&intx,&inty,&win_id)) != 0 ) return(status);

   func_vp->lit.vec_va.x_val = (v2float)intx;
   func_vp->lit.vec_va.y_val = (v2float)inty;
   func_vp->lit.vec_va.z_val = 0.0;

   if ( func_pc > 0 )
     {
     litval.lit.int_va = win_id;
     inwvar(func_pv[1].par_ty,func_pv[1].par_va.lit.adr_va,0,NULL,&litval);
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evscr2()

/*      Evaluates function SCREEN_2().
 *
 *      In:   extern proc_pv => Pekare till array med parameterv�rden
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 1998-03-23 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *      2007-08-01 1.19, J.Kjellander
 *
 ******************************************************!*/

 {
   short   status;
   int     ix1,iy1,ix2,iy2,rubmod;
   wpw_id  grw_id;
   PMLITVA litval;

/*
***H�mta in 2 koordinater.
*/
   rubmod = proc_pv[4].par_va.lit.int_va;
   if ( rubmod < 0 ) rubmod = 0;
   if ( rubmod > 3 ) rubmod = 3;

   status = WPgtsw(&grw_id,&ix1,&iy1,&ix2,&iy2,rubmod,FALSE);
/*
***REJECT eller GOMAIN skall returnera p1 = p2.
*/
   if ( status < 0 )
     {
     ix1 = ix2 = iy1 = iy2 = 0;
     }
/*
***Skriv resultatet till MBS-variabler.
*/
   litval.lit.int_va = grw_id;
   inwvar(proc_pv[1].par_ty,
              proc_pv[1].par_va.lit.adr_va,0,NULL,&litval);

   litval.lit.vec_va.x_val = (v2float)ix1;
   litval.lit.vec_va.y_val = (v2float)iy1;
   litval.lit.vec_va.z_val = 0.0;
   inwvar(proc_pv[2].par_ty,
              proc_pv[2].par_va.lit.adr_va,0,NULL,&litval);

   litval.lit.vec_va.x_val = (v2float)ix2;
   litval.lit.vec_va.y_val = (v2float)iy2;
   litval.lit.vec_va.z_val = 0.0;
   inwvar(proc_pv[3].par_ty,
              proc_pv[3].par_va.lit.adr_va,0,NULL,&litval);
/*
***The end.
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evpos()

/*      Evaluerar funktionen POS.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *          Global  func_pc  => Number of parameters 
 *
 *      Ut: Global *func_vp  => Pointer to result value.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 18/5/92 J. Kjellander
 *
 *      2001-03-05 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
     char   pektkn;
     double fx,fy;
     short   status;
     PMLITVA litva;

/*
***L�s position och pektecken.
*/
    if ( (status=EXpos(&fx,&fy,&pektkn) ) != 0 ) return(status);
/*
***Kopiera pektecknet till PMLITVA.
*/
    litva.lit.str_va[0] = pektkn;
    litva.lit.str_va[1] = '\0';
/*
***Skriv parameterv�rdet till motsvarande MBS-variabel.
*/
    if ( func_pc == 1 )
      inwvar(func_pv[1].par_ty,func_pv[1].par_va.lit.adr_va,0,NULL,&litva);
/*
***Returnera positionen som funktionsv�rde.
*/
    func_vp->lit.vec_va.x_val = (v2float)fx;
    func_vp->lit.vec_va.y_val = (v2float)fy;
    func_vp->lit.vec_va.z_val = 0.0;
/*
***Slut.
*/
     return(0);
  }

/********************************************************/
/*!******************************************************/

        short evpos2()

/*      Evaluates function POS_2().
 *
 *      In: extern proc_pv => Pekare till array med parameterv�rden
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 1998-03-23 J. Kjellander
 *
 *      1998-04-23 REJECT och GOMAIN, J.Kjellander
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   char    errbuf[V3STRLEN];
   short   status;
   int     ix1,iy1,ix2,iy2,rubmod;
   v2float mx1,my1,mx2,my2;
   wpw_id  grw_id;
   WPWIN  *winptr;
   WPGWIN *gwinpt;
   PMLITVA litval;

/*
***Get 2 coordinates.
*/
   rubmod = proc_pv[3].par_va.lit.int_va;
   if ( rubmod < 0 ) rubmod = 0;
   if ( rubmod > 3 ) rubmod = 3;

   status = WPgtsw(&grw_id,&ix1,&iy1,&ix2,&iy2,rubmod,FALSE);
/*
***Translate window ID to C pointer. This function only
***supports WPGWIN's.
*/
   if ( (winptr=WPwgwp((wpw_id)grw_id)) != NULL  &&
         winptr->typ == TYP_GWIN )
     {
     gwinpt = (WPGWIN *)winptr->ptr;
     }
   else
     {
     sprintf(errbuf,"%d",grw_id);
     return(erpush("WP1382",errbuf));
     }
/*
***REJECT or GOMAIN returnes p1 = p2.
*/
   if ( status < 0 )
     {
     mx1 = mx2 = my1 = my2 = 0.0;
     }
/*
***Transform to model coordinates.
*/
   else
     {
     mx1 = gwinpt->vy.modwin.xmin + (ix1 - gwinpt->vy.scrwin.xmin)*
                      (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin)/
                      (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
     my1 = gwinpt->vy.modwin.ymin + (iy1 - gwinpt->vy.scrwin.ymin)*
                      (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin)/
                      (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);

     mx2 = gwinpt->vy.modwin.xmin + (ix2 - gwinpt->vy.scrwin.xmin)*
                      (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin)/
                      (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin);
     my2 = gwinpt->vy.modwin.ymin + (iy2 - gwinpt->vy.scrwin.ymin)*
                      (gwinpt->vy.modwin.ymax - gwinpt->vy.modwin.ymin)/
                      (gwinpt->vy.scrwin.ymax - gwinpt->vy.scrwin.ymin);
     }
/*
***Write result to MBS-variables.
*/
   litval.lit.vec_va.x_val = mx1;
   litval.lit.vec_va.y_val = my1;
   litval.lit.vec_va.z_val = 0.0;
   inwvar(proc_pv[1].par_ty,
              proc_pv[1].par_va.lit.adr_va,0,NULL,&litval);

   litval.lit.vec_va.x_val = mx2;
   litval.lit.vec_va.y_val = my2;
   litval.lit.vec_va.z_val = 0.0;
   inwvar(proc_pv[2].par_ty,
              proc_pv[2].par_va.lit.adr_va,0,NULL,&litval);
/*
***The end.
*/
   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evidnt()

/*      Evaluerar funktionen IDENT.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *          Global  func_pc  => Number of parameters 
 *
 *      Ut: Global *func_vp  => Pointer to result value.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 18/5/92 J. Kjellander
 *
 *      2001-03-05 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    DBetype typmask;
    bool    end,right;
    short   status;
    PMLITVA val1,val2;

/*
***Typmasken = parameter 1.
*/
    typmask = (DBetype)func_pv[1].par_va.lit.int_va;
/*
***Anropa exe-rutinen.
*/
    if ( (status=EXidnt(&typmask,func_vp->lit.ref_va,&end,&right) ) != 0 )
      return( status );
/*
***�nde och sida till PMLITVA.
*/
    if ( end == TRUE ) val1.lit.int_va = 1;
    else               val1.lit.int_va = 0;

    if ( right == TRUE ) val2.lit.int_va = 1;
    else                 val2.lit.int_va = 0;
/*
***Ev. skrivning till motsvarande optionella variabler.
*/
    if ( func_pc > 1 ) inwvar(func_pv[2].par_ty,func_pv[2].par_va.lit.adr_va,0,NULL,&val1);

    if ( func_pc > 2 ) inwvar(func_pv[3].par_ty,func_pv[3].par_va.lit.adr_va,0,NULL,&val2);
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evidn2()

/*      Evaluates function IDENT_2().
 *
 *      In:   extern proc_pv => Pekare till array med parameterv�rden
 *
 *      Ut: Inget.
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      Felkoder: IN5422 = F�r m�nga ID:n
 *
 *      (C)microform ab 18/5/92 J. Kjellander
 *
 *      1998-04-03 PRTTYP, J.Kjellander
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    DBetype typ,typmask,typvek[IGMAXID];
    short   nok,status,hitmod,maxref;
    int     i,idant;
    DBptr   la,lavek[IGMAXID];
    PMLITVA litval;
    PMREFVA idvek[MXINIV];

/*
***Typmask, mode och maxref.
*/
    typmask = (DBetype)proc_pv[1].par_va.lit.int_va;
    hitmod  = (short)proc_pv[2].par_va.lit.int_va;
    maxref  = (short)proc_pv[3].par_va.lit.int_va;
/*
***Se till att maxref ligger inom rimliga gr�nser.
*/
    if      ( maxref < 0 )       maxref = 0;
    else if ( maxref > IGMAXID ) maxref = IGMAXID;
/*
***H�mta ID:n med f�nster.
*/
    idant = maxref;
    if ( (typvek[0]=typmask) == PRTTYP ) typvek[0] = ALLTYP;
    status = WPgmlw(lavek,&idant,typvek,hitmod);
    if ( status == REJECT  ||  status == GOMAIN ) idant = -1;
    else if ( status < 0 ) return(status);
/*
***Returnera parameter 4, idmat. Om parter beg�rts returnerar
***vi bara partid:n.
*/
    nok = 0;
    for ( i=0; i<idant; ++i )
       {
       DBget_id(lavek[i],idvek);
       if ( typmask == PRTTYP )
         {
         idvek[0].p_nextre = NULL;
         DBget_pointer('I',idvek,&la,&typ);
         if ( typ == PRTTYP )
           {
           if ( evwrve(idvek,nok,4,proc_pv) < 0 ) return(erpush("IN5422",""));
         ++nok;
           }
         }
       else
         {
         if ( evwrve(idvek,nok,4,proc_pv) < 0 ) return(erpush("IN5422",""));
       ++nok;
         }
       }
/*
***Returnera parameter 5 antal godk�nda ID:n.
*/
    litval.lit.int_va = nok;
    inwvar(proc_pv[5].par_ty,
                   proc_pv[5].par_va.lit.adr_va,0,NULL,&litval);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evhigh()

/*      Evaluerar funktionen HIGHLIGHT.
 *
 *      In: extern pv   => Pekare till array med parameterv�rden
 *          extern pc   => Antal parametrar.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IN5432 = Storhet finns ej
 *
 *      (C)microform ab 1998-03-23 J. Kjellander
 *
 *      2006-12-30 Removed GP, J.Kjellander
 *
 ******************************************************!*/

 {
   bool    draw;
   DBptr   la;
   DBetype typ;

/*
***Rita eller sudda ?
*/
   if ( proc_pv[1].par_va.lit.int_va == 1 ) draw = TRUE;
   else                                     draw = FALSE;
/*
***If only one param. is supplied, highligt all entities.
***This code only works if draw = FALSE because there is
***is no WP-routine to highlight all entities.
*/
   if ( proc_pc == 1 )
     {
     WPerhg();
     }
/*
***If 2 params. are supplied only one entity is to be highlighted.
*/
   else
     {
     if ( DBget_pointer('I',proc_pv[2].par_va.lit.ref_va,&la,&typ) < 0 )
       return(erpush("IN5432",""));
     WPhgen(GWIN_ALL,la,draw);
     }

   return(0);
 }

/********************************************************/

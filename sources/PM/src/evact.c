/**********************************************************************
*
*    evact.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    This file includes the following routines:
*
*    evalev();     Evaluates ACT_LEV
*    evablk();     Evaluates ACT_BLANK
*    evavin();     Evaluates ACT_VNAM
*    evapen();     Evaluates ACT_PEN
*    evawdt();     Evaluates ACT_WIDTH
*    evascl();     Evaluates ACT_SCL
*    evadsc();     Evaluates ACT_DSCL
*    evacac();     Evaluates ACT_CACC
*    evagrx();     Evaluates ACT_GRIDX
*    evagry();     Evaluates ACT_GRIDY
*    evagdx();     Evaluates ACT_GRIDDX
*    evagdy();     Evaluates ACT_GRIDDY
*    evagrd();     Evaluates ACT_GRID
*    evajbn();     Evaluates ACT_JOBNAM
*    evamtp();     Evaluates ACT_MTYPE
*    evamat();     Evaluates ACT_MATTR
*    evajbd();     Evaluates ACT_JOBDIR
*    evapft();     Evaluates ACT_PFONT
*    evalft();     Evaluates ACT_LFONT
*    evaaft();     Evaluates ACT_AFONT
*    evacft();     Evaluates ACT_CFONT
*    evaxft();     Evaluates ACT_XFONT
*    evapsi();     Evaluates ACT_PDASHL
*    evaldl();     Evaluates ACT_LDASHL
*    evaadl();     Evaluates ACT_ADASHL
*    evacdl();     Evaluates ACT_CDASHL
*    evaxdl();     Evaluates ACT_XDASHL
*    evatsi();     Evaluates ACT_TSIZE
*    evatwi();     Evaluates ACT_TWIDTH
*    evatsl();     Evaluates ACT_TSLANT
*    evadts();     Evaluates ACT_DTSIZ
*    evadas();     Evaluates ACT_DASIZ
*    evadnd();     Evaluates ACT_DNDIG
*    evadau();     Evaluates ACT_DAUTO
*    evacsy();     Evaluates ACT_CSY
*    evapid();     Evaluates ACT_PID
*    evavvr();     Evaluates ACT_VARKON_VERS
*    evavsr();     Evaluates ACT_VARKON_SERIAL
*    evaost();     Evaluates ACT_OSTYPE
*    evahst();     Evaluates ACT_HOST
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

#ifdef UNIX
#include <sys/utsname.h>
#endif

extern V2NAPA defnap;
extern V3MDAT sydata;
extern char   pidnam[],jobnam[],jobdir[];
extern pm_ptr actmod;
extern DBptr  lsysla;

extern PMLITVA *func_vp;   /* Pekare till resultat. */

/*!******************************************************/

        short evalev()

/*      Evaluerar funktionen ACT_LEV.
 *
 *      In: 
 *
 *      Ut: Global *func_vp  =  Activ level.
 *
 *      FV:
 *
 *      (C)microform ab 31/5/86 R. Svedin
 *
 *      31/10/86 Nytt namn och flyttad från eval3.c, R. Svedin
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    func_vp->lit.int_va = defnap.level;

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evablk()

/*      Evaluerar funktionen ACT_BLANK.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    func_vp->lit.int_va = defnap.blank;

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evavin()

/*      Evaluates function ACT_VNAM(). Returns the
 *      name of the view currently active in the
 *      main graphics window.
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      (C)2006-12-31 J.Kjellander
 *
 ******************************************************!*/

  {
   WPWIN  *winptr;
   WPGWIN *gwinpt;

/*
***Get a ptr to GWIN_MAIN. To be changed in the future so
***that any window can be used.
*/
   winptr = WPwgwp((wpw_id)GWIN_MAIN);
   gwinpt = (WPGWIN *)winptr->ptr;

   strcpy(func_vp->lit.str_va , gwinpt->vy.name);

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short evapen()

/*      Evaluerar funktionen ACT_PEN.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    func_vp->lit.int_va = defnap.pen;

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evawdt()

/*      Evaluerar funktionen ACT_WIDTH.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    func_vp->lit.float_va = defnap.width;

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evascl()

/*      Evaluerar funktionen ACT_SCL.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *      2006-12-30 removed GP, J.Kjellander
 *
 ******************************************************!*/

  {
   double  skala;
   WPWIN  *winptr;
   WPGWIN *gwinpt;
/*
***Get a pointer to the main graphics window.
*/
   winptr = WPwgwp((wpw_id)GWIN_MAIN);
   gwinpt = (WPGWIN *)winptr->ptr;
/*
***Current scale factor in this window =
***(n_pixels * pixel_size) / model_window_size
*/
   skala = (gwinpt->vy.scrwin.xmax - gwinpt->vy.scrwin.xmin) *
            gwinpt->geo.psiz_x /
           (gwinpt->vy.modwin.xmax - gwinpt->vy.modwin.xmin);

   func_vp->lit.float_va = skala;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short evadsc()

/*      Evaluerar funktionen ACT_DSCL.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    func_vp->lit.float_va = 1.0;

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evacac()

/*      Evaluerar funktionen ACT_CACC.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *      2006-12-31 Removed GP, J.Kjellander
 *
 ******************************************************!*/

  {
    double c;

    WPget_cacc(&c);

    func_vp->lit.float_va = c;
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evagrx()

/*      Evaluerar funktionen ACT_GRIDX.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *      2007-03-04 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    DBfloat rstrox,rstroy,rstrdx,rstrdy;
    bool    rstron;

/*
***Get current grid.
*/
    WPget_grid(&rstrox,&rstroy,&rstrdx,&rstrdy,&rstron);
/*
***Return value.
*/
    func_vp->lit.float_va = rstrox;
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evagry()

/*      Evaluerar funktionen ACT_GRIDY.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *      2007-03-04 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    DBfloat rstrox,rstroy,rstrdx,rstrdy;
    bool    rstron;

/*
***Get current grid.
*/
    WPget_grid(&rstrox,&rstroy,&rstrdx,&rstrdy,&rstron);
/*
***Return value.
*/
    func_vp->lit.float_va = rstroy;
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evagdx()

/*      Evaluerar funktionen ACT_GRIDDX.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *      2007-03-04 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    DBfloat rstrox,rstroy,rstrdx,rstrdy;
    bool    rstron;

/*
***Get current grid.
*/
    WPget_grid(&rstrox,&rstroy,&rstrdx,&rstrdy,&rstron);
/*
***Return value.
*/
    func_vp->lit.float_va = rstrdx;
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evagdy()

/*      Evaluerar funktionen ACT_GRIDDY.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *      2007-03-04 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    DBfloat rstrox,rstroy,rstrdx,rstrdy;
    bool    rstron;

/*
***Get current grid.
*/
    WPget_grid(&rstrox,&rstroy,&rstrdx,&rstrdy,&rstron);
/*
***Return value.
*/
    func_vp->lit.float_va = rstrdy;
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evagrd()

/*      Evaluerar funktionen ACT_GRID.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *      2007-03-04 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    DBfloat rstrox,rstroy,rstrdx,rstrdy;
    bool    rstron;

/*
***Get current grid.
*/
    WPget_grid(&rstrox,&rstroy,&rstrdx,&rstrdy,&rstron);
/*
***Return value.
*/
    if ( rstron ) func_vp->lit.int_va = 1;
    else          func_vp->lit.int_va = 0;
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evajbn()

/*      Evaluerar funktionen ACT_JOBNAM.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    strcpy(func_vp->lit.str_va , jobnam);

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evamtp()

/*      Evaluerar funktionen ACT_MTYPE. Returnerar
 *      modultyp för aktiv modul, dvs. modulen på
 *      högsta nivån, inte den som exekverar just nu.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      23/9/93  pmrmod(), J. Kjellander
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    pm_ptr   oldmod;
    PMMODULE modhed;

/*
***Sätt aktiv basadress till aktiv modul och spara
***nuvarande. Läs modulhuvudet och sätt tillbaks
***aktiv basadress till det gamla värdet igen.
*/
    oldmod = pmgbla();
    pmsbla(actmod);
    pmrmod(&modhed);
    pmsbla(oldmod);
/*
***Returnera modultyp.
*/
    func_vp->lit.int_va = modhed.mtype;

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evamat()

/*      Evaluerar funktionen ACT_MATTR.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      23/9/93  pmrmod(), J. Kjellander
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    pm_ptr   oldmod;
    PMMODULE modhed;

/*
***Sätt aktiv basadress till aktiv modul och spara
***nuvarande. Läs modulhuvudet och sätt tillbaks
***aktiv basadress till det gamla värdet igen.
*/
    oldmod = pmgbla();
    pmsbla(actmod);
    pmrmod(&modhed);
    pmsbla(oldmod);
/*
***Returnera modulattribut.
*/
    func_vp->lit.int_va = modhed.mattri;

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evajbd()

/*      Evaluerar funktionen ACT_JOBDIR.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    strcpy(func_vp->lit.str_va , jobdir);

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evapft()
 
/*      Evaluates function ACT_PFONT().
 *
 *      (C)2007-03-24, J.Kjellander
 *
 ******************************************************!*/

  {
    func_vp->lit.int_va = defnap.pfont;
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evalft()
 
/*      Evaluerar funktionen ACT_LFONT.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    func_vp->lit.int_va = defnap.lfont;

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evaaft()
 
/*      Evaluerar funktionen ACT_AFONT.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    func_vp->lit.int_va = defnap.afont;
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evacft()
 
/*      Evaluerar funktionen ACT_CFONT.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    func_vp->lit.int_va = defnap.cfont;
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evaxft()
 
/*      Evaluerar funktionen ACT_XFONT.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    func_vp->lit.int_va = defnap.xfont;

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evapsi()

/*      Evaluates function ACT_PSIZE().
 *
 *      (C)2007-03-24, J.Kjellander
 *
 ******************************************************!*/

  {
    func_vp->lit.float_va = defnap.psize;
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evaldl()

/*      Evaluerar funktionen ACT_LDASHL.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    func_vp->lit.float_va = defnap.ldashl;

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evaadl()

/*      Evaluerar funktionen ACT_ADASHL.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    func_vp->lit.float_va = defnap.adashl;
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evacdl()

/*      Evaluerar funktionen ACT_CDASHL.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    func_vp->lit.float_va = defnap.cdashl;
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evaxdl()

/*      Evaluerar funktionen ACT_XLDASHL.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    func_vp->lit.float_va = defnap.xdashl;

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evatsi()

/*      Evaluerar funktionen ACT_TSIZE.
 *
 *      In: Inget.
 *
 *      Ut: *valp  = Aktiv textstorlek.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 ******************************************************!*/

  {

    func_vp->lit.float_va = defnap.tsize;

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evatwi()

/*      Evaluerar funktionen ACT_TWIDTH.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    func_vp->lit.float_va = defnap.twidth;

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evatsl()

/*      Evaluerar funktionen ACT_TSLANT.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    func_vp->lit.float_va = defnap.tslant;

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evadts()

/*      Evaluerar funktionen ACT_DTSIZ.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    func_vp->lit.float_va = defnap.dtsize;

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evadas()

/*      Evaluerar funktionen ACT_DASIZ.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    func_vp->lit.float_va = defnap.dasize;

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evadnd()

/*      Evaluerar funktionen ACT_DNDIG.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    func_vp->lit.int_va = defnap.dndig;

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evadau()

/*      Evaluerar funktionen ACT_DAUTO.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 31/10/86 R. Svedin
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    func_vp->lit.int_va = defnap.dauto;

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evacsy()

/*      Evaluerar funktionen ACT_CSY.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 6/2/95 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {

    if ( lsysla == DBNULL )
      {
      func_vp->lit.ref_va[0].seq_val = 0;
      func_vp->lit.ref_va[0].ord_val = 1;
      func_vp->lit.ref_va[0].p_nextre = NULL;
      }
    else
      {
      DBget_id(lsysla,&func_vp->lit.ref_va[0]);
      }

    return(0);

  }

/********************************************************/
/*!******************************************************/

        short evapid()

/*      Evaluerar funktionen ACT_PID.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 13/9/95 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    strcpy(func_vp->lit.str_va,pidnam);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evavvr()

/*      Evaluerar funktionen ACT_VARKON_VERSION.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 13/9/95 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    sprintf(func_vp->lit.str_va,"%d.%d%c",
            (int)sydata.vernr,(int)sydata.revnr,sydata.level);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evavsr(PMLITVA *valp)

/*      Evaluerar funktionen ACT_VARKON_SERIAL.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 13/9/95 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
    func_vp->lit.int_va = (DBint)sydata.sernr;

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evaost()

/*      Evaluerar funktionen ACT_OSTYPE.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 13/9/95 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
/*
***Defaultvärde.
*/
    strcpy(func_vp->lit.str_va,"unknown");

#ifdef UNIX
    strcpy(func_vp->lit.str_va,"UNIX");
#endif

#ifdef WIN32
    strcpy(func_vp->lit.str_va,"WIN32");
#endif

#ifdef VMS
    strcpy(func_vp->lit.str_va,"VMS");
#endif

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short evahst()

/*      Evaluerar funktionen ACT_HOST.
 *
 *      In:
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV:
 *
 *      (C)microform ab 13/9/95 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
#ifdef UNIX
    struct utsname name;
#endif

/*
***Defaultvärde.
*/
    strcpy(func_vp->lit.str_va,"unknown");
/*
***Alla UNIX-system bör ha uname().
*/
#ifdef UNIX
    uname(&name);
    strcpy(func_vp->lit.str_va,name.nodename);
#endif

    return(0);
  }

/********************************************************/

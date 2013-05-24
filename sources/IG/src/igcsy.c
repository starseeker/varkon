/*!******************************************************************/
/*  igcsy.c                                                         */
/*  =======                                                         */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGcs3p();    Generate csys_3p... statement                      */
/*  IGcs1p();    Generate csys_1p... statement                      */
/*  IGmodb();    Genererate mode_basic.... statement                */
/*  IGmodg();    Genererate mode_global.... statement               */
/*  IGmodl();    Genererate mode_local.... statement                */
/*  IGupcs();    Redisplay currently active coordinate system       */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://www.tech.oru.se/cad/varkon                         */
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

#include "../../DB/include/DB.h"
#include "../include/IG.h"
#include "../../WP/include/WP.h"

extern short   modtyp;
extern char    actcnm[];
extern DBptr   lsysla;
extern DBTmat *lsyspk;

/*!******************************************************/

       short IGcs3p()

/*      Genererar csys_3p.... sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa CSYS_3P sats
 *
 *      (C)microform ab 17/2/85 J. Kjellander
 *
 *      3/7/85   Felhantering, B. Doverud
 *      4/9/85   Anrop till IGcges(), B. Doverud
 *      16/11/85 t-sträng, J. Kjellander
 *      6/3/86   Defaultsträng, J. Kjellander
 *      20/3/86  Anrop pmtcon, B. Doverud
 *      23/3/86  IGcpos(pnr, B. Doverud
 *      24/3/86  Felutgång, B. Doverud
 *      5/10/86  GOMAIN, B. Doverud
 *      1997-03-12 2D, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  exnpt1,exnpt2,exnpt3,exnpt4;
    pm_ptr  valparam;
    pm_ptr  retla,dummy;
    char    istr[V3STRLEN+1];

/*
***Skapa 3 positioner.
*/
start:
    if ( (status=IGcpos(262,&exnpt2)) < 0 ) goto end;
    if ( (status=IGcpos(272,&exnpt3)) < 0 ) goto end;

    if ( modtyp == 3 )
      {
      if ( (status=IGcpos(273,&exnpt4)) < 0 ) goto end;
      }
/*
***Skapa sträng.
*/
    if ( (status=IGcstr(306,"",istr,&exnpt1)) < 0 ) goto end;
/*
***Länka ihop parameterlistan.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);

    if ( modtyp == 3 )
      {
      pmtcon(exnpt3,retla,&retla,&dummy);
      pmtcon(exnpt4,retla,&valparam,&dummy);
      }
    else
      {
      pmtcon(exnpt3,retla,&valparam,&dummy);
      }
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( IGcges("CSYS_3P",valparam) < 0 ) goto error;

    WPerhg();
    goto start;

end:
    WPerhg();
    return(status);

/*
***Felutgångar.
*/
error:
    erpush("IG5023","CSYS_3P");
    errmes();
    WPerhg();
    goto start;
  }
  
/********************************************************/
/*!******************************************************/

       short IGcs1p()

/*      Genererar csys_1p.... sats.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      Felkod: IG5023 = Kan ej skapa CSYS_1P sats
 *
 *      (C)microform ab 1/10/87 J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    pm_ptr  exnpt1,exnpt2,exnpt3,exnpt4,exnpt5;
    pm_ptr  valparam;
    pm_ptr  retla,dummy;
    char    istr[V3STRLEN+1];
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "0.0";
/*
***Skapa position.
*/
start:
    if ( (status=IGcpos(262,&exnpt2)) < 0 ) goto end;
/*
***Skapa vinklar.
*/
    if ( modtyp == 3 )
      {
      if ( (status=IGcflt(390,dstr,istr,&exnpt3)) < 0 ) goto end;
      strcpy(dstr,istr);
      if ( (status=IGcflt(391,dstr,istr,&exnpt4)) < 0 ) goto end;
      strcpy(dstr,istr);
      if ( (status=IGcflt(392,dstr,istr,&exnpt5)) < 0 ) goto end;
      strcpy(dstr,istr);
      }
    else
      {
      if ( (status=IGcflt(392,dstr,istr,&exnpt5)) < 0 ) goto end;
      strcpy(dstr,istr);
      val.lit_type = C_FLO_VA;
      val.lit.float_va = 0.0;
      pmclie(&val,&exnpt3);
      pmclie(&val,&exnpt4);
      }
/*
***Skapa sträng.
*/
    if ( (status=IGcstr(306,"",istr,&exnpt1)) < 0 ) goto end;
/*
***Skapa listan med obligatoriska parametrar.
*/
    pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
    pmtcon(exnpt2,retla,&retla,&dummy);
    pmtcon(exnpt3,retla,&retla,&dummy);
    pmtcon(exnpt4,retla,&retla,&dummy);
    pmtcon(exnpt5,retla,&valparam,&dummy);
/*
***Skapa, interpretera och länka in satsen i modulen.
*/
    if ( IGcges("CSYS_1P",valparam) < 0 ) goto error;

    WPerhg();
    goto start;

end:
    WPerhg();
    return(status);

/*
***Felutgångar.
*/
error:
    erpush("IG5023","CSYS_1P");
    errmes();
    WPerhg();
    goto start;
  }
  
/********************************************************/
/*!******************************************************/

       short IGmodb()

/*      Create MODE_BASIC(); statement.
 *
 *      Error: IG5023 = Can't crete MODE_BASIC() statement
 *
 *      (C)microform ab 1997-03-12 J. Kjellander
 *
 *      2007-03-17 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr   old_la;

/*
***If lsyspk = NULL the BASIC system might already be active.
*/
    if ( lsyspk == NULL && (strcmp(actcnm,"BASIC") == 0 ) )
      {
      erpush("IG2312","");
      goto errend;
      }
/*
***DBptr of current csys.
*/
    old_la = lsysla;
/*
***Delete a possible grid now before the new csys
***becomes active.
*/
    WPdelete_grid(GWIN_ALL);
/*
***Create, execute and add the statement to the active module.
*/
    if ( IGcprs("MODE_BASIC",(pm_ptr)NULL) < 0 )
      {
      erpush("IG5023","MODE_BASIC");
      goto errend;
      }
/*
***Update graphical windows.
*/
    IGupcs(old_la,V3_CS_NORMAL);
/*
***Display the new grid (if on).
*/
    WPdraw_grid(GWIN_ALL);
/*
***The end.
*/
    return(0);
/*
***Error exit.
*/
errend:
    errmes();
    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short IGmodg()

/*      Create MODE_GLOBAL(); statement.
 *
 *      Error: IG5023 = Can't create MODEL_GLOBAL(); statement
 *             IG2262 = Global system already active
 *
 *      (C)microform ab 9/2/85 J. Kjellander
 *
 *      12/11/85 Koll av lsyspk, J. Kjellander
 *      1997-03-11 IGupcs(), J.Kjellander
 *      2007-03-18 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr old_la;

/*
***If lsyspk = NULL the global system might already be active.
*/
    if ( lsyspk == NULL && (strcmp(actcnm,"GLOBAL") == 0 ) )
      {
      erpush("IG2262","");
      goto errend;
      }
/*
***DBptr of current csys.
*/
    old_la = lsysla;
/*
***Delete a possible grid now before the new csys
***becomes active.
*/
    WPdelete_grid(GWIN_ALL);
/*
***Create, execute and add the statement to the active module.
*/
    if ( IGcprs("MODE_GLOBAL",(pm_ptr)NULL) < 0 )
      {
      erpush("IG5023","MODE_GLOBAL");
      goto errend;
      }
/*
***Update graphical windows.
*/
    IGupcs(old_la,V3_CS_NORMAL);
/*
***Display the new grid (if on).
*/
    WPdraw_grid(GWIN_ALL);
/*
***The end.
*/
    return(0);
/*
***Error exit.
*/
errend:
    errmes();
    return(0);
  }
  
/********************************************************/
/*!******************************************************/

       short IGmodl()

/*      Create a MODEL_LOCAL(#n); statement.
 *
 *      Return:      0 = OK.
 *              REJECT = Cancel operation.
 *              GOMAIN = Back to main menu.
 *
 *      Error: IG5023 = Can't create MODE_LOCAL(#n); statement
 *
 *      (C)microform ab 9/2/85 J. Kjellander
 *
 *      4/7/84   Felhantering, B. Doverud
 *      9/9/85   Higlight, R. Svedin
 *      30/10/85 Ände och sida, J. Kjellander
 *      20/3/86  Anrop pmtcon, pmclie B. Doverud
 *      24/3/86  Felutgång B. Doverud
 *      6/10/86  GOMAIN, J. Kjellander
 *      1997-03-11 IGupcs(), J.Kjellander
 *      2007-03-18 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    DBetype typ;
    DBptr   old_la,new_la;
    bool    right,end;
    pm_ptr  valparam,exnpt,dummy;
    short   status;
    PMREFVA idvek[MXINIV];
    PMLITVA litval;

/*
***DBptr of current csys.
*/
    old_la = lsysla;
/*
***We don't use IGcref() here because we want to have
***direct access to the ID of the csys so that we can check
***if it's already active. 
***Put out the promt.
*/
    WPaddmess_mcwin(IGgtts(271),WP_MESSAGE);
/*
***Get csys ID.
*/
    typ = CSYTYP;
    status = IGgsid(idvek,&typ,&end,&right,(short)0);
    WPclear_mcwin();
    WPerhg();
    if ( status != 0 ) return(status);
/*
**Get the DBptr of the new csys and check if it's already active.
*/
    if ( DBget_pointer('I',idvek,&new_la,&typ) < 0  ||  typ != CSYTYP )
      {
      erpush("EX1402",""); /* This error is unlikely ! */
      goto error;
      }

    if ( new_la == lsysla )
      {
      erpush("IG2322","");
      goto error;
      }
/*
***Create litval.
*/
    litval.lit_type = C_REF_VA;
    litval.lit.ref_va[0].seq_val  = idvek[0].seq_val;
    litval.lit.ref_va[0].ord_val  = idvek[0].ord_val;
    litval.lit.ref_va[0].p_nextre = idvek[0].p_nextre;
    pmclie(&litval,&exnpt);
/*
***Create parameter list.
*/
    pmtcon(exnpt,(pm_ptr)NULL,&valparam,&dummy);
/*
***Delete a grid now before the new csys
***becomes active.
*/
    WPdelete_grid(GWIN_ALL);
/*
***Create, execute and add the statement to the active module.
*/
    if ( IGcprs("MODE_LOCAL",valparam) < 0 )
      {
      erpush("IG5023","MODE_LOCAL");
      goto error;
      }
/*
***Update graphical windows.
*/
    IGupcs(old_la,V3_CS_NORMAL);
    IGupcs(lsysla,V3_CS_ACTIVE);
/*
***Display the new grid (if on).
*/
    WPdraw_grid(GWIN_ALL);
/*
***The end.
*/
    return(0);
/*
***Error exit.
*/
error:
    errmes();
    return(0);
  }
  
/********************************************************/
/*!******************************************************/

        short IGupcs(
        DBptr la,
        int   mode)

/*      Updates the graphical appearence of a csys after
 *      activation/deactivation.
 *
 *      In: la   = DBptr to csys or DBNULL.
 *                 Usually = lsysla.
 *          mode = V3_CS_NORMAL eller V3_CS_ACTIVE
 *
 *      (C)microform ab 1997-03-11 J. Kjellander
 *
 *      2007-03-18 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
   DBCsys  csy;

/*
***Read the csys from DB and call WPupcs().
*/
   if ( la != DBNULL )
     {
     DBread_csys(&csy,NULL,la);
     WPupcs(&csy,la,mode,GWIN_ALL);
     }
/*
***The end.
*/
   return(0);
   }

/********************************************************/

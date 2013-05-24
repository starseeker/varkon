/*!******************************************************************/
/*  igsymb.c                                                        */
/*  ========                                                        */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGsymb();    Genererate symb.. statement                        */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://varkon.sourceforge.net                             */
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

extern char jobdir[],jobnam[];
extern int  sysmode;

/********************************************************/

        short IGsymb()

/*      Create SYMB() statement. (Import PLT file).
 *
 *      Return:      0 = OK
 *              REJECT = Cancel
 *              GOMAIN = Main menu
 *
 *      Error: IG5023 = Can't create SYMBOL statement
 *
 *      (C)2007-11-30 2.0, J.Kjellander
 *
 ********************************************************/

  {
    char    istr[V3STRLEN],symname[V3STRLEN],symdir[V3PTHLEN],
            act_jobdir[V3PTHLEN];
    short   status;
    int     i;
    bool    setpath;
    pm_ptr  valparam;
    pm_ptr  retla,dummy;
    pm_ptr  exnpt1,exnpt2,exnpt3,exnpt4;
    PMLITVA litval;


    static char nstr[V3STRLEN] = "";
    static char sstr[V3STRLEN] = "1.0";
    static char vstr[V3STRLEN] = "0.0";

/*
***Default name is jobname. Default directory is jobdir.
*/
   if ( nstr[0] == '\0') strcpy(nstr,jobnam);
   strcpy(symdir,jobdir);
/*
***In GENERIC mode the PLT file must be in the current job
***directory. In explicit mode the PLT-file may reside in any
***directory.
*/
    if ( sysmode == EXPLICIT ) setpath = TRUE;
    else                       setpath = FALSE;
/*
***Get the name and optional path of the plotfile.
*/
   status = WPfile_selector(IGgtts(138),symdir,setpath,nstr,"*.PLT",symname);
   if ( status == 0 )
     {
     if ( IGcmpw("*.PLT",symname) )
       {
       i = strlen(symname) - 4;
       symname[i] = '\0';
       }
     }
   else return(status);
/*
***Create a literal with the symbol name.
*/
   litval.lit_type = C_STR_VA;
   strcpy(litval.lit.str_va,symname);
   pmclie(&litval,&exnpt1);
/*
***Position.
*/
   if ( (status=IGcpos(264,&exnpt2)) < 0 ) goto end;
/*
***Size.
*/
   if ( (status=IGcflt(136,sstr,istr,&exnpt3)) < 0 ) goto end;
   strcpy(sstr,istr);
/*
***Rotation.
*/
   if ( (status=IGcflt(17,vstr,istr,&exnpt4)) < 0 ) goto end;
   strcpy(vstr,istr);
/*
***Create parameter list.
*/
   pmtcon(exnpt1,(pm_ptr)NULL,&retla,&dummy);
   pmtcon(exnpt2,retla,&retla,&dummy);
   pmtcon(exnpt3,retla,&retla,&dummy);
   pmtcon(exnpt4,retla,&valparam,&dummy);
/*
***Create, execute and save statement. In explicit mode
***jobdir needs to point to the actual directory (symdir)
***during execution so that EX can find the PLT-file.
*/
   if ( sysmode == EXPLICIT )
     {
     strcpy(act_jobdir,jobdir);
     strcpy(jobdir,symdir);
     }

   status = IGcges("SYMB",valparam);

   if ( sysmode == EXPLICIT )
     {
     strcpy(jobdir,act_jobdir);
     }

   if ( status < 0 )
     {
     erpush("IG5023","");
     errmes();
     goto end;
     }
/*
***Remember this name as default name.
*/
  strcpy(nstr,symname);
/*
***The end.
*/
end:
   WPerhg();
   return(status);
  }

/********************************************************/

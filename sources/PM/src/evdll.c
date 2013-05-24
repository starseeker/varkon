/**********************************************************************
*
*    evdll.c
*    =======
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    This file includes the following routines:
*
*    evlddl();     Evaluerar LOAD_DLL
*    evcldl();     Evaluerar CALL_DLL_FUNCTION
*    evuldl();     Evaluerar UNLOAD_DLL
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

#ifdef WIN32

#include "windows.h"

extern PMPARVA *proc_pv;  /* inproc.c *pv      Access structure for MBS routines */
extern short    proc_pc;  /* inproc.c parcount Number of actual parameters */

extern PMPARVA *func_pv;   /* Access structure for MBS routines */
extern PMLITVA *func_vp;   /* Pekare till resultat. */

/*
***Efter många timmars strul stängde jag av optimeringen på
***den här filen eftersom det inte funkar annars.
***Varför är inte utrett men med -Og (Global optimering)
***kraschar systemet när evcldl() gör return(0); efter
***att ha anropat DLL-funktionen.
*/
#pragma optimize( "", off )

typedef struct dll_library
{
char       name[V3STRLEN+1];
HINSTANCE  hndl;
}DLL_LIBRARY;

#define MAXDLL 25

static DLL_LIBRARY dlltab[MAXDLL] = {"",0,"",0,"",0,"",0,"",0,
                                     "",0,"",0,"",0,"",0,"",0,
                                     "",0,"",0,"",0,"",0,"",0,
                                     "",0,"",0,"",0,"",0,"",0,
                                     "",0,"",0,"",0,"",0,"",0};

/* dlltab har plats för MAXDLL laddade DLL:er. */

static int fndfree();
static int fnddll(char *name);

/*!******************************************************/

        short evlddl()

/*      Evaluerar LOAD_DLL.
 *
 *      In: Global  func_pv  => Parameter value array
 *
 *      Ut: Global *func_vp =   0 = OK.
 *                             -1 = Fel.
 *
 *      Felkod: IN5282 = Har redan laddat max antal DLL:er
 *
 *      (C)microform ab 1997-01-29 J. Kjellander
 *
 *      1997-03-11 Status = 1 om laddad, J.Kjellander
 *      2001-03-05 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   int        i;
   char      *name;
   HINSTANCE  dll_hndl;


/*
***DLL:ens namn ev med path.
*/
   name = func_pv[1].par_va.lit.str_va;
/*
***Om den redan är laddad returnerar vi en etta.
*/
   if ( fnddll(name) != -1 )
     {
     func_vp->lit.int_va = 1;
     return(0);
     }
/*
***Kolla att det finns plats i dlltab.
*/
   if ( (i=fndfree()) == -1 ) return(erpush("IN5282",name));
/*
***Prova att ladda.
*/
   dll_hndl = LoadLibrary(name);
/*
***Om det inte gick bra returnerar vi -1.
*/
   if ( dll_hndl == NULL )
     {
     func_vp->lit.int_va = -1;
     return(0);
     }
/*
***Annars sparar vi namn och handtag i dlltab.
*/
   else
     {
     strcpy(dlltab[i].name,name);
     dlltab[i].hndl = dll_hndl;
     func_vp->lit.int_va = 0;
     return(0);
     }
 }

/********************************************************/
/*!******************************************************/

        short evcldl()

/*      Evaluerar CALL_DLL_FUNCTION.
 *
 *      In: extern proc_pv  = Pekare till array med parametervärden
 *          extern proc_pc  = Antal parametrar, 2 eller 3.
 *
 *      Felkod: IN5292 = DLL %s ej laddad
 *              IN5313 = DLL-Funktionen %s finns ej
 *
 *      (C)microform ab 1997-01-29 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   int     i;
   char   *dllnam,*funam;
   FARPROC funadr;
   DWORD   errcode;


/*
***DLL:ens namn ev med path.
*/
   dllnam = proc_pv[1].par_va.lit.str_va;
/*
***Funktionens namn.
*/
   funam = proc_pv[2].par_va.lit.str_va;
/*
***Kolla att den är laddad.
*/
   if ( (i=fnddll(dllnam)) == -1 ) return(erpush("IN5292",dllnam));
/*
***Hämta rutinens C-adress.
*/
   funadr = GetProcAddress(dlltab[i].hndl,funam);


   if ( funadr == NULL )
     {
     errcode = GetLastError();
     return(erpush("IN5313",funam));
     }
/*
***Anropa. 0, 1, 2 eller 3 strängparametrar.
*/
   switch ( proc_pc )
     {
     case 2:
     (*funadr)();
     break;

     case 3:
     (*funadr)(proc_pv[3].par_va.lit.str_va);
     break;

     case 4:
     (*funadr)(proc_pv[3].par_va.lit.str_va,
               proc_pv[4].par_va.lit.str_va);
     break;

     case 5:
     (*funadr)(proc_pv[3].par_va.lit.str_va,
               proc_pv[4].par_va.lit.str_va,
               proc_pv[5].par_va.lit.str_va);
     break;
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evuldl()

/*      Evaluerar UNLOAD_DLL.
 *
 *      In: extern proc_pv   => Pekare till array med parametervärden
 *
 *      Felkod: IN5292 = DLL %s ej laddad
 *              IN5303 = Systemfel
 *
 *      (C)microform ab 1997-01-29 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   int   i;
   char *name;

/*
***DLL:ens namn ev med path.
*/
   name = proc_pv[1].par_va.lit.str_va;
/*
***Kolla att den är laddad.
*/
   if ( (i=fnddll(name)) == -1 ) return(erpush("IN5292",name));
/*
***Befria den.
*/
   if ( FreeLibrary(dlltab[i].hndl) )
     {
     dlltab[i].name[0] = '\0';
     dlltab[i].hndl = 0;
     return(0);
     }
   else return(erpush("IN530",name));
 }

/********************************************************/
/*!******************************************************/

 static int fnddll(char *name)

/*      Söker upp en DLL i dlltab.
 *
 *      In: name = DLL:ens filnamn ev. med path
 *
 *      FV:   -1 = Finns inte
 *          >= 0 = Index i dlltab.
 *
 *      (C)microform ab 1997-01-29 J. Kjellander
 *
 ******************************************************!*/

 {
   int i;

   for ( i=0; i<MAXDLL; ++i )
     if ( strcmp(name,dlltab[i].name) == 0 ) return(i);

   return(-1);
 }

/********************************************************/
/*!******************************************************/

 static int fndfree()

/*      Söker upp ledig plats i dlltab.
 *
 *      FV:   -1 = dlltab full.
 *          >= 0 = Index till ledig plats i dlltab.
 *
 *      (C)microform ab 1997-01-29 J. Kjellander
 *
 ******************************************************!*/

 {
   int i;

   for ( i=0; i<MAXDLL; ++i )
     if ( dlltab[i].hndl == 0 ) return(i);

   return(-1);
 }

/********************************************************/

#else
/*
***UNIX mfl. returnerar bara felmeddelande.
*/

/*!******************************************************/

        short evlddl()

/*      UNIX-version av LOAD_DLL.
 *
 *      In: pv   => Pekare till array med parametervärden
 *          valp => Pekare till funktionsvärde.
 *
 *      Ut: *valp = Funktionsvärde.
 *
 *      Felkod: IN5263 = %s är reserverad för MS-WIN32.
 *
 *      (C)microform ab 1997-01-29 J. Kjellander
 *
 ******************************************************!*/

 {
   return(erpush("IN5263","LOAD_DLL"));
 }

/********************************************************/
/*!******************************************************/

        short evcldl()

/*      UNIX-version av CALL_DLL_FUNCTION.
 *
 *      In: pv   => Pekare till array med parametervärden
 *
 *      Felkod: IN5263 = %s är reserverad för MS-WIN32.
 *
 *      (C)microform ab 1997-01-29 J. Kjellander
 *
 ******************************************************!*/

 {
   return(erpush("IN5263","CALL_DLL_FUNCTION"));
 }

/********************************************************/
/*!******************************************************/

        short evuldl()

/*      UNIX-version av UNLOAD_DLL.
 *
 *      In: pv   => Pekare till array med parametervärden
 *
 *      Felkod: IN5263 = %s är reserverad för MS-WIN32.
 *
 *      (C)microform ab 1997-01-29 J. Kjellander
 *
 ******************************************************!*/

 {
   return(erpush("IN5263","UNLOAD_DLL"));
 }

/********************************************************/
#endif

/**********************************************************************
*
*    calext.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    This file includes the following routines:
*
*     evcaxt();   Evaluerar CALL_EXTERN
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

extern PMPARVA *proc_pv;  /* inproc.c *pv      Access structure for MBS routines */
extern short    proc_pc;  /* inproc.c parcount Number of actual parameters */

/*!*******************************************************
*
*     Så här gör man när man vill lägga till en ny
*      rutin:
*
*      1. Deklarera den under. Punkt 1 nedan.
*      2. Öka FUNTAB_SIZE med 1. Punkt 2 nedan.
*      3. Lägg in namnen i funtab. Punkt 3 nedan.
*      4. Lägg till själva C-funktionen. Punkt 4 nedan.
*
*      Observera att evcaxt bara kopierar så många
*      flyttal som anges av parameter 2 (nflt) innan
*      den egna rutinen anropas men att alla värden
*      i FLOAT-arrayen kopieras tillbaks när rutinen
*      är klar. Man bör alltså inte dimensionera en
*      FLOAT array större än vad som behövs. Man bör
*      också se till att ge alla värden i arrayen väl-
*      definierade värden även om de inte används.
*
*******************************************************!*/

/*
***Här deklareras funktioner som inte finns definierade
***i denna fil men som anropas från denna fil.
*/
/* extern my_external_routine(); */
/*
***Punkt1.
***Här deklareras de funktioner man skriver själv. För
***varje ny funktion lägger man till en rad enl. nedan.
***Det är funktionens C-namn man skall deklarera.
*/
static void mall1();       /* Exempel 1 att utgå ifrån */
static void mall2();       /* Exempel 2 att utgå ifrån */

/*
***Nedanstående definition beskriver mappningen mellan
***den externa funktionens namn i MBS (en sträng) och
***motsvarande funktions namn i C (en adress). Ändra
***inte i denna definition.
*/
typedef struct extfunc
{
char  mbsnamn[V2SYNLEN+1];  /* Funktionens namn i MBS */
void  (*Cnamn)();           /* Funktionens namn i C */
}EXTFUNC;

/*
***Punkt2 och 3.
***Nedanstående tabell deklarerar namnen på alla
***externa funktioner. För varje ny funktion skall
***dess MBS-namn och C-namn läggas in på en ny rad
***i tabellen. Glöm inte att ändra FUNTAB_SIZE också.
***Max antal tecken i namn är c:a 30. MBS-namnet och
***C-namnet behöver inte vara lika. Försök använda
***rimligt korta namn.
*/
#define FUNTAB_SIZE 2

static EXTFUNC funtab[FUNTAB_SIZE] = {
                                      {"mall1",mall1},
                                      {"mall2",mall2},
                                      };

/*!******************************************************/

        short evcaxt()

/*      Evaluerar proceduren CALL_EXTERN. Denna rutin
 *      anropas av VARKON och anropar i sin tur den
 *      rutin som anges av 1:a parametern genom att
 *      söka upp namnet i funtab. Om namnet inte finns
 *      returneras felmeddelande.
 *
 *      OBS. Inga ändringar får göras i denna rutin !
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Ut: Inget.
 *
 *      Felkoder: IN5482 = nflt < 0
 *                IN5492 = Deklarerad storlek < nflt
 *                IN5502 = Kan ej mallokera
 *
 *      FV: Returnerar anropade rutiners status.
 *
 *      (C)microform ab 1998-05-14 J. Kjellander
 *
 *      2001-02-14 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

{
   char   *name,errbuf[81];
   short   status;
   int     i,nflt,dekl_dim,fltsiz;
   long    fltadr;
   double *fpek;
   void  (*function)();
   STTYTBL typtbl;
   STARRTY flttyp;
   PMLITVA val;

/*
***Vilken rutin skall anropas ?
*/
   name = proc_pv[1].par_va.lit.str_va;

   for ( i=0; i<FUNTAB_SIZE; ++i)
     if ( strcmp(name,funtab[i].mbsnamn) == 0 ) break;

   if ( i == FUNTAB_SIZE ) return(erpush("IN5512",name));
   else                    function = funtab[i].Cnamn;
/*
***Hur många flyttal i arrayen.
*/
   nflt = proc_pv[2].par_va.lit.int_va;
/*
***Kolla att antalet flyttal är rimligt.
*/
   if ( nflt < 0 ) return(erpush("IN5482",""));
/*
***Kolla att deklarerad dimension på MBS-arrayen är tillräcklig för
***nflt flyttal.
*/
   fltadr = proc_pv[3].par_va.lit.adr_va;
   strtyp(proc_pv[3].par_ty,&typtbl);
   strarr(typtbl.arr_ty,&flttyp);
   dekl_dim = flttyp.up_arr - flttyp.low_arr + 1;
   if ( dekl_dim < nflt ) return(erpush("IN5492",""));

   strtyp(flttyp.base_arr,&typtbl);
   fltsiz = typtbl.size_ty;
/*
***Allokera minne.
*/
   if ( (fpek=(gmflt *)v3mall(nflt*sizeof(double),"evcaxt")) == NULL )
     {
     sprintf(errbuf,"%d",nflt);
     return(erpush("IN5502",errbuf));
     }
/*
***Kopiera från RTS till allokerad area.
*/
   for ( i=0; i<nflt; ++i )
     {
     ingval(fltadr+i*fltsiz,flttyp.base_arr,FALSE,&val);
    *(fpek+i) = (double)val.lit.float_va;
     }
/*
***Anropa funktionen.
*/
   (*function)(nflt,fpek);
/*
***Kopiera tillbaks dekl_dim st. värden till FLOAT-arrayen.
*/
   status = evwfvk(fpek,(short)dekl_dim,(short)3,proc_pv);
/*
***Deallokera minne.
*/
   v3free(fpek,"evcaxt");

   return(status);
}

/********************************************************/


/*
***Punkt 4.
***Här nedan lägger man till sina egna rutiner.
*/


/*!******************************************************/

 static void mall1(nflt,fpek)
        int     nflt;
        double *fpek;

/*      Detta är bara en mall. Den adderar 1 till alla
 *      tal den får som indata.
 *
 ******************************************************!*/

{
   int i;

   for ( i=0; i < nflt; ++i ) *(fpek+i) += 1;

   return;
}

/********************************************************/
/*!******************************************************/

 static void mall2(nflt,fpek)
        int     nflt;
        double *fpek;

/*      Detta är bara en annan mall. Den delar alla
 *      indata med 2.
 *
 ******************************************************!*/

{
   int i;

   for ( i=0; i < nflt; ++i ) *(fpek+i) = *(fpek+i)/2;
   return;
}

/********************************************************/

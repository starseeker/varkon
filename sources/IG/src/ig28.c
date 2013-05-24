/*!******************************************************************/
/*  File: ig28.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  igmain();  Starts Varkon in generic mode                        */
/*  irmain();  Starts Varkon in explicit mode                       */
/*  igebas();  Generic start routine                                */
/*  iglsjb();  Lists jobs                                           */
/*  igdljb();  Deletes jobs                                         */
/*  igmvrr();  Copies RES-file to RIT-file                          */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
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
/*  (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/IG.h"
#include <string.h>

extern char   pidnam[],jobdir[],jobnam[],mdffil[];
extern short  actfun,pant,mant,v3mode,igtrty;
extern V3MDAT sydata;

static short igebas();

/*!******************************************************/

       short  igmain()

/*     Huvudrutin för basmodulen.
*
*      Felkoder : IG0252 = Kan ej starta Basmodulen
*
*      (C) microform ab 2/3/88  J. Kjellander.
*
*      12/12/94 v3mode, J. Kjellander
*
*******************************************************!*/

 {

/*
***Kolla att basmodulen ingår.
*/
   if ( sydata.opmode != BAS_MOD )
     {
     erpush("IG0252","");
     return(EREXIT);
     }
/*
***Starta basmodulen med meny 0.
*/
   v3mode = BAS2_MOD;

   return(igebas());

 }

/******************************************************!*/
/*!******************************************************/

       short  irmain()

/*     Huvudrutin för ritpaketet.
*
*      (C) microform ab 2/3/88  J. Kjellander.
*
*      12/12/94 v3mode, J. Kjellander
*
*******************************************************!*/

 {
   short status;

/*
***Ställ om opmode i sydata och anropa basmodulen
***med v3mode = RIT_MOD.
*/
   if ( sydata.opmode == BAS_MOD )
     {
     sydata.opmode = RIT_MOD;
     v3mode = RIT_MOD;
     status = igebas();
     sydata.opmode = BAS_MOD;
     }
   else
     {
     v3mode = RIT_MOD;
     status = igebas();
     }

   return(status);

 }

/******************************************************!*/
/*!******************************************************/

static short igebas()

/*     Huvudrutin för Basmodulen.
*
*      In : Inget.
*
*      Felkoder : IG0232 = Du har ej valt projekt
*                 IG0242 = Projektet finns ej
*                 IG0262 = Kan ej ladda menyer
*                 IG0342 = %s otillåtet jobnamn
*
*      (C) microform ab 2/3/88  J. Kjellander.
*
*      8/5/89 igckjn(), J. Kjellander
*
*******************************************************!*/

 {
   short status,alt;

/*
***Ladda menyerna. 
*/
   if ( iginit(mdffil) < 0 )
     {
     erpush("IG0262",mdffil);
     return(EREXIT);
     }
/*
***Global initiering av PM.
*/
   if ( pminit() < 0 ) return(EREXIT);
/*
***Starta basmodulen.
*/
   pant = 0;
   mant = 0;

   if ( (status=igload()) < 0 ) return(status);
/*
***Loopa med igexfu() och meny 0 = Huvudmenyn.
*/
   for (;;)
     {
     actfun = -1;
     status = igexfu(0,&alt);

     if ( status == GOMAIN )
       {
       pant = 0;
       igrsma();
       mant = 0;
       }
     else if ( status == EXIT )
       {
       return(EXIT);
       }
     else if ( status == EREXIT )
       {
       return(EREXIT);
       }
     }

 }

/******************************************************!*/
/*!******************************************************/

       short  iglsjb()

/*     Varkon-funktion för att lista job.
 *
 *     Felkoder:
 *
 *     (C) microform ab 28/9/95  J. Kjellander.
 *
 ******************************************************!*/

 {
   char buf[JNLGTH+1];

   igselj(buf);

   return(0);
 }

/******************************************************!*/
/*!******************************************************/

       short  igdljb()

/*     Varkon-funktion för att ta bort job.
 *
 *     Felkoder : IG0452 = Jobbet %s är aktivt.
 *
 *     (C) microform ab 28/9/95  J. Kjellander.
 *
 *******************************************************!*/

 {
    char  job[JNLGTH+1];
    char  jobfil[V3PTHLEN+1];
    char  mbsfil[V3PTHLEN+1];
    char  mbofil[V3PTHLEN+1];
    char  resfil[V3PTHLEN+1];
    char  pltfil[V3PTHLEN+1];
    char  ritfil[V3PTHLEN+1];
    short status;

/*
***Läs in jobbnamn.
*/
    status = igselj(job);
    if ( status < 0 ) return(status);
/*
***Kolla att det inte är aktivt.
*/
   if ( strcmp(job,jobnam) == 0 )
     {
     erpush("IG0452",job);
     errmes();
     return(0);
     }
/*
***Ta bort ev. JOB-fil ?
*/
   strcpy(jobfil,jobdir);
   strcat(jobfil,job);
   strcat(jobfil,JOBEXT);

   if ( v3ftst(jobfil)  &&  igialt(442,67,68,FALSE) ) v3fdel(jobfil);
/*
***Ta bort ev. MBO-fil ?
*/
   strcpy(mbofil,jobdir);
   strcat(mbofil,job);
   strcat(mbofil,MODEXT);

   if ( v3ftst(mbofil)  &&  igialt(443,67,68,FALSE)  &&
                            igialt(444,67,68,FALSE)  ) v3fdel(mbofil);
/*
***Ta bort ev. MBS-fil ?
*/
   strcpy(mbsfil,jobdir);
   strcat(mbsfil,job);
   strcat(mbsfil,MBSEXT);

   if ( v3ftst(mbsfil)  &&  igialt(445,67,68,FALSE)  &&
                            igialt(446,67,68,FALSE) ) v3fdel(mbsfil);
/*
***Ta bort ev. RES-fil ?
*/
   strcpy(resfil,jobdir);
   strcat(resfil,job);
   strcat(resfil,RESEXT);

   if ( v3ftst(resfil)  &&  igialt(447,67,68,FALSE) ) v3fdel(resfil);
/*
***Ta bort ev. RIT-fil ?
*/
   strcpy(ritfil,jobdir);
   strcat(ritfil,job);
   strcat(ritfil,RITEXT);

   if ( v3ftst(ritfil)  &&  igialt(448,67,68,FALSE)  &&
                            igialt(449,67,68,FALSE) ) v3fdel(ritfil);
/*
***Ta bort ev. PLT-fil ?
*/
   strcpy(pltfil,jobdir);
   strcat(pltfil,job);
   strcat(pltfil,PLTEXT);

   if ( v3ftst(pltfil)  &&  igialt(450,67,68,FALSE) ) v3fdel(pltfil);

   return(0);
 }

/******************************************************!*/
/*!******************************************************/

       short  igmvrr()

/*     Varkon-funktion för att kopiera en RES-fil
 *     till en RIT-fil.
 *
 *     Felkoder : IG0232 = Du har ej valt projekt
 *                IG0242 = Projektet finns ej.
 *                IG0312 = Resultatfil finns ej
 *
 *     (C) microform ab 16/3/88  J. Kjellander.
 *
 *******************************************************!*/

 {
    char  job[JNLGTH+1];
    char  resfil[V3PTHLEN+1];
    char  ritfil[V3PTHLEN+1];
    short status;
    FILE  *fpk;

/*
***Kolla att pidnam finns.
*/
   if ( pidnam[0] == '\0' )
     {
     erpush("IG0232","");
     errmes();
     return(0);
     }
/*
***Ladda aktuell PID-fil.
*/
    if ( igldpf(pidnam) < 0 )
      {
      erpush("IG0242",pidnam);
      errmes();
      return(0);
      }
/*
***Läs in jobnamn.
*/
    status = igssip(iggtts(400),job,"",JNLGTH);
    if ( status < 0 ) return(status);
/*
***Kolla om RES-filen finns.
*/
   strcpy(resfil,jobdir);
   strcat(resfil,job);
   strcat(resfil,RESEXT);

   if ( (fpk=fopen(resfil,"r")) == 0 )
     {
     erpush("IG0312",resfil);
     errmes();
     return(0);
     }
   fclose(fpk);
/*
***Kolla om RIT-filen finns.
*/
   strcpy(ritfil,jobdir);
   strcat(ritfil,job);
   strcat(ritfil,RITEXT);

   if ( (fpk=fopen(ritfil,"r")) != 0 )
     {
     fclose(fpk);
     if ( !igialt(419,67,68,TRUE) ) return(0);
     }
/*
***Kopiera.
*/
   if ( v3fcpy(resfil,ritfil) < 0 ) errmes();

   return(0);
 }

/******************************************************!*/

/**********************************************************************
*
*    ms33.c
*    ======
*
*    This file is part of the VARKON MS-library including
*    Microsoft WIN32 specific parts of the Varkon
*    WindowPac library.
*
*    This file includes:
*
*     msmrdb();   Create/delete resource database
*     msgrst();   Get resource value
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
*    (C) 1984-1999, Johan Kjellander, Microform AB
*    (C) 200-2004, Johan Kjellander, Örebro university
*
***********************************************************************/

#include "../../../sources/DB/include/DB.h"
#include "../../../sources/IG/include/IG.h"
#include "../../../sources/WP/include/WP.h"

static int  load_file(); /* Läser och tolkar en resursfil */
static bool add_res();   /* Lägger till en resurs till rtab */

/*
*******************************************************!*/

#define MAXRESNL 30
#define MAXRESVL 80

typedef struct resource
{
char name[MAXRESNL+1];             /* Resursens namn, tex. MARGIN_UP */
char value[MAXRESVL+1];            /* Resursens värde, tex. 20 */
} RESOURCE;

static RESOURCE *rtabpek = NULL;
static int       rtabsiz = 0;
static int       resant  = 0;

/* rtabpek = pekare till resurstabellen. Om NULL är den ej allokerad.
   rtabsiz = Allokerad men ej nödvändigtvis helt utnyttjad storlek.
   resant  = Antal resurser i rtab.
*/

/*!******************************************************/

        int  msmrdb(filnam,create)
        char *filnam;
        bool  create;

/*      Skapar/dödar resursdatabasen.
 *
 *      In: filnam = Path till init-fil.
 *          create = TRUE  => Skapa
 *                   FALSE => Döda
 *
 *      Ut: Inget.
 *
 *      FV: 0 = Ok.
 *
 *      (C)microform ab 19/12/95 J. Kjellander
 *
 ******************************************************!*/

 {
   char inifil[V3PTHLEN+1],errbuf[V3STRLEN];


/*
***Skapa.
*/
   if ( create )
     {
/*
***Börja med att ladda systemets egen initfil.
*/
     strcpy(inifil,v3genv(VARKON_INI));
     strcat(inifil,"VARKON.INI");
     if ( load_file(inifil) < 0 ) return(-2);
/*
***Skall ytterligare fil laddas ?
*/
     if ( strlen(filnam) > 0 )
       {
/*
***Ja, ladda om filen finns annars felstatus.
*/
       if ( v3ftst(filnam) ) return(load_file(filnam));
       else
         {
         sprintf(errbuf,"Can't load initfile : %s",filnam);
         MessageBox(NULL,errbuf,"VARKON init error !",MB_ICONSTOP | MB_OK);
         return(-2);
         }
       }
     }
/*
***Om vi inte skall skapa skall vi istället deallokera.
*/
   else
     {
     resant = 0;
     return((int)v3free(rtabpek,"msmrdb"));
     }
   return(0);
 }

/********************************************************/
/*!******************************************************/

        bool  msgrst(name,valpek)
        char *name;
        char *valpek;

/*      Returnerar resursvärde.
 *
 *      In: name = Resursnamn
 *
 *      Ut: *valpek = Resursens värde
 *
 *      FV: TRUE  = Resurs med detta namn finns.
 *          FALSE = Resurs med detta namn finns inte.
 *
 *      (C)microform ab 19/12/95 J. Kjellander
 *
 ******************************************************!*/

 {
    int i;

    for ( i=0; i<resant; ++i )
      {
      if ( sticmp(name,(rtabpek+i)->name) == 0 )
        {
        strcpy(valpek,(rtabpek+i)->value);
        return(TRUE);
        }
      }

    strcpy(valpek,"");
    return(FALSE);
 }

/********************************************************/
/*!******************************************************/

 static bool add_res(new_res)
        RESOURCE *new_res;

/*      Lägger till resurs.
 *
 *      In: new_res = Pekare till ny resurs.
 *
 *      Ut: Inget.
 *
 *      FV:  TRUE  = Ok.
 *           FALSE = Kan ej allokera minne.
 *
 *      (C)microform ab 19/12/95 J. Kjellander
 *
 ******************************************************!*/

 {

/*
****Kolla att minne är allokerat för rtab.
*/
   if ( rtabsiz == 0 )
     {
     if ( (rtabpek=(RESOURCE *)v3mall(10*sizeof(RESOURCE),"addres")) == NULL ) return(FALSE);
     rtabsiz = 10;
     }
/*
***Kolla att det finns plats i rtab.
*/
   else if ( resant == rtabsiz )
     {
     if ( (rtabpek=(RESOURCE *)v3rall((char *)rtabpek,
                                             (rtabsiz+10)*sizeof(RESOURCE),
                                             "addres")) == NULL ) return(FALSE);
     rtabsiz += 10;
     }
/*
***Plats finnes. Kopiera resursen till rtab.
*/
   V3MOME(new_res,rtabpek+resant,sizeof(RESOURCE));
/*
***Räkna upp resant.
*/
 ++resant;

   return(TRUE);
 }

/********************************************************/
/*!******************************************************/

 static int   load_file(path)
        char *path;

/*      Laddar resursfil. Ev. tidigare resurser skrivs
 *      ej över.
 *
 *      In: Filnamn.
 *
 *      Ut: Inget.
 *
 *      FV:  0  = Ok.
 *
 *      (C)microform ab 19/12/95 J. Kjellander
 *
 ******************************************************!*/

 {
   int      i;
   char     rad[1024],tmp[1024],ord1[1024],ord2[1024];
   FILE    *fp;
   RESOURCE res;


/*
***Öppna resurs-filen. Om den inte finns är det inget fel.
*/
   if ( (fp=fopen(path,"r")) == NULL ) return(0);
/*
***Läs en rad.
*/
   while ( fgets(rad,1023,fp) != NULL )
     {
     rad[1023] = '\0';
     if ( rad[strlen(rad)-1] == '\n' ) rad[strlen(rad)-1] = '\0';
/*
***Skippa inledande blanka.
*/
     i = 0;
     while ( rad[i] == ' '  ||  rad[i] == '\t' ) ++i;

     if ( i > 0 )
       {
       strcpy(tmp,&rad[i]);
       strcpy(rad,tmp);
       }
/*
***Läs 1:a ordet.
*/
    *ord1 = '\0';
     sscanf(rad,"%s",ord1);
/*
***Om det finns ett 1:a ord läser vi vidare till nästa ord.
*/     
     if ( strlen(ord1) > 0  &&  *ord1 != '!' )
       {
       i = strlen(ord1);
       while ( rad[i] == ' '  ||  rad[i] == '\t'  ||  rad[i] == '=' ) ++i;

       strcpy(ord2,&rad[i]);
/*
***Om det finns ett 2:a ord skippar vi avslutande blanka och tar resten
***som värde.
*/
       if ( (i=strlen(ord2)) > 0 )
         {
       --i;
         while ( i > 0  &&  ord2[i] == ' ' )
           {
           ord2[i] = '\0';
         --i;
           }
/*
***Strippa ev. inledande och avslutande dubbelfnutt i ord2.
*/
         if ( ord2[0] == '"'  &&  ord2[strlen(ord2)-1] == '"' )
           {
           strcpy(tmp,&ord2[1]);
           strcpy(ord2,tmp);
           ord2[strlen(ord2)-1] = '\0';
           }
/*
***Lgra.
*/
         ord1[MAXRESNL] = '\0';
         strcpy(res.name,ord1);
         ord2[MAXRESVL] = '\0';
         strcpy(res.value,ord2);
         if ( !add_res(&res) )
           {
           fclose(fp);
           return(-1);
           }
         }
       }
     }
/*
***Slut.
*/
   fclose(fp);

   return(0);
 }

/********************************************************/

/*!*****************************************************
*
*    exdia2.c
*    ========
*
*    EXinpt();      Interface routine for INPMT
*    EXinfn();      Interface routine for INFNAME
*    EXdirl();      Returns "dir"-list
*
*    This file is part of the VARKON Execute  Library.
*    URL:  http://www.varkon.com
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
*
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#ifdef V3_X11
#include "../../WP/include/WP.h"
#endif
#include "../include/EX.h"
#include <string.h>

#ifdef WIN32
#include <io.h>
#endif 


extern short ialy,ialx;

#ifdef WIN32
extern int msilse();
#endif

/*!******************************************************/

        short EXinpt(
        char   *pmt,
        char   *dstr,
        DBshort ntkn,
        char   *istr)

/*      Interface-rutin för INPMT. Skriver en fråga med
 *      ev. defaultvärde på inmatningsraden och läser in 
 *      en sträng med ev. maxlängd som svar.
 *
 *      In: pmt  = Promtsträng.
 *          dstr = Defaultsträng.
 *          ntkn = Fältvidd.
 *
 *      Ut: *istr = Inläst sträng.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 10/11/88 J. Kjellander
 *
 ******************************************************!*/

  {
    short fwdth;

    if ( ntkn == 0 ) fwdth = V3STRLEN;
    else fwdth = ntkn;

    igglin(pmt,dstr,&fwdth,istr);

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXinfn(
        char *pmt,
        char *dstr,
        char *path,
        char *pattern,
        char *name)

/*      Interface-rutin för INFNAME.
 *
 *      In: pmt     = Rubriktext.
 *          dstr    = Default i editboxen.
 *          path    = Filkatalog.
 *          pattern = Söksträng.
 *          name    = Pekare till utdata.
 *
 *      Ut: *name = Filnamn.
 *
 *      FV:  0     => Ok.
 *
 *      (C)microform ab 1988-04-01 J. Kjellander
 *
 ******************************************************!*/

  {
   short status;
   char *pekarr[1000],strarr[20000];
   DBint nfiles;

/*
***Skapa filförteckning.
*/
   EXdirl(path,pattern,1000,20000,pekarr,strarr,&nfiles);
/*
***Dialogbox.
*/
#ifdef V3_X11
   status = WPilse(20,20,pmt,dstr,pekarr,-1,nfiles,name);
#endif
#ifdef WIN32
   status = (short)msilse(20,20,pmt,dstr,pekarr,-1,nfiles,name);
#endif
/*
***Hur gick det ?
*/
   if ( status < 0 ) *name = '\0';

   return(0);
  }

/********************************************************/
/*!******************************************************/

        short EXdirl(
        char  *inpath,
        char  *pattern,
        DBint  maxant,
        DBint  maxsiz,
        char  *pekarr[],
        char  *strarr,
        DBint *nf)

/*      Gör "ls" på en filkatalog. Denna rutin hette 
 *      tidigare igdir().
 *
 *      In:
 *          inpath  = Sökvägbeskrivning, tex. "/usr/v3/pid".
 *                    Ev. slash på slutet går bra.
 *          pattern = Söksträng, tex. *.RES eller * eller A*B.
 *          maxant  = Max antal filer (pekarr:s storlek)
 *          maxsiz  = Max antal tecken (buf:s storlek).
 *          strarr  = Plats att lagra filnamn.
 *
 *      Ut:
 *          pekarr = Array med nf stycken pekare till filnamn.
 *          nf     = Antal filer.
 *
 *      FV:  0 = Ok.
 *
 *      (C)microform ab 2/11/95 J. Kjellander
 *
 *      1998-04-10 typ->pattern, J.Kjellander
 *
 ******************************************************!*/

  {
   char  lscmd[V3PTHLEN+1],path[V3PTHLEN+1];
   char *actpek;
   int   actsiz,i;


#ifdef WIN32
   struct _finddata_t fildata;
   long               fp;

/*
***Initiering.
*/
   actpek = strarr;
   actsiz = 0;
  *nf     = 0;
/*
***För säkerhets skull kopierar vi path till en
***lokal buffert och strippar ev. '\' och/eller '.' sist.
***path = "." eller ".\" kan tex. förekomma.
*/
   strcpy(path,inpath);

   i = strlen(path) - 1;
   if ( path[i] == '\\' ) path[i] = '\0';

   i = strlen(path) - 1;
   if ( path[i] == '.' ) path[i] = '\0';
/*
***Skapa hela sökkommandot.
*/
   if ( strlen(path) > 0 )
     {
     strcpy(lscmd,path);
     strcat(lscmd,"\\");
     strcat(lscmd,pattern);
     }
   else
     {
     strcpy(lscmd,pattern);
     }
/*
***Öppna directoryt och hämta 1:a filnamnet.
*/
   if ( (fp=_findfirst(lscmd,&fildata)) == -1L ) return(0);
/*
***Sen tar vi resten också.
*/
   else do
     {
     if ( strcmp(fildata.name,".")   != 0  &&
          strcmp(fildata.name,".." ) != 0 )
       {
       strcpy(actpek,fildata.name);
       pekarr[*nf] = actpek;
       actpek += strlen(fildata.name)+1;
       actsiz += strlen(fildata.name)+1;
      *nf += 1;
       }
     } while ( *nf < maxant  &&  actsiz < (maxsiz-JNLGTH-5)  &&
                                _findnext(fp,&fildata) == 0 );

   _findclose(fp);

return(0);
#endif







#ifdef UNIX
   extern FILE *popen();
   extern int   pclose();

   char  buf[V3PTHLEN+1];
   int   n;
   FILE *fp;

/*
***Initiering.
*/
   actpek = strarr;
   actsiz = 0;
  *nf     = 0;
/*
***För säkerhets skull kopierar vi path till en
***lokal buffert och strippar ev. '/' och/eller '.' sist.
***path = "." eller "./" kan tex. förekomma.
*/
   strcpy(path,inpath);

   i = strlen(path) - 1;
   if ( path[i] == '/' ) path[i] = '\0';

   i = strlen(path) - 1;
   if ( path[i] == '.' ) path[i] = '\0';
/*
***En pipe till ls.
*/
   if ( strlen(path) > 0 )
     {
     strcpy(lscmd,"cd ");
     strcat(lscmd,path);
     strcat(lscmd,";ls ");
     strcat(lscmd,pattern);
     }
   else
     {
     strcpy(lscmd,"ls ");
     strcat(lscmd,pattern);
     }

   if ( (fp=popen(lscmd,"r")) == NULL ) return(erpush("IG0442",lscmd));
/*
***Läs en rad i taget. Max 1000 projekt och max 20000 tecken totalt.
*/
   while ( fgets(buf,V3PTHLEN,fp) != NULL  && 
                     *nf < maxant  &&  actsiz+JNLGTH+5 < maxsiz )
     {
     if ( (n=strlen(buf)) > 0 ) buf[n-1] = '\0';
     strcpy(actpek,buf);
     pekarr[*nf] = actpek;
     actpek += strlen(buf)+1;
     actsiz += strlen(buf)+1;
    *nf += 1;
     }

   pclose(fp);

   return(0);
#endif
  }

/********************************************************/

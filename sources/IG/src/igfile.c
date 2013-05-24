/*!******************************************************************/
/*  igfile.c                                                        */
/*  ========                                                        */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGfmov();          Move/Rename file                             */
/*  IGfcpy();          Copy files                                   */
/*  IGfapp();          Add two files                                */
/*  IGfdel();          Delete file                                  */
/*  IGfacc();          Test file accessability                      */
/*  IGftst();          Test file existance                          */
/*  IGmkdr();          Create directory                             */
/*  IGcmpw();          Compare strings with wildcard                */
/*  IGcmos();          Interactive OS-command                       */
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
#include "../../EX/include/EX.h"

#ifdef UNIX
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif

/*!******************************************************/

        short IGfmov(
        char *from,
        char *to)

/*      Ändrar filen "from:s" namn till "to".
 *
 *      In: from => Pekare till gammal vägbeskrivning.
 *          to   => Pekare till ny vägbeskrivning.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          IG0482 => Kan ej byta namn på filen %s
 *          IG0512 => Kan ej skapa filen %s
 *          IG0632 => MOVE_FILE, %s är öppen.
 *
 *      (C)microform ab 1996-01-25 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Kolla att ingen av filerna är öppnade från MBS.
*/
   if ( v3isop(from) ) return(erpush("IG0632",from));
   if ( v3isop(to) )   return(erpush("IG0632",to));
/*
***I WIN32 får inte en fil med samma namn finnas sedan tidigare.
*/
#ifdef WIN32
    if ( IGfacc(to,'X') ) IGfdel(to);
#endif
/*
***Byt namn.
*/
    if ( rename(from,to) == 0 ) return(0);
    else
      {
      erpush("IG0482",from);
      return(erpush("IG0512",to));
      }
/*
***Slut.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

        short IGfcpy(
        char *from,
        char *to)

/*      Kopierar filen "from" till filen "to".
 *
 *      In: from => Pekare till vägbeskrivning för gamla filen.
 *          to   => Pekare till vägbeskrivning för nya filen.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          IG0073 => Kan ej öppna filen %s
 *          IG0063 => Fel vid skrivning till %s
 *          IG0622 => COPY_FILE, %s är öppen.
 *
 *      (C)microform ab 30/7/85 J. Kjellander
 *
 *      9/1/96  WIN32, J. Kjellander
 *
 ******************************************************!*/

  {
     FILE  *frompk;
     FILE  *topk;
     char   buf[BUFSIZ];
     size_t n;

/*
***Kolla att ingen av filerna är öppnade från MBS.
*/
   if ( v3isop(from) ) return(erpush("IG0622",from));
   if ( v3isop(to) )   return(erpush("IG0622",to));
/*
***Prova att öppna den gamla filen.
*/
#ifdef WIN32
     if ( (frompk=fopen(from,"rb")) == 0 ) return(erpush("IG0073",from));
#else
     if ( (frompk=fopen(from,"r")) == 0 ) return(erpush("IG0073",from));
#endif
/*
***Öppna den nya filen.
*/
#ifdef WIN32
     if ( (topk=fopen(to,"wb")) == 0 ) return(erpush("IG0073",to));
#else
     if ( (topk=fopen(to,"w")) == 0 ) return(erpush("IG0073",to));
#endif
/*
***Kopiera.
*/
loop:
     n = fread(buf,1,BUFSIZ,frompk);

     if ( n != 0 )
       {
       if ( fwrite(buf,1,n,topk) == n ) goto loop;
       else return(erpush("IG0063",from));
       }
/*
***Stäng filerna.
*/
     fclose(frompk);
     fclose(topk);

     return(0);
  }

/********************************************************/
/*!******************************************************/

        short IGfapp(
        char *from,
        char *to)

/*      Lägger till filen "from" sist i filen "to".
 *
 *      In: from => Pekare till fil att addera till to.
 *          to   => Pekare till fil som det skall läggas till i.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          IG0073 => Kan ej öppna filen %s
 *          IG0063 => Fel vid skrivning till %s
 *          IG0602 => APPEND_FILE, %s är öppen.
 *
 *      (C)microform ab 1996-01-25 J. Kjellander
 *
 ******************************************************!*/

  {
     FILE  *frompk;
     FILE  *topk;
     char   buf[BUFSIZ];
     size_t n;
/*
***Kolla att ingen av filerna är öppnade från MBS.
*/
   if ( v3isop(from) ) return(erpush("IG0602",from));
   if ( v3isop(to) )   return(erpush("IG0602",to));
/*
***Prova att öppna den fil det skall läsas ur.
*/
#ifdef WIN32
     if ( (frompk=fopen(from,"rb")) == 0 ) return(erpush("IG0073",from));
#else
     if ( (frompk=fopen(from,"r")) == 0 ) return(erpush("IG0073",from));
#endif
/*
***Öppna den fil som det skall skrivas (sist) i.
*/
#ifdef WIN32
     if ( (topk=fopen(to,"ab")) == 0 ) return(erpush("IG0073",to));
#else
     if ( (topk=fopen(to,"a")) == 0 ) return(erpush("IG0073",to));
#endif
/*
***Läs och skriv.
*/
loop:
     n = fread(buf,1,BUFSIZ,frompk);

     if ( n != 0 )
       {
       if ( fwrite(buf,1,n,topk) == n ) goto loop;
       else return(erpush("IG0063",from));
       }
/*
***Stäng filerna.
*/
     fclose(frompk);
     fclose(topk);

     return(0);
  }

/********************************************************/
/*!******************************************************/

        short IGfdel(char *fil)

/*      Tar bort en fil.
 *
 *      In: fil => Pekare till vägbeskrivning för filen.
 *
 *      Ut: Inget.
 *
 *      Felkod: IG0492 = Kan ej ta bort filen %s
 *              IG0612 => DELETE_FILE, %s är öppen.
 *
 *      (C)microform ab 29/11/88 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Kolla att inte filen är öppnad från MBS.
*/
   if ( v3isop(fil) ) return(erpush("IG0612",fil));
/*
***Ta bort den.
*/
#ifdef UNIX
     if ( unlink(fil) == 0 ) return(0);
     else                    return(erpush("IG0492",fil));
#endif

#ifdef WIN32
     if ( unlink(fil) == 0 ) return(0);
     else                    return(erpush("IG0492",fil));
#endif

  }

/********************************************************/
/*!******************************************************/

        bool IGfacc(
        char *fil,
        char  mode)

/*      Kollar om fil kan accessas på visst sätt.
 *
 *      In: fil  = Pekare till vägbeskrivning för filen.
 *          mode = "R", "W" eller "X".
 *
 *      Ut: Inget.
 *
 *      FV: TRUE  = Filen kan accessas på det sätt som önskats.
 *          FALSE = Det går inte.
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 ******************************************************!*/

  {
   int status,amode;
/*
***Vilken accessmode önskas ?
*/
   switch ( mode )
     {
#ifdef UNIX
     case 'r': case 'R': amode = R_OK; break;
     case 'w': case 'W': amode = W_OK; break;
     case 'x': case 'X': amode = F_OK; break;
#endif
#ifdef WIN32
     case 'r': case 'R': amode = 4; break;
     case 'w': case 'W': amode = 2; break;
     case 'x': case 'X': amode = 0; break;
#endif
     default: return(FALSE);
     }
/*
***Anropa access().
*/
   status = access(fil,amode);
/*
***Hur gick det ?
*/
   if ( status == 0 ) return(TRUE);
   else               return(FALSE);
  }

/********************************************************/
/*!******************************************************/

        bool IGftst(char *fil)

/*      Kollar om fil går att öppna för läsning. OBS,
 *      i Win95 får denna rutin inte användas på en
 *      filkatalog-fil!!!!!
 *
 *      In: fil => Pekare till vägbeskrivning för filen.
 *
 *      Ut: Inget.
 *
 *      FV: TRUE  = Filen finns.
 *          FALSE = Filen finns ej eller går ej att öppna.
 *
 *      (C)microform ab 27/11/88 J. Kjellander
 *
 ******************************************************!*/

  {
     FILE  *f;

/*
***Prova att öppna den gamla filen.
*/
     if ( (f=fopen(fil,"r")) == 0 ) return(FALSE);
     else
       {
       fclose(f);
       return(TRUE);
       }
  }

/********************************************************/
/*!******************************************************/

        short IGmkdr(char *dirnam)

/*      Skapar directory.
 *
 *      In: fil => Pekare till vägbeskrivning för filkatalog.
 *
 *      Ut: Inget.
 *
 *      FV:   0 = Filkatalog skapad.
 *          < 0 = Kan ej skapa filkatalog.
 *
 *      Felkoder: IG0402 = Kan ej skapa filkatalog %s, fel från OS !
 *
 *      (C)microform ab 22/9/95 J. Kjellander
 *
 *      1998-02-06 IXUSR etc. J.Kjellander
 *
 ******************************************************!*/

  {
#ifdef UNIX
    if ( mkdir(dirnam,S_IXUSR | S_IRUSR | S_IWUSR | S_IROTH |
                                 S_IXOTH | S_IRGRP | S_IXGRP) < 0 )
      {
      return(erpush("IG0402",dirnam));
      }
    else
      {
      return(0);
      }
#endif

#ifdef WIN32
    if ( mkdir(dirnam) < 0 ) return(erpush("IG0402",dirnam));
    else                     return(0);
#endif
  }

/********************************************************/
/*!******************************************************/

        bool  IGcmpw(
        char *wc_str,
        char *tststr)

/*      Jämför 2 strängar varav en med wildcard "*".
 *      Godtyckligt antal stjärnor får förekomma i
 *      söksträngen (wc_str) varsomhelst men inte intill
 *      varandra. Om så returneras FALSE.
 *
 *      Om någon av strängarna = "" returneras FALSE.
 *
 *      In: wc_str => Sträng med 0, 1 eller flera stjärnor.
 *          tststr => Sträng utan stjärnor.
 *
 *      Ut:  TRUE  = Strängarna matchar.
 *           FALSE = Matchar ej.
 *
 *      (C)microform ab 1996-02-12 J. Kjellander
 *
 *      1998-04-02 Bug, J.Kjellander
 *
 ******************************************************!*/

  {
   register char *p1,*p2;

/*
***Initiering.
*/
   p1 = wc_str;
   p2 = tststr;
/*
***Innan vi börjar jämföra måste vi kolla om någon eller
***båda = "" eftersom resten av rutinen förutsätter att
***så inte är fallet.
*/
   if ( *p1 == '\0'  ||  *p2 == '\0' ) return(FALSE);
/*
***Ingen av strängarna är "". Sätt igång och jämför!
*/
loop1:
/*
***Är det en stjärna.
*/
   if ( *p1 == '*' )
     {
   ++p1;
/*
***Ja, söksträngen slutar med en stjärna.
*/
     if ( *p1 == '\0' ) return(TRUE);
/*
***2 stjärnor efter varandra är inte tillåtet.
*/
     else if ( *p1 == '*' ) return(FALSE);
/*
***Stjärnan ej i slutet. Leta upp nästa tecken i p2 som matchar
***tecknet efter stjärnan.
*/
     else
       {
loop2:
/*
***Ett tecken i p2 matchar tecknet efter stjärnan i p1.
***Strippa eventuella ytterligare lika tecken i p2 som inte
***matchar nästa tecken i p1. Tex. p1=*poi och p2=me_ppoi
***1998-04-02 JK.
*/
       if ( *p1 == *p2 )
         {
         while ( *p1 == *(p2+1)  &&  *(p2+1) != *(p1+1) ) ++p2;
         goto loop1;
         }
       else
         {
       ++p2;
         if ( *p2 == '\0' ) return(FALSE);
         goto loop2;
         }
       }
     }
/*
***Ingen stjärna, jämför 2 riktiga tecken.
*/
   else
     {
/*
***Lika.
*/
     if ( *p1 == *p2 )
       {
     ++p1;
     ++p2;
       if ( *p1 == '\0' )
         {
/*
***p1 och p2 båda slut.
*/
         if ( *p2 == '\0' ) return(TRUE);
/*
***p1 slut men inte p2.
*/
         else return(FALSE);
         }
       else
         {
/*
***p1 inte slut men p2.
*/
         if ( *p2 == '\0' )
           {
           if ( *p1 == '*'  &&  *(p1+1) == '\0' ) return(TRUE);
           else                                   return(FALSE);
           }
/*
***Varken p1 eller p2 slut.
*/
         else goto loop1;
         }
       }
/*
***Ej lika.
*/
     else return(FALSE);
     }
  }

/********************************************************/
/*!******************************************************/

       short IGcmos(char oscmd[])

/*      Interaktivt kommando till OS.
 *
 *      In: oscmd -> Kommandosträng
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 5/3/88 J. Kjellander
 *
 *      4/11/88 CGI, N220G, J. Kjellander
 *
 ******************************************************!*/

 {
   char  s[2];
   short status;

/*
***Starta upp ny process och vänta tills den är klar.
***Oavsett terminaltyp har igintt() gjorts i v3.c, alltså
***måste igextt göras nu.
*/
   EXos(oscmd,(short)0);
/*
***Vänta på användarens <CR>.
*/
   status = IGssip("",IGgtts(3),s,"",1);
/*
***Slut.
*/
   return(status);
 }

/*********************************************************************/

/*!******************************************************************/
/*  igPID.c                                                         */
/*  =======                                                         */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  igchpr();  Change PID                                           */
/*  igcnpr();  Create new PID                                       */
/*  igselp();  Select PID                                           */
/*  iglspr();  List/edit PID                                        */
/*  igdlpr();  Delete PID                                           */
/*  igldpf();  Load PID file                                        */
/*  igdir();   Create directory listing                             */
/*  igckpr();  Check PID accessability                              */
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
#include "../../DB/include/DBintern.h"
#include "../include/IG.h"
#include "../../EX/include/EX.h"
#include "../../WP/include/WP.h"
#include <string.h>

extern char   pidnam[],jobdir[],jobnam[],amodir[],
              asydir[],hlpdir[],mdffil[],tmprit[],
              mbsdir[],mbodir[];
extern bool   jnflag;
extern short  v3mode,igtrty,actfun;

static char *pidpath();   /* Returnerar path till PID-katalogen */

/*!******************************************************/

       short  igchpr()

/*     Varkon-funktion för byt projekt. Om projektet
 *     inte finns skapas ett. 
 *
 *      Felkoder: 
 *
 *      (C) microform ab 27/9/95 J. Kjellander.
 *
 *******************************************************!*/

 {
   short status;
   char  oldpid[JNLGTH+1],newpid[JNLGTH+1];
   char  oldjob[JNLGTH+1],newjob[JNLGTH+1];
   char  pidfil[V3PTHLEN+1],resfil[V3PTHLEN+1];
   bool  saveflag;

/*
***Lite initiering.
*/
   strcpy(oldpid,pidnam);
   strcpy(oldjob,jobnam);
/*
***Byta projekt får man inte göra utan att avsluta aktivt jobb.
***Skall det lagras eller inte.
*/
   saveflag = igialt(194,67,68,FALSE);
/*
***Om jobbet skall sparas men inte har nåt namn frågar
***vi efter nytt namn här.
*/
   if ( saveflag  &&  !jnflag )
       {
       igptma(193,IG_INP);
       if ( (status=igssip(iggtts(400),newjob,"",JNLGTH)) < 0 )
         {
         igrsma();
         return(status);
         }
       igrsma();
       }
/*
***Välj ett nytt projektnamn.
*/
   status = igselp(newpid);
   if      ( status == REJECT ) return(REJECT);
   else if ( status <  0 )
     {
     errmes();
     return(0);
     }
/*
***Om projektet inte finns kanske vi skall skapa ett.
*/
   sprintf(pidfil,"%s%s%s",pidpath(),newpid,PIDEXT);

   if ( strcmp(newpid,".") != 0  &&  !v3ftst(pidfil) )
     {
     if ( igialt(195,67,68,TRUE) )
       {
       if ( igcnpr(newpid) < 0 )
         {
         errmes();
         return(0);
         }
       }
     else return(0);
     }
/*
***Vilket jobb på det nya projektet.
***För att igselj() skall kunna lista dem som finns
***måste vi tillfälligt aktivera projektet och sen
***aktivera det gamla igen så att det gamla jobbet
***lagras på rätt ställe.
*/
   if ( igldpf(newpid) < 0  ||  igckpr() < 0 )
     {
     igldpf(oldpid);
     errmes();
     return(0);
     }
   strcpy(pidnam,newpid);
   status = igselj(newjob);
   strcpy(pidnam,oldpid);
   igldpf(oldpid);

   if      ( status == REJECT ) return(REJECT);
   else if ( status <  0 )
     {
     errmes();
     return(0);
     }
/*
***Nu är det dags att avsluta aktivt jobb.
*/
   if ( saveflag )
     { 
     if ( !jnflag )
       {
       if ( igchjn(newjob) < 0 )
         {
         errmes();
         return(0);
         }
       else jnflag = TRUE;
       }
     igsjpg();
     }
/*
***Aktivt jobb skall inte lagras.
*/
   else
     {
     gmclpf();

     if ( v3mode == RIT_MOD ) v3fdel(tmprit);
     else
       {
       strcpy(resfil,jobdir);
       strcat(resfil,jobnam);
       strcat(resfil,RESEXT);
       v3fdel(resfil);
       }
     }
/*
***Vi är nu tillfälligt utan jobb.
***Innan nytt jobb startas laddar vi det nya projektet.
***Om detta inte går laddar vi tillbaks såväl det gamla
***projektet som jobbet igen.
*/
   if ( igldpf(newpid) < 0 )
     {
     igldpf(oldpid);
     errmes();
     }
   else
     {
     strcpy(pidnam,newpid);
     strcpy(jobnam,newjob);
     }
/*
***Byter man projekt måste man ladda nya menyer. 
***Om inte det går måste vi ladda tilbaks både projekt
***och gamla menyer igen.
*/
   if ( iginit(mdffil) < 0 )
     {
     erpush("IG0262",mdffil);
     igldpf(oldpid);
     strcpy(pidnam,oldpid);
     strcpy(jobnam,oldjob);
     iginit(mdffil);
     errmes();
     }
/*
***Om PID-fil eller menyer inte gick bra att ladda
***laddar vi tillbaks det gamla jobbet igen.
***Om allt gick bra laddar vi det nya jobbet.
*/
   WPclrg();
   WPexfn();

   status = igload();

   if ( status < 0 )
     {
     if ( status != REJECT  &&  status != GOMAIN ) errmes();
     if (saveflag )
       {
       igldpf(oldpid);
       strcpy(pidnam,oldpid);
       strcpy(jobnam,oldjob);
       iginit(mdffil);
       WPclrg();
       WPexfn();
       if ( igload() < 0 ) return(EREXIT);
       else return(GOMAIN);
       }
     else return(EXIT);
     }
   else
     {
     jnflag = TRUE;
     return(GOMAIN);
     }
  }

/********************************************************/
/*!******************************************************/

       short  igcnpr(char *newpid)

/*     Varkon-funktion för att skapa nytt projekt.
 *
 *     In: newpid = Pekare till projektnamn.
 *
 *     FV: -1 = PID-filen finns redan.
 *
 *     Felkoder :  IG0272 = Kan ej skapa PID-fil %s
 *                 IG0412 = Kan ej skapa menyfil %s
 *
 *     (C) microform ab 22/9/95  J. Kjellander.
 *
 *     1996-12-20 mbs, J.Kjellander
 *     1997-01-20 Skriv ej över MDF-fil, J.Kjellander
 *     1997-01-20 v3facc(), J.Kjellander
 *
 ******************************************************!*/

 {
   short status;
   char  pidfil[V3PTHLEN+1],newrot[V3PTHLEN+1],
         newjob[V3PTHLEN+1],newlib[V3PTHLEN+1],newsym[V3PTHLEN+1],
         newmbs[V3PTHLEN+1],newdoc[V3PTHLEN+1],newmdf[V3PTHLEN+1];
   FILE  *pidfpk,*mdffpk;

/*
***Skapa fullständig vägbeskrivning till aktuell PID-fil.
*/
    sprintf(pidfil,"%s%s%s",v3genv(VARKON_PID),newpid,PIDEXT);
/*
***Kolla att PID-filen inte redan finns.
*/
   if ( v3ftst(pidfil) == TRUE ) return(-1);
/*
***Kolla att ny pidfil går att skapa. Filkatalogen för PID-filer
***kanske inte finns eller rättigheter kanske saknas etc.
*/
   if ( (pidfpk=fopen(pidfil,"w+")) == 0 ) return(erpush("IG0272",pidfil));
   else                                    fclose(pidfpk);
/*
***Det verkar gå bra. Testet ovan har nu resulterat i en tom fil.
***Denna tar vi bort så att det inte lämnas tomma filer efter oss
***om vi avbryter längre ner.
*/
   v3fdel(pidfil);
/*
***Skapa rootkatalognamn för det nya projektet.
*/
   sprintf(newrot,"%s%s",v3genv(VARKON_PRD),newpid);
/*
***Lägg på root-directoryt på katalognamnen.
*/
#ifdef UNIX
   sprintf(newjob,"%s/job",newrot);
   sprintf(newlib,"%s/lib",newrot);
   sprintf(newsym,"%s/sym",newrot);
   sprintf(newdoc,"%s/doc",newrot);
   sprintf(newmdf,"%s/mdf",newrot);
   sprintf(newmbs,"%s/mbs",newrot);
#endif

#ifdef WIN32
   sprintf(newjob,"%s\\job",newrot);
   sprintf(newlib,"%s\\lib",newrot);
   sprintf(newsym,"%s\\sym",newrot);
   sprintf(newdoc,"%s\\doc",newrot);
   sprintf(newmdf,"%s\\mdf",newrot);
   sprintf(newmbs,"%s\\mbs",newrot);
#endif
/*
***Om inte filkatalogerna finns, skapa dem. v3ftst() funkar inte
***på filkataloger i Win95 men v3facc(path,'X') går bra.
*/
   if ( v3facc(newrot,'X') == FALSE  &&  (status=v3mkdr(newrot)) < 0 )
     return(status);

   if ( v3facc(newjob,'X') == FALSE  &&  (status=v3mkdr(newjob)) < 0 )
     return(status);

   if ( v3facc(newlib,'X') == FALSE  &&  (status=v3mkdr(newlib)) < 0 )
     return(status);

   if ( v3facc(newsym,'X') == FALSE  &&  (status=v3mkdr(newsym)) < 0 )
     return(status);

   if ( v3facc(newdoc,'X') == FALSE  &&  (status=v3mkdr(newdoc)) < 0 )
     return(status);

   if ( v3facc(newmdf,'X') == FALSE  &&  (status=v3mkdr(newmdf)) < 0 )
     return(status);

   if ( v3facc(newmbs,'X') == FALSE  &&  (status=v3mkdr(newmbs)) < 0 )
     return(status);
/*
***Skapa menyfil om den inte finns.
*/
#ifdef UNIX
   sprintf(newmdf,"%s/%s%s",newmdf,newpid,MDFEXT);
#endif

#ifdef WIN32
   sprintf(newmdf,"%s\\%s%s",newmdf,newpid,MDFEXT);
#endif

   if ( v3ftst(newmdf) == FALSE )
     {
     if ( (mdffpk=fopen(newmdf,"w+")) == 0 ) return(erpush("IG0412",newmdf));
     else
       {
#ifdef WIN32
       fprintf(mdffpk,"#include %c$VARKON_MDF\\v319.MDF%c\n",'\042','\042');
#else
       fprintf(mdffpk,"#include %c$VARKON_MDF/v318.MDF%c\n",'\042','\042');
#endif
       fclose(mdffpk);
       }
     }
/*
***Skapa själva PID-filen. Här skriver vi ut vägbeskrivningar
***relativt $VARKON_PRD.
*/
#ifdef UNIX
   sprintf(newjob,"$VARKON_PRD/%s/job",newpid);
   sprintf(newlib,"$VARKON_PRD/%s/lib",newpid);
   sprintf(newsym,"$VARKON_PRD/%s/sym",newpid);
   sprintf(newdoc,"$VARKON_PRD/%s/doc",newpid);
   sprintf(newmdf,"$VARKON_PRD/%s/mdf/%s%s",newpid,newpid,MDFEXT);
   sprintf(newmbs,"$VARKON_PRD/%s/mbs",newpid);
#endif

#ifdef WIN32
   sprintf(newjob,"$VARKON_PRD\\%s\\job",newpid);
   sprintf(newlib,"$VARKON_PRD\\%s\\lib",newpid);
   sprintf(newsym,"$VARKON_PRD\\%s\\sym",newpid);
   sprintf(newdoc,"$VARKON_PRD\\%s\\doc",newpid);
   sprintf(newmdf,"$VARKON_PRD\\%s\\mdf\\%s%s",newpid,newpid,MDFEXT);
   sprintf(newmbs,"$VARKON_PRD\\%s\\mbs",newpid);
#endif

    pidfpk = fopen(pidfil,"w+");
    fprintf(pidfpk,"%s\n%s\n%s\n%s\n%s\n%s\n",newjob,newlib,
                                      newsym,newdoc,newmdf,newmbs);
    fclose(pidfpk);
/*
***Ladda den nya PID-filen.
*/
    strcpy(pidnam,newpid);
    igldpf(pidnam);

    return(0);
 }

/******************************************************!*/
/*!******************************************************/

        short igselp(char *projekt)

/*      Interaktiv funktion för att välja projekt.
 *
 *      In: projekt = Pekare till utdata.
 *
 *      Ut: *projekt = Projektnamn eller odefinierat.
 *
 *      FV:  0      = Ok, projekt = definierat.
 *           REJECT = Avbryt, projekt oförändrat.
 *          -1      = Kan ej skapa pipe till "ls".
 *
 *      Felkoder: IG0432 = %s är ett otillåtet projekt.
 *                IG0442 = Kan ej öppna pipe %s
 *                IF0462 = Finns ej i VMS.
 *
 *      (C)microform ab 25/9/95 J. Kjellander
 *
 *      1998-11-03 actfun, J.Kjellander
 *
 ******************************************************!*/

  {
   short status,oldafu;
   char *pekarr[1000],strarr[20000];
   char  path[V3PTHLEN+1];
   int   i,nstr,actalt;

/*
***Aktiv funktion, specialare för hjälp-systemet.
*/
   oldafu = actfun;
   actfun = 1000;
/*
***Skapa filförteckning.
*/
   strcpy(path,v3genv(VARKON_PID));
   igdir(path,PIDEXT,1000,20000,pekarr,strarr,&nstr);
/*
***Vilket av dem är aktivt ?
*/
   for ( i=0; i<nstr; ++i ) if ( strcmp(pekarr[i],pidnam) == 0 ) break;

   if ( i < nstr ) actalt =  i;
   else            actalt = -1;
/*
***Låt användaren välja.
*/   
#ifdef V3_X11
   status = WPilse(20,20,iggtts(401),"",pekarr,actalt,nstr,projekt);
#endif
#ifdef WIN32
   status = msilse(20,20,iggtts(401),"",pekarr,actalt,nstr,projekt);
#endif
  
   actfun = oldafu;
 
   if ( status <  0 ) return(status);
/*
***Han kan ha matat in ett PID-namn från tangentbordet så det är
***bäst att kolla att det följer reglerna.
*/
   if ( strcmp(projekt,".") != 0  &&  igckjn(projekt) < 0 )
     return(erpush("IG0432",projekt));
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

       short  iglspr()

/*     Interaktiv funktion för att lista/ändra PID-filer.
 *
 *      (C) microform ab 26/9/95  J. Kjellander.
 *
 *     Felkoder: IG0562 = Fel vid ändring av projekt
 *               IG0572 = MDF-fil finns ej
 *               IG0582 = Ej ändra jobkat då projektet aktivt
 *               IG0592 = Ej ändra menyfil då projektet aktivt
 *
 *     1997-01-03 igmsip(), J.Kjellander
 *     1997-01-20 v3facc(), J.Kjellander
 *
 *******************************************************!*/

 {
   short status,ml[6];
   int   i;
   char  projekt[JNLGTH+1],pfnam[V3PTHLEN+1],rubrik[V3STRLEN];
   char  tmpjob[V3PTHLEN+1],tmpamo[10*V3PTHLEN+10],
         tmpasy[V3PTHLEN+1],tmphlp[V3PTHLEN+1],
         tmpmdf[V3PTHLEN+1],tmpmbs[V3PTHLEN+1],
         tmpbuf[V3PTHLEN+1],tmpmbo[V3PTHLEN+1];
   char  ps[6][V3STRLEN],*p_ps[6],
         is[6][10*V3PTHLEN+10],*p_is[6],
         ds[6][10*V3PTHLEN+10],*p_ds[6];
   FILE *pidfpk;

/*
***Välj ett projekt.
*/
   status = igselp(projekt);
   if      ( status == REJECT ) return(REJECT);
   else if ( status <  0 )
     {
     errmes();
     return(0);
     }
/*
***Bilda pidfilsnamn.
*/
   strcpy(pfnam,pidpath());
   strcat(pfnam,projekt);
   strcat(pfnam,PIDEXT);
/*
***Läs PID-filen.
*/
   if ( (pidfpk=fopen(pfnam,"r")) == 0 )
     {
     erpush("IG0242",projekt);
     errmes();
     return(0);
     }

   fscanf(pidfpk,"%s",tmpjob);
   fscanf(pidfpk,"%s",tmpamo);
   fscanf(pidfpk,"%s",tmpasy);
   fscanf(pidfpk,"%s",tmphlp);
   fscanf(pidfpk,"%s",tmpmdf);
   if ( fscanf(pidfpk,"%s",tmpmbs) != 1 ) strcpy(tmpmbs,tmpjob);

   fclose(pidfpk);
/*
***Rubrik.
*/
   strcpy(rubrik,iggtts(187));
   strcat(rubrik,projekt);
/*
***Initiera promtar.
*/
   strcpy(ps[0],iggtts(188));
   strcpy(ps[1],iggtts(192));
   strcpy(ps[2],iggtts(1601));
   strcpy(ps[3],iggtts(189));
   strcpy(ps[4],iggtts(191));
   strcpy(ps[5],iggtts(190));

   for ( i=0; i<6; ++i ) p_ps[i] = ps[i];
/*
***Initiera input. Här räcker det med pekare.
*/
   for ( i=0; i<6; ++i ) p_is[i] = is[i];
/*
***Initiera defaultsträngar.
*/
   strcpy(ds[0],tmpjob);
   strcpy(ds[1],tmpmdf);
   strcpy(ds[2],tmpmbs);
   strcpy(ds[3],tmpamo);
   strcpy(ds[4],tmphlp);
   strcpy(ds[5],tmpasy);

   for ( i=0; i<6; ++i ) p_ds[i] = ds[i];
/*
***Maxlängder.
*/
   for ( i=0; i<6; ++i ) ml[i] = (short)(10*V3PTHLEN);
/*
***Editera.
*/
   igplma(rubrik,IG_INP);
   status = igmsip(p_ps,p_is,p_ds,ml,(short)6);
   igrsma();
   if ( status == REJECT ) return(REJECT);
/*
***Om inget har ändrats kan vi sluta här.
*/
   if ( strcmp(tmpjob,is[0]) == 0  &&
        strcmp(tmpmdf,is[1]) == 0  &&
        strcmp(tmpmbs,is[2]) == 0  &&
        strcmp(tmpamo,is[3]) == 0  &&
        strcmp(tmphlp,is[4]) == 0  &&
        strcmp(tmpasy,is[5]) == 0 ) return(0);
/*
***Har jobkatalogen ändrats ?
***Isåfall kollar vi att den finns eller skapar den men
***om projektet är aktivt får den inte ändras !
*/
   if ( strcmp(tmpjob,is[0]) != 0  )
     {
     if ( strcmp(projekt,pidnam) == 0 )
       {
       erpush("IG0582","");
       errmes();
       return(0);
       }
     else
       {
       v3trfp(is[0],tmpjob);
       if ( v3facc(tmpjob,'X') == FALSE )
         {
         if ( igialt(1602,67,68,TRUE) )
           {
           if ( v3mkdr(tmpjob) < 0 )
             {
             erpush("IG0562",projekt);
             errmes();
             return(0);
             }
           }
         else return(0);
         }
       }
     }
/*
***Har mdf-filen ändrats ?
***Isåfall kollar vi att den finns men om
***projektet är aktivt får den inte ändras !
*/
   if ( strcmp(tmpmdf,is[1]) != 0  )
     {
     if ( strcmp(projekt,pidnam) == 0 )
       {
       erpush("IG0592","");
       errmes();
       return(0);
       }
     else
       {
       v3trfp(is[1],tmpmdf);
       if ( v3ftst(tmpmdf) == FALSE )
         {
         erpush("IG0572",tmpmdf);
         erpush("IG0562",projekt);
         errmes();
         return(0);
         }
       }
     }
/*
***Har MBS-katalogen ändrats ?
***Kolla isåfall att den finns eller skapa den.
*/
   if ( strcmp(tmpmbs,is[2]) != 0  )
     {
     v3trfp(is[2],tmpmbs);
     if ( v3facc(tmpmbs,'X') == FALSE )
       {
       if ( igialt(1603,67,68,TRUE) )
         {
         if ( v3mkdr(tmpmbs) < 0 )
           {
           erpush("IG0562",projekt);
           errmes();
           return(0);
           }
         }
       else return(0);
       }
     }
/*
***Har amodir ändrats ?
***Klipp ut mbodir ur amodir och kolla att den finns.
*/
   if ( strcmp(tmpamo,is[3]) != 0  )
     {
     for ( i=0; i<(int)strlen(is[3]); ++i )
       {
#ifdef UNIX
       if ( is[3][i] != ':'  &&  is[3][i] != ';' ) tmpbuf[i] = is[3][i];
#endif
#ifdef WIN32
       if ( is[3][i] != ';' ) tmpbuf[i] = is[3][i];
#endif
       else break;
       }
     tmpbuf[i] = '\0';

     v3trfp(tmpbuf,tmpmbo);

       {
       if ( v3facc(tmpmbo,'X') == FALSE )
         {
         if ( igialt(1604,67,68,TRUE) )
           {
           if ( v3mkdr(tmpmbo) < 0 )
             {
             erpush("IG0562",projekt);
             errmes();
             return(0);
             }
           }
         else return(0);
         }
       }
     }
/*
***Har hlp-katalogen ändrats ?
***Kolla isåfall att den finns.
*/
   if ( strcmp(tmphlp,is[4]) != 0  )
     {
     v3trfp(is[4],tmphlp);
     if ( v3facc(tmphlp,'X') == FALSE )
       {
       if ( igialt(1605,67,68,TRUE) )
         {
         if ( v3mkdr(tmphlp) < 0 )
           {
           erpush("IG0562",projekt);
           errmes();
           return(0);
           }
         }
       else return(0);
       }
     }
/*
***Har sym-katalogen ändrats ?
***Kolla isåfall att den finns.
*/
   if ( strcmp(tmpasy,is[5]) != 0  )
     {
     v3trfp(is[5],tmpasy);
     if ( v3facc(tmpasy,'X') == FALSE )
       {
       if ( igialt(1606,67,68,TRUE) )
         {
         if ( v3mkdr(tmpasy) < 0 )
           {
           erpush("IG0562",projekt);
           errmes();
           return(0);
           }
         }
       else return(0);
       }
     }
/*
***Något är ändrat. Fråga om det skall ändras nu !
*/
   if ( !igialt(1607,67,68,TRUE) ) return(0);
/*
***Uppdatera pid-filen. Här skriver vi ut de strängar
***som vi fick från igmsip() utan att översätta eventuella
***env-namn till klartext.
*/
   pidfpk = fopen(pfnam,"w+");
   fprintf(pidfpk,"%s\n%s\n%s\n%s\n%s\n%s\n",is[0],is[3],
                                       is[5],is[4],is[1],is[2]);
   fclose(pidfpk);
/*
***Om projektet är aktivt skall även globala variabler uppdateras.
***Detta gör vi genom att ladda filen igen. På sås vis garanterar
***vi också att det blir en slash i slutet på dom kataloger som
***skall ha det.
*/
   if ( strcmp(projekt,pidnam) == 0 ) igldpf(projekt);
/*
***Slut.
*/
   return(0);
 }

/******************************************************!*/
/*!******************************************************/

       short  igdlpr()

/*     Varkon-funktion för att ta bort projekt.
 *
 *     Felkod: IG0242 = Projektet finns ej.
 *
 *     (C) microform ab 27/9/95  J. Kjellander.
 *
 *     1997-01-07 filer, J.Kjellander
 *
 *******************************************************!*/

 {
   short status;
   char  projekt[JNLGTH+1],pfnam[V3PTHLEN+1];
   char  tmpjob[V3PTHLEN+1],tmpamo[10*V3PTHLEN+10],
         tmpasy[V3PTHLEN+1],tmphlp[V3PTHLEN+1],
         tmpmdf[V3PTHLEN+1],tmpmbs[V3PTHLEN+1];
   FILE *pidfpk;
/*
***Välj ett projekt.
*/
   status = igselp(projekt);
   if      ( status == REJECT ) return(REJECT);
   else if ( status <  0 )
     {
     errmes();
     return(0);
     }
/*
***Bilda pidfilsnamn.
*/
   strcpy(pfnam,pidpath());
   strcat(pfnam,projekt);
   strcat(pfnam,PIDEXT);
/*
***Läs PID-filen.
*/
   if ( (pidfpk=fopen(pfnam,"r")) == 0 )
     {
     erpush("IG0242",projekt);
     errmes();
     return(0);
     }

   fscanf(pidfpk,"%s",tmpjob);
   fscanf(pidfpk,"%s",tmpamo);
   fscanf(pidfpk,"%s",tmpasy);
   fscanf(pidfpk,"%s",tmphlp);
   fscanf(pidfpk,"%s",tmpmdf);
   if ( fscanf(pidfpk,"%s",tmpmbs) != 1 ) strcpy(tmpmbs,tmpjob);

   fclose(pidfpk);
/*
***Ta bort PID-filen.
*/
   v3fdel(pfnam);
   igwtma(107);
/*
***Slut.
*/
   return(0);
 }

/******************************************************!*/
/*!******************************************************/

        short igldpf(char *filnam)

/*      Laddar PID-fil.
 *
 *      In: filnam = Pekare till pidfilsnamn utan path.
 *
 *      (C)microform ab 3/3/88 J. Kjellander
 *
 *      15/2/95    VARKON_PID, J. Kjellander
 *      25/9/95    erpush(), J. Kjellander
 *      1997-01-03 mbsdir, J.Kjellander
 *      1999-04-22 Cray, J.Kjellander
 *
 ******************************************************!*/

  {
   int   i,n;
   char  slash,pidfil[V3PTHLEN+1],radbuf[V3PTHLEN+1];
   FILE *pidfpk;

/*
***Projektet "." har ingen pidfil.
*/
   if ( strcmp(filnam,".") == 0 )
     {
#ifdef UNIX
     strcpy(jobdir,"./");
     strcpy(amodir,".");
     strcpy(asydir,"./");
     strcpy(hlpdir,"./");
     strcpy(mdffil,"/usr/v3/mdf/v318.MDF");
     strcpy(mbsdir,"./");
#endif
#ifdef WIN32
     strcpy(jobdir,".\\");
     strcpy(amodir,".");
     strcpy(asydir,".\\");
     strcpy(hlpdir,".\\");
     strcpy(mdffil,"\\usr\\v3\\mdf\\v319.MDF");
     strcpy(mbsdir,".\\");
#endif
     }
/*
***Övriga har, bilda pidfils-namn. På Cray i BATCH mode
***lägger vi till aktuell PE till filnamnet.
*/
   else
     {
#ifdef _CRAYT3E
     if ( igtrty == BATCH )
       sprintf(pidfil,"%s%s%d%s",pidpath(),filnam,_my_pe(),PIDEXT);
     else
       sprintf(pidfil,"%s%s%s",pidpath(),filnam,PIDEXT);
#else
     sprintf(pidfil,"%s%s%s",pidpath(),filnam,PIDEXT);
#endif
/*
***Läs data från pid-filen.
*/
     if ( (pidfpk=fopen(pidfil, "r")) == 0 )
       return(erpush("IG0242",filnam));
/*
***Jobkatalog. Här kan Env-parameter användas.
*/
     if ( fscanf(pidfpk,"%s",radbuf) != 1 )
       return(erpush("IG0472",pidfil));
     else v3trfp(radbuf,jobdir);
/*
***Alternativa modulbibliotek. Evaluering av ev. $ENVPARAM
***görs inte här utan i pmallo() via v3trfp().
*/
     if ( fscanf(pidfpk,"%s",amodir) != 1 )
       return(erpush("IG0472",pidfil));
/*
***Symbolbiblioteket. Här kan Env-parameter användas.
*/
     if ( fscanf(pidfpk,"%s",radbuf) != 1 )
       return(erpush("IG0472",pidfil));
     else v3trfp(radbuf,asydir);
/*
***Hjälpfiler. Här kan Env-parameter användas.
*/
     if ( fscanf(pidfpk,"%s",radbuf) != 1 )
       return(erpush("IG0472",pidfil));
     else v3trfp(radbuf,hlpdir);
/*
***Menyfil. Här kan också env-parameter användas men eftersom
***iglmdf() packar up en sådan struntar vi i att göra det här
***också. iglmdf() måste under alla omständigheter göra det eftersom
***menyfiler kan göra #include på andra menyfiler.
*/
     if ( fscanf(pidfpk,"%s",mdffil) != 1 )
       return(erpush("IG0472",pidfil));
/*
***Från och med 1.14E kan även en rad med MBS-katalog ingå.
***Om inte (gammal pidfil) sätter vi mbsdir = jobdir.
*/
     if ( fscanf(pidfpk,"%s",radbuf) != 1 )
       strcpy(mbsdir,jobdir);
     else v3trfp(radbuf,mbsdir);
/*
***Stäng filen igen.
*/
     fclose(pidfpk);
     }
/*
***Primär mbo-katalog är det samma som 1:a katalogen
***i amodir. Kopiera tecken till 1:a semikolon.
*/
   for ( i=0; i<(int)strlen(amodir); ++i )
     {
#ifdef UNIX
     if ( amodir[i] != ':'  &&  amodir[i] != ';' ) radbuf[i] = amodir[i];
#endif
#ifdef WIN32
     if ( amodir[i] != ';' ) radbuf[i] = amodir[i];
#endif
     else break;
     }

   radbuf[i] = '\0';

   v3trfp(radbuf,mbodir);
/*
***Vissa skall ha exakt en slash sist.
*/
#ifdef UNIX
   slash = '/';
#endif
#ifdef WIN32
     slash = '\\';
#endif

   n = strlen(jobdir) - 1;
   while ( n > 0  &&  jobdir[n] == slash ) --n;
   jobdir[n+1] = slash; jobdir[n+2] = '\0';

   n = strlen(mbsdir) - 1;
   while ( n > 0  &&  mbsdir[n] == slash ) --n;
   mbsdir[n+1] = slash; mbsdir[n+2] = '\0';

   n = strlen(mbodir) - 1;
   while ( n > 0  &&  mbodir[n] == slash ) --n;
   mbodir[n+1] = slash; mbodir[n+2] = '\0';

   n = strlen(hlpdir) - 1;
   while ( n > 0  &&  hlpdir[n] == slash ) --n;
   hlpdir[n+1] = slash; hlpdir[n+2] = '\0';

   n = strlen(asydir) - 1;
   while ( n > 0  &&  asydir[n] == slash ) --n;
   asydir[n+1] = slash; asydir[n+2] = '\0';
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

        short igdir(
        char *inpath,
        char *typ,
        int   maxant,
        int   maxsiz,
        char *pekarr[],
        char *strarr,
        int  *nf)

/*      Returnerar filer med visst efternamn. Efternanmnet
 *      returneras inte.
 *
 *      In:
 *          inpath = Sökvägbeskrivning, tex. "/usr/v3/pid".
 *                   Ev. slash på slutet går bra.
 *          typ    = Filtyp inklusive punkt, tex. ".PID".
 *          maxant = Max antal filer (pekarr:s storlek)
 *          maxsiz = Max antal tecken (buf:s storlek).
 *          strarr = Plats att lagra filnamn.
 *
 *      Ut:
 *          pekarr = Array med nf stycken pekare till filnamn.
 *          nf     = Antal filer.
 *
 *      FV:  0 = Ok.
 *
 *      (C)microform ab 1998-04-10 J. Kjellander
 *
 ******************************************************!*/

  {
   char  pattern[V3STRLEN],*s;
   short status;
   int   i,n;
   DBint nfiles;

/*
***Skapa söksträng.
*/
   strcpy(pattern,"*");
   strcat(pattern,typ);
/*
***Hämta filförteckning.
*/
   status = EXdirl(inpath,pattern,maxant,maxsiz,pekarr,strarr,&nfiles);
   if ( status < 0 ) return(status);
  *nf = nfiles;
/*
***Strippa efternamnen.
*/
   for ( i=0; i<*nf; ++i )
     {
     s = pekarr[i];
     n = strlen(s) - strlen(typ);
     if ( n >= 0 ) *(s+n) = '\0';
     }
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/*!******************************************************/

static  char *pidpath()

/*      Returnerar en pekare till den path som leder fram
 *      till systemets PID-filskatalog.
 *
 *      (C)microform ab 26/9/95 J. Kjellander
 *
 *      1997-01-15 v3genv(), J.Kjellander
 *
 ******************************************************!*/

  {
static char path[V3PTHLEN+1];

/*
***Ganska enkelt med v3genv().
*/
    strcpy(path,v3genv(VARKON_PID));
/*
***Slut.
*/
    return(path);
  }

/********************************************************/
/*!******************************************************/

       short igckpr()

/*     Kollar att filer och kataloger i aktivt
 *     projekt finns. Om inte skapas dom.
 *
 *     Felkoder: IG0652 = Jobkatalog måste finnas 
 *               IG0642 = Jobkatalogen är skrivskyddad
 *
 *     FV:  0 = Ok.
 *         <0 = Fel.
 *
 *     (C)microform ab 1998-01-06  J. Kjellander.
 *
 ******************************************************!*/

 {
   char  path[V3PTHLEN+1];
   short status;

/*
***Om aktiv jobkatalog inte finns provar vi att skapa den.
***jobdir har en slash sist som vi strippar.
*/
   strcpy(path,jobdir); path[strlen(path)-1] = '\0';

   if ( !v3facc(path,'X') )
     {
     if ( igialt(1622,67,68,FALSE) )
       {
       if ( (status=v3mkdr(path)) < 0 ) return(status);
       }
     else return(erpush("IG0652",""));
     }
/*
***Den finns, får vi skriva i den, dvs. skapa nya 
***filer där ?
*/
   if ( !v3facc(path,'W') ) return(erpush("IG0642",path));
/*
***Slut.
*/
    return(0);
 }

/******************************************************!*/

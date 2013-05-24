/*!******************************************************************/
/*  File: msw32dll.c                                                */
/*  ================                                                */
/*                                                                  */
/*  This file includes the sources to the WIN32 plotter             */
/*  driver DLL for Varkon for Windows95/98/NT.                      */
/*                                                                  */
/*  msw32();         DLL Main entrypoint                            */
/*  plinpl();        Init plotter                                   */
/*  plexpl();        Exit plotter                                   */
/*  plmove();        Upp/Move                                       */
/*  pldraw();        Down/Draw                                      */
/*  plchpn();        New pen                                        */
/*  plchwd();        New width                                      */
/*  plfill();        Fill area                                      */
/*  plargv();        Creates UNIX style argv and argc               */
/*                                                                  */
/*  This file is part of the VARKON Plotter Library.                */
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
#include "../../IG/include/IG.h"
#include "../include/PL.h"
#include "../include/params.h"
#include <string.h>

double pixlsx;           /* Plotter pixel x-size i mm */
double pixlsy;           /* Plotter pixel y-size i mm */
int    npixy;            /* Antal pixels i Y-led */
bool   arccon;           /* Måla cirklars rand */
bool   clip;             /* Klippflagga */
bool   color_printer;    /* TRUE = Färgskrivare */
double clipw[4];         /* Klippfönster */
int    lastx;            /* Sista pos X */
int    lasty;            /* Sista pos Y */
int    actpen;           /* Aktiv Varkon-penna */
double actwdt;           /* Aktiv Varkon-linjebredd */
HPEN   act_phnd;         /* Aktiv WIN32-pen */
HBRUSH act_bhnd;         /* Aktiv WIN32-brush */
V3MDAT sydata;           /* Dummy för ige29.c */
HDC    pl_dc;            /* Plotterns DC */
FILE  *pltfpk;           /* Plotfil */
FILE  *spool_fp;         /* Spoolerfil */
HANDLE prhndl;           /* Skrivarhandtag */
DWORD  spool_id;         /* Spooljob ID */

__declspec(dllexport) int msw32();

static HPEN   plcpen(int colnum, double width);
static HBRUSH plcbrush(int colnum);
static int    rawprt();
static int    plargv(char *args,int *pargc,char *argv[]);

/*!******************************************************/

        int msw32(arg1,arg2)
        char *arg1;
        char *arg2;

/*      Huvudrutin för msw32.dll
 *
 *      In: arg1 = Plottfilens namn (och path)
 *          arg2 = Resten av parametrarna
 *
 *      (C)microform ab 1997-02-07 J.Kjellander
 *
 ******************************************************!*/

 {
   int    i;
   int    argc=25;
   char  *argv[25];
   char  *argbuf;
   char   errbuf[1000];

/*
***Målningssystemet för penbredd via pennfil är
***normalt avstängt.
*/
   pfrnam[0] = '\0';
/*
***1:a parametern skall vara plottfilens namn.
*/
   if ( *arg1 != '\0' ) strcpy(pltfil,arg1);
/*
***Processa kommandoraden, dvs. parsa arg2.
*/
   argbuf = v3mall(argc*V3STRLEN,"msw32");
   for ( i=0; i<argc; ++i ) argv[i] = argbuf + i*V3STRLEN;
   plargv(arg2,&argc,argv);
   plppar(argc,argv);
   v3free(argbuf,"msw32");
/*
***Ladda eventuell penbreddsfil.
*/
   if ( pfrnam[0] != '\0' )
     {
     pllpfp("MSW32.PEN");
     pllpfr(pfrnam);
     }
/*
***Klipp-fönster.
*/
   clip  = FALSE;
/*
***Öppna filen som skall plottas/skrivas ut.
*/
   if ( (pltfpk=fopen(pltfil,"r")) == NULL )
     {
     sprintf(errbuf,"Can't open file %s",pltfil);
     MessageBox(NULL,errbuf,
              "VARKON:Printing error !",MB_OK | MB_ICONSTOP);
     return(0);
     }
/*
***Initiera plotter
*/
   if ( plinpl() < 0 ) 
     {
     MessageBox(NULL,"Plotter not initialized !",
               "VARKON:Plotting error !",MB_ICONSTOP | MB_OK);
     return(0);
     }
/*
***Processa plotfil.
*/
   if ( raw ) rawprt();
   else       plprpf();
/*
***Avsluta plotter
*/
   plexpl();
/*
***Slut.
*/
   return(0);
  }

/********************************************************/
/********************************************************/

        short plinpl()

/*      Initiering av plotter.
 *
 *      (C)microform 1996-02-22 J.Kjellander
 *
 *      1996-06-14 Default skrivare, J.Kjellander
 *      1997-01-22 -raw, J.Kjellander
 *      1997-03-24 ENUM_DEFAULT, J.Kjellander
 *      1997-04-01 J.Kjellander
 *      1997-11-17 VC++5.0, J.Kjellander
 *      1997-11-27 Färg, J.Kjellander
 *      1997-12-14 NT, J.Kjellander
 *      1997-12-28 Färg+bredd, J.Kjellander
 *      2000-02-05 bw, J.Kjellander
 *      2001-05-29 W2000, J.Kjellander
 *
 ********************************************************/

{
   char             errbuf[1000];
   char             ai1buf[1000];
   bool             ok;
   int              j;
   DWORD            i,type,needed,nprinters,size,errnum;
   LPPRINTER_INFO_2 bufptr;
   DOCINFO          di;
   OSVERSIONINFO    os_version;

   ADDJOB_INFO_1 *ai1 = (ADDJOB_INFO_1 *)ai1buf;

/*
***Initiering.
*/
   lastx    =  lasty = 0;
   actpen   = -1;
   actwdt   =  0.0;
   act_phnd =  (HPEN)NULL;
   act_bhnd =  (HBRUSH)NULL;
/*
***Hur många skrivare finns installerade och hur stor dataarea
***behöver allokeras för att hämta information om dem ? Olika
***för NT och Win95.
*/
   os_version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&os_version);

   if ( os_version.dwPlatformId == VER_PLATFORM_WIN32_NT )
     {
     EnumPrinters(PRINTER_ENUM_CONNECTIONS | 
                  PRINTER_ENUM_LOCAL,NULL,2,NULL,0,
                                        &needed,&nprinters);
     }
   else
     {
     EnumPrinters(PRINTER_ENUM_LOCAL,NULL,2,NULL,0,
                                        &needed,&nprinters);
     }
/*
***Allokera med samma metod som i C-exemplet "PRINTER". Varför
***man skall allokera 4 extra bytes vet bara gud !!! Typiskt Microsoft !!
*/
   size = needed;

   if (!(bufptr = (LPPRINTER_INFO_2) LocalAlloc(LPTR,size + 4)))
    {
    MessageBox (NULL,"LocalAlloc() failed !",
                "VARKON:Plotting error !",MB_ICONSTOP | MB_OK);
    return(-1);
    }
/*
***Hämta information om skrivarna. Har skrivarnamn utelämnats
***frågar vi efter default skrivare annars frågar vi om alla.
***NT och W95 är olika.
***Lagt till lokala skrivare på NT 1998-09-26 JK.
*/
   if ( os_version.dwPlatformId == VER_PLATFORM_WIN32_NT )
     {
     type = PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_LOCAL;
     }
   else
     {
     if ( *plnamn == '\0' ) type = PRINTER_ENUM_DEFAULT;
     else                   type = PRINTER_ENUM_LOCAL;
     }

   if ( EnumPrinters(type,
                     NULL,
                     2,
                     (LPBYTE)bufptr,
                     size,
                    &needed,
                    &nprinters) )
     {

/*
***Finns det nån skrivare i systemet ?
*/
     if ( nprinters == 0 )
       {
       MessageBox(NULL,"No printer installed !",
                 "VARKON:Plotting error !",MB_ICONSTOP | MB_OK);
       return(-1);
       }
/*
***Default skrivare. Om det gäller raw mode gör vi OpenPrinter()
***annars CreateDC() på den 1:a skrivaren = default.
*/
     else if ( *plnamn == '\0' )
       {
       if ( raw )
         {
         ok = OpenPrinter(bufptr->pPrinterName,&prhndl,NULL);
         if ( !ok )
           {
           sprintf(errbuf,"Can't open default printer");
           MessageBox(NULL,errbuf,
                     "VARKON:Printing error !",MB_ICONSTOP | MB_OK);
           return(-1);
           }
         }
       else
         {
/*
***Bytt plats på DriverName och pPrinterName. 1997-04-01
***Testat på Högskolans nät 970402 OK.
*/
         pl_dc = CreateDC(bufptr->pDriverName,
                          bufptr->pPrinterName,NULL,NULL);
         if ( pl_dc == NULL )
           {
           sprintf(errbuf,"Can't create DC for default printer");
           MessageBox(NULL,errbuf,
                     "VARKON:Plotting error !",MB_ICONSTOP | MB_OK);
           return(-1);
           }
         }
       }
/*
***Eller har ett skrivarnamn angetts på kommandoraden ?
*/
     else
       {
       for ( i=0; i<nprinters; ++i )
         {
         if ( strcmp(plnamn,(bufptr+i)->pPrinterName) == 0 )
           {
           if ( raw )
             {
             ok = OpenPrinter((bufptr+i)->pPrinterName,&prhndl,NULL);
             if ( !ok )
               {
               sprintf(errbuf,"Can't open printer %s",plnamn);
               MessageBox(NULL,errbuf,
                         "VARKON:Printing error !",MB_ICONSTOP | MB_OK);
               return(-1);
               }
             }
           else
             {
             pl_dc = CreateDC((bufptr+i)->pDriverName,
                              (bufptr+i)->pPrinterName,NULL,NULL);
             if ( pl_dc == NULL )
               {
               sprintf(errbuf,"Can't create DC for %s",plnamn);
               MessageBox(NULL,errbuf,
                         "VARKON:Plotting error !",MB_ICONSTOP | MB_OK);
               return(-1);
               }
             }
           break;
           }
         }
       if ( i == nprinters )
         {
         sprintf(errbuf,"There is no printer with name = %s",plnamn);
         MessageBox(NULL,errbuf,
                    "VARKON:Plotting error !",MB_ICONSTOP | MB_OK);
         for ( j=0; j<(signed int)nprinters; ++j )
           {
           sprintf(errbuf,"A printer with name %s is installed !",(bufptr+j)->pPrinterName);
           MessageBox(NULL,errbuf,
                   "VARKON:Plotting error !",MB_ICONSTOP | MB_OK);
           }
         return(-1);
         }
       }
     }
   else
     {
     errnum = GetLastError();
     sprintf(errbuf,"Can't enum printers, errcode=%d",errnum);
     MessageBox(NULL,errbuf,
               "VARKON:Plotting error !",MB_ICONSTOP | MB_OK);
     return(-1);
     }
/*
***I raw mode kopierar vi standard input till en temporär spooler-fil.
***Här fixar vi fram ett lämpligt filnamn med AddJob() och öppnar filen.
*/
   if ( raw )
     {
     ok = AddJob(prhndl,(DWORD)1,(LPBYTE)ai1buf,sizeof(ai1buf),&needed);
     if ( !ok )
       {
       if ( needed > sizeof(ai1buf) )
         {      
         sprintf(errbuf,"AddJob() error 1, needed = %d",needed);
         MessageBox(NULL,errbuf,"VARKON:Printing error !",MB_ICONSTOP | MB_OK);
         return(-1); 
         }
       else
         {
         sprintf(errbuf,"AddJob() error 2, errcode = %d",GetLastError());
         MessageBox(NULL,errbuf,"VARKON:Printing error !",MB_ICONSTOP | MB_OK);
         return(-1);
         }
       }

     spool_id = ai1->JobId;

     if ( (spool_fp=fopen(ai1->Path,"w+")) == NULL )
       {
       sprintf(errbuf,"Can't create spool-file %s",ai1->Path);
       MessageBox(NULL,errbuf,
                "VARKON:Printing error !",MB_OK | MB_ICONSTOP);
       ClosePrinter(prhndl);
       return(0);
       }
     }
/*
***Om det inte är raw mode är det en riktig plottning.
***Hur stora är pixlarna ? Spara antalet pixels i Y-led så
***att origo kan transformeras till nedre vänstra hörnet.
*/
   else
     {
     pixlsx = (double)GetDeviceCaps(pl_dc,HORZSIZE) /
              (double)GetDeviceCaps(pl_dc,HORZRES);

     npixy  = GetDeviceCaps(pl_dc,VERTRES);
     pixlsy = (double)GetDeviceCaps(pl_dc,VERTSIZE) / (double)npixy;
/*
***Finns det färg ?
*
     nplanes = GetDeviceCaps(pl_dc,PLANES);
     if ( nplanes > 1 ) color_printer = TRUE;
     else               color_printer = FALSE;
*/
     if ( bw ) color_printer = FALSE;
     else      color_printer = TRUE;
/*
***Initiera printern.
*/
     di.cbSize      = sizeof(DOCINFO);
     di.lpszDocName = "VARKON Plot";
     di.lpszOutput  = NULL;
     di.fwType      = 0;        /*1997-11-17 JK */

     StartDoc(pl_dc,&di);
     StartPage(pl_dc);
/*
***Lämna tillbaks allokerat minne.
*/
     LocalFree(LocalHandle(bufptr));
     }

   return(0);
}

/********************************************************/ 
/*!******************************************************/

        int rawprt()

/*      Kopierar pltfil till spoolfil.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0.
 *
 *      (C)microform ab 1997-01-28 J. Kjellander
 *
 ******************************************************!*/

  {
   int   ic;

/*
***Kopiera tecken från pltfil till spool-filen.
*/
   while ( (ic=getc(pltfpk)) != EOF ) putc(ic,spool_fp);


   return(0);
  }

/********************************************************/
/********************************************************/

        short plexpl()

/*      Avslutning av plotter 
 *
 *      (C)microform 1996-02-22 J.Kjellander
 *
 *      1996-10-07 fclose(), J.Kjellander
 *      1997-01-22 -raw, J.Kjellander
 *      1997-03-24 -del, J.Kjellander
 *      1997-11-27 Färg, J.Kjellander
 *
 ********************************************************/

{

/*
***Stäng plotfilen.
*/
   fclose(pltfpk);
/*
***Skicka jobbet till spoolern.
*/
   if ( raw )
     {
     if ( formf ) putc((int)12,spool_fp);
     fclose(spool_fp);
     ScheduleJob(prhndl,spool_id);
     ClosePrinter(prhndl);
     }
   else
     {
     EndPage(pl_dc);
     EndDoc(pl_dc);
     if ( act_phnd != (HPEN)NULL ) DeleteObject(act_phnd);
     if ( act_bhnd != (HBRUSH)NULL ) DeleteObject(act_bhnd);
     DeleteDC(pl_dc);
     }
/*
***Skall filen tas bort efteråt ?
*/
   if ( del ) unlink(pltfil);

   return(0);
}

/********************************************************/ 
/********************************************************/

        short plmove(x,y)
        double   x;
        double   y;

/*      Förflyttning av penna,  penna upp först.
 *
 *      In: x och y modellkoordinater i mm.
 *  
 *      (C)microform 1996-02-22 J.Kjellander
 *
 ********************************************************/

{
    int ix,iy;

    ix = (int)(x/pixlsx);
    iy = npixy - (int)(y/pixlsy);

    if ( ix != lastx || iy != lasty )
      {
      MoveToEx(pl_dc,ix,iy,NULL);
      lastx = ix;
      lasty = iy;
      }

    return(0);
}

/********************************************************/ 
/********************************************************/

        short pldraw(x,y)
        double   x;
        double   y;

/*      Rita med penna, penna ned först.
 *
 *      In: x och y modellkoordinater i mm.
 *  
 *      (C)microform 1996-02-22 J.Kjellander
 *
 ********************************************************/

{
    int ix,iy;

    ix = (int)(x/pixlsx);
    iy = npixy - (int)(y/pixlsy);

    if ( ix != lastx || iy != lasty )
      {
      LineTo(pl_dc,ix,iy);
      lastx = ix;
      lasty = iy;
      }

    return(0);
}

/********************************************************/ 
/********************************************************/

        short plchpn(short pn)

/*      Byt penna.
 *
 *      (C)microform 1996-02-22 J.Kjellander
 *
 *      1997-11-27 Färg, J.Kjellander
 *
 ********************************************************/

{
   HPEN   newpen;
   HBRUSH newbrush;

   if ( (int)pn != actpen )
     {
     actpen = (int)pn;
     newpen = plcpen(actpen,actwdt);
     newbrush = plcbrush(actpen);
     SelectObject(pl_dc,newpen);
     SelectObject(pl_dc,newbrush);
     if ( act_phnd != (HPEN)NULL )
       {
       DeleteObject(act_phnd);
       DeleteObject(act_bhnd);
       }
     act_phnd = newpen;
     act_bhnd = newbrush;
     }

    return(0);
}

/********************************************************/ 
/********************************************************/

        short  plchwd(width)
        double width;

/*      Byt linjebredd.
 *
 *      (C)microform 1997-12-28 J.Kjellander
 *
 ********************************************************/

{  
   HPEN newpen;  

   if ( width != actwdt )
     {
     actwdt = width;
     newpen = plcpen(actpen,actwdt);
     SelectObject(pl_dc,newpen);
     if ( act_phnd != (HPEN)NULL ) DeleteObject(act_phnd);
     act_phnd = newpen;
     }

    return(0);
}

/********************************************************/ 
/********************************************************/

        short  plfill(
        short  k,
        double x[],
        double y[],
        char   a[])

/*      Fill area.
 *
 *      (C)microform 1997-12-28 J.Kjellander
 *
 ********************************************************/

{
   int    i;
   POINT  p[PLYMXV+1];
   double cosv,sinv,xs,ys,xt,yt;


   cosv = COS(vinkel*DGTORD);
   sinv = SIN(vinkel*DGTORD);
/*
***Transform and make POINT array.
*/
   for ( i=0; i<=k; ++i )
     {
     xs = skala * x[i];
     ys = skala * y[i];
     xt = (xs*cosv - ys*sinv) - xmin;
     yt = (xs*sinv + ys*cosv) - ymin;
     p[i].x = (int)(xt/pixlsx);
     p[i].y = npixy - (int)(yt/pixlsy);
     }
/*
***Fill polygon.
*/
   Polygon(pl_dc,p,(int)(k+1));

    return(0);
}

/********************************************************/ 
/*!******************************************************/

        static int plargv(char *args,int *pargc,char *argv[])

/*      Skapar argc/argv från WinMains args-parameter.
 *      Vid anropet är pargc satt dimensionen hos argv,
 *      dvs. hur många parametrar som får plats i argv.
 *      Pekarna i argv är initierade till att peka på
 *      pargc st. ej initierade strängar V3STRLEN stora.
 *
 *      In: args  = Kommandoraden som en enda sträng.
 *          pargc = Max antal parametrar.
 *
 *      Ut: *pargs = Antal parametrar.
 *          *argv  = Pekare till parametrarna.
 *
 *      FV: 0.
 *
 *      (C)microform ab 1997-02-07 J.Kjellander
 *
 ******************************************************!*/

  {
   int  start,stop,maxpar,npar,args_len;
   char parbuf[V3STRLEN];

/*
***Initiering.
*/
  *plnamn   = '\0';
   args_len = strlen(args);
   maxpar   = *pargc;
/*
***1:a parametern skall vara kommandot som startade plotprogrammet.
*/
   strcpy(argv[0],"msw32");
   npar = 1;
/*
***Scanna args. När vi hittar parametern -P är följande tecken fram
***till nästa minustecken eller slutet namnet på printern. Det kan tex.
***se ut på följande sätt:
***-PCanon FAX-B320/B340 -s2.5
***-s2.5   -PCanon FAX-B320/340   -v90
***etc. etc. Observera att det skall vara stora "P".
*/
   start = stop = 0;

   for ( ;; )
     {
/*
***Leta upp ett blanktecken eller slut på rad.
*/
     if ( *(args+stop) == ' '  ||  *(args+stop) == '\0' )
       {
       if ( stop > start )
         {
         if ( stop-start < V3STRLEN )
           {
/*
***start pekar nu på början av ett ord och stop på efterföljande ' ' eller '\0'.
***Om det är -Pprinternamn plockar vi ut namnet på printern utan att lagra
***i argv[]. Observera att printernamn kan innehålla blanktecken och binde-
***streck. Vissa plottprogram använder -P för parametern "pinnar" 8/24 på
***matrisskrivare men här betyder den alltså "plotternamn".
*/
           if ( *(args+start)   == '-'  &&
                *(args+start+1) == 'P' )
             {
             start += 2;
/*
***För att hitta slutet på printernanet scannar vi fram till nästa förekomst
***av " -" eller '\0'. Om printernamnet innehåller " -" blir det problem.
*/
             while ( *(args+stop) != '\0'  &&  
                     strncmp(args+stop," -",2) != 0 ) ++stop;
           --stop;
             while ( *(args+stop) == ' ' ) --stop;
           ++stop;
             strncpy(plnamn,(args+start),stop-start);
             plnamn[stop-start] = '\0';
             }
/*
***Om det inte är -Pprinter är det en vanlig parameter som skall
***lagras i argv[].
*/
           else
             {
             strncpy(parbuf,(args+start),stop-start);
             parbuf[stop-start] = '\0';
             if ( *parbuf == '-' )
               {
               strcpy(argv[npar],parbuf);
             ++npar;
               if ( npar == maxpar ) goto end;
               }
             }
           while ( *(args+stop) == ' ' ) ++stop;
           start = stop;
           }
         else ++stop;
         }
       else ++stop;
       }
     else ++stop;
     if ( stop > args_len ) goto end;
     }
/*
***Slut.
*/
end:

  *pargc  = npar;

   return(0);
  }

/********************************************************/
/*!******************************************************/

        static HPEN plcpen(int colnum, double width)

/*      Skapa penna med viss färg och bredd.
 *
 *      In: colnum = Färgnummer, Varkon's PEN
 *          width  = Linjebredd, Varkon's WIDTH
 *
 *      Ut: Inget.
 *
 *      FV: Penn-handtag.
 *
 *      (C)microform ab 1997-12-28 J. Kjellander
 *
 ******************************************************!*/

  {
    int      npix;
    HPEN     newpen;
    LOGBRUSH lb;
    COLORREF color;

/*
***Beräkna antal pixels.
*/
    npix = (int)(skala*width/((pixlsx + pixlsy)/2.0) + 0.5);
/*
***Lite rimlighetskontroll.
*/
    if ( npix < 1 )     npix = 1;
    if ( npix > npixy ) npix = npixy;
/*
***Färg.
*/
    if ( color_printer )
      {
      switch ( colnum )
        {
        case  0: color = RGB(255,255,255); break;
        case  1: color = RGB(  0,  0,  0); break;
        case  2: color = RGB(255,  0,  0); break;
        case  3: color = RGB(  0,255,  0); break;
        case  4: color = RGB(  0,  0,255); break;
        case  5: color = RGB(127,  0,  0); break;
        case  6: color = RGB(  0,127,  0); break;
        case  7: color = RGB(  0,  0,127); break;
        case  8: color = RGB(255,127,  0); break;
        case  9: color = RGB(255,255,  0); break;
        default: color = RGB(  0,  0,  0); break;
        }
      }
    else color = RGB(0,0,0);
/*
***Skapa penna.
*/
    lb.lbStyle = BS_SOLID;
    lb.lbColor = color;
    lb.lbHatch = 0;

    newpen = ExtCreatePen(PS_GEOMETRIC    |
                          PS_SOLID        |
                          PS_ENDCAP_ROUND |
                          PS_JOIN_ROUND,
                          npix,
                          &lb,
                          0,
                          NULL);

    return(newpen);
  }

/********************************************************/
/*!******************************************************/

        static HBRUSH plcbrush(int colnum)

/*      Skapa brush med viss färg.
 *
 *      In: colnum = Färgnummer, Varkon's PEN
 *
 *      Ut: Inget.
 *
 *      FV: Brush-handtag.
 *
 *      (C)microform ab 1999-12-15 J. Kjellander
 *
 ******************************************************!*/

  {
    HBRUSH   newbrush;
    COLORREF color;

/*
***Färg.
*/
    if ( color_printer )
      {
      switch ( colnum )
        {
        case  0: color = RGB(255,255,255); break;
        case  1: color = RGB(  0,  0,  0); break;
        case  2: color = RGB(255,  0,  0); break;
        case  3: color = RGB(  0,255,  0); break;
        case  4: color = RGB(  0,  0,255); break;
        case  5: color = RGB(127,  0,  0); break;
        case  6: color = RGB(  0,127,  0); break;
        case  7: color = RGB(  0,  0,127); break;
        case  8: color = RGB(255,127,  0); break;
        case  9: color = RGB(255,255,  0); break;
        default: color = RGB(  0,  0,  0); break;
        }
      }
    else color = RGB(0,0,0);
/*
***Skapa brush.
*/
    newbrush = CreateSolidBrush(color);

    return(newbrush);
  }

/********************************************************/

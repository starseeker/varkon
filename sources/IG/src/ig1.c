/*!******************************************************************/
/*  File: ig1.c                                                     */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  iginit();      Inits IG                                         */
/*  iglmdf();      Loads MDF-file                                   */
/*  igexit();      Exits IG                                         */
/*  igexfu();      Executes function                                */
/*  igdofu();      Executes function                                */        
/*  notimpl();     Dummy-routine                                    */
/*  wpunik();      Dummy-routine                                    */
/*  editcopy();    Interpret %ASCII-code                            */
/*  igstmu();      Stores menu im allocated memory                  */
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
#include "../../WP/include/WP.h"
#include "../include/futab.h"
#include "../../EX/include/EX.h"
#include <string.h>

#ifdef UNIX
#undef VSTART
#include "fcntl.h"
#include <unistd.h>
#endif

#ifdef SCO_UNIX
#include "termio.h"
#endif
char   txtmem[CHRMAX]; /* "ragged" array  of t-strings */
char  *txtind[TXTMAX]; /* t-string ptrs */
char  *fstmem;         /* ptr to first empty element in txtmem */
MNUDAT mnutab[MNUMAX]; /* Menus */
MNUALT smbind[SMBMAX]; /* Shortcuts */




IGTATT igttab[] =
  { 
  {80,72, 57,68,9,12,1,12, 13,49,5,2, 17,1, 2,78,14,1,3,14, 15,1},
  {80,33, 66,71,3,10,1, 9, 13,20,1,1, 33,1, 1,79,31,1,3,31, 32,1},
  {80,32,  1, 9,4,12,20,9, 13,20,1,4,  2,1, 2,78,32,2,4,32,  1,1},
  {80,24, 66,71,3,10,1, 9, 13,20,1,1, 24,1, 2,78,22,1,3,22, 23,1},
  {80,24, 66,71,3,10,1, 9, 13,20,1,1, 24,1, 2,78,22,1,3,22, 23,1},
  {80,72, 62,67,3,10,1, 9, 13,20,7,1, 72,7, 7,70,70,1,3,70, 71,7},
  {80,72, 62,67,3,10,1, 9, 13,20,7,1, 72,7, 7,70,70,1,3,70, 71,7},
  {80,24, 66,71,3,10,1, 9, 13,20,1,1, 24,1, 2,78,22,1,3,22, 23,1},
  {80,30, 66,71,3,10,1, 9, 13,20,1,1, 30,1, 1,79,28,1,3,28, 29,1},
  {80,30, 66,71,3,10,1, 9, 13,20,1,1, 30,1, 1,79,28,1,3,28, 29,1},
  {80,24, 30,41,9,37,1, 6, 13,20,1,1, 24,1, 1,79,22,1,3,22, 23,1},
  {72,35,  2, 6,3, 7,23,7, 13,12,1,2, 35,2, 2,71,33,1,3,33, 34,2},
  {79,35,  2, 6,3, 7,23,7, 13,12,1,1, 34,2, 2,77,33,1,3,33, 33,2},
  { 0, 0,  0, 0,0, 0, 0,0,  0, 0,0,0,  0,0, 0, 0, 0,0,0, 0,  0,0},
  {72,35,  2, 6,3, 7,23,7, 13,12,1,2, 35,2, 2,71,33,1,3,33, 34,2},
  {72,35,  2, 6,3, 7,23,7, 13,12,1,2, 35,2, 2,71,33,1,3,33, 34,2},
  {80,24, 30,41,9,37,1, 6, 13,20,1,1, 24,1, 1,79,22,1,3,22, 23,1}
  }; /* !.   . .  . .  . ! .  . . . ! . . !.  .  . . .  . ! . .
  .  .  !.   . .  . .  . ! .  . . . ! . . !.  .  . . .  . ! . .
  r  b  !s   s s  s s  s ! m  m m m ! i i !l  l  l l l  l ! m m
  m  m  !a   a a  a a  h ! h  w e e ! a a !a  a  a a a  a ! a a
  a  a  !r   d r  d y  g ! g  d n n ! l l !f  f  l h f  l ! l l
  r  r  !x   x f  f    t ! t  t u u ! y x !c  w  y e l  l ! y x
  g  g  !      w  w      !      r r !     !o       d i  i !
        !                !      x y !     !l       y n  n !  
        !                !          !     !               !
  Skärm !     Status     !  Menyer  !Inmat!    Listarea   !Medd*/

/* igttab är en tabell med data för olika skärmar
   Variabeln igtrty håller reda på vilken skärm i
   i igttab som används. 1=LVT100 2=V550 3=IP3215 4=MG400 5=MG420
   6=FT4600 7=BATCH 8=MG700 9=T4207 10=MO2000 11=VT100 12=N220G
   13=CGI 14=MX7250 15=MSCOLOUR 16=MSMONO 17=X11
*/


short igtrty;

/* igtrty håller reda på vilken skärm som används. */


short rmarg,bmarg;

/* VT100-skärmens antal kolumner och rader */


short sarx,sadx,sarfw,sadfw,say,shgt;

/* Status-arean */


short mhgt,mwdt,menurx,menury;

/* Meny-arean */


short ialy,ialx;

/* Input-arean */


short lafcol,lafw,laly,lahedy,laflin,lallin;

/* List-arean */


short maly,malx;

/* Meddelande-arean */


DIGDAT digdes = { FALSE,0,0,0.0,0.0,0.0,0.0,0.0,1.0 };

/* Digitizer-data */

extern short  v3mode,menlev,stalev,gptrty;
extern short  astack[],mant;
extern pm_ptr pmstkp;
extern bool   igbflg;

extern char    speed[];
extern char    pltyp[];
extern char    port[];
extern short   actfun;

/* Plott-variabler */


#define W_REC 1            /* mode = vänta på record */
#define L_REC 2            /* mode = ladda record */
#define E_FIL 3            /* mode = end of file */

#define CDUMMY '+'         /* dummykonstant */

#define SK_SP "%*[\n\t ]"  /* skip whitespace */
#define SK_CT "%*[\136\n]" /* skip comment (to <CR>) */
#define SK_EQ "%*[=\n\t ]" /* skip w.spaces och '=' */
#define SK_CA "%*[,\n\t ]" /* skip w.spaces och ',' */
#define SK_SC "%*[;\n\t ]" /* skip w.spaces och ';' */

/* get t-record text-string*/
#define GT_TXT "%*[\042]%[\136\042]%*[\042]"


/* Get Menu Header fält, 27/2/91 JK */
#define GT_MH "%*[\042]%[\136\042]%*[\042]%*[\136\n]"

/* get menu alternativ field */
#define GT_MA "%[\136\042]%*[\042%*[ ,]%c%hd"


/* get s-record */
#define GT_SMB "%*[\42]%[\136\42]%*[\42]%*[ ,]%c%hd"

/*!******************************************************/

        short iginit(char *fnam)

/*      Initieringsrutin för menyhanteraren.
 *
 *      In: fnam => Pekare till MDF-filnamn.
 *
 *      Ut: Inget.
 *
 *      Felkoder: IG0282 = Kan ej initiera digitizer %s
 *                IG0292 = Okänd terminaltyp %s
 *
 *      (C)microform ab 19/1/85 M. Nelson
 *
 *      REVIDERAD:
 *
 *      6/9-85   J. Kjellander, Felhantering
 *      3/10-85  Ulf Johansson, Numerisk ASCII-ekvivalent
 *      25/10-85 Ulf Johansson, Tillägg av s-strängar
 *      24/2/86  Ny terminal MG420, R. Svedin
 *      2/10/86  menlev, J. Kjellander
 *      8/10/86  Tagit bort MNUDAT.nrows, J. Kjellander
 *      28/10/86 Tagit bort variabler för stacken, R. Svedin
 *      5/1/87   Ny terminal FT4600, R. Svedin
 *      18/4/87  Ny terminal BATCH, J. Kjellander
 *      7/12/87  Ny terminal MG700 B. Doverud
 *      21/1/88  Nya terminaler T4207, MO2000, B. Doverud
 *      14/3/88  Digitizer, J. Kjellander
 *      20/10/88 CGI, J. Kjellander 
 *      20/10/88 Ny terminal N220G, B. Doverud 
 *      21/2/91  MSKERMIT, J. Kjellander
 *      27/3-03  Removed digitizer support, J. Kjellander
 *
 ******************************************************!*/

  {
   short i,status;

#ifdef SCO_UNIX
   struct termio dgport;
   int fd;
#endif

/*
***Initiera skärmkonfigurationen.
*/
   menlev = stalev = 1;
/*
***Initiering av t-sträng-pekar arrayen.
*/
   fstmem = txtmem;

   for ( i=0; i<TXTMAX; ++i ) txtind[i] = NULL;
/*
***Initiering av s-sträng pekar-arrayen.
*/
   for ( i=0; i<SMBMAX; i++ ) smbind[i].str = NULL;
/*
***Initiering av meny-tabellen.
*/
   for ( i=0; i<MNUMAX; i++ ) mnutab[i].rubr = NULL;
/*
***Initiera aktiv huvudmeny. 0 är detsamma som systemets
***default-meny dvs. 2 för BAS_2MOD, 3 för BAS3_MOD och
***4 för RIT_MOD.
*/
   igsmmu((short)0);
/*
***Ladda MDF-filen.
*/
   if ( (status=iglmdf(fnam)) < 0 ) return(status);
/*
***Initiera skärm-parametrar.
*/
   if ( strcmp("LVT100",iggtts(351)) == 0 )        igtrty = LVT100;
   else if ( strcmp("V550",iggtts(351)) == 0 )     igtrty = V550;
   else if ( strcmp("IP3215",iggtts(351)) == 0 )   igtrty = IP3215;
   else if ( strcmp("MG400",iggtts(351)) == 0 )    igtrty = MG400;
   else if ( strcmp("MG420",iggtts(351)) == 0 )    igtrty = MG400;
   else if ( strcmp("FT4600",iggtts(351)) == 0 )   igtrty = FT4600;
   else if ( strcmp("BATCH",iggtts(351)) == 0 )    igtrty = BATCH;
   else if ( strcmp("MG700",iggtts(351)) == 0 )    igtrty = MG400;
   else if ( strcmp("T4207",iggtts(351)) == 0 )    igtrty = T4207;
   else if ( strcmp("MO2000",iggtts(351)) == 0 )   igtrty = MO2000;
   else if ( strcmp("VT100",iggtts(351)) == 0 )    igtrty = VT100;
   else if ( strcmp("N220G",iggtts(351)) == 0 )    igtrty = N220G;
   else if ( strcmp("MX7250",iggtts(351)) == 0 )   igtrty = T4207;
   else if ( strcmp("MSKERMIT",iggtts(351)) == 0 ) igtrty = MSMONO;
#ifdef V3_X11
   else if ( strcmp("X11",iggtts(351)) == 0 )      igtrty = X11;
#endif
#ifdef WIN32
   else if ( strcmp("MSWIN",iggtts(351)) == 0 )    igtrty = MSWIN;
#endif
   else return(erpush("IG0292",iggtts(351)));
/*
***In batch mode, force igtrty to BATCH.
*/
   if ( igbflg ) igtrty = BATCH;
/*
***Set upp text mode parameters.
*/
   rmarg = igttab[igtrty-1].rmarg;
   bmarg = igttab[igtrty-1].bmarg;
   sarx = igttab[igtrty-1].sarx;
   sadx = igttab[igtrty-1].sadx;
   sarfw = igttab[igtrty-1].sarfw;
   sadfw = igttab[igtrty-1].sadfw;
   say = igttab[igtrty-1].say;
   shgt = igttab[igtrty-1].shgt;
   mhgt = igttab[igtrty-1].mhgt;
   mwdt = igttab[igtrty-1].mwdt;
   menurx = igttab[igtrty-1].menurx;
   menury = igttab[igtrty-1].menury;
   ialy = igttab[igtrty-1].ialy;
   ialx = igttab[igtrty-1].ialx;
   lafcol = igttab[igtrty-1].lafcol;
   lafw = igttab[igtrty-1].lafw;
   laly = igttab[igtrty-1].laly;
   lahedy = igttab[igtrty-1].lahedy;
   laflin = igttab[igtrty-1].laflin;
   lallin = igttab[igtrty-1].lallin;
   maly = igttab[igtrty-1].maly;
   malx = igttab[igtrty-1].malx;
/*
*** Defaultvärden för plottning.
*/
   strcpy(speed,iggtts(334));
   strcpy(pltyp,iggtts(348));
   strcpy(port,iggtts(346));
/*
***Initiering av digitizer.
*/
#ifdef SCO_UNIX
   if ( digdes.def ) close(digdes.fd);

   if ( txtind[101] == NULL )
     {
     digdes.def = FALSE;
     }
   else
     {
     if      ( strcmp(iggtts(101),"C91360P") == 0 ) digdes.typ = C91360P;
     else if ( strcmp(iggtts(101),"C2200P") == 0 ) digdes.typ = C2200P;
     else if ( strcmp(iggtts(101),"MM1200P") == 0 ) digdes.typ = MM1200P;
     else    return(erpush("IG0282",iggtts(101)));

     if ( (fd=open(iggtts(100),O_RDWR)) < 0 )
       return(erpush("IG0282",iggtts(100)));
     digdes.def = TRUE;
     digdes.fd = fd;
     digdes.ldx = 0.0;
     digdes.ldy = 0.0;
     digdes.lmx = 0.0;
     digdes.lmy = 0.0;
     digdes.v = 0.0;
     digdes.s = 1.0;
     ioctl(fd,TCGETA,&dgport);
     dgport.c_lflag &= ~ICANON;
     dgport.c_lflag &= ~ECHO;
     dgport.c_cc[4] = 1;
     dgport.c_cc[5] = 0;
     ioctl(fd,TCSETA,&dgport);
/*
***Gör reset på digitizern.
*/
     switch (digdes.typ)
       {
       case C91360P:
       write(fd,"\033%VR\015",5);
       break;
/*
***MM1200, " " = Något att trigga Baudrate på, "T" = 2 pts/sec,
***"c" = Origo i nedre vänstra hörnet, För en 961:a i stående A4.
***"B" = Point mode, "i" = 20 lines/mm,
*/
       case MM1200P:
       write(fd," TcBi",5);
       break;
       }
     }
#endif
/*
***Kör vi WIN32 kan vi nu bygga huvudfönstrets menyträd.
*/
#ifdef WIN32
    mscrmu();
#endif
/*
***Sudda skärmen.
*/
    igersc();

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short iglmdf(char *fnam)

/*      Laddar MDF-fil.
 *
 *      In: fnam => Pekare till MDF-filnamn.
 *
 *      Ut: Inget.
 *
 *      FV:  0 => Ok.
 *          -1 => Fel vid laddning.
 *
 *      Felkoder: IG0392 = Menyfilen %s finns ej.
 *                IG0522 = Kan ej inkludera menyfilen %s 
 *                IG0532 = Felaktigt recordnummer
 *                IG0542 = Ej include efter #
 *
 *      (C)microform ab 18/4/87 J. Kjellander
 *
 *      19/4/87 #include, J. Kjellander
 *      7/10/87 Redefine på s- och t-str. J. Kjellander
 *      18/2/92 MACRO, J. Kjellander
 *      23/3/95 v3trfp(), J. Kjellander
 *      1996-02-26 Mer erpush(), J. Kjellander
 *      1996-02-26 main_menu, J. Kjellander
 *
 ******************************************************!*/

  {
    FILE *menufil;         /* menyfilen, pekare till */
    short mode;            /* inläsningsmode W_REC, L_REC eller E_FIL */
    char rectyp;           /* recordtyp t,s,i eller m */
    int recnum;            /* recordnummer */
    char fldtyp;           /* fälttyp */
    short num1;            /* nummer */
    char teck1;            /* tecken */
    short mnum,nalt;       /* Menynummer och antal alternativ */
    char tbuf[V3STRLEN+1]; /* temp. buffert för textsträngar */
    char rubr[V3STRLEN+1]; /* Menyrubrik */
    char altstr[20][V3STRLEN+1]; /* Alternativtexter, max 20 st */
    char alttyp[20];       /* Alternativtyper */
    short altnum[20];      /* Alternativnummer */
    char  trfnam[V3PTHLEN+1]; /* Filnamn processat för $env */

/*
***Processa filnamnet map. $ENVIRONMENT-parameter.
*/
    v3trfp(fnam,trfnam);
/*
***Öppna MDF-filen.
*/
    if ( (menufil=fopen(trfnam,"r")) == NULL )
      {
      erpush("IG0392",trfnam);
      return(-1);
      }
/*
***Läs kommentarer etc. till nästa t-sträng eller meny.
*/
    mode = W_REC;

    while ( mode != E_FIL )            /* läs record för record */
      {
      switch ( mode )
        {
        case W_REC:                    /* vänta på record ? */
        fscanf(menufil,SK_SP);         /* Ja, skippa whitespaces */
        rectyp = CDUMMY;               /* initiera rectyp med dummy */
        fscanf(menufil,"%c",&rectyp);  /* läs recordtyp */

        switch ( rectyp ) 
          {
          case SCOM:                    /* är det en kommentar ? */
          fscanf(menufil,SK_CT);  /* Ja, skippa kommentar */
          break;

          case INCL:
          fscanf(menufil,SK_SP);               /* Skippa whitespaces */
          fscanf(menufil,"%7s",tbuf);
          if ( strcmp(tbuf,"include") == 0 )
            {
            fscanf(menufil,SK_SP);             /* Skippa whitespaces */
            fscanf(menufil,GT_TXT,tbuf);       /* Läs filnamn */
            if ( iglmdf(tbuf) < 0 )
              {
              erpush("IG0522",tbuf);
              goto errend;
              }
            }
          else
            {
            erpush("IG0542",tbuf);
            goto errend;
            }
          break;

          case TSTR:                    /* är det en textrecord */
          case SYMBOL:                  /* är det ett symbolrecord */
          case MENU:                    /* Menu eller 'm' i main_menu */
          case MAIN_MENU:               /* 'M' i Main_menu */
          mode = L_REC;                 /* ja, dags att ladda record */
          break;

          default:
          mode = E_FIL;                 /* Okänd record => avslut !*/
          break;
          }
        break;
/*
***Record har påträffats. Läs recordnummer om det finns eller
***är det kanske ett "m" i main_menu... eller "M" i Main_menu...
*/    
        case L_REC:
        if ( fscanf(menufil,"%d",&recnum) != 1  &&  
             ( rectyp == 'm'  ||  rectyp == 'M' )  )
          {
          fscanf(menufil,"%8s",tbuf);
          if ( strcmp(tbuf,"ain_menu") == 0 )
            {
            fscanf(menufil,SK_EQ);
            fscanf(menufil,"%d",&recnum);
            rectyp = MAIN_MENU;
            }
          else
            {
            erpush("IG0532","");
            goto errend;
            }
          }
        else fscanf(menufil,SK_EQ);
/*
***Vilken typ av record är det ?
*/
        switch ( rectyp )
          {
/*
***Huvudmenyn.
*/
          case MAIN_MENU:
          igsmmu((short)recnum);
          mode = W_REC;
          break;
/*
***t-sträng.
*/
          case TSTR:
          if ( recnum < 0 || recnum >= TXTMAX ) goto error3;
          fscanf(menufil,GT_TXT,tbuf);   /* Läs t-strängen */

/*          if(txtindÄrecnumÅ != NULL) goto error6;              JK */
/*          if ( fstmem > txtmem+CHRMAX-81 ) goto error2;           */
/*          txtind[recnum] = fstmem;                                */
/*          editcopy(fstmem,tbuf);                                  */
/*          fstmem += strlen(fstmem) + 1;                           */

          EXcrts((short)recnum,tbuf);
          mode = W_REC;
          break;  
/*
***Symbol.
*/
          case SYMBOL:
          if ( recnum < 0 || recnum >= SMBMAX ) goto error10; 
          if ( fscanf(menufil,GT_SMB,tbuf,&teck1,&num1) != 3 ) goto error12; 
          if ( fstmem > (txtmem+CHRMAX-81) ) goto error2;       
          if ( teck1 != MENU && teck1 != ALT && teck1 != PART  &&
               teck1 != RUN && teck1 != FUNC && teck1 != MFUNC &&
               teck1 != OLDMF ) goto error13;
          if ( num1 < 0 ) goto error14;
          if ( teck1 == OLDMF ) teck1 = MFUNC;
          smbind[ recnum ].str = fstmem;         /* string pointer */
          smbind[ recnum ].acttyp = teck1;       /* action type */
          smbind[ recnum ].actnum = num1;        /* action number */
          editcopy(fstmem,tbuf);                         /* store string */
          fstmem += strlen(fstmem) + 1;
          mode = W_REC;
          break;  
/*
***Meny.
*/ 
          case MENU:
          if ( recnum < 0 || recnum >= MNUMAX ) goto error4;
          mnum = recnum;
          fldtyp = CDUMMY;
          nalt = 0;
/*
***Header.
*/
          fscanf(menufil,GT_MH,tbuf);
          editcopy(rubr,tbuf);
/*
***Alternativ.
*/
          while ( fldtyp != EOR )
             {
             fscanf(menufil,SK_SP);   /* skippa whitespaces */
             fscanf(menufil,"%c",&fldtyp);
             switch(fldtyp) /* titta på fältets första tecken */
                {
                case STRING:
                fscanf(menufil,GT_MA,tbuf,&alttyp[nalt],&altnum[nalt]);
                editcopy(altstr[nalt],tbuf);
                if ( alttyp[nalt] == OLDMF ) alttyp[nalt] = MFUNC;
                nalt++;
                break;

                case SCOM:                      /* kommentar */
                fscanf(menufil,SK_CT);      /* skippa kommentar */
                break;

                case EOR:                       /* slut på meny */
                if ( igstmu(mnum,rubr,nalt,altstr,alttyp,altnum) < 0 )
                  goto error5;
                mode=W_REC;                         /* vänta på ny record */
                break;             

                default:
                goto error8;
                }
             }
             break;
             default:
             goto error7;
           }
           break;
         }
       }
/*
***Stäng MDF-filen.
*/
    if ( fclose(menufil) == EOF ) goto error9;

    return(0);
/*
***Felutgångar.
*/
error2:
    printf("V3: Too many characters in MDF-file : %s\n",trfnam);
    goto errend;

error3:
    printf("V3: Illegal t-string number %d in MDF-file : %s\n",recnum,trfnam);
    goto errend;

error4:
    printf("V3: Illegal menu-number %d in MDF-file : %s\n",recnum,trfnam);
    goto errend;

error5:
    printf("V3: Can't malloc() for menu %d in MDF-file : %s\n",recnum,trfnam);
    goto errend;

error7:
    printf("V3: Illegal recordtype %c in MDF-file : %s\n",rectyp,trfnam);
    goto errend;

error8:
    printf("V3: Illegal menu-field in menu m%d in MDF-file : %s\n",
           recnum,trfnam);
    goto errend;

error9:
    printf("V3: Can't close MDF-file : %s\n",trfnam);
    return(-1);

error10:
    printf("V3: Illegal s-number %d. Must be >= 0 and < %d. MDF-file : %s\n",
            recnum,SMBMAX,trfnam);
    goto errend;

error12:
    printf("V3: Please check syntax in s%d. MDF-file: %s\n",recnum,trfnam);
    goto errend;

error13:
    printf(
    "V3: Illegal action type %c in s%d. Use %c,%c,%c,%c or %c. MDF-file: %s\n",
            teck1,recnum,MENU,ALT,RUN,PART,FUNC,trfnam);
    goto errend;

error14:
    printf("V3: Illegal action number %d in s%d. Must be >= 0. MDF-file: %s\n",
            num1,recnum,trfnam);
    goto errend;

errend:
    fclose(menufil);
    return(-1);
  }

/********************************************************/
/*!******************************************************/

        short igexit()

/*      Avslutningsrutin för V3:s toppnivå. Denna
 *      rutin anropas bara då man vill återvända till
 *      OS och terminalen skall återställas till det
 *      skick den var innan man startade V3. När
 *      igexit() anropas förutsätts terminalen vara
 *      i VT100-mode.
 *
 *      FV: 0
 *
 *      (C)microform ab 8/9/85 J. Kjellander
 *
 *      9/11/88  CGI, J. Kjellander
 *
 ******************************************************!*/

 {

#ifdef UNIX
/*
***Sudda skärm och återställ scroll region.
*/
   if ( igtrty != VT100  &&
        igtrty != X11    &&
        igtrty != BATCH )
     {
     igmvac(1,1);
     igerar(rmarg,bmarg);
     printf("\033[1;%dr",bmarg);
     }
#endif

   return(0);
 }

/********************************************************/
/*!******************************************************/

       short igexfu(
       short mnum,
       short *palt)

/*      Skriver ut menyn mnum, läser in svar från 
 *      operatören och utför genom att antingen
 *      skriva ut en ny meny eller anropa igdofu().
 *
 *      In: mnum   => Menynummer.
 *          palt   => Pekare till alternativ.
 *
 *      Ut: *palt   => alternativnummer
 *
 *      Felkoder : IG0044 => Menyn finns ej.
 *
 *      (C)microform ab 9/1/85 J. Kjellander
 *
 *      REVIDERAD:
 *
 *      29/10-85 Snabbval, Ulf Johansson
 *      5/11/85  Futab, J. Kjellander
 *      6/11/85  Meny noll, J. Kjellander
 *      9/10/86  GOMAIN, J. Kjellander
 *      25/4/87  MFUNC, J. Kjellander
 *      1/3/94   Snabbval, J. Kjellander
 *
 ******************************************************!*/

  {
    MNUALT *altptr;
    char    numstr[7];
    short   alttyp,status;

/*
***Kolla att menyn är definierad.
*/
    if ( mnum != 0  &&  mnutab[mnum].rubr == NULL )
      {
      sprintf(numstr,"%d",mnum);
      erpush("IG0024",numstr);
      errmes();
      *palt = REJECT;
      return(0);
      }
/*
***Skriv ut menyn, pip om nestnivån för stor.
*/
    if (!igaamu(mnum))
      {
      igbell();
      *palt = REJECT;
      return(0);
      }
/*
***Läs in alternativ.
*/
loop:
    if ( v3mode == RIT_MOD  &&  mnum == 4 ) pmsstp(pmstkp);
    iggalt(&altptr,&alttyp);
/*
***altptr == NULL => backa 1 meny eller alla.
*/
    if ( altptr == NULL )
      {
      switch( alttyp )
        {
        case SMBRETURN:
        igsamu();
        *palt = REJECT;
        return(0);
        break;

        case SMBMAIN:
        *palt = GOMAIN;
        return(GOMAIN);
        break;
        }
      }
/*
***altptr != NULL => val har gjorts, utför valet.
*/
    else
      {
      switch (altptr->acttyp)
        {
        case ALT:
        *palt = altptr->actnum;
        igsamu();
        return(0);
        break;
/*
***Meny, part eller funktion.
*/ 
        case MENU:
        case PART:
        case RUN:
        case FUNC:
        case MFUNC:
        status = igdofu(altptr->acttyp,altptr->actnum);
        if ( status == GOMAIN ) return(GOMAIN);
        else if ( status == EXIT ) return(EXIT);
        else if ( status == EREXIT ) return(EREXIT);
/*
***Inget alternativ aktivt längre.
*/
        igerpl();
        astack[mant] = -1;
        break;
        }
     }
     goto loop;
  }

/********************************************************/
/*!******************************************************/

       short igdofu(
       short atyp,
       short anum)

/*     
 *      Skriver ut en ny meny eller anropar en Varkon-
 *      funktion eller eller skapa en part-sats.
 *
 *      In: atyp   => Typ av aktion.
 *          anum   => Alternativ.
 *
 *      Felkoder : IG0043 => f%s kan ej anropas i detta sammanhang
 *                 IG2202 => f%s finns ej i systemet
 *                 IG2292 => f%s kräver X-Windows
 *
 *      (C)microform ab 9/1/85 J. Kjellander
 *
 *      13/10/86 Direkt till huvudmenyn, J. Kjellander
 *      20/10/86 HELP, J. Kjellander
 *      25/4/87  MFUNC, J. Kjellander
 *      9/12/84  Ny futab, J. Kjellander
 *      1997-01-15 f153, J.Kjellander
 *
 ******************************************************!*/

  {
    char  errbuf[81];
    short dummy,status,oldfun;

/*
***Initiering.
*/
    status = 0;
/*
***Vilken aktionskod är det ?
*/
    switch (atyp)
      {
      case MENU:
      return(igexfu(anum,&dummy));
      break;
/*
***En modul skall anropas. Då får inte en annan vara aktiv
***redan. Modul får bara anropas från meny.
*/
      case PART:
      case RUN:
      case MFUNC:
      if ( actfun  !=  -1 )
        {
        igbell();
        return(0);
        }
/*
***Ett part- eller macro-anrop kan avslutas med GOMAIN eller
***REJECT, annars är det fel.
*/
      if ( (status=igcpts(iggtts(anum),atyp)) == GOMAIN ) return(GOMAIN);
      else if ( status == REJECT ) return(0);
      else if ( status < 0 ) errmes();
      break;
/*
***Funktion, kolla att funktionsnumret är rimligt stort.
*/
      case FUNC:
      if ( anum < 1  ||  anum > FTABSIZ )
        {
        sprintf(errbuf,"%d",anum);
        erpush("IG2202",errbuf);
        errmes();
        return(0);
        }
/*
***Om denna funktion anropas i en annan funktion eller
***under körning av modul måste den vara en sån som
***får det.
*/
      if ( actfun != -1  &&  futab[anum-1].snabb == FALSE )
        {
        igbell();
        return(0);
        }
/*
***Om anum = actfun betyder det att vi försöker
***anropa samma funktion 2 ggr. efter varandra utan att
***göra klart. Detta skall väl inte vara möjligt.
*/
      if ( anum == actfun )
        {
        igbell();
        return(0);
        }
/*
***Spara actfun så den kan återställas efter att
***funktionen anum har anropats. Om anum = actfun försöker
***vi anropa samma funktion 2 ggr. efter varandra utan att
***göra klart. Detta skall väl inte vara möjligt. Funktionen
***Hjälp (ighelp()=f153) skall inte vis hjälp om sig själv
***ifall den anropas utan om den situation som gällde när
***den anropades.
*/
      oldfun = actfun;
      if ( anum != 153 ) actfun = anum;
/*
***Anropa funktionen. Om den klassats för NONE_MOD
***anropar vi den ändå så att notimpl() eller wpunik()
***får ta hand om felhanteringen.
*/
      if ( (futab[anum-1].modul & v3mode)  ||
                             (futab[anum-1].modul == NONE_MOD) )
        {
/*
***Vissa funktioner får bara anropas med X11 som terminaltyp.
*/
        if ( (futab[anum-1].modul & X11_MOD) && (gptrty != X11) )
          {
          sprintf(errbuf,"%d",anum);
          erpush("IG2292",errbuf);
          errmes();
          }
/*
***Andra får det inte.
*/
        else if ( (futab[anum-1].modul & NO_X11_MOD) && (gptrty == X11) )
          {
          sprintf(errbuf,"%d",anum);
          erpush("IG2302",errbuf);
          errmes();
          }
/*
***Övriga är OK.
*/
        else status = ((*futab[anum-1].fnamn)());
        }
/*
***Vissa får bara användas i RIT eller BAS-2D osv.
*/
      else
        {
        sprintf(errbuf,"%d",anum);
        erpush("IG0043",errbuf);
        errmes();
        }

      actfun = oldfun;
      break;
      }
/*
***Slut.
*/
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short notimpl()

/*      Dummyfunktion för de funktionsnummer i funktions-
 *      tabellerna som inte motsvarar riktiga Varkon-
 *      funktioner.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder : IG2202 => Funktionen finns ej.
 *
 *      (C)microform ab 9/1/85 J. Kjellander
 *
 ******************************************************!*/

{
   char buf[V3STRLEN+1];

   sprintf(buf,"%d",actfun);
   erpush("IG2202",buf);
   errmes();

   return(0);
}

/********************************************************/
/*!******************************************************/

        short wpunik()

/*      Dummyfunktion för de funktionsnummer i funktions-
 *      tabellen som  motsvarar Varkonfunktioner som bara
 *      får anropas från ett WPGWIN-fönster.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      Felkoder : IG2282 => Funktionen %s skall anropas från
 *                           ett fönster.
 *
 *      (C)microform ab 12/1-95 J. Kjellander
 *
 ******************************************************!*/

{
   char buf[V3STRLEN+1];

   sprintf(buf,"%d",actfun);
   erpush("IG2282",buf);
   errmes();

   return(0);
}

/********************************************************/
/*!******************************************************/

        short editcopy(
        char *p1,
        char *p2)

/*      Kopierar p2 till p1 och konverterar numeriskt
 *      angivna ASCII-tecken.
 *
 *      In: p1 = Pekare till sträng ev. med %ASCII-kod.
 *
 *      Ut: p2 = Pekare till sträng utan %.
 *
 *      Felkoder : Inga.
 *
 *      (C)microform ab 1985 U. Johansson
 *
 ******************************************************!*/

{
     register char *s1,*s2;
     register char ch;
     register short n;

     s1 = p1;
     s2 = p2;

     do {
          ch = *s2++;
          while (ch == NUMASCII) {
               ch = *s2++;
               if (ch >= '0' && ch <= '9') {
                    n = ch - '0';
                    ch = *s2++;
                    if (ch >= '0' && ch <= '9') {
                         n = 10*n + ch - '0';
                         ch = *s2++;
                         if (ch >= '0' && ch <= '9') {
                              n = 10*n + ch - '0';
                              ch = *s2++;
                         }
                    }
                    *s1++ = (char)n;
               } else
                    break;
          }
          *s1++ = ch;
     } while (ch != '\0');

  return(0);
}

/********************************************************/
/*!******************************************************/

        short igstmu(
        short mnum,
        char *rubr,
        short nalt,
        char  altstr[][V3STRLEN+1],
        char  alttyp[],
        short altnum[])

/*      mallocerar minne och lagrar meny.
 *
 *      In: mnum   = Menynummer.
 *          rubr   = Pekare till rubriktext.
 *          nalt   = Antal alternativ i menyn.
 *          altstr = Pekare till array med alternativtexter.
 *          alttyp = Pekare till array med alttyp-tecken.
 *          altnum = Pekare till array med alternativnummer.
 *
 *      Ut: Inget.
 *
 *      FV:  0 = Ok.
 *          -1 = Can't malloc().
 *
 *      (C)microform ab 27/2/91 J. Kjellander
 *
 ******************************************************!*/

{
   int     siz,i,maxwdt;
   char   *pstr;
   MNUALT *palt;

/*
***Om menyn mnum redan finns, lämna först tillbaks den.
*/
   if ( mnutab[mnum].rubr != NULL )
     {
     v3free(mnutab[mnum].rubr,"");
     v3free(mnutab[mnum].alt,"");
     mnutab[mnum].rubr = NULL;
     }
/*
***Om rubr = "tom sträng", lagra ingenting.
*/
   if ( *rubr == '\0' ) return(0);
/*
***Allokera minne.
*/
   siz = strlen(rubr) + 1;
   for ( i=0; i<nalt; ++i ) siz += strlen(altstr[i]) + 1;
   if ( (pstr=v3mall(siz,"")) == NULL ) return(-1);
   mnutab[mnum].rubr = pstr;

   siz = nalt * sizeof(MNUALT);
   if ( (palt=(MNUALT *)v3mall(siz,"")) == NULL ) return(-1);
   mnutab[mnum].alt = palt;
/*
***Lagra.
*/
   strcpy(pstr,rubr);
   pstr += strlen(rubr)+1;

   for ( i=0; i<nalt; ++i )
     {
     palt->str = pstr;
     palt->acttyp = alttyp[i],
     palt->actnum = altnum[i];
     strcpy(pstr,altstr[i]); 
     pstr += strlen(altstr[i]) + 1;
     ++palt;
     }
/*
***Maxbredd.
*/
   maxwdt = strlen(rubr);
   for ( i=0; i<nalt; ++ i )
     if ( strlen(altstr[i]) > maxwdt ) maxwdt = strlen(altstr[i]);
   mnutab[mnum].wdth = maxwdt;
/*
***Antal alternativ.
*/
   mnutab[mnum].nalt = nalt;

   return(0);
}

/********************************************************/

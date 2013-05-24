/*!******************************************************************/
/*  File: iginit.c                                                  */
/*  ==============                                                  */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  IGinit();      Inits IG                                         */
/*  IGlmdf();      Loads MDF-file                                   */
/*  IGexfu();      Executes function                                */
/*  IGdofu();      Executes function                                */
/*  IGatoc();      Interpret %ASCII-code                            */
/*  IGstmu();      Stores menu im allocated memory                  */
/*  IGsini();      Inits signals                                    */
/*  IGgtts();      Get C-ptr to t-string                            */
/*  IGckhw();      Check HW                                         */
/*                                                                  */
/*  notimpl();     Dummy-routine                                    */
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
#include "../include/futab.h"
#include "../../EX/include/EX.h"
#include <string.h>
#include <signal.h>

#ifdef UNIX
#undef VSTART
#include "fcntl.h"
#include <unistd.h>
#include "/usr/include/sys/utsname.h"
#endif

/*
***Menus and texts are stored here.
*/
char   txtmem[CHRMAX]; /* "ragged" array  of t-strings */
char  *txtind[TXTMAX]; /* t-string ptrs */
char  *fstmem;         /* ptr to first empty element in txtmem */
MNUDAT mnutab[MNUMAX]; /* Menus */
MNUALT smbind[SMBMAX]; /* Shortcuts */

extern short  sysmode;
extern short  mant;
extern pm_ptr pmstkp;
extern char   speed[];
extern char   pltyp[];
extern char   port[];
extern int    actfunc;
extern bool   intrup;
extern V3MDAT sydata;

/*
***scanf() defines for menufile processing.
*/
#define W_REC 1            /* mode = wait for record */
#define L_REC 2            /* mode = load record */
#define E_FIL 3            /* mode = end of file */
#define CDUMMY '+'         /* dummy constant */
#define SK_SP "%*[\n\t ]"  /* skip whitespace */
#define SK_CT "%*[\136\n]" /* skip comment (to <CR>) */
#define SK_EQ "%*[=\n\t ]" /* skip w.spaces och '=' */
#define SK_CA "%*[,\n\t ]" /* skip w.spaces och ',' */
#define SK_SC "%*[;\n\t ]" /* skip w.spaces och ';' */

/* get t-record text-string*/
#define GT_TXT "%*[\042]%[\136\042]%*[\042]"

/* Get Menu Header field, 27/2/91 JK */
#define GT_MH "%*[\042]%[\136\042]%*[\042]%*[\136\n]"

/* get menu alternativ field */
#define GT_MA "%[\136\042]%*[\042%*[ ,]%c%hd"

/* get s-record */
#define GT_SMB "%*[\42]%[\136\42]%*[\42]%*[ ,]%c%hd"

/*
***Prototypes for internal functions.
*/
static void sigtrp(int sigval);

/*!******************************************************/

        short IGinit(char *fnam)

/*      Init IG. Loads menus and texts.
 *
 *      In: fnam => Ptr to name of MDF-file including
 *                  path.
 *
 *      (C)microform ab 19/1/85 M. Nelson
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
 *      2007-01-01 Removed GP, J.Kjellander
 *
 ******************************************************!*/

  {
   short i,status;

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
***Which menu is the main menu ?
***3 for GENERIC, 4 for EXPLICIT or user defined.
*/
   IGsmmu((short)0);
/*
***Load the MDF-file.
*/
   if ( (status=IGlmdf(fnam)) < 0 ) return(status);
/*
*** Defaultvärden för plottning.
*/
   strcpy(speed,IGgtts(334));
   strcpy(pltyp,IGgtts(348));
   strcpy(port,IGgtts(346));
/*
***Kör vi WIN32 kan vi nu bygga huvudfönstrets menyträd.
*/
#ifdef WIN32
    mscrmu();
#endif

    return(0);
  }

/********************************************************/
/*!******************************************************/

        short IGlmdf(char *fnam)

/*      Loads an MDF-file.
 *
 *      In: fnam => Ptr to path.
 *
 *      Error exits: IG0392 = Menyfilen %s finns ej.
 *                   IG0522 = Kan ej inkludera menyfilen %s 
 *                   IG0532 = Felaktigt recordnummer
 *                   IG0542 = Ej include efter #
 *                   IG0552 = Many other errors
 *
 *      (C)microform ab 18/4/87 J. Kjellander
 *
 *      19/4/87 #include, J. Kjellander
 *      7/10/87 Redefine på s- och t-str. J. Kjellander
 *      18/2/92 MACRO, J. Kjellander
 *      23/3/95 IGtrfp(), J. Kjellander
 *      1996-02-26 Mer erpush(), J. Kjellander
 *      1996-02-26 main_menu, J. Kjellander
 *      2007-01-07 Removed GP, J.Kjellaner
 *
 ******************************************************!*/

  {
    FILE *menufil;                /* menyfilen, pekare till */
    short mode;                   /* inläsningsmode W_REC, L_REC eller E_FIL */
    char  rectyp;                 /* recordtyp t,s,i eller m */
    int   recnum;                 /* recordnummer */
    char  fldtyp;                 /* fälttyp */
    short num1;                   /* nummer */
    char  teck1;                  /* tecken */
    short mnum,nalt;              /* Menynummer och antal alternativ */
    char  tbuf[V3STRLEN+1];       /* temp. buffert för textsträngar */
    char  rubr[V3STRLEN+1];       /* Menyrubrik */
    char  altstr[20][V3STRLEN+1]; /* Alternativtexter, max 20 st */
    char  alttyp[20];             /* Alternativtyper */
    short altnum[20];             /* Alternativnummer */
    char  trfnam[V3PTHLEN+1];     /* Filnamn processat för $env */
    char  errbuf[V3STRLEN];       /* Buffer for error mesage */

/*
***Process path for $ENVIRONMENT names.
*/
    IGtrfp(fnam,trfnam);
/*
***Open MDF-file.
*/
    if ( (menufil=fopen(trfnam,"r")) == NULL )
      {
      return(erpush("IG0392",trfnam));
      }
/*
***Read comments etc. to next t-string or menu.
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
          case SCOM:                    /* Skip comments */
          fscanf(menufil,SK_CT);
          break;

          case INCL:                    /* Include another file */
          fscanf(menufil,SK_SP);
          fscanf(menufil,"%7s",tbuf);
          if ( strcmp(tbuf,"include") == 0 )
            {
            fscanf(menufil,SK_SP);
            fscanf(menufil,GT_TXT,tbuf);       /* Read filnamne */
            if ( IGlmdf(tbuf) < 0 )            /* Load recursively */
              {
              fclose(menufil);
              return(erpush("IG0522",tbuf));
              }
            }
          else
            {
            fclose(menufil);
            return(erpush("IG0542",tbuf));
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
            fclose(menufil);
            return(erpush("IG0532",""));
            }
          }
        else fscanf(menufil,SK_EQ);
/*
***Vilken typ av record är det ?
*/
        switch ( rectyp )
          {
/*
***Main menu.
*/
          case MAIN_MENU:
          IGsmmu((short)recnum);
          mode = W_REC;
          break;
/*
***t-string.
*/
          case TSTR:
          if ( recnum < 0 || recnum >= TXTMAX ) goto error3;
          fscanf(menufil,GT_TXT,tbuf);
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
               teck1 != RUN && teck1 != CFUNC && teck1 != MFUNC &&
               teck1 != OLDMF ) goto error13;
          if ( num1 < 0 ) goto error14;
          if ( teck1 == OLDMF ) teck1 = MFUNC;
          smbind[ recnum ].str = fstmem;         /* string pointer */
          smbind[ recnum ].acttyp = teck1;       /* action type */
          smbind[ recnum ].actnum = num1;        /* action number */
          IGatoc(fstmem,tbuf);                         /* store string */
          fstmem += strlen(fstmem) + 1;
          mode = W_REC;
          break;  
/*
***Menu.
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
          IGatoc(rubr,tbuf);
/*
***Menu alternative.
*/
          while ( fldtyp != EOR )
             {
             fscanf(menufil,SK_SP);   /* skippa whitespaces */
             fscanf(menufil,"%c",&fldtyp);
             switch(fldtyp) /* titta på fältets första tecken */
                {
                case STRING:
                fscanf(menufil,GT_MA,tbuf,&alttyp[nalt],&altnum[nalt]);
                IGatoc(altstr[nalt],tbuf);
                if ( alttyp[nalt] == OLDMF ) alttyp[nalt] = MFUNC;
                nalt++;
                break;

                case SCOM:                      /* kommentar */
                fscanf(menufil,SK_CT);      /* skippa kommentar */
                break;

                case EOR:                       /* slut på meny */
                if ( IGstmu(mnum,rubr,nalt,altstr,alttyp,altnum) < 0 )
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
***Close MDF-file.
*/
    if ( fclose(menufil) == EOF ) goto error9;

    return(0);
/*
***Error exit's.
*/
error2:
    sprintf(errbuf,"Too many characters in MDF-file : %s",trfnam);
    goto errend;

error3:
    sprintf(errbuf,"Illegal t-string number %d in MDF-file : %s",recnum,trfnam);
    goto errend;

error4:
    sprintf(errbuf,"Illegal menu-number %d in MDF-file : %s",recnum,trfnam);
    goto errend;

error5:
    sprintf(errbuf,"Can't malloc() for menu %d in MDF-file : %s",recnum,trfnam);
    goto errend;

error7:
    sprintf(errbuf,"Illegal recordtype %c in MDF-file : %s",rectyp,trfnam);
    goto errend;

error8:
    sprintf(errbuf,"Illegal menu-field in menu m%d in MDF-file : %s",
           recnum,trfnam);
    goto errend;

error9:
    sprintf(errbuf,"Can't close MDF-file : %s",trfnam);
    goto errend;

error10:
    sprintf(errbuf,"Illegal s-number %d. Must be >= 0 and < %d. MDF-file : %s",
            recnum,SMBMAX,trfnam);
    goto errend;

error12:
    sprintf(errbuf,"Please check syntax in s%d. MDF-file: %s",recnum,trfnam);
    goto errend;

error13:
    sprintf(errbuf,"Illegal action type %c in s%d. Use %c,%c,%c,%c or %c. MDF-file: %s",
            teck1,recnum,MENU,ALT,RUN,PART,CFUNC,trfnam);
    goto errend;

error14:
    sprintf(errbuf,"Illegal action number %d in s%d. Must be >= 0. MDF-file: %s",
            num1,recnum,trfnam);
    goto errend;

errend:
    fclose(menufil);
    return(erpush("IG0552",errbuf));
  }

/********************************************************/
/*!******************************************************/

       short IGexfu(
       short mnum,
       short *palt)

/*      Pushes a menu, wats for user action and reacts
 *      by either pushing a new menu or making an action
 *      by calling IGdofu().
 *
 *      In: mnum   => Menu number.
 *          palt   => User action.
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
 *      2007-04-03 1.19, J.Kjellander
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
      erpush("IG0022",numstr);
      errmes();
      *palt = REJECT;
      return(0);
      }
/*
***Push menu, pip om nestnivån för stor.
*/
    if (!IGaamu(mnum))
      {
      WPbell();
      *palt = REJECT;
      return(0);
      }
/*
***Läs in alternativ.
*/
loop:
    if ( sysmode == EXPLICIT  &&  mnum == 4 ) pmsstp(pmstkp);
    IGgalt(&altptr,&alttyp);
/*
***altptr == NULL => backa 1 meny eller alla.
*/
    if ( altptr == NULL )
      {
      switch( alttyp )
        {
        case SMBRETURN:
        IGsamu();
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
        IGsamu();
        return(0);
        break;
/*
***Menu, part eller function.
*/ 
        case MENU:
        case PART:
        case RUN:
        case CFUNC:
        case MFUNC:
        status = IGdofu(altptr->acttyp,altptr->actnum);
        if ( status == GOMAIN ) return(GOMAIN);
        else if ( status == EXIT ) return(EXIT);
        else if ( status == EREXIT ) return(EREXIT);
        break;
        }
     }
     goto loop;
  }

/********************************************************/
/********************************************************/

       short IGdofu(
       short atyp,
       short anum)

/*      Executes different types of actions.
 *
 *      In: atyp   => Type of action.
 *          anum   => Alternative.
 *
 *      Felkoder : IG0043 => f%s may not be used in this context
 *                 IG2202 => f%s does not exist
 *
 *      (C)microform ab 9/1/85 J. Kjellander
 *
 *      13/10/86 Direkt till huvudmenyn, J. Kjellander
 *      20/10/86 HELP, J. Kjellander
 *      25/4/87  MFUNC, J. Kjellander
 *      9/12/84  Ny futab, J. Kjellander
 *      1997-01-15 f153, J.Kjellander
 *      2007-01-03 Removed GP, J.Kjellander
 *      2007-11-14 2.0, J.Kjellander
 *
 ******************************************************!*/

  {
    char  errbuf[81];
    short dummy,status,oldfun;

/*
***Init.
*/
    status = 0;
/*
***Clear possible tooltip.
*/
    WPclear_tooltip();
/*
***What kind of action is it ? A menu maybe ?
*/
    switch ( atyp )
      {
      case MENU:
      return(IGexfu(anum,&dummy));
      break;
/*
***A module call. Check that another module is not
***already executing.
*/
      case PART:
      case RUN:
      case MFUNC:
      if ( actfunc  !=  -1 )
        {
        WPbell();
        return(0);
        }
/*
***Part- or macro- calls can return REJECT or GOMAIN or
***error.
*/
      if ( atyp == MFUNC ) status = IGcall_macro(IGgtts(anum));
      else                 status = IGcall_part(IGgtts(anum),atyp);

      if      ( status == GOMAIN ) return(GOMAIN);
      else if ( status == REJECT ) return(0);
      else if ( status < 0 ) errmes();
      break;
/*
***A C function, check that the function number is valid.
*/
      case CFUNC:
      if ( anum < 1  ||  anum > FTABSIZ )
        {
        sprintf(errbuf,"%d",anum);
        erpush("IG2202",errbuf);
        errmes();
        return(0);
        }
/*
***If a function is already actice, check that this function
***can interrupt.
*/
      if ( actfunc != -1  &&  futab[anum-1].call == FALSE )
        {
        WPbell();
        return(0);
        }
/*
***If anum = actfun something must be wrong. There is no
***reason for a function to call itself.
*/
      if ( anum == actfunc )
        {
        WPbell();
        return(0);
        }
/*
***Save current actfunc and set it to thenew value.
***f153 is IGhelp() and should not display
***help about itself.
*/
      oldfun = actfunc;
      if ( anum != 153 ) actfunc = anum;
/*
***Call the function if it is allowed in this context.
*/
      if ( (futab[anum-1].mode & sysmode) )
        {
        status = ((*futab[anum-1].fnamn)());
        }
/*
***The function is not allowed in this context.
*/
      else
        {
        sprintf(errbuf,"%d",anum);
        erpush("IG0043",errbuf);
        errmes();
        }
/*
***Reset actfun.
*/
      actfunc = oldfun;
      break;
      }
/*
***The end.
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

   sprintf(buf,"%d",actfunc);
   erpush("IG2202",buf);
   errmes();

   return(0);
}

/********************************************************/
/*!******************************************************/

        short IGatoc(
        char *p1,
        char *p2)

/*      Copies p2 to p1 and converts numerically specified
 *      %ASCII-codes to single byte char.
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

        short IGstmu(
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
   int     siz,i;
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
***Antal alternativ.
*/
   mnutab[mnum].nalt = nalt;

   return(0);
}

/********************************************************/
/*!******************************************************/

        short IGsini()

/*      Sätter upp vilka signaler som skall fångas.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      (C)microform ab 25/11/85 J. Kjellander
 *
 *      23/10/86 Ny hantering av SIGFPE, J. Kjellander
 *      4/3/92   Hangup, J. Kjellander
 *      3/4/92   SIGTERM, J. Kjellander
 *
 ******************************************************!*/

  {

#ifdef WIN32
   return(0);
#else
/*
***Fånga signalerna hangup, interrupt och quit.
*/
    signal(SIGHUP,sigtrp);
    signal(SIGINT,sigtrp);
    signal(SIGQUIT,sigtrp);
    signal(SIGTERM,sigtrp);
/*
***Fånga inte flyttalsfel men låt core-fil skapas.
*/
    signal(SIGFPE,SIG_DFL);
#endif

    return(0);
  }

/********************************************************/
/*!******************************************************/

static void sigtrp(int sigval)

/*      Trap-rutin för signaler.
 *
 *      In: sigval => Nummer på signal som orsakat trap.
 *
 *      Ut: Inget.
 *
 *      FV: Inget.
 *
 *      (C)microform ab 20/2/85 J. Kjellander
 *
 *      9/5/86   SIGFPE, J. Kjellander
 *      23/10/86 Ny hantering av SIGFPE, J. Kjellander
 *      16/3/88  IGexit(), J. Kjellander
 *      4/3/92   Hangup, J. Kjellander
 *      3/4/92   SIGTERM, J. Kjellander
 *      1998-02-22 void, J.Kjellander
 *
 ******************************************************!*/

  {

#ifdef WIN32
   return;
#else

   switch (sigval)
     {
/*
***Hangup, tex. om ett modem kopplar ner.
*/
     case SIGHUP:
     case SIGTERM:
     signal(SIGHUP,SIG_IGN);
     signal(SIGTERM,SIG_IGN);
     if ( sysmode & (GENERIC+EXPLICIT) ) IGexit_sa();
     IGexit();
     break;
/*
***Interrupt, normalt <CTL>c.
*/
     case SIGINT:
     IGsini();
     intrup = TRUE;
     break;
/*
***Quit, normalt <DEL>.
*/
     case SIGQUIT:
     if ( sysmode & (GENERIC+EXPLICIT) ) IGexit_sa();
     IGexit();
     break;
     }
#endif

   return;
  }

/********************************************************/
/*!******************************************************/

       char *IGgtts(int tnr)

/*      Returnerar en c-pekare till den med tnr angivna
 *      t-strängen.
 *
 *      In: tnr = T-strängpekarens index i txtind.
 *
 *      FV: C-pekare till sträng.
 *
 *      (C)microform ab 1996-05-30 J.Kjellander
 *
 ******************************************************!*/


  {
    char  *tp; 
    static char notdef[80];

/*
***Har tnr ett rimligt värde ?
*/
    if ( tnr < 0  ||  tnr >= TXTMAX )
      {
      sprintf(notdef,"<invalid t%d>",tnr);
      return(notdef);
      }
/*
***tnr == 0 means NULL string.
*/
    if ( tnr == 0 )
      {
      notdef[0] = '\0';
      return(notdef);
      }
/*
***Fixa fram motsvarande pekare.
*/
    tp = txtind[tnr];
/*
***Är den definierad ?
*/
    if ( tp == NULL )
      {
      sprintf(notdef,"<no t%d>",tnr);
      return(notdef);
      }

    return(tp);
  }

/*!******************************************************/
/*!******************************************************/

        short IGckhw()

/*      Initierar sydata och kollar att rätt hårdvara
 *      används. Krypterar serienumret.
 *
 *      (C)microform ab 3/3/88 J. Kjellander
 *
 *      1996-01-25 Tagit bort sysserial, J. Kjellander
 *
 ******************************************************!*/

  {

/*
***Unix.
*/

#ifdef UNIX
    struct utsname name;

    uname(&name);
    strncpy(sydata.sysname,name.sysname,8);
    sydata.sysname[8] = '\0';
    strncpy(sydata.release,name.release,8);
    sydata.release[8] = '\0';
    strncpy(sydata.version,name.version,8);
    sydata.version[8] = '\0';
#endif

/*
***Microsoft Windows.
*/

#ifdef WIN32
    char  buf[9];
    int   major,minor;
    DWORD version;

    strcpy(sydata.sysname,"Win32");
    version = GetVersion();
    major = (int)(version&0x00FF);
    minor = (int)((version&0xFF00)>>8);
    sprintf(buf,"%d",major);
    buf[8] = '\0';
    strcpy(sydata.release,buf);
    sprintf(buf,"%d",minor);
    buf[8] = '\0';
    strcpy(sydata.version,buf);
#endif

/*
***Encrypted serial number is not used any longer.
*/
   sydata.ser_crypt = 0;

   return(0);
  }

/********************************************************/

/**********************************************************************
*
*    evfile.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    evopen();     Evaluates OPEN
*    evclos();     Evaluates CLOSE
*    v3isop();     Is this file open ?
*
*    evappf();     Evaluates APPEND_FILE
*    evdelf();     Evaluates DELETE_FILE
*    evmovf();     Evaluates MOVE_FILE
*    evcopf();     Evaluates COPY_FILE
*    evtstf();     Evaluates TEST_FILE
*
*    evouti()      Evaluates OUTINT
*    evoutf()      Evaluates OUTFLT
*    evouts()      Evaluates OUTSTR
*    evoutl();     Evaluates OUTLIN
*    evoutb()      Evaluates OUTBIN
*
*    evinii()      Evaluates ININT
*    evinif()      Evaluates INFLT
*    evinis()      Evaluates INSTR
*    evinl()       Evaluates INLIN
*    evinb()       Evaluates INBIN
*
*    evseek()      Evaluates SEEK
*    evfpos()      Evaluates FPOS
*    eviost()      Evaluates IOSTAT
*    evufnm()      Evaluates UNIQUE_FILENAME
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
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/indef.h"
#include <string.h>

#ifdef UNIX
#include <unistd.h>
#include <stdio.h>
#endif

#ifdef WIN32
#include <io.h>
#endif

/*
***POSIX functions.
*/
extern int   mkstemp();
extern FILE *fdopen();

/*
***Non ANSI functions.
*/
extern FILE *popen();
extern int   pclose();

/*
***fileno() is also non ANSI but trying to declare it here
***kills the build process on FreeBSD. Untill this problem
***is resolved we will not try to declare fileno(). This
***will however create the following compiler warning:
***warning: implicit declaration of function 'fileno'
*/
/* extern int   fileno(); */

/*
***Global variables.
*/
extern PMPARVA *proc_pv;  /* inproc.c *pv Access structure for MBS routines */
extern short    proc_pc;  /* inproc.c parcount Number of actual parameters */

extern PMPARVA *func_pv;  /* Access structure for MBS routines */
extern short    func_pc;  /* Number of actual parameters */
extern PMLITVA *func_vp;  /* Pekare till resultat. */

/*
***MBS table of open files.
*/
typedef struct open_file
{
char       name[V3STRLEN+1];
V2FILE    *fptr;
}OPEN_FILE;

#define MAXOFILES 50

static OPEN_FILE filtab[MAXOFILES] =
     {{"",NULL},{"",NULL},{"",NULL},{"",NULL},{"",NULL},
      {"",NULL},{"",NULL},{"",NULL},{"",NULL},{"",NULL},
      {"",NULL},{"",NULL},{"",NULL},{"",NULL},{"",NULL},
      {"",NULL},{"",NULL},{"",NULL},{"",NULL},{"",NULL},
      {"",NULL},{"",NULL},{"",NULL},{"",NULL},{"",NULL},
      {"",NULL},{"",NULL},{"",NULL},{"",NULL},{"",NULL},
      {"",NULL},{"",NULL},{"",NULL},{"",NULL},{"",NULL},
      {"",NULL},{"",NULL},{"",NULL},{"",NULL},{"",NULL},
      {"",NULL},{"",NULL},{"",NULL},{"",NULL},{"",NULL},
      {"",NULL},{"",NULL},{"",NULL},{"",NULL},{"",NULL}};

/*
***Prototypes for internal functions.
*/
static int   fndfree();
static int   close_all();
static int   fndfil(char *name);
static int   fndptr(V2FILE *fptr);
static short rfield(V2FILE *f, int fieldlen, char *fieldstr);
static short evinio(pmvaty iotyp, PMPARVA pv[], PMLITVA *valp);

/*!******************************************************/

        short evopen()

/*      Evaluerar OPEN.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Felkod: IN3253 = Felaktig mode = %s
 *              IN5322 = Filen %s är redan öppen
 *              IN5332 = Max antal filer redan öppna
 *
 *      (C)microform ab 1997-01-30 J. Kjellander
 *
 *      2001-02-12 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   int        i;
   char       pmode[2];
   char      *name,*mode;
   V2FILE    *f;

/*
***Filens namn ev med path.
*/
   name = proc_pv[3].par_va.lit.str_va;
/*
***Kolla att den inte redan är öppnad.
*/
   if ( v3isop(name) ) return(erpush("IN5322",name));
/*
***Kolla att det finns plats i filtab.
*/
   if ( (i=fndfree()) == -1 ) return(erpush("IN5332",name));
/*
***Filvariabel.
*/
   f = proc_pv[1].par_va.lit.fil_va;
/*
***Kolla mode.
*/
   mode = proc_pv[2].par_va.lit.str_va;

   if ( sticmp("w",mode) == 0 )
     {
     f->mode[0] = 'w';
     f->mode[1] = '\0';
     }
   else if ( sticmp("wp",mode) == 0 )
     {
     f->mode[0] = 'w';
     f->mode[1] = 'p';
     f->mode[2] = '\0';
     }
   else if ( sticmp("r",mode) == 0 )
     {
     f->mode[0] = 'r';
     f->mode[1] = '\0';
     }
   else if ( sticmp("rp",mode) == 0 )
     {
     f->mode[0] = 'r';
     f->mode[1] = 'p';
     f->mode[2] = '\0';
     }
   else if ( sticmp("u",mode) == 0 )
     {
     f->mode[0] = 'u';
     f->mode[1] = '\0';
     }
   else if ( sticmp("a",mode) == 0 )
     {
     f->mode[0] = 'a';
     f->mode[1] = '\0';
     }
   else return(erpush("IN3253",mode));
/*
***Öppna för update.
*/
   if ( f->mode[0] == 'u' )
     {
     if ( (f->fp=fopen(name,"r+")) == NULL )
       {
       if ( (f->fp=fopen(name,"w+")) == NULL ) f->iostat = -2;
       else                                    f->iostat =  0;
       }
     else f->iostat = 0;
     }
/*
***Pipe open.
*/
   else if ( f->mode[1] == 'p' )
     {
     if ( f->mode[0] == 'r' ) strcpy(pmode,"r");
     else                     strcpy(pmode,"w");
#ifdef WIN32
     if ( (f->fp = _popen(name,pmode) ) == NULL ) f->iostat = -2;
     else                                         f->iostat =  0;
#else
     if ( (f->fp =  popen(name,pmode) ) == NULL ) f->iostat = -2;
     else                                         f->iostat =  0;
#endif
     }
/*
***Vanlig read/write/append mode.
*/
   else
     {
     if ( (f->fp=fopen(name,f->mode)) == NULL ) f->iostat = -2;
     else                                       f->iostat =  0;
     }
/*
***Hur gick det ?
*/
   if ( f->iostat == 0 )
     {
     strcpy(filtab[i].name,name);
     filtab[i].fptr = f;
     f->open = TRUE;
     }
   else f->open = FALSE;

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evclos()

/*      Evaluerar CLOSE.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *          extern proc_pc => Antal parametrar.
 *
 *      Felkod: IN5342 = Kan ej stänga, filen ej öppen
 *              IN5353 = Systemfel, kan ej stänga %s
 *
 *      (C)microform ab 1997-01-30 J. Kjellander
 *
 *      2001-02-12 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   int     i,status;
   V2FILE *f;

/*
***On ingen parameter skickats med skall alla filer stängas.
*/
   if ( proc_pc == 0 ) return((short)close_all());
/*
***Annars finns det en parameter = Filpekare.
*/
   f = proc_pv[1].par_va.lit.fil_va;
/*
***Kolla att den är öppnad.
*/
   if ( (i=fndptr(f)) == -1 ) return(erpush("IN5342",""));
/*
***Stäng den.
*/
   if ( f->open )
     {
/*
***Pipe.
*/
     if ( f->mode[1] == 'p' )
       {
#ifdef WIN32
       if ( _pclose(f->fp) < 0 ) f->iostat = -3;
       else                      f->iostat =  0;
#else
       if (  pclose(f->fp) < 0 ) f->iostat = -3;
       else                      f->iostat =  0;
#endif
       }
/*
***Vanlig fil.
*/
     else
       {
       if ( fclose(f->fp) < 0 ) f->iostat = -3;
       else                     f->iostat =  0;
       }
     }
/*
***f->open = FALSE. Skall ju inte kunna hända numera men får
***vara kvar ändå.
*/
   else f->iostat = -10;
/*
***Stänga en fil bör man alltid kunna göra om den var öppen.
*/
   if ( f->iostat < 0 ) status = erpush("IN5353",filtab[i].name);
   else                 status = 0;
/*
***Oavsett hur det gick noterar vi stängt i filvariabel och filtabell.
*/
   f->open = FALSE;

  *filtab[i].name = '\0';
   filtab[i].fptr = NULL;

   return(status);
 }

/********************************************************/
/*!******************************************************/

        short evoutl()

/*      Evaluerar OUTLIN.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Felkod: IN5362 = Filen ej öppen
 *
 *      (C)microform ab 1997-01-30 J. Kjellander
 *
 *      2001-02-12 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   int     i;
   V2FILE *f;

/*
***Filpekare.
*/
   f = proc_pv[1].par_va.lit.fil_va;
/*
***Kolla att den är öppnad.
*/
   if ( (i=fndptr(f)) == -1 ) return(erpush("IN5362",""));
/*
***Kolla att den är öppnad med rätt mode.
*/
   if ( !( ( (f->mode[0] == 'w') ||
             (f->mode[0] == 'a') ||
             (f->mode[0] == 'u') )  && f->open ) )
     {
     f->iostat = -4;
     return(0);
     }
/*
***Skriv.
*/
   fputc('\n',f->fp);
/*
***Special för standard output.
*/
   if ( f->fp == stdout ) fflush(stdout);
/*
***Slut.
*/
   f->iostat = 0;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        bool v3isop(char *path)

/*      Kollar om fil är öppen.
 *
 *      In: path = Filnamn ev. med path.
 *
 *      FV: TRUE  = Filen är öppen.
 *          FALSE = Filen är inte öppen.
 *
 *      (C)microform ab 1997-02-05 J. Kjellander
 *
 ******************************************************!*/

 {
   if ( fndfil(path) == -1 ) return(FALSE);
   else                      return(TRUE);
 }

/********************************************************/
/*!******************************************************/

 static int close_all()

/*      Stänger all öppna filer i filtab.
 *
 *      FV:   0.
 *
 *      (C)microform ab 1998-01-01 J. Kjellander
 *
 *      1998-02-19 Bugfix, J.Kjellander
 *
 ******************************************************!*/

 {
   int     i;
   V2FILE *f;

/*
***Gå igenom filtab och sök upp alla öppna filer.
*/
   for ( i=0; i<MAXOFILES; ++i )
     {
     if ( *filtab[i].name != '\0' )
       {
       f = filtab[i].fptr;
/*
***En öppen fil !
*/
       if ( f->open )
         {
/*
***Pipe.
*/
         if ( f->mode[1] == 'p' )
           {
#ifdef WIN32
           if ( _pclose(f->fp) < 0 ) f->iostat = -3;
#else
           if (  pclose(f->fp) < 0 ) f->iostat = -3;
#endif
           else                      f->iostat =  0;
           }
/*
***Vanlig fil.
*/
         else
           {
           if ( fclose(f->fp) < 0 ) f->iostat = -3;
           else                     f->iostat =  0;
           }
         f->open = FALSE;
        *filtab[i].name = '\0';
         filtab[i].fptr = NULL;
         }
       }
     }

   return(0);
 }

/********************************************************/
/*!******************************************************/

 static int fndfil(char *name)

/*      Söker upp en fil i filtab.
 *
 *      In: name = Filens namn ev. med path
 *
 *      FV:   -1 = Finns inte
 *          >= 0 = Index i filtab.
 *
 *      (C)microform ab 1997-01-30 J. Kjellander
 *
 ******************************************************!*/

 {
   int i;

#ifdef WIN32
   for ( i=0; i<MAXOFILES; ++i )
     if ( *filtab[i].name != '\0'  &&
          sticmp(name,filtab[i].name) == 0 ) return(i);
#else
   for ( i=0; i<MAXOFILES; ++i )
     if ( *filtab[i].name != '\0'  &&
          strcmp(name,filtab[i].name) == 0 ) return(i);
#endif

   return(-1);
 }

/********************************************************/
/*!******************************************************/

 static int fndptr(V2FILE *fptr)

/*      Söker upp en fil i filtab.
 *
 *      In: name = Filens fptr.
 *
 *      FV:   -1 = Finns inte
 *          >= 0 = Index i filtab.
 *
 *      (C)microform ab 1997-01-30 J. Kjellander
 *
 ******************************************************!*/

 {
   int i;

   for ( i=0; i<MAXOFILES; ++i )
     if ( fptr == filtab[i].fptr ) return(i);

   return(-1);
 }

/********************************************************/
/*!******************************************************/

 static int fndfree()

/*      Söker upp ledig plats i filtab.
 *
 *      FV:   -1 = filtab full.
 *          >= 0 = Index till ledig plats i filtab.
 *
 *      (C)microform ab 1997-01-30 J. Kjellander
 *
 ******************************************************!*/

 {
   int i;

   for ( i=0; i<MAXOFILES; ++i )
     if ( *filtab[i].name == '\0' ) return(i);

   return(-1);
 }

/********************************************************/
/*!******************************************************/

        short evappf()

/*      Evaluerar APPEND_FILE.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Felkod: 
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 *      2001-02-13 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   return(IGfapp(proc_pv[1].par_va.lit.str_va,
	             proc_pv[2].par_va.lit.str_va));
 }

/********************************************************/
/*!******************************************************/

        short evdelf()

/*      Evaluerar DELETE_FILE.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Felkod: 
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 *      2001-02-13 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   return(IGfdel(proc_pv[1].par_va.lit.str_va));
 }

/********************************************************/
/*!******************************************************/

        short evmovf()

/*      Evaluerar MOVE_FILE.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Felkod: 
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 *      2001-02-13 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   return(IGfmov(proc_pv[1].par_va.lit.str_va,
	             proc_pv[2].par_va.lit.str_va));
 }

/********************************************************/
/*!******************************************************/

        short evcopf()

/*      Evaluerar COPY_FILE.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Felkod: 
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 *      2001-02-13 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   return(IGfcpy(proc_pv[1].par_va.lit.str_va,
	             proc_pv[2].par_va.lit.str_va));
 }

/********************************************************/
/*!******************************************************/

        short evtstf()

/*      Evaluerar TEST_FILE.
 *
 *      In: Global  func_pv  => Parameter value array
 *          Global *func_vp  => Pointer to result value.
 *
 *      Ut: Global *func_vp  =  ID.
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1996-01-26 J. Kjellander
 *
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

 {
   char  mode;
   char *path;
   int   status;

   path = func_pv[1].par_va.lit.str_va;
   mode = func_pv[2].par_va.lit.str_va[0];

   if ( IGfacc(path,mode) ) status = 1;
   else                     status = 0;

   func_vp->lit.int_va = status;

   return(0);
 }

/********************************************************/
/*!******************************************************/

        short evseek() 

/*      Evaluate procedure SEEK.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Out:  
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *      2001-02-12 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   int fstatus;
   V2FILE *f;                /* parameter for SEEK */

/*
***FILE f 
*/
   f = proc_pv[1].par_va.lit.fil_va;

/*
***check if file is open for "u"
*/
   if ( !( ( f->mode[ 0 ] == 'u' ) && f->open ) )

/*
***trying to seek, when not open for "u"
*/
       { 
       f->iostat = -9;
       return(0);
       }

/*
***execute SEEK
*/
   if ( ( fstatus = fseek( f->fp, (long)proc_pv[2].par_va.lit.int_va, 0 ) ) == 0 )
       f->iostat = 0;
   else
       f->iostat = -8;

   return( 0 );
  }  

/********************************************************/
/*!******************************************************/

        short evouti() 

/*      Evaluate procedure OUTINT.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Out:  
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *      2001-02-12 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   V2FILE *f;                  /* parameter for OUTINT */
   int format;                 /*       - " -          */

   char formstr[ IOFSTRL ];    /* the c:s format string */

/*
**FILE f
*/
   f = proc_pv[1].par_va.lit.fil_va;
/*
***check if file is open for "w", "u" or "a"
*/
   if ( !( ( (f->mode[0] == 'w') || (f->mode[0] == 'a') || (f->mode[0] == 'u') )
                                                         && f->open ) )
/*
***trying to write, when not open for "w", "u" or "a"
*/
       {
       f->iostat = -4;
       return(0);
       }
/*
***INT format, optional parameter
*/
   format = (int)proc_pv[3].par_va.lit.int_va;
/*
***make c:s format string
*/
   mkform( C_INT_VA, format, (int)0, formstr );
/*
***execute OUTINT
*/
   fprintf( f->fp, formstr, (int)proc_pv[2].par_va.lit.int_va );
/*
***flush std-output
*/
   if ( f->fp == stdout ) fflush(stdout);    

   f->iostat = 0;

   return( 0 );
  }  

/********************************************************/
/*!******************************************************/

        short evoutf()

/*      Evaluate procedure OUTFLT.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Out:  
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *      2001-02-12 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   V2FILE *f;                 /* parameter for OUTFLT  */
   int format1;               /*       - " -           */
   int format2;               /*       - " -           */

   char formstr[ IOFSTRL ];   /* the c:s format string */


/*
***FILE f
*/
   f = proc_pv[1].par_va.lit.fil_va;
/*
***check if file is open for "w", "u" or "a"
*/
   if ( !( ( (f->mode[0] == 'w') || (f->mode[0] == 'a') || (f->mode[0] == 'u') )
                                                      && f->open ) )
/*
***trying to write, when not open for "w", "u" or "a"
*/
       {
       f->iostat = -4;
       return(0);
       }
/*
***INT format1, optional parameter
*/
   format1 = (int)proc_pv[3].par_va.lit.int_va;
/*
***INT format2, optional parameter 
*/
   format2 = (int)proc_pv[4].par_va.lit.int_va;
/*
***make c:s format string
*/
   mkform( C_FLO_VA, format1, format2, formstr );
/*
***execute OUTFLT
*/
   fprintf( f->fp, formstr, proc_pv[2].par_va.lit.float_va );
/*
***flush std-output
*/
   if ( f->fp == stdout ) fflush(stdout);

   f->iostat = 0;

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short evouts()     

/*      Evaluate procedure OUTSTR.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Out:  
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *      2001-02-12 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   V2FILE *f;                     /* parameter for OUTSTR */
   char sdata[ V3STRLEN ];        /*       - " -          */
   int format;                    /*       - " -          */

   char formstr[ IOFSTRL ];       /* the c:s format string */

/*
***FILE f
*/
   f = proc_pv[1].par_va.lit.fil_va;

/* 
***check if file is open for "w", "u" or "a"
*/
   if ( !( ( (f->mode[0] == 'w') || (f->mode[0] == 'a') || (f->mode[0] == 'u') )
                                                         && f->open ) )
/*
***trying to write, when not open for "w", "u" or "a"
*/
       {
       f->iostat = -4;
       return(0);
       }

/*
*** STRING sdata
*/
   strcpy( sdata, proc_pv[2].par_va.lit.str_va );
/*
***INT format, optional parameter
*/
   format = (int)proc_pv[3].par_va.lit.int_va;
/*
***check format
*/
   if ( format == 0 )
/*
***no actual parameter use string length
*/
       {
       format = strlen( sdata );
       } 
/*
***make c:s format string
*/
   mkform( C_STR_VA, format, (int)-1, formstr );
/*
***execute OUTSTR
*/
   fprintf( f->fp, formstr, sdata );
/*
***flush std-output
*/
   if ( f->fp == stdout ) fflush(stdout);    

   f->iostat = 0;

   return( 0 );
  }  

/********************************************************/
/*!******************************************************/

        short evoutb()    

/*      Evaluate procedure OUTBIN.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *
 *      Out:  
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *      2001-02-12 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   int fd;               /* file descriptor for low level I/O */
   long binsize;         /* size of the binary data block */
   char *binaddr=NULL;   /* address to the binary data block */
   STTYTBL type;         /* interface struct for type */

   V2FILE *f;            /* parameter for OUTBIN */


/*
***FILE f 
*/
   f = proc_pv[1].par_va.lit.fil_va;

/*
***check if file is open for "w", "u" or "a"
*/
   if ( !( ( (f->mode[0] == 'w') || (f->mode[0] == 'a') || (f->mode[0] == 'u') )
                                                         && f->open ) )
/*
***trying to write, when not open for "w", "u" or "a"
*/
       {
       f->iostat = -4;
       goto endl;
       }

/*
***bindata
***read type info for bindata
*/
   strtyp( proc_pv[2].par_ty, &type );

/*
***check if the evaluated value is an address
*/
   if ( proc_pv[2].par_va.lit_type == C_ADR_VA )
/*
***calculate run-time stack address
*/
       {
       binaddr = incrsa( proc_pv[2].par_va.lit.adr_va );
       }
   else
       {
       switch( type.kind_ty )
         {
         case C_INT_VA:
             binaddr = (char *)&proc_pv[2].par_va.lit.int_va;
             break;

         case C_FLO_VA:
             binaddr = (char *)&proc_pv[2].par_va.lit.float_va;
             break;

         case C_STR_VA:
             binaddr = (char *)proc_pv[2].par_va.lit.str_va;
             break;

         case C_VEC_VA:
             binaddr = (char *)&proc_pv[2].par_va.lit.vec_va;
             break;

         case C_REF_VA:
             binaddr = (char *)proc_pv[2].par_va.lit.ref_va;
             break;

         case C_FIL_VA:
             binaddr = (char *)proc_pv[2].par_va.lit.fil_va;
             break;
         }
       }

   binsize = type.size_ty;

/*
***get the file descriptor
*/
   fd = fileno( f->fp );

/*
**execute OUTBIN
*/
   if ( write( fd, binaddr, binsize ) != binsize )
       {
       f->iostat = -6;
       }
   else
       f->iostat = 0;

   endl:

   return( 0 );

  }

/********************************************************/
/*!******************************************************/

        short evinii()
 
/*      Evaluate evaluate function ININT-i/o.
 *
 *      In: Global  func_pv  => Parameter value array
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 2001-03-07 R.Svedin
 *
 ******************************************************!*/

  {

   return(evinio( C_INT_VA, func_pv, func_vp ));

  }

/********************************************************/
/*!******************************************************/

        short evinif()
 
/*      Evaluate evaluate function INFLT-i/o.
 *
 *      In: Global  func_pv  => Parameter value array
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 2001-03-07 R.Svedin
 *
 ******************************************************!*/

  {

   return(evinio( C_FLO_VA, func_pv, func_vp ));

  }

/********************************************************/
/*!******************************************************/

        short evinis()
 
/*      Evaluate evaluate function INSTR-i/o.
 *
 *      In: Global  func_pv  => Parameter value array
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: FV: Returnerar anropade rutiners status. 
 *
 *      (C)microform ab 2001-03-07 R.Svedin
 *
 ******************************************************!*/

  {

   return(evinio( C_STR_VA, func_pv, func_vp ));

  }

/********************************************************/
/*!******************************************************/

 static short evinio(
        pmvaty iotyp,
        PMPARVA pv[],
        PMLITVA *valp)

/*      Evaluate evaluate function IN-i/o.
 *
 *      In:   iotyp   =>  I/O data type 
 *            pv[]    =>  Parameter value array
 *
 *      Out:  *valp   =>  C-pointer to result value
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   int  fstatus;

   V2FILE *f;                    /* parameter for IN-i/o */
   int format,ival;              /*       - " -          */

   char fieldstr[ V3STRLEN ];    /* area for field under formated input */
   bool inform;                  /* indicates input format */
   char formstr[ IOFSTRL ];      /* the c:s format string */


/*
***FILE f
*/
   f = pv[1].par_va.lit.fil_va;

/*
***check if file is open for "r" or "u"
*/
   if ( !( ((f->mode[0] == 'r') || (f->mode[0] == 'u')) && f->open ) )
/*
***trying to read, when not open for "r" or "u"
*/
       {
       f->iostat = -5;
       goto endl;
       }

/*
***INT format
*/
   format = (int)pv[2].par_va.lit.int_va;

/*
***check if no format
*/
   if ( format == 0 )
       {
       inform = FALSE; 
       } 
   else
       {
       inform = TRUE;
       }

/*
***make c:s format string
*/
   mkform( iotyp, format, (int)-2, formstr );

/*
***execute IN-i/o
*/
   if ( inform || f->fp == stdin )
/*
***Extract field and read from it
*/
       {
       rfield( f, format, fieldstr );
       switch( iotyp )
         {
         case C_INT_VA:
             fstatus = sscanf( fieldstr, formstr, &ival);
             valp->lit.int_va = ival;
             if ( f->iostat != EOF )
                 if ( fstatus == 0 )
                     f->iostat = -11;            /* item not read */
             break;
         case C_FLO_VA:
             fstatus = sscanf( fieldstr, formstr, &valp->lit.float_va );
             if ( f->iostat != EOF )
                 if ( fstatus == 0 )
                     f->iostat = -11;            /* item not read */
             break;
         case C_STR_VA:
             strcpy( valp->lit.str_va, fieldstr );
             break;
         }
       }
   else
       {
       switch( iotyp )
         {
         case C_INT_VA:
           if ( ( fstatus = fscanf( f->fp,formstr, &ival) ) == EOF )
/*
***End of file
*/
               {  
               f->iostat = -1;
               }
           else if ( fstatus == 0 )
               f->iostat = -11;                  /* item not read */
           else
               valp->lit.int_va = ival;
               f->iostat = 0;
           break;

         case C_FLO_VA:
           if ( ( fstatus = fscanf( f->fp,formstr, &valp->lit.float_va)) == EOF )
/*
***End of file
*/
               {
               f->iostat = -1;
               }
           else if ( fstatus == 0 )
            f->iostat = -11;                     /* item not read */
           else
               f->iostat = 0;
           break;

         case C_STR_VA:
           if ( ( fstatus = fscanf( f->fp,formstr, valp->lit.str_va ) ) == EOF )
/*
***End of file
*/
               { 
               f->iostat = -1;
               }
           else if ( fstatus == 0 )              /* Ny rad i samband med 1.9E JK */
               f->iostat = -11;                  /* item not read */
           else
               f->iostat = 0;
           break;
         }
       }

   endl:

   return( 0 );
  }  

/********************************************************/
/*!******************************************************/

        short evinl()

/*      Evaluate function INLIN.
 *
 *      In: Global  func_pv  => Parameter value array
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: Return - error severity code 
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   char *fschp;
   int slen;

   V2FILE *f;                 /* parameter for INLIN */

/*
***FILE f 
*/
   f = func_pv[1].par_va.lit.fil_va;

/*
***check if file is open for "r" or "u"
*/
   if ( !( ((f->mode[0] == 'r') || (f->mode[0] == 'u')) && f->open ) )
/*
***trying to read, when not open for "r" or "u"
*/
       {
       f->iostat = -5;
       return(0);
       }
/*
***Execute INLIN
***Om läsning från terminal, läs med editering och eko.
*/
   if ( f->fp == stdin )
     {
     rfield(f,(int)V3STRLEN,func_vp->lit.str_va);
     }
/*
***Annars läs från fil med fgets.
*/
   else
     {
     if ( ( fschp = fgets( func_vp->lit.str_va, V3STRLEN + 1, f->fp ) ) == NULL )
/*
***End of file or error
*/
       {
       f->iostat = -1;
       }
     else
       {
       /* if newlin is in string, delete it */
       slen = strlen( func_vp->lit.str_va );
       if ( func_vp->lit.str_va[ slen -1 ] == '\n' )
            func_vp->lit.str_va[ slen - 1 ] = '\0';
       f->iostat = 0;
       }
     }

   return( 0 );

  }

/********************************************************/
/*!******************************************************/

        short evinb()

/*      Evaluate function INBIN.
 *
 *      In: extern proc_pv => Pekare till array med parametervärden
 *          extern proc_pc => Antal parametrar.
 *
 *      Out:  
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *      2001-02-12 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   int fd;             /* file descriptor for low level I/O */
   long binsize;       /* size of the binary data block */
   int  nbytes;        /* Optional user specified number of bytes */
   char *binaddr;      /* address to the binary data block */
   char  cdum;         /* Very short int */
   short sdum;         /* Short int */
   DBint idum;         /* VARKON int */
   float fdum;         /* C float */
   v2float ddum;       /* VARKON float */
   STTYTBL type;       /* type info */
   V2FILE *f;          /* parameter for INBIN */

/*
***FILE f 
*/
   f = proc_pv[1].par_va.lit.fil_va;

/*
***check if file is open for "r" or "u"
*/
   if ( !( ((f->mode[0] == 'r') || (f->mode[0] == 'u') ) && f->open ) )
/*
***trying to write, when not open for "r" or "u"
*/
       {
       f->iostat = -5;
       return(0);
       }
/*
***check that the evaluated value is an address
*/
   if ( ! proc_pv[2].par_va.lit_type == C_ADR_VA )
/*
***Not address, error
*/
       {
       return( erpush( "IN3263", "" ) );
       }
/*
***calculate run-time stack address
*/
   binaddr = incrsa( proc_pv[2].par_va.lit.adr_va );

/*
***read bindata type info
*/
   strtyp( proc_pv[2].par_ty, &type );

   binsize = type.size_ty;

/*
***get the file descriptor
*/
   fd = fileno( f->fp );

/*
***Läs så många bytes som variabeln är deklarerad till.
*/
   if ( proc_pc == 2 )
      {
      if ( read( fd, binaddr, binsize ) != binsize ) f->iostat = -7;
      else                                           f->iostat = 0;
      }
/*
***Läs av användaren specificerat antal bytes och konvertera.
*/
   else
      {
      nbytes = proc_pv[3].par_va.lit.int_va;
      if ( nbytes < 1  ||  nbytes > 132 ) return(erpush("IN3422",""));

      switch( type.kind_ty )
        {
/*
***För INT är 1, 2 och 4 tillåtet.
*/
        case C_INT_VA:
        if ( nbytes == sizeof(char) )
          {
          if ( read(fd,&cdum,nbytes) != nbytes ) f->iostat = -7;
          else
            {
            f->iostat = 0;
            idum = (DBint)cdum;
            V3MOME(&idum,binaddr,sizeof(idum));
            }
          }
        else if ( nbytes == sizeof(short) )
          {
          if ( read(fd,&sdum,nbytes) != nbytes ) f->iostat = -7;
          else
            {
            f->iostat = 0;
            idum = (DBint)sdum;
            V3MOME(&idum,binaddr,sizeof(idum));
            }
          }
        else if ( nbytes == sizeof(DBint) )
          {
          if ( read(fd,binaddr,nbytes) != nbytes ) f->iostat = -7;
          else                                     f->iostat =  0;
          }
        else return(erpush("IN3432",""));
        break;
/*
***För FLOAT är 4 och 8 tillåtet.
*/
        case C_FLO_VA:
        if ( nbytes == sizeof(float) )
          {
          if ( read(fd,&fdum,nbytes) != nbytes ) f->iostat = -7;
          else
            {
            f->iostat = 0;
            ddum = (v2float)fdum;
            V3MOME(&ddum,binaddr,sizeof(ddum));
            }
          }
        else if ( nbytes == sizeof(v2float) )
          {
          if ( read(fd,binaddr,nbytes) != nbytes ) f->iostat = -7;
          else                                     f->iostat =  0;
          }
        else return(erpush("IN3432",""));
        break;
/*
***För STRING är 1 -> 132 tillåtet.
*/
        case C_STR_VA:
        if ( read(fd,binaddr,nbytes) != nbytes ) f->iostat = -7;
        else                                     f->iostat =  0;
       *(binaddr+nbytes) = '\0';
        break;
 
        default:
        break;
        }
      }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short evfpos()    

/*      Evaluate function FPOS.
 *
 *      In: Global  func_pv  => Parameter value array
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: Return - error severity code 
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   V2FILE *f;             /* parameter for FPOS */

/*
***FILE f
*/
   f = func_pv[1].par_va.lit.fil_va;
/*
***check if file is open
*/
   if ( !( f->open ) )
/*
***trying to access, when not open
*/
       {
       f->iostat = -10;
       return(0);
       }
/* 
***Execute FPOS 
*/

/*
***Varför skall fpos() returnera EOF när man står i slutet ?
***På vaxen verkar det under inga förhållanden funka på rätt sätt.
***l_anastr.MBS funkar inte på VAX:en. Har man en gång hamnat i slutet
***av filen verkar det som fpos() returnerar -1 för all framtid även
***om man med seek() flyttar sig tillbaks igen och även om man sedan skriver
***och/eller läser. seek() verkar funka trots att fpos() hela tiden
***returnerar -1.
***Macro:t feof() i VAX-version returnerar inte samma resultat som
***i UNIX/DNIX/XENIX etc. ftell() verkar dock funka. Alltså kan man strunta
***i om man får EOF() i detta sammanhang.
*/
   if ( feof( f->fp ) )
       { 
#ifdef VAX
       f->iostat = 0;
       func_vp->lit.int_va = (DBint)ftell( f->fp );
#else
       f->iostat = -1;
       func_vp->lit.int_va = -1;
#endif
       }
else
       {
       func_vp->lit.int_va = (DBint)ftell( f->fp );
       f->iostat = 0;
       }

   return( 0 );
  }

/********************************************************/
/*!******************************************************/

        short eviost()

/*      Evaluate function IOSTAT.
 *
 *      In: Global  func_pv  => Parameter value array
 *
 *      Ut: Global *func_vp  =  Pointer to result value.
 *
 *      FV: Return - error severity code 
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *      2001-03-06 In-Param changed to Global variables, R Svedin
 *
 ******************************************************!*/

  {
   V2FILE *f;              /* parameter for IOSTAT */

/*
***FILE f
*/
   f = func_pv[1].par_va.lit.fil_va;

/*
***Execute IOSTAT
*/
   func_vp->lit.int_va = (DBint)(f->iostat);

   return( 0 );
  }

/********************************************************/
/********************************************************/

        short evufnm()

/*      Evaluate function UNIQUE_FILENAME(). This
 *      implementation is a quick fix that will work
 *      in most cases but there is no clue to what 
 *      directory is used.
 *
 *      Out: Global *func_vp  =  Pointer to result value.
 *
 *      Return: Error severity code.
 *
 *      (C)2008-02-15 J.Kjellander
 *
 ******************************************************!*/

  {
   char  template[7];
   int   fd;
   FILE *fp;

/*
***Call mkstemp() and close the file immediately.
*/
   strncpy(template,"XXXXXX",7);
   fd = mkstemp(template);
   fp = fdopen(fd,"w+");
   fclose(fp);
/*
***A closed empty file now exists somewhere.
***Clean up.
*/
   if ( IGftst(template) ) IGfdel(template);
/*
***Return filenamen.
*/
   strcpy(func_vp->lit.str_va,template);
/*
***The end.
*/
    return(0);
  }

/********************************************************/
/*!******************************************************/

 static short   rfield(
        V2FILE *f,
        int     fieldlen,
        char   *fieldstr)

/*      Read field from i/o.
 *
 *      In:   *f        =>   Pointer to an MBS i/o controll block 
 *             fieldlen =>   The field length 
 *
 *      Out:  *f        =>   Pointer to an MBS i/o controll block 
 *            *fieldstr =>   out - the extracted field 
 *
 *      FV:   return - error severity code
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-13 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   short ntkn;

/*
***Om läsning från terminal, läs med editering och eko.
*/
   if ( f->fp == stdin )
     {
     if ( fieldlen == 0 ) ntkn = V3STRLEN;
     else ntkn = fieldlen;
#ifdef VARKON
     /* igglin("","",&ntkn,fieldstr); */
#else
     return(-1);
#endif
     if ( ntkn == 0 )
       {
       f->iostat = -1;
       }
     else
       {
       f->iostat = 0;
       }
     }
/*
***Om läsning från fil läs med fgets().
*/
   else
    {
     if ( fgets( fieldstr, fieldlen + 1, f->fp) == NULL )
/*
***End of file
*/
       {
       f->iostat = -1;
       }
     else
       f->iostat = 0;
    } 
   return( 0 );
  }

/********************************************************/

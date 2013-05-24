/**********************************************************************
*
*    anascan.c
*    =========
*
*    This file is part of the VARKON Analyzer Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes the following routines:
*
*    asinit();         Init scanner to scan from file or string.
*    anascan();        Scan for a lexikal token.
*    anapeek();        Lookahead one lexical token.
*    asexit();         Exit scanner.
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
#include "../../IG/include/isch.h"
#include "../include/AN.h"

/*
***Vid övergång till 8-bits ASCII krävs att asgcurch etc. är deklarerade
***som unsigned char. Därmed kan de inte användas för att hålla den av C
***definierade konstanten EOF som är -1 och som getc() returnerar.
***Istället fixar vi en egen som är positiv.
*/

#define AN_EOF 255

/*
***Scanner global variables
*/
short  anstyp;                   /* ANRDFIL -> from file, ANRDSTR -> from 
                                    string */

static short ascurlin;           /* Current source line. short 861027JK */
static short ascurcol;           /* column, */
static unsigned char  ascurch;   /* and character */
static unsigned char  aspch;     /* Previous character */
static bool  peeked;             /* YES - token peeked */
static struct ANSYREC ptok;      /* peeked token - if any */
static FILE *assrcfil;           /* Ptr to source file if anstyp = ANRDFIL */
static char *assrcstr;           /* Ptr to source string if anstyp = ANRDSTR */

/********************************************************/
/*!******************************************************/

        void asgch()

/*      Get another character from the source file. If the current character
 *      is a '\n' increment line count and reset column.
 *
 *      In:  => -       
 *
 *      Out: => Global character 'ascurch is set to the next character read.
 *
 *      (C)microform ab 1985-05-10 Thomas Nilsson
 *
 *      1996-01-19 8-bits ASCII, J. Kjellander
 *      1999-04-16 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   int tkn;

   aspch = ascurch;

   if ( ascurch == AN_EOF ) return;    /* No more characters to fetch */
/*
***Uppdatera rad och kolumn.
*/
   if ( ascurch == '\n' )
     {
     ascurcol = 1;
     ascurlin++;
     }
   else ascurcol++;
/*
***Läs nästa tecken från fil eller primärminne.
*/
   if ( anstyp == ANRDFIL )
     {
     if ( (tkn=getc(assrcfil)) == EOF ) ascurch = AN_EOF;
     else                               ascurch = (unsigned char)tkn;
     }
   else
     {
     ascurch = *(assrcstr++);
     if ( ascurch == '\0' ) ascurch = AN_EOF;
     }
  }

/********************************************************/
/*!******************************************************/

        void asbch()

/*      Backup over the last character read. Current character is reset.
 *
 *      In:         
 *
 *      Out:        
 *
 *      (C)microform ab 1985-11-08 Thomas Nilsson
 *
 *      1999-04-16 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   if (anstyp == ANRDFIL)
     ungetc(ascurch, assrcfil);
   else
     assrcstr--;

    ascurch = aspch;               /* Previous character */
    ascurcol--;
  }

/********************************************************/
/*!******************************************************/

        void asskpspc ()

/*      Skip any white space from here up to the next symbol.
 *
 *      In:     
 *
 *      Out:    
 *
 *      (C)microform ab 1985-05-10 Thomas Nilsson
 *
 *      1999-04-16 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   while ((ascurch == ' ') || (ascurch == '\t') || (ascurch == '\n')
   || (ascurch == '!'))
     {
     if (ascurch == '!')
       while ((ascurch != '\n') && (ascurch != AN_EOF))
         asgch();
      asgch ();
     };
  }

/********************************************************/
/*!******************************************************/

        void asskpcmt (short commentstartlin)

/*      Skip a multi-line comment.
 *
 *      In: commentstartlin => line where a multiline comment started 
 *
 *      Out:   
 *
 *      (C)Iouri Simernitski 2000-04-02 
 *
 ******************************************************!*/

   {
   do
     {
      asgch();
      switch( ascurch )
       {
       case AN_EOF:
         {
         char buffer[10];	/* must be enough for a short */
         sprintf( buffer, "%d", commentstartlin );
         analog ( ascurlin, ascurcol, "AN1123", buffer );
         }
       break;

       case '*': /* Possible end of comment */
       asgch(); 
       if(ascurch == '/' ) return;
       asbch();					
       break;

       case '/': /* Possible recursive comment */
       asgch();
       if( ascurch == '*' ) asskpcmt(ascurlin);
       /* recursive comment = recursive call */
       else asbch();					
       break; 
       }
     } while (ascurch != AN_EOF);
   }
  
/********************************************************/
/*!******************************************************/

        v2float asgint(
        v2int  *count)

/*      Get an integer literal. Read characters from the source to form an
 *      integer and calculate it's value and return it. Also count the number
 *      of digits found.
 *      If an integer literal couldn't be found -1 else the value of the
 *      scanned integer.
 *
 *      In:  count => Pointer to a counter of number if digits.
 *
 *      Out:                 
 *
 *      (C)microform ab 1985-05-10 Thomas Nilsson
 *
 *      1999-04-16 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   v2float i;

   if (!isdigit (ascurch)) return ((v2float)-1);       /* No value present */

   i = 0;
   while (isdigit (ascurch))
     {
     (*count)++;
     i = i*10 + ascurch - '0';
/*
***Följande rad är till för att skydda VAX-versionen av V3 mot
***floating overflow om en mycket lång sträng bestående av siffror
***skickas till scannern. Genom att sätta i = ANMAXINT + 1 kommer
***den anropande rutinen att generera fel och interaktiva V3 (genstr)
***istället tolka det hela som bara text. 19/11/92 JK.
*/
     if ( i > ANMAXINT ) i = ANMAXINT + 1.0;
/*
***Slut VAX-fix.
*/
       asgch ();
     };

   return (i);
  }

/********************************************************/
/*!******************************************************
 *
 *      isidchar(
 *      char chr)
 *
 *      Check if 'chr' is a legal character in an identifier.
 *      NOTE: implemented as a macro.
 *      TRUE if character is legal else FALSE.
 *
 *      In:  chr => Character to check.
 *
 *      Out:               
 *
 *      (C)microform ab 1985-05-10 Thomas Nilsson
 *
 *      1999-04-16 Rewritten, R. Svedin
 *
 ******************************************************!*/

#define isidchar(chr) (isialpha(chr)||isdigit(chr)||(chr == '_'))

/********************************************************/
/*!******************************************************/

        short assyeq(
        struct ANSYREC *sy,
        char            str[])

/*      Check for equality between a symbol name and a string.
 *      TRUE if the name of the symbol is equal to the string else FALSE.
 *
 *      In:  sy  = Pointer to the symbol record where the name of the 
 *                 symbol is.
 *           str = String to compare to.
 *
 *      Out:            
 *
 *      (C)microform ab 1985-05-10 Thomas Nilsson
 *
 *      1999-04-16 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   return ( sticmp(sy->syval.name,str) == 0 );
  }

/********************************************************/
/*!******************************************************/

        void asgid(
        struct ANSYREC *sy)

/*      Get an identifier or possibly a reserved word from the source file.
 *      Read characters until the next one isn't legal in an identifier, then
 *      check if it was one of the reserved words.
 *
 *      In:  sy  => Pointer to the symbol record where the name of the symbol
 *                  is.
 *
 *      Out:       Updated symbolrecord (pointed to by 'sy').
 *
 *      (C)microform ab 1985-05-10 Thomas Nilsson
 *
 *      1992-03-13 Macro, J. Kjellander
 *      1999-04-16 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   short i;
   ANSYTYP typ;
   short lin,col;

   i = 0;
   lin = ascurlin;
   col = ascurcol;
/*
***Read all characters of a legal identifier
*/
   while ((i <= ANSYNLEN) && isidchar (ascurch))
     {
     sy->syval.name[i++] = ascurch;
     asgch ();
     };
/*
***Terminate name with NULL
*/
   sy->syval.name [i] = '\0';
/*
***Then there are more characters in this identifier. Skip !
*/
   if (isidchar (ascurch))
     {
     analog (lin, col, "AN1051", sy-> syval.name);
     while (isidchar (ascurch))
       asgch ();
/*
*** Since it was too long it can't be a reserved word
*/
     sy-> sytype = ANSYID;
     }

   else
     {
/*
*** Assume an identifier, but could be a Reserved Word, check !
*/
     typ = ANSYID;
/*
*** The check is simple but not inefficient. SWITCH on the
*** length and compare with the reserved words of that length.
*** In this case one length calculation and max 30 character
*** comparisons (5 reserved words of length 6) is necessary.
*** Could be improved !
*/
     switch (strlen (sy-> syval.name))
       {
       case 2:
         if (assyeq (sy, "IF"))
           typ = ANSYIF;
         else if (assyeq (sy, "OR"))
           typ = ANSYOR;
         else if (assyeq (sy, "TO"))
           typ = ANSYTO;
         else if (assyeq (sy, "DO"))
           typ = ANSYDO;
         break;

       case 3:
         if (assyeq (sy, "AND"))
           typ = ANSYAND;
         else if (assyeq (sy, "FOR"))
           typ = ANSYFOR;
         else if (assyeq (sy, "INT"))
           typ = ANSYINT;
         else if (assyeq (sy, "NOT"))
           typ = ANSYNOT;
         else if (assyeq (sy, "REF"))
           typ = ANSYREF;
         else if (assyeq (sy, "VAR"))
           typ = ANSYVAR;
         break;

       case 4:
         if (assyeq (sy, "ELIF"))
           typ = ANSYELIF;
         else if (assyeq (sy, "ELSE"))
           typ = ANSYELSE;
         else if (assyeq (sy, "FILE"))
           typ = ANSYFILE;
         else if (assyeq (sy, "GOTO"))
           typ = ANSYGOTO;
         else if (assyeq (sy, "STEP"))
           typ = ANSYSTEP;
         else if (assyeq (sy, "THEN"))
           typ = ANSYTHEN;
         break;

       case 5:
         if (assyeq (sy, "ENDIF"))
           typ = ANSYEIF;
         else if (assyeq (sy, "FLOAT"))
           typ = ANSYFLT;
         else if (assyeq (sy, "LOCAL"))
           typ = ANSYLOCL;
         else if (assyeq (sy, "BASIC"))
           typ = ANSYBASI;
         else if (assyeq (sy, "MACRO"))
           typ = ANSYMAC;
         break;

       case 6:
         if (assyeq (sy, "ENDFOR"))
           typ = ANSYEFOR;
         else if (assyeq (sy, "GLOBAL"))
           typ = ANSYGLOB;
         else if (assyeq (sy, "MODULE"))
           typ = ANSYMOD;
         else if (assyeq (sy, "STRING"))
           typ = ANSYSTR;
         else if (assyeq (sy, "VECTOR"))
           typ = ANSYVECT;
         break;

       case 7:
         if (assyeq (sy, "DRAWING"))
           typ = ANSYDRAW;
         break;

       case 8:
         if (assyeq (sy, "CONSTANT"))
           typ = ANSYCONS;
         else if (assyeq (sy, "GEOMETRY"))
           typ = ANSYGEO;
         break;

       case 9:
         if (assyeq (sy, "ENDMODULE"))
           typ = ANSYEMOD;
         break;

       case 11:
         if (assyeq (sy, "BEGINMODULE"))
           typ = ANSYBMOD;
         break;

       default:
         break;
       };

       sy-> sytype = typ;
     };
  }

/********************************************************/
/*!******************************************************/

        v2int asgexp(
        v2int e)

/*      Read the exponent part of a decimal literal. Add the previously
 *      calculated exponent and return it.
 *      Return new exponent value.
 *
 *      In:  e => Previously calculated exponent value.
 *
 *      Out:            
 *
 *      (C)microform ab 1985-05-13 Thomas Nilsson
 *
 *      1999-04-16 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   short sign;
   v2int i,d;

   sign = 1;
   if ( ascurch == '+' )
     asgch ();
   else if (ascurch == '-')
     {
     sign = -1;
     asgch ();
     };
/*
***Exponent missing
*/
   if ((i = (v2int)asgint(&d)) == -1)
     {
     analog (ascurlin, ascurcol, "AN1022", "");
     return (e);
     }
   else
     return (sign*i + e);
  }

/********************************************************/
/*!******************************************************/

        void asgnlit (
        struct ANSYREC *sy)

/*      Get a numeric literal. Read characters from the source so to form an
 *      numeric literal (i.e. Decimal or Integer). At entry current character
 *      is either a '.' or a digit.
 *
 *      In:  sy => Pointer to the symbol record where the symbol should be put.
 *
 *      Out:       Updated symbol record (pointed to by 'sy').
 *
 *      (C)microform ab 1985-05-10 Thomas Nilsson
 *
 *      1999-04-16 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   v2int e, digits;
   v2float f;
   char str[3];
   bool decnum;
   short lin,col;
        
   digits = 0; f = 0.0;
   lin = ascurlin;
   col = ascurcol;

   if (isdigit(ascurch))
     f = asgint (&digits); 

   e = 0; decnum = FALSE;
   if (ascurch == '.')
     {
     decnum = TRUE; 
     asgch();                      /* Skip '.' */
/*
*** Pick up the fractional part
*/
     while (isdigit(ascurch))
       {
       e--;
       digits++;
       f = 10*f + ascurch - '0';
       asgch ();
       };
/*
*** Normalize mantissa
***
***f = f/10 ger skräp i 7:e decimalen med kompilatoroptionen
***-Od som ju snarare borde motverka detta. Om tex. flyttalet
***1.0 skall scannas fås 1.0000000XXXXXX där X=skräp.
*/
       if( f != 0 )
         {
         while (f > 1)
           {
           f = f/10;
           e++;
           };
         while (f < 0.1)
           {
           f = f*10;
           e--;
           };
         }

         if (digits > ANMAXDIG)
           {
           sprintf (str, "%d", ANMAXDIG);
           analog (lin, col, "AN1071", str);
           };
     }
/*
***Exponent part
*/
   if ((ascurch == 'E') || (ascurch == 'e'))
     {
     decnum = TRUE;
     asgch ();
     e = asgexp (e);

     if (abs(e) > ANMAXEXP)
       {
       sprintf (str, "%d\004%d", ANMAXEXP, ANMAXEXP);
       analog (lin, col, "AN1082", str);
       f = 0;
       }
     };

   if (decnum)
     {
     sy -> sytype=ANSYDECL;
     if (f != 0.0)
       while (e != 0)
         {
         if (e < 0)
           {
           f = f/10;
           e++;
           }
         else
           {
           f = f*10;
           e--;
           }
         };

     sy -> syval.fval = f;
     }
   else
    {
    sy -> sytype = ANSYINTL;

    if (f > ANMAXINT)
      {
      analog (lin, col, "AN1062", "MAXINT");
      f = ANMAXINT;
      };

    sy -> syval.ival = (v2int) f;
    };
  } 

/********************************************************/
/*!******************************************************/

        void asgslit(
        struct ANSYREC *sy)

/*      Get a String Literal. Read characters so that a string literal is
 *      formed. All characters up to next '"' is included. 
 *      A newline forces termination of the string. Double '"' is 
 *      interpreted as a '"' included in the string.
 *
 *      In:  sy => pointer to the symbol table to update.
 *
 *      Out:       Updated symbol record (pointed to by 'sy').
 *
 *      (C)microform ab 1985-05-10 Thomas Nilsson
 *
 *      1999-04-16 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   short i;                        /* Index in to the string */
   char lstr [4];
   short lin, col;

   i = 0;
   sy -> sytype = ANSYSTRL;        /* String Literal */

   lin = ascurlin;
   col = ascurcol;

   asgch();
   while (i <= ANSTRLEN)
     {
     if (ascurch == '"')
       {
/*
***Could be end of string, or a double "
*/
       asgch ();
       if (ascurch == '"')                /* Yes, quote mark */
         {
         sy -> syval.sval [i++] = '"';
         asgch ();
         }
       else                               /* End of string */
         {
         sy -> syval.sval [i] = '\0';
         break;
         };
       }
     else if ((ascurch == '\n') || (ascurch == AN_EOF))
/*
***String terminated by newline or End-Of-File
*/
       {
       analog (lin, col, "AN1031", "");
       sy -> syval.sval [i] = '\0';
       break;
       }
     else
/*
***Ordinary character
*/
       {
       sy -> syval.sval [i++] = ascurch;
       asgch ();
       };
     };

   if (i > ANSTRLEN)
/*
***More characters in the string
*/
     {
     sprintf (lstr, "%d", ANSTRLEN);
     analog (lin, col, "AN1041", lstr);
     while ((ascurch != '"') && (ascurch != '\n'))
       asgch ();                          /* Skip truncated characters */
     if(ascurch == '"')
       asgch();
     };
  } 

/********************************************************/
/*!******************************************************/

        void asgrlit(
        struct ANSYREC *sy)

/*      Get a reference literal ('#' int Ä '.' int Å )
 *
 *      In:  sy => pointer to the symbol record to update.
 *
 *      Out:       Updated symbol record.
 *
 *      (C)microform ab 1985-11-26 Thomas Nilsson
 *
 *      1994-03-11 Globala referenser, J. Kjellander
 *      1999-04-16 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   v2float ref1,ref2;
   v2int   count,lin,col;
   bool    glbflg;

   sy->sytype = ANSYREFL;           /* A reference literal value */

   asgch();                         /* Skip '#' */
/*
***Globala referenser. Om dubbla brädhögar, sätt flagga
***och justera tecknet på slutet.
*/
   if ( ascurch == '#' )
     {
     asgch();
     glbflg = TRUE;
     }
   else glbflg = FALSE;
/*
***Slut globala referenser. 3/11-94 JK
*/
   lin = ascurlin;
   col = ascurcol;

   if ((ref1 = asgint(&count)) == (v2float) -1)
/*
*** Integer expected
*/
     analog ((short)lin, (short)col, "AN1092","");
   else
     if (ref1 > ANMAXREF)
       {
       ref1 = -1;
/*
***Overflow
*/
       analog ((short)lin, (short)col, "AN1102","");
       };

   if (ascurch == '.')
     {
     asgch();                      /* skip '.' */
     lin = ascurlin;
     col = ascurcol;

     if ((ref2 = asgint(&count)) == (v2float) -1)
/*
***Integer expected
*/
       analog ((short)lin, (short)col, "AN1092","");
     else
       if (ref2 > ANMAXREF)
         {
         ref2 = -1;
/*
***Too large
*/
         analog((short)lin,(short)col, "AN1102","");
         };
     }
   else
     ref2 = 0;
/*
***Om global referens skall sekvensnumret ha negativt tecken,
*/
   if ( glbflg )
     sy->syval.rval.seq_val = (pmseqn) -ref1;
   else
     sy->syval.rval.seq_val = (pmseqn) ref1;

   sy->syval.rval.ord_val = (pmseqn) ref2;
  }

/********************************************************/
/*!******************************************************/

        void asgchtok(
        struct ANSYREC *sy)

/*      Get a single or multiple character token.
 *
 *      In:  sy => Pointer to symbol record to update.
 *
 *      Out:       Updated symbol record.
 *
 *      (C)microform ab 1985-05-10 Thomas Nilsson
 *
 *      1999-04-16 Rewritten, R. Svedin
 *
 ******************************************************!*/
  {
   char tokstr [2];

   switch (ascurch) {

   case ',':
     sy -> sytype = ANSYCOM;
     asgch ();
     break;

   case ';':
     sy -> sytype = ANSYSEMI;
     asgch ();
     break;

   case ')':
     sy -> sytype = ANSYRPAR;
     asgch ();
     break;

   case '+':
     sy -> sytype = ANSYPLUS;
     asgch ();
     break;

   case '-':
     sy -> sytype = ANSYMINS;
     asgch ();
     break;

   case '=':
     sy -> sytype = ANSYEQ;
     asgch ();
     break;

   case '_':
     sy -> sytype = ANSYUND;
     asgch ();
     break;
/*
***Multiple Character Tokens
*/

   case '/':
     asgch ();
     if (ascurch == '*')
       {
       asskpcmt (ascurlin);       
       asgch();
       sy->sytype = (ANSYTYP)NULL; /* No token found ! */
       }
     else 
       sy -> sytype = ANSYDIV;
     break;
     
   case '.':
     asgch ();
     if (isdigit(ascurch))
       {
       asbch();                     /* Backup over last character */
       asgnlit(sy);
       }
     else 
       sy -> sytype = ANSYDOT;
     break;

   case ':':
     asgch ();
     if (ascurch == '=')
       {
       sy -> sytype = ANSYASGN;
       asgch ();
       }
     else
       sy -> sytype = ANSYCOL;
     break;

   case '(':
     asgch ();
     sy -> sytype = ANSYLPAR;
     break;

   case '*':
     asgch ();
     if (ascurch == '*')
       {
       sy -> sytype = ANSYEXP;
       asgch ();
       }
     else
       sy -> sytype = ANSYMUL;
       break;

   case '<':
     asgch ();
     if (ascurch == '>')
       {
       sy -> sytype = ANSYNEQ;
       asgch ();
       }
     else if (ascurch == '=')
       {
       sy -> sytype = ANSYLE;
       asgch ();
       }
     else
       sy -> sytype = ANSYLT;
     break;

   case '>':
     asgch ();
     if (ascurch == '=')
       {
       sy -> sytype = ANSYGE;
       asgch ();
       }
     else
       sy -> sytype = ANSYGT;
     break;

   default:
     tokstr [0] = ascurch;
     tokstr [1] = '\0';
     analog (ascurlin, ascurcol, "AN1002", tokstr);
     asgch ();
     sy->sytype = (ANSYTYP)NULL; /* No token found ! */
   };
  } 

/********************************************************
 *
 * Public Procedures
 *
 ********************************************************/
/*!******************************************************/

        short asinit(
        char src[],
        short alt)

/*      Init scanner to scan from file or string.
 *
 *      In:  src => name of the file or the string to scan
 *           alt => ANRDFIL -> src is a file name, read source from file
 *           alt => ANRDSTR -> src is a source string, read source from 
 *                  that string
 *
 *      Out: 
 *
 *      (C)microform ab 1985-05-10 Thomas Nilsson
 *
 *      1999-04-16 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   anstyp = alt;                   /* store source type */
   switch(anstyp)
     {
     case ANRDFIL:                 /* source from file */
       if ((assrcfil = fopen (src, "r")) == NULL)
         return (erpush ("AN0013", src));
       break;
     case ANRDSTR:                 /* source from string */
       assrcstr = src;
       break;
     default:
       printf("asinit, internal error : type not ANRDFIL or ANRDSTR\n");
       break;
     }

   ascurlin = 1;                   /* Current line is the first */
   ascurcol = 0;                   /* No characters seen yet */
   ascurch = ' ';                  /* so it is a space */
   peeked=FALSE;                   /* No token peeked */
   return (0);
  }

/********************************************************/
/*!******************************************************/

        int anascan(
        struct ANSYREC *sy)

/*      Scan for a lexikal token
 *
 *      In:  sy => Pointer to the symbol record to fill with next symbol.
 *
 *      Out:       Updated symbol record (pointed to by 'sy').
 *
 *      (C)microform ab 1985-05-10 Thomas Nilsson
 *
 *      1999-04-16 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   if (peeked)                         /* peeked token ? */
     {
     V3MOME(&ptok,sy,sizeof(ptok));    /* yes ! , use that one */
     peeked=FALSE;
     return(0);
     }
   do {
     asskpspc ();                      /* Skip any white space */
                                       /* up to next symbol */
     sy-> sypos.srclin = ascurlin;
     sy-> sypos.srccol = ascurcol;

     if (ascurch == AN_EOF)
       sy-> sytype = ANSYEOF;          /* End Of File symbol */
     else if (isialpha (ascurch))
       asgid (sy);                     /* Get an Identifier or Reserved Word */
     else if (isdigit (ascurch))
       asgnlit (sy);                   /* Get a Numeric Literal */
     else if (ascurch == '"')
       asgslit (sy);                   /* Get a String Literal */
     else if (ascurch == '#')
       asgrlit (sy);                   /* Get a Reference Literal */
/*
***Här kan man lägga in tex: else if (ascurch == '!') asgcmnt(sy)
***Samtidigt får man ta bort eller villkora i asskspc() via parameter
***på kommandoraden.
*/
     else
       asgchtok (sy);                  /* Get a Singel or Mulitple Character
                                          Token */

     } while (sy->sytype == (ANSYTYP)NULL); /* Until a legal token is found */

   return(0);
  } 

/********************************************************/
/*!******************************************************/

        int anapeek(
        struct ANSYREC *sy)

/*      Lookahead one lexical token
 *
 *      In:  
 *
 *      Out: 
 *
 *      (C)microform ab 1985-09-20 Mats Nelson
 *
 *      1999-04-16 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   anascan(sy);
   V3MOME(sy,&ptok,sizeof(ptok));
   peeked=TRUE;
   return(0);
  }

/*!******************************************************/

        short asexit()

/*      Exit scanner.
 *
 *      In:  
 *
 *      Out: 
 *
 *      (C)microform ab 1985-05-10 Thomas Nilsson
 *
 *      1999-04-16 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   if ( anstyp == ANRDFIL )
     fclose (assrcfil);

   return (0);
  }

/*********************************************************************/

/**********************************************************************
*
*    gp22.c
*    ======
*
*    This file is part of the VARKON Graphics  Library.
*    URL: http://www.varkon.com
*
*   *gpitoa();   short to ASCII
*    gpaton();   ASCII to float
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
*    (C)Microform AB 1984-1999, Johan Kjellander, johan@microform.se
*
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/GP.h"

/* Tabell för gpitoa */

static short tal[] = {
9000, 8000, 7000, 6000, 5000, 4000, 3000, 2000, 1000,
900, 800, 700, 600, 500, 400, 300, 200, 100,
90, 80, 70, 60, 50, 40, 30, 20, 10 };

static short *acc_tab[] = { &tal[0], &tal[9], &tal[18]};

static char ch[] = {'9','8','7','6','5','4','3','2','1','0'};

/* Tabell för gpaton */

static double tab_1[] = {
      0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001, 0.0000001,
      0.00000001, 0.000000001, 0.0000000001};
static double tab_2[] = { 
      0.2, 0.02, 0.002, 0.0002, 0.00002, 0.000002, 0.0000002, 
      0.00000002, 0.000000002, 0.0000000002};
static double tab_3[] = { 
      0.3, 0.03, 0.003, 0.0003, 0.00003, 0.000003, 0.0000003, 
      0.00000003, 0.000000003, 0.0000000003};
static double tab_4[] = { 
      0.4, 0.04, 0.004, 0.0004, 0.00004, 0.000004, 0.0000004, 
      0.00000004, 0.000000004, 0.0000000004};
static double tab_5[] = { 
      0.5, 0.05, 0.005, 0.0005, 0.00005, 0.000005, 0.0000005,
      0.00000005, 0.000000005, 0.0000000005};
static double tab_6[] = { 
      0.6, 0.06, 0.006, 0.0006, 0.00006, 0.000006, 0.0000006,
      0.00000006, 0.000000006, 0.0000000006};
static double tab_7[] = { 
      0.7, 0.07, 0.007, 0.0007, 0.00007, 0.000007, 0.0000007, 
      0.00000007, 0.000000007, 0.0000000007};
static double tab_8[] = { 
      0.8, 0.08, 0.008, 0.0008, 0.00008, 0.000008, 0.0000008, 
      0.00000008, 0.000000008, 0.0000000008};
static double tab_9[] = { 
      0.9, 0.09, 0.009, 0.0009, 0.00009, 0.000009, 0.0000009, 
      0.00000009, 0.000000009, 0.0000000009};

/*!******************************************************/

        char *gpitoa(
        char *buf,
        short n)

/*      Konvertera heltal till ASCII.
 *
 *      In: buf  => Pekare till resultat.
 *          n    => Heltal att konvertera.
 *
 *      Ut: *buf => ASCII-sträng.
 *
 *      FV: Pekare till nästa lediga pos. i buf.
 *
 *      (C)microform ab 19/3/86 J. Kjellander
 *
 ******************************************************!*/

  {
    register unsigned int j, lzero = 1;
    register short *talptr;
    register short **accptr = &acc_tab[0];
    register char  *chptr;

    for (j=0;j<3;++j)
      {
      talptr = *accptr++;
      if (n >= *(talptr + 8))
        {
        for (chptr = &ch[0]; n < *talptr; ++chptr,++talptr);
        *buf++ = *chptr;
        if (lzero) lzero = 0;
        n -= *talptr;
        continue;
        }
      else
        if (!lzero) *buf++ = '0';
      }
      *buf++ = n +'0';

    return(buf);
  }

/********************************************************/
/*!******************************************************/

        double gpaton(
        char   *pos)

/*      Konvreterar ascii till flyttal i området 0 - 1.
 *
 *      In: pos -> Pekare till en asciisträng.
 *
 *      Ut: Strängen konv. till flyttal.
 *
 *      FV: Inget. 
 *
 *      LDAB  18/12/85  Håkan Svensson 
 *
 *      (C)microform ab 
 *
 *      19/12/85 Modifierad till varkonformat R. Svedin
 *      29/3/86  Flyttad från exe7.c  R. Svedin
 *
 ******************************************************!*/
   {
     register short  i,decno;
     register char  *start = pos;
     register double sum;

     while (*pos++ != '.');

     if ( *(pos-2) == '1') return(1.0);

     decno = 10 - (pos-start);
     sum = 0.0;

     for (i=0;i<decno;++i) 
       {
       switch (*pos) 
         {
         case '0': break;

         case '1': sum += tab_1[i]; break;

         case '2': sum += tab_2[i]; break;

         case '3': sum += tab_3[i]; break;

         case '4': sum += tab_4[i]; break;
	
         case '5': sum += tab_5[i]; break;

         case '6': sum += tab_6[i]; break;

         case '7': sum += tab_7[i]; break;

         case '8': sum += tab_8[i]; break;

         case '9': sum += tab_9[i]; break;

         default:;

         }
 
       ++pos;

      }

    return(sum);

  }

/********************************************************/


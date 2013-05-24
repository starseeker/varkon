/**********************************************************************
*
*    inac19.c
*    ========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://www.varkon.com
*
*    short mkform()    make I/O format string
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
#include "../include/indef.h"

/*
***Global variables for arguments
*/
extern PMPARVA *func_pv;
extern short    func_pc;
extern PMLITVA *func_vp;
extern pm_ptr  *func_ty;

/********************************************************/
/*!******************************************************/

        short mkform(
        pmvaty  typ,     
        int     form1,   
        int     form2,   
        char   *formstr)

/*      Make I/O format string.
 *
 *      In:    typ    =>  I/O data type 
 *             form1  =>  I/O format1 value 0 indicates default
 *             form2  =>  I/O format2        - " -             
 *
 *      Out: *formstr =>  C:s I/O format string 
 *
 *      (C)microform ab 1985-10-23 Per-Ove Agne'
 *
 *      1999-11-12 Rewritten, R. Svedin
 *
 ******************************************************!*/

  {
   switch ( typ )
     {
     case C_INT_VA:

/*
***no format and input
*/
         if ( form1 == 0 )
             strcpy(formstr,"%d");
         else
             sprintf( formstr, "%%%dd",form1 );
     break;

     case C_FLO_VA:
/*
***no format and input
*/
         if ( form1 == 0 ) 
             {
             strcpy(formstr,"%lf");
             }
         else if ( form2 < 0 )
             {
/*
***output and exponent notation
*/
             if ( form2 == -1 )
                 { 
                 form2 = form1 - 7;
                 if ( form2 > 13 )
                     form2 = 13;

                 if ( form2 >= 0 )
                     sprintf( formstr, "%%%d.%dle", form1, form2 );
                 else
                     sprintf( formstr, "%%%dle", form1 );
                 }
             else
/*
***input
*/
                 {
                 sprintf( formstr, "%%%dlf", form1 );
                 }
             }
         else
             {
             sprintf( formstr, "%%%d.%dlf", form1, form2 );
             }
     break;

     case C_STR_VA:
      
         if ( form2 == -1 )
/*
***output
*/
             { 
             if ( form1 == 0 )
                 strcpy(formstr,"%s");
             else
                 sprintf( formstr, "%%%ds", form1 );
             }
         else
/*
***input
*/
             { 
             if ( form1 == 0 )
                 strcpy(formstr,"%[\136\n]");
             else
                 sprintf( formstr, "%%%d[\136\n]", form1 );
             }

     break;

     }
   return( 0 );
  }

/********************************************************/

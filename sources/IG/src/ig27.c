/*!******************************************************************/
/*  File: ig27.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  igcmos();     Interactive OS-command                            */
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
#include "../../EX/include/EX.h"

/*!******************************************************/

       short igcmos(char oscmd[])

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
   status = igssip(iggtts(3),s,"",1);
/*
***Slut.
*/
   return(status);
 }

/*********************************************************************/

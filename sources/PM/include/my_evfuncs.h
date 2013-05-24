/*!******************************************************************
*  File: my_evfuncs.h 
*  ==================   
* 
*  This is a sample my_evfuncs.h. All MBS function calls are
*  linked to a common evaluator that just prints an error message.
*
*  This file is part of the Program Module (PM) Library. 
* 
*    
*  This library is free software; you can redistribute it and/or 
*  modify it under the terms of the GNU Library General Public 
*  License as published by the Free Software Foundation; either 
*  version 2 of the License, or (at your option) any later 
*  version. 
* 
*  This library is distributed in the hope that it will be 
*  useful, but WITHOUT ANY WARRANTY; without even the implied  
*  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
*  PURPOSE.  See the GNU Library General Public License for more  
*  details.   
*   
*  You should have received a copy of the GNU Library General 
*  Public License along with this library; if not, write to the 
*  Free Software Foundation, Inc., 675 Mass Ave, Cambridge, 
*  MA 02139, USA. 
*       
*  (C)2006-11-18 J.Kjellander, Örebro university
*   
******************************************************************/

/*
***The following table defines the entrypoints for
***the C-functions that evaluates each MBS routine.
*/
static short (*functab[])() =

{
/*
***When building mbsc only a few routines need to be included.
*/
#ifdef ANALYZER
     NULL,      NULL,      NULL,      NULL,      NULL, /* 04 */
     NULL,      NULL,      NULL,      NULL,      NULL, /* 09 */
     NULL,      NULL,      NULL,      NULL,      NULL, /* 14 */
     NULL,      NULL,      NULL,      NULL,      NULL, /* 19 */
     NULL,      NULL,      NULL,      NULL,      NULL, /* 24 */
     NULL,      NULL,      NULL,      NULL,    evacos, /* 29 */
   evasin,    evatan,
    evcos,     evsin,     evtan,    evnlog,    evlogn,
   evsqrt,     evabs,    evfrac,    evroud,    evtrnc,
     NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 49 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,    evvecp,      NULL,      NULL, /* 59 */
     NULL,      NULL,      NULL,      NULL,     evvec,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 69 */
   evasci,     evstr,     evchr,      NULL,    evleng,
   evsubs,
/*
***When building xvarkon all routines are included.
*/
#else
     NULL,      NULL,      NULL,      NULL,      NULL, /* 04 */
     NULL,      NULL,      NULL,      NULL,      NULL, /* 09 */
     NULL,      NULL,      NULL,      NULL,      NULL, /* 14 */
     NULL,      NULL,      NULL,      NULL,      NULL, /* 19 */
     NULL,      NULL,      NULL,      NULL,      NULL, /* 24 */
     NULL,      NULL,      NULL,      NULL,      NULL, /* 29 */
     NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 49 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 59 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 69 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 79 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 89 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 99 */
     NULL,      NULL,      NULL,      NULL,      NULL, 
     NULL,      NULL,      NULL,      NULL,      NULL, /* 109 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 119 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 129 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 139 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 149 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 159 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 169 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 179 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 189 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 199 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 209 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 219 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 229 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 239 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 249 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 259 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 269 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 279 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 289 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 299 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 309 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 319 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 329 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 339 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 349 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 359 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 369 */
     NULL,      NULL,      NULL,      NULL,      NULL,
     NULL,      NULL,      NULL,      NULL,      NULL, /* 379 */
     NULL,      NULL,      NULL,      NULL, 
#endif
};

/******************************************************************/

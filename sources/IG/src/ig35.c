/*!******************************************************************/
/*  File: ig35.c                                                    */
/*  ============                                                    */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  iganrf();    Analyzes reference dependencies                    */
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
#include "../../GP/include/GP.h"
#include <string.h>

/*!******************************************************/

        short iganrf()

/*      Varkonfunktion för att analysera vilka storheter
 *      som refererar till en utpekad storhet.
 *
 *      In: Inget.
 *
 *      Ut: Inget.
 *
 *      FV: 0 = OK, REJECT = avsluta, GOMAIN = huvudmenyn
 *
 *      (C)microform ab 6/5/92 J. Kjellander
 *
 ******************************************************!*/

  {
    int      refant;
    char     buf[80],typstr[20];
    short    status,nid,i;
    DBetype  typv[IGMAXID];
    PMREFVA  idmat[IGMAXID][MXINIV],id;
    PMREFL  *reftab;
    GMRECH   hdr;
    GMPRT    part;
/*
***Hämta id för någon av storhetens instanser.
*/
    nid = 1;
    igptma(268,IG_MESS);
    typv[0] = ALLTYP;
    status = getmid(idmat,typv,&nid);
    igrsma();
    if ( status < 0 ) goto exit;
/*
***Test av pmwrme().
*/
    status = pmwrme((DBId *)idmat,&reftab,&refant);
    if ( status < 0 )
      {
      errmes();
      goto exit;
      }
    else
      {
      if ( refant > 1 )
        {
        sprintf(buf," Entity #%d refereced by the following %d entities !",
                      idmat[0][0].seq_val,refant-1);
        iginla(buf);
  
        for ( i=1; i<refant; ++i )
          {
/*
***Kolla i GM vilken typ av storhet det är.
*/
          id.seq_val  = reftab[i].snr;
          id.ord_val  = 1;
          id.p_nextre = NULL;
          EXgthd(&id,&hdr);

          switch ( hdr.type )
            {
            case POITYP: strcpy(typstr,"POINT"); break;
            case LINTYP: strcpy(typstr,"LINE"); break;
            case ARCTYP: strcpy(typstr,"ARC"); break;
            case CURTYP: strcpy(typstr,"CURVE"); break;
            case TXTTYP: strcpy(typstr,"TEXT"); break;
            case XHTTYP: strcpy(typstr,"HATCH"); break;
            case LDMTYP: strcpy(typstr,"LINEAR DIMENSION"); break;
            case CDMTYP: strcpy(typstr,"DIAMETER DIMENSION"); break;
            case RDMTYP: strcpy(typstr,"RADIUS DIMENSION"); break;
            case ADMTYP: strcpy(typstr,"ANGULAR DIMENSION"); break;
            case GRPTYP: strcpy(typstr,"GROUP"); break;
            case CSYTYP: strcpy(typstr,"COORDINATE SYSTEM"); break;
            case PRTTYP: strcpy(typstr,"PART"); break;
            case BPLTYP: strcpy(typstr,"B-PLANE"); break;
            case MSHTYP: strcpy(typstr,"MESH"); break;
            default:     strcpy(typstr,"Unknown !"); break;
            }
/*
***Skriv ut.
*/
          sprintf(buf,"%-16s #%-5d",
                       typstr,reftab[i].snr);
          if ( hdr.type == PRTTYP )
            {
            EXgtpt(&id,&part);
            strcat(buf,"Name = ");
            strcat(buf,part.name_pt);
            }
          igalla(buf,(short)1);
          }
        igexla();
        }
      else
        {
        sprintf(buf," Entity #%d referenced by the following %d entities !",
                      idmat[0][0].seq_val,refant-1);
        iginla(buf);
        igalla("Entity not referenced !",(short)1);
        igexla();
        }
      }
/*
***Slut.
*/
exit:
    gphgal(0);

    return(status);
  }

/********************************************************/

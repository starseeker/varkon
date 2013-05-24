/********************************************************************/
/*                                                                  */
/*  This file is part of the VARKON Geometry Library.               */
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
/*  (C)Microform AB 1984-1999, Gunnar Liden, gunnar@microform.se    */
/*                                                                  */
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../include/GE.h"

/********************************************************************/
/*!                                                                 */
/*  Function: varkon_sur_poiprint                  File: sur288.c   */
/*  =============================================================   */
/*                                                                  */
/*  Purpose                                                         */
/*  -------                                                         */
/*                                                                  */
/*  The function prints out data in table SUR_POI                   */
/*                                                                  */
/*  Author:   Gunnar Liden                                          */
/*                                                                  */
/*  Revisions                                                       */
/*                                                                  */
/*  1995-04-01   Originally written                                 */
/*  1999-12-13   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/


/* ------------- Short description of function -----------------*/
/*                                                              */
/*sdescr varkon_sur_poiprint   Printout of table SURPOI         */
/*                                                              */
/*------------------------------------------------------------- */

/*!-------------- Function calls (external) ------------------------*/
/*                                                                  */
/* varkon_erpush           * Error message to terminal              */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!------------ Error messages and warnings ------------------------*/
/*                                                                  */
/* SU2993 = Severe program error (  ) in varkon_sur_poiprint        */
/*                                                                  */
/*-----------------------------------------------------------------!*/

/*!New-Page--------------------------------------------------------!*/

/*!****************** Function **************************************/

      DBstatus        varkon_sur_poiprint (

/*-------------- Argument declarations -----------------------------*/
/*                                                                  */
/* In:                                                              */
  DBint    nu_poi,       /* Number of points in U direction         */
  DBint    nv_poi,       /* Number of points in V direction         */
  SURPOI  *p_surpoi,     /* Point table SURPOI (alloc. area)  (ptr) */
  DBint    s_record,     /* Start record (Eq:  -1: Not defined)     */
  DBint    e_record )    /* End   record (Eq:  -1: Not defined)     */

/*                                                                  */
/*      Data is modified in table POISUR                            */
/*                                                                  */
/*-----------------------------------------------------------------!*/

{ /* Start of function */

/*!--------------- Internal variables ------------------------------*/
/*                                                                  */
  DBint   i_rec;         /* Loop index record in SURPOI             */
  SURPOI *p_rec;         /* Record in table SURPOI            (ptr) */

/*                                                                  */
/*-----------------------------------------------------------------!*/

  char   errbuf[80];     /* String for error message fctn erpush    */

/*--------------end-of-declarations---------------------------------*/

/*!New-Page--------------------------------------------------------!*/

/*!                                                                 */
/* Algorithm                                                        */
/* =========                                                        */
/*                                                                 !*/

/*!                                                                 */
/* 1. Check of input data and initiations                           */
/* ______________________________________                           */
/*                                                                  */
/*                                                                 !*/

#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
{
fprintf(dbgfil(SURPAC),
"sur288 Enter***varkon_sur_poiprint**  p_surpoi= %d\n", p_surpoi);
}
#endif
 
  if ( s_record < 0 ) s_record = 1;
  if ( e_record < 0 ) e_record = nu_poi*nv_poi;

  if ( s_record < 1 || s_record > nu_poi*nv_poi )
    {
    sprintf(errbuf,"s_record not OK%%varkon_sur_poiprint");
    return(varkon_erpush("SU2993",errbuf));
    }

  if ( e_record < 2 || e_record > nu_poi*nv_poi )
    {
    sprintf(errbuf,"e_record not OK%%varkon_sur_poiprint");
    return(varkon_erpush("SU2993",errbuf));
    }

  if ( e_record < s_record )
    {
    sprintf(errbuf,"e_record<s_record%%varkon_sur_poiprint");
    return(varkon_erpush("SU2993",errbuf));
    }

/*!                                                                 */
/* 2. Printout of records                                           */
/* ______________________                                           */
/*                                                                  */
/*                                                                  */
/* Loop all records i_rec= s_record, s_record+1, .. , e_record      */
/*                                                                 !*/

for (i_rec= s_record;i_rec<=e_record;    /* Start loop record       */
                    i_rec=i_rec+1)       /*                         */
 {                                       /*                         */

/*!                                                                 */
/*    Retrieve record p_surpoi+i_rec-1 from SURPOI                  */
/*                                                                 !*/

    p_rec = p_surpoi + i_rec - 1;        /* Current SURPOI adress   */


#ifdef DEBUG
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur288 Record %d Adress %d p_rec->iu %d p_rec->iv %d \n",
     i_rec, p_rec, p_rec->iu,p_rec->iv  ); 
  fflush(dbgfil(SURPAC)); 
  }

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur288 Point      %15.8f %15.8f %15.8f\n",
      p_rec->spt.r_x, p_rec->spt.r_y, p_rec->spt.r_z);
  fflush(dbgfil(SURPAC)); 
  }

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur288 U tangent  %15.8f %15.8f %15.8f\n",
      p_rec->spt.u_x, p_rec->spt.u_y, p_rec->spt.u_z);
  fflush(dbgfil(SURPAC));
  }

if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC), 
  "sur288 V tangent  %15.8f %15.8f %15.8f\n",
      p_rec->spt.v_x, p_rec->spt.v_y, p_rec->spt.v_z);
  fflush(dbgfil(SURPAC));
  }

#endif

}
/*!                                                                 */
/* End  all records i_rec= s_record, s_record+1, .. , e_record      */
/*                                                                 !*/

/*                                                                  */
/* 3. Exit                                                          */
/* ______                                                           */
/*                                                                  */
/*                                                                  */

#ifdef DEBUG                             /* Debug printout          */
if ( dbglev(SURPAC) == 1 )
  {
  fprintf(dbgfil(SURPAC),
  "sur288 Exit***varkon_sur_poiprint ** \n");
  fflush(dbgfil(SURPAC));
  }
#endif

    return(SUCCED);

  } /* End of function */

/*********************************************************/

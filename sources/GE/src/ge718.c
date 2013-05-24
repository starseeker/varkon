/*!******************************************************************/
/*  File: ge718.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE718() Calculates relative arclength for given global u        */
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

      DBstatus GE718(
      DBAny   *pstr,
      DBSeg   *pseg,
      DBTmat  *pc,
      DBfloat  uglobs,
      DBfloat  uglobe,
      DBfloat *prel_leng)

/*      The function calculates the relative arclength for a given
 *      global parameter value u.
 *
 *      In:  pstr   = The entity
 *           pseg   = Optional segments
 *           pc     = Active coordinate system
 *           uglobs = Global u start value
 *           uglobe = Global u end value
 *
 *      Out: *prel_leng = Relative arclength between the input values
 *
 *      (C)microform ab 1991-12-10 G.Liden
 *
 *      1994-10-09 Debug changed, bugs fixed G Liden
 *      1994-11-24 comptol (for surface curves) G Liden
 *      1995-02-28 Bug whole curve, ctol sur751 G Liden
 *      1999-05-26 Rewritten, J.Kjellander
 *      1999-12-18 sur753->varkon_comptol sur751->.._ctol G Liden
 *
 *****************************************************************!*/

 {
   DBfloat  comptol;    /* Computer tolerance (accuracy)          */
   DBfloat  ctol;       /* End coordinate calculation criterion   */
   short    status;     /* Function value from called function    */
   DBetype  type;       /* The input curve type                   */
   short    noseg;      /* Number of segments in the curve        */
   DBfloat  interv[2];  /* Local u value for GE120               */
   DBfloat  sum_leng;   /* Sum of segment arclengths              */
   DBfloat  tot_leng;   /* Total arclength                        */
   DBfloat  dl;         /* Arclength for one segment              */
   short    iseg;       /* Loop index segment number              */
   short    isegs;      /* Start segment number                   */
   short    isege;      /* End   segment number                   */

/*
***Computer accuracy and end coordinate criterion
*/
   comptol    = varkon_comptol();
   ctol       = varkon_ctol();
/*
***Determine the curve type and retrieve noseg
*/
   type = pstr->poi_un.hed_p.type;
/*
***Line.
*/
   if ( type == LINTYP) return(erpush("GE7373","GE718"));
/*
***2D arc.
*/
   else if ( type == ARCTYP ) 
     {
     noseg = pstr->arc_un.ns_a;
     if ( noseg == 0 ) return(erpush("GE7373","GE718"));
/*
***3D arc.
*/
     status = GEarclength(pstr,pseg,&tot_leng);
     if ( status < 0 ) return(erpush("GE8243","GE718"));
     }
/*
***Curve.
*/
   else if ( type == CURTYP )
     {
     noseg    = pstr->cur_un.ns_cu;
     tot_leng = pstr->cur_un.al_cu;
     if ( tot_leng < comptol )
       {
       status = GEarclength(pstr,pseg,&tot_leng);
       if(status<0)return(erpush("GE8243","GE717"));
       }
     }
/*
***Illegal type of entity.
*/
   else return(erpush("GE7993","GE718"));
          
/*
***Check input global parameter values.
*/
   if ( uglobs < 1.0 - comptol ) return(erpush("GE7383","GE718"));

   if ( uglobe > noseg + 1.0 + comptol ) return(erpush("GE7383","GE718"));

   if ( uglobe >= noseg + 1.0 ) uglobe = uglobe-comptol;

   if ( ABS(uglobs - uglobe) < comptol ) 
     {
     sum_leng = 0.0;
     goto  sum;
     }

   if ( uglobs > uglobe ) return(erpush("GE7383","GE718"));
/*
***Calculate the absolute arclength sum_leng.
*/
   sum_leng  = 0.0;

   isegs = (DBint)HEL(uglobs);
   isege = (DBint)HEL(uglobe);
/*
***Arclength for part of the first segment.
*/
   if ( isegs == isege )
     {
     interv[0] = uglobs - isegs;
     interv[1] = uglobe - isege;
     }
   else
     {
     interv[0] = uglobs - isegs;
     interv[1] = 1.0 - comptol;
     }

   status = GE120(pstr,pseg+isegs-1,interv,&dl);
   if ( status < 0 ) return(erpush("GE1273","GE718"));

   sum_leng += dl;
/*
***Goto sum if isegs and isege are equal
*/
   if ( isegs == isege ) goto sum;
/*
***Goto last if isegs is equal to isege-1
*/
   if ( isegs == isege - 1 ) goto last;
/*
***Loop for intermediate segments.
*/
   for ( iseg=isegs; iseg < isege-1; iseg++ )
      {
      if ( type == CURTYP ) dl = (pseg+iseg)->sl;
      else
        {
        interv[0] = 0.0;
        interv[1] = 1.0;
        status = GE120(pstr,pseg+iseg,interv,&dl);
        if ( status < 0 ) return(erpush("GE1273","GE718"));
        }
      sum_leng += dl;
      }
/*
***Last segment.
*/
last:
   interv[0] = comptol;
   interv[1] = uglobe - isege;

   if ( ABS(interv[1]) > comptol )
     {
     status = GE120(pstr,pseg+isege-1,interv,&dl);
     if ( status < 0 ) return(erpush("GE1273","GE718"));
     }
   else dl = 0.0;

   sum_leng += dl;
/*
***Relative arclength.
*/
sum:

   if ( ABS(tot_leng) > comptol ) *prel_leng = sum_leng/tot_leng;

   if ( *prel_leng > 1.0  ) *prel_leng = 1.0;

   return(0);
 }

/********************************************************************/

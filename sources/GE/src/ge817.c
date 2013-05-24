/*!******************************************************************/
/*  File: ge817.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE817() Trim and/or change direction of a curve                 */
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

/*!******************************************************************/

      DBstatus GE817(
      DBAny   *pstr,
      DBSeg   *pseg,
      DBCurve *pcur_out,
      DBSeg   *pseg_out,
      DBfloat glob_s,
      DBfloat glob_e)

/*    Trim and/or change direction of a curve.
 *
 *      In: pstr   = The curve
 *          pseg   = Its segments
 *          glob_s = Global u start value
 *          glob_e = Global u end value
 *
 *      Out: *pcur_out = The trimmed curve
 *           *pseg_out = Its segments
 *
 *      (C)microform ab 1992-04-03 Stefan Westberg
 *
 *  1992-04-07 VAX/VMS, #ifdef's, J. Kjellander
 *  1994-11-23 UV segments & bug G. Liden
 *  1995-05-11 hed_cu.type = CURTYP  & f_end G. Liden
 *  1997-04-27 Tolerance bug same segment test
 *             sur778, al_cu= 0
 *  1997-04-29 Elimination of compilation warning G Liden
 *  1999-05-21 Rewritten, J.Kjellander
 *  1999-12-18 sur778->varkon_ini_gmcur  G Liden
 *  2003-03-03 error msg if NURBS segment Sören Larsson
 *
 *****************************************************************!*/

 {
    DBetype  type;       /* The input curve type                   */
    DBCurve *pcur;       /* If the curve is a  curve               */
    DBint   noseg;       /* Number of segments in curve            */
    DBint   noseg_tr;    /* Number of segments in trimmed curve    */
    DBint   iseg;        /* Loop index segment in curve            */
    DBint   f_start;     /* =1 when start segment found            */
    DBint   f_end;       /* =1 when end segment found              */
    DBfloat local_u;     /* Local u value for the curve            */
    DBfloat uextr[2];    /* Extrapolation parameter values         */
                         /* values on the curve !!                 */ 
                         /* uextrÄ0Å = u0 if u0 > 1.0              */ 
                         /*     else = 1.0                         */  
                         /* uextrÄ1Å = u1 if u1 < nseg+1           */
                         /*     else = nseg + 0.999                */ 
    DBfloat utmp[2];     /* Extrapolation parameter values         */
                         /* utmpÄ0Å = u0   Start                   */ 
                         /* utmpÄ1Å = u1   End                     */
    DBint   status;      /* Error code from a called function      */


/*
***Check input global parameter values
*/
   if ( glob_s < -100.0 ) return(erpush("GE8993","GE817"));
/*
***Initialize f_start, f_end and noseg_tr
*/
   f_start = 0;
   f_end   = 0;
   noseg_tr = 0;

   iseg = I_UNDEF;
/*
***Determine the curve type and retrieve noseg
*/
   type = pstr->hed_un.type;
/*
***Straight line: Program error
***2D arc:        Program error
***3D arc:        Not implemented
***Curve:         Retrieve number of segments noseg
*/
   
   if      ( type == LINTYP ) return(erpush("GE7373","GE817"));

   else if ( type == ARCTYP ) return(erpush("GE7373","GE817"));
/*
***Curve with rational cubic segments
*/
   else if ( type == CURTYP )
     {
     if(pseg[0].typ==NURB_SEG)return(erpush("GE7373","GE817"));
     
     pcur     = (DBCurve *)pstr;
     noseg    = pcur->ns_cu;
/*
***Initialize GMCUR
*/
     varkon_ini_gmcur(pcur_out); 
     pcur_out->al_cu =  0.0;
     pcur_out->hed_cu.type = CURTYP;
     }
/*
***Unknown type of entity.
*/
    else return(erpush("GE7993","GE817"));
/*
***Data (coefficients) for all segments of the curve
***Modify extrapolation parameters if the curve is to be extrapolated
***outside the current curve
*/    
   if ( glob_s < glob_e )
     { 
     utmp[0] = 123.456;
     if ( glob_s < 1.0 ) 
       {
       utmp[0]  = glob_s;
       glob_s   = 1.0;
       }

     utmp[1] = 123.456;
     if ( glob_e > (noseg+1) ) 
       {
       utmp[1]  = glob_e;
       glob_e   = noseg + 0.999; 
       }
     } 
   else 
     {
     utmp[0] = 123.456;
     if ( glob_s > (noseg+1) ) 
       {
       utmp[0]  = glob_s;
       glob_s   = noseg + 0.999;
       }

     utmp[1] = 123.456;
     if ( glob_e < 1.0 ) 
       {
       utmp[1]  = glob_e;
       glob_e   = 1.0; 
       }
     } 
/*
***Check if glob_s and glob_e is on the same segment
*/
   if ( HEL(glob_s) == HEL(glob_e) )
     {
     noseg_tr = 1;
     uextr[0] = DEC(glob_s);
     uextr[1] = DEC(glob_e);
     iseg     = (DBint)HEL(glob_s);
     }
   else if ( HEL(glob_s) == HEL(glob_e-TOL4) && glob_s < glob_e)
     {
     noseg_tr = 1;
     uextr[0] = glob_s - (gmint)floor(glob_s);
     uextr[1] = 1.0;
     iseg = (DBint)HEL(glob_s);
     }
   else if ( HEL(glob_s-TOL4) == HEL(glob_e) && glob_s > glob_e)
     {
     noseg_tr = 1;
     uextr[0] = 1.0;
     uextr[1] = glob_e - (gmint)floor(glob_e);
     iseg = (DBint)HEL(glob_e);
     }
   else if ( HEL(glob_s+TOL4) == HEL(glob_e) && glob_s < glob_e)
     {
     noseg_tr = 1;
     uextr[0] = 0.0;
     uextr[1] = glob_e - (gmint)floor(glob_e);
     iseg = (DBint)HEL(glob_s+TOL4);
     }
   else if ( HEL(glob_s+TOL4) == HEL(glob_e-TOL4) && glob_s < glob_e)
     {
     noseg_tr = 1;
     uextr[0] = 0.0;
     uextr[1] = 1.0;
     iseg = (DBint)HEL(glob_s+TOL4);
     }
   else if ( HEL(glob_s-TOL4) == HEL(glob_e+TOL4) && glob_s > glob_e)
     {
     noseg_tr = 1;
     uextr[0] = 1.0;
     uextr[1] = 0.0;
     iseg = (DBint)HEL(glob_s-TOL4);
     }
/*
***Reparameterize segment iseg, call GE135
*/
   if ( noseg_tr == 1 )
     {
     status = GE135(pseg+iseg-1,uextr,pseg_out);     
     if ( status < 0 ) return(erpush("GE1253","GE817"));
/*
***Segment data is calculated
*/
     goto _10050;
     } 
/*
***Loop for all segments in the curve
*/
   if ( glob_s < glob_e ) 
     {
/*
***Start case A: glob_s < glob_e
*/
     for ( iseg=1; iseg <= noseg; iseg++)  
       {
/*
***Start forward loop
*/ 
       local_u = iseg;

       if ( glob_s >= local_u && glob_s < local_u+1.0 )  
         {
/*
***Start segment found
*/ 
         f_start = 1;
         noseg_tr++;
         uextr[0] = glob_s - local_u;
         if ( ABS(uextr[0]-1.0) < TOL4 ) uextr[0] = 0.0;
         uextr[1] = 1.0;
/*
***Compute, call GE135
*/
         status = GE135(pseg+iseg-1,uextr,pseg_out+noseg_tr-1);     
         if ( status < 0 ) return(erpush("GE1253","GE817"));
         }
/*
***End segment found
*/
       else if ( glob_e == local_u ) 
         {
         f_end = 1;
         }

       else if ( glob_e > local_u && glob_e <= local_u+1 ) 
         {
         f_end = 1;
         noseg_tr++;
         uextr[0] = 0;
         uextr[1] = glob_e - local_u;
/*
***Compute, call GE135
*/
         status = GE135(pseg+iseg-1,uextr,pseg_out+noseg_tr-1);      
         if ( status < 0 ) return(erpush("GE1253","GE817")); 
         }  
/*
***Intermediate segment.
*/
       else
         {
         if (f_start && !f_end)
           {
           noseg_tr++;
           uextr[0] = 0;
           uextr[1] = 1.0;
/*
***Compute, call GE135
*/
           status = GE135(pseg+iseg-1,uextr,pseg_out+noseg_tr-1);     
           if ( status < 0 ) return(erpush("GE1253","GE817"));
           }
         }
       } /* End forward loop */
     } /* End case A */ 
/*
***Start case B: glob_e < glob_s
*/
   else  
     {
     for ( iseg=noseg; iseg >= 1; iseg--)  
       {
       local_u = iseg;
/*
***Start segment.
*/
       if (glob_s > local_u && glob_s <= local_u+1.0)
         {
         f_start = 1;
         noseg_tr++;
         uextr[0] = glob_s - local_u;
         if ( ABS(uextr[0]) < TOL4 ) uextr[0] = 1.0;
         uextr[1] = 0.0;
/*
***Compute, call GE135
*/
         status = GE135(pseg+iseg-1,uextr,pseg_out+noseg_tr-1);      
         if ( status < 0 ) return(erpush("GE1253","GE817")); 
         }
/*
***End segment.
*/
       else if ( glob_e == local_u+1.0 )
         {
         f_end = 1;
         }

       else if ( glob_e >= local_u && glob_e < local_u+1.0 )
         {
         f_end   = 1;
         noseg_tr++;
         uextr[0] = 1.0;
         uextr[1] = glob_e-local_u;
/*
***Compute, call GE135
*/
         status = GE135(pseg+iseg-1,uextr,pseg_out+noseg_tr-1);      
         if ( status < 0 ) return(erpush("GE1253","GE817")); 
         }
/*
***Intermediate segment.
*/
       else
         {
         if ( f_start && !f_end )   /* Tillbaks 1995-05-11 */
           {
           noseg_tr++;
           uextr[0] = 1.0;
           uextr[1] = 0.0;
/*
***Compute, call GE135
*/
           status = GE135(pseg+iseg-1,uextr,pseg_out+noseg_tr-1);      
           if ( status < 0 ) return(erpush("GE1253","GE817")); 
           }
         }
       } /* End backward loop */ 
     } /* End case B */

_10050: /* All segment data for the trimmed curve is calculated     */
 
/*
***Modifications if the end segments are extrapolated
*/
   if (glob_s < glob_e)
     {
     if (utmp[0] != 123.456)
       {
       if ( pseg->typ ==  UV_SEG )
         {
         erinit();
         return(erpush("GE8293","GE817")); 
         }
       glob_s = utmp[0];
       uextr[0] = glob_s-1.0;
       uextr[1] = 1.0; 
/*
***Compute, call GE135
*/
       status = GE135(pseg,uextr,pseg_out);      
       if ( status < 0 ) return(erpush("GE1253","GE817")); 
       }
     if (utmp[1] != 123.456)
       {
       if ( (pseg+noseg-1)->typ ==  UV_SEG )
         {
         erinit();
         return(erpush("GE8293","GE817")); 
         }
       glob_e = utmp[1];
       uextr[0] = 0.0;
       uextr[1] = glob_e-(gmint)floor(glob_e)+1.0; 
/*
***Compute, call GE135
*/
       status = GE135(pseg+noseg-1,uextr,pseg_out+noseg_tr-1);      
       if ( status < 0 ) return(erpush("GE1253","GE817")); 
       }
     }
   else   
     {
     if (utmp[0] != 123.456)
       {
       glob_s = utmp[0];
       uextr[0] = glob_s-(gmint)floor(glob_s)+1.0;
       uextr[1] = 0.0; 
/*
***Compute, call GE135
*/
       status = GE135(pseg+noseg-1,uextr,pseg_out);      
       if ( status < 0 ) return(erpush("GE1253","GE817")); 
       }

     if (utmp[1] != 123.456)
       {
       glob_e = utmp[1];
       uextr[0] = 1.0;
       uextr[1] = glob_e-1.0; 
/*
***Compute, call GE135
*/
       status = GE135(pseg,uextr,pseg_out+noseg_tr-1);      
       if ( status < 0 ) return(erpush("GE1253","GE817")); 
       }
     }
/*
***Number of segments in the trimmed output curve
*/
   pcur_out->ns_cu =  (short)noseg_tr;
/*
***A planar input curve results in a planar output curve and
***a non-planar curve results in a non-planar output curve.
*/
   pcur_out->plank_cu = pcur->plank_cu;
   if ( pcur->plank_cu ) GE814(pcur_out,&pcur->csy_cu);

   return(0);
  }

/********************************************************************/

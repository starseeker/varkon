/*!******************************************************************/
/*  File: ge206.c                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GE206() Creation of a tangent 2D line to two circles            */
/*          The equivalent for lin_tan2 in MBS                      */
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
/*                                                                  */
/*  The circles (GMARC) in Varkon have a 2D representation          */
/*  (local coordinate system, origin, radius, start and             */
/*   end angles) and a 3D representation as rational                */
/*  quadratic segments.                                             */
/*                                                                  */
/*  The 2D representation can be used if circles are defined        */
/*  in Basic and probably also if the local system is from          */
/*  a 2D (DRAWING) job. But not for other cases, for instance       */
/*  that the input circle has been transformated. Transformations   */
/*  and local system for GMARCs are not handled OK. TODO ....       */
/*  The origin seem not to be stored in the local system ???        */
/*  The transformation matrix for the circle is NULL after a        */
/*  transformation of the circle ....                               */
/*                                                                  */
/*  Other functions are using the data (geo622, geo640, geo707,     */
/*  geo708, GEposition, geo821, geo822, geo824) and a feature       */
/*  recognition approach is used for the fix. The right solution    */
/*  to the problem is to check and modify all functions that use    */
/*  the 2D data.                                                    */
/*                                                                  */
/*  Gunnar Liden 1985-07-24                                         */
/*                                                                  */
/*  Revised                                                         */
/*  Removed nalt2 22/8/85 J. Kjellander                             */
/*  Removed transformation to basic 18/9/85 J. Kjellander           */
/*  Changed method to select soloution 25/9/85 J. Kjellander        */
/*  86/04/27 Bug och optimering, J. Kjellander                      */
/*  1986-05-11 extern short, B. Doverud                             */
/*  1991-12-12 Tagit bort gei206(), J. Kjellander                   */
/*  1998-11-15 geo208, to a 3D function, cases added Gunnar Liden   */
/*  1999-12-18 sur741->varkon_idpoint                Gunnar Liden   */
/*                                                                  */
/********************************************************************/


/********************************************************************/


  DBstatus GE206(
      GMARC   *pa1,      /* Pointer to the output circle            */
      GMSEG   *paseg1,   /* Pointer to the array of segments for    */
                         /* rational cubic data                     */
      DBTmat  *p_csys_1, /* Coord. system circle 1            (ptr) */
      GMARC   *pa2,      /* Pointer to the output circle            */
      GMSEG   *paseg2,   /* Pointer to the array of segments for    */
                         /* rational cubic data                     */
      DBTmat  *p_csys_2, /* Coord. system circle 2            (ptr) */
      DBshort  alt,      /* Defines the tangent point (1,2,3,4)     */
      DBTmat  *pc,       /* Pointer to the coordinate system        */
      GMLIN   *plin)     /* Pointer to the output line              */


{ /* Start of function */

/* Internal variables                                               */
/* ...................                                              */

   DBTmat csys_arcs;     /* Coordinate system for circles           */
   DBVector  center_1;   /* Calculated center for circle 1          */
   DBVector  center_2;   /* Calculated center for circle 2          */
   DBVector  s_pt_1;     /* Start point circle 1                    */
   DBVector  m_pt_1;     /* Mid   point circle 1                    */
   DBVector  e_pt_1;     /* End   point circle 1                    */
   DBVector  s_pt_2;     /* Start point circle 2                    */
   DBVector  m_pt_2;     /* Mid   point circle 2                    */
   DBVector  e_pt_2;     /* End   point circle 2                    */
   DBVector  xaxis;      /* X axis for coordinate system            */
   DBVector  yaxis;      /* Y axis for coordinate system            */
   GMARC  arc1;          /* New arc 1 in local system csys_arcs     */
   GMARC  arc2;          /* New arc 2 in local system csys_arcs     */
   GMSEG  seg_1[4];      /* Segment data (dummy) for arc1           */
   GMSEG  seg_2[4];      /* Segment data (dummy) for arc2           */
   short  alt_arc;       /* Alternative 2D (2) or 3D (3) for arc    */
   short  status;        /* Status from a called function           */
   gmint  n_lin;         /* Number of lines for whole circles       */
   DBVector  start;      /* The start point of the line             */
   DBVector  end;        /* The end   point of the line             */
   gmflt  test_dist_1;   /* Distance center pts circle 1            */
   gmflt  test_dist_2;   /* Distance center pts circle 2            */
   gmflt  idpoint;       /* Identical points criterion              */

/*------------end-of-declarations-----------------------------------*/


/*
***Get identical points criterion
*/
   idpoint = varkon_idpoint();
/*
*** No checks can be made if there are no 3D segments
*** TODO Make additional tests for this case
*/
    if ( 0 == pa1->ns_a  &&  0 == pa2->ns_a ) goto arcs_2d;
/*
***Check that input circles are in the same plane
*/


#ifdef KEEP_CODE_FOR_THE_FUTURE_WHEN_2D_IS_FIXED
extern short geo622();
   DBTmat *p_csys;       /* Coordinate system for circles     (ptr) */
   GMARC  arc1_tra;      /* Transformated circle 1                  */
   GMARC  arc2_tra;      /* Transformated circle 2                  */
   GMARC  *p_arc1_tra;   /* Transformated circle 1            (ptr) */
   GMARC  *p_arc2_tra;   /* Transformated circle 2            (ptr) */
   GMSEG  seg1_tra[4];   /* Segments for transformed circle 1       */
   GMSEG  seg2_tra[4];   /* Segments for transformed circle 2       */
   DBVector test_pt;       /* Pt used for test of identical matrices  */
   DBVector t_t1_pt;       /* Pt used for test of identical matrices  */
   DBVector t_t2_pt;       /* Pt used for test of identical matrices  */
   gmflt  test_dist;     /* Distance,   test of identical matrices  */
      p_csys        =         NULL;
      p_arc1_tra   =         NULL;
      p_arc2_tra   =         NULL;
      test_pt.x_gm = -0.123456789;
      test_pt.y_gm = -0.123456789;
      test_pt.z_gm = -0.123456789;
      t_t1_pt.x_gm = -0.123456789;
      t_t1_pt.y_gm = -0.123456789;
      t_t1_pt.z_gm = -0.123456789;
      t_t2_pt.x_gm = -0.123456789;
      t_t2_pt.y_gm = -0.123456789;
      t_t2_pt.z_gm = -0.123456789;
      test_dist    = -0.123456789;
/* Only same coordinate system (or NULL) is implemented */
   if     ( NULL == p_csys_1 && NULL == p_csys_2 ) 
     {
     p_csys = NULL;
     }
   else if ( NULL == p_csys_1 || NULL == p_csys_2 ) 
     {
     return(erpush("GE2203","GE206"));
     }
   else
     {
     /* Test for equal matrices with a transformation */
     test_pt.x_gm = 5000.0;
     test_pt.y_gm = 5000.0;
     test_pt.z_gm = 5000.0;
     GEtfpos_to_basic(&test_pt,p_csys_1,&t_t1_pt);
     GEtfpos_to_basic(&test_pt,p_csys_2,&t_t2_pt);
     test_dist = sqrt( 
        (t_t1_pt.x_gm-t_t2_pt.x_gm)*(t_t1_pt.x_gm-t_t2_pt.x_gm)+
        (t_t1_pt.y_gm-t_t2_pt.y_gm)*(t_t1_pt.y_gm-t_t2_pt.y_gm)+
        (t_t1_pt.z_gm-t_t2_pt.z_gm)*(t_t1_pt.z_gm-t_t2_pt.z_gm));
     if ( test_dist > idpoint ) return(erpush("GE2203","GE206"));
     p_csys = p_csys_1;
     }

/*
***Transform the circles if defined in a local systems.
*/

  if ( p_csys != NULL)
    {
    status=geo622(pa1, paseg1, p_csys, &arc1_tra, seg1_tra);
    if ( status < 0 ) return(status);
    p_arc1_tra = &arc1_tra;
    status=geo622(pa2, paseg2, p_csys, &arc2_tra, seg2_tra);
    if ( status < 0 ) return(status);
    p_arc2_tra = &arc2_tra;
    /* Temporary !!! Not nicely programmed !!!!!!!!!!!!!!!!!!!!!! */
    pa1 = p_arc1_tra;
    pa2 = p_arc2_tra;
    } /* End p_csys != NULL */
#endif /*  KEEP_CODE_FOR_THE_FUTURE_WHEN_2D_IS_FIXED */

/*
*** Calculate center points for circles
*/
   status = GEcentre((DBAny *)pa1, paseg1, 1.0, &center_1);
   if ( status < 0 ) return (status);
   status = GEcentre((DBAny *)pa2, paseg2, 1.0, &center_2);
   if ( status < 0 ) return (status);

   test_dist_1 = sqrt( 
        (center_1.x_gm-pa1->x_a)*(center_1.x_gm-pa1->x_a)+
        (center_1.y_gm-pa1->y_a)*(center_1.y_gm-pa1->y_a)+
        (center_1.z_gm-  0.0   )*(center_1.z_gm-  0.0   ));
   test_dist_2 = sqrt( 
        (center_2.x_gm-pa2->x_a)*(center_2.x_gm-pa2->x_a)+
        (center_2.y_gm-pa2->y_a)*(center_2.y_gm-pa2->y_a)+
        (center_2.z_gm-  0.0   )*(center_2.z_gm-  0.0   ));

   if ( test_dist_1 < idpoint && test_dist_2 < idpoint )
       goto  centers_ok;

/*
*** Calculate points for coordinate system and circles
*/
   status = GEposition((DBAny *)pa1, (char *)paseg1, 0.0 ,0.0, &s_pt_1);
   if ( status < 0 ) return (status);
   status = GEposition((DBAny *)pa1, (char *)paseg1, 0.1 ,0.0, &m_pt_1);
   if ( status < 0 ) return (status);
   status = GEposition((DBAny *)pa1, (char *)paseg1, 0.99, 0.0,&e_pt_1);
   if ( status < 0 ) return (status);
   status = GEposition((DBAny *)pa2, (char *)paseg2, 0.0 ,0.0, &s_pt_2);
   if ( status < 0 ) return (status);
   status = GEposition((DBAny *)pa2, (char *)paseg2, 0.1 ,0.0 ,&m_pt_2);
   if ( status < 0 ) return (status);
   status = GEposition((DBAny *)pa2, (char *)paseg2, 0.99, 0.0,&e_pt_2);
   if ( status < 0 ) return (status);
/*
*** Create coordinate system for circles
*/
   xaxis.x_gm = s_pt_1.x_gm - center_1.x_gm;
   xaxis.y_gm = s_pt_1.y_gm - center_1.y_gm;
   xaxis.z_gm = s_pt_1.z_gm - center_1.z_gm;
   yaxis.x_gm = m_pt_1.x_gm - center_1.x_gm;
   yaxis.y_gm = m_pt_1.y_gm - center_1.y_gm;
   yaxis.z_gm = m_pt_1.z_gm - center_1.z_gm;

   status = GEmktf_3p(&center_1, &xaxis, &yaxis, &csys_arcs);
   if ( status < 0 ) return (status);
/*
*** Transformate all points to the local system
*/
   status = GEtfpos_to_local(&center_1, &csys_arcs, &center_1);
   if ( status < 0 ) return (status);
   status = GEtfpos_to_local(&s_pt_1  , &csys_arcs, &s_pt_1  );
   if ( status < 0 ) return (status);
   status = GEtfpos_to_local(&m_pt_1  , &csys_arcs, &m_pt_1  );
   if ( status < 0 ) return (status);
   status = GEtfpos_to_local(&e_pt_1  , &csys_arcs, &e_pt_1  );
   if ( status < 0 ) return (status);
   status = GEtfpos_to_local(&center_2, &csys_arcs, &center_2);
   if ( status < 0 ) return (status);
   status = GEtfpos_to_local(&s_pt_2  , &csys_arcs, &s_pt_2  );
   if ( status < 0 ) return (status);
   status = GEtfpos_to_local(&m_pt_2  , &csys_arcs, &m_pt_2  );
   if ( status < 0 ) return (status);
   status = GEtfpos_to_local(&e_pt_2  , &csys_arcs, &e_pt_2  );
   if ( status < 0 ) return (status);
/*
*** Check that all points are in the same plane
*/
   if ( fabs(center_1.z_gm) > idpoint ||
        fabs(  s_pt_1.z_gm) > idpoint ||
        fabs(  m_pt_1.z_gm) > idpoint ||
        fabs(  e_pt_1.z_gm) > idpoint ||
        fabs(center_2.z_gm) > idpoint ||
        fabs(  s_pt_2.z_gm) > idpoint ||
        fabs(  m_pt_2.z_gm) > idpoint ||
        fabs(  e_pt_2.z_gm) > idpoint   )
     {
     return(erpush("GE2223","GE206"));
     }
/*
*** Create new 2D circles
*/
   alt_arc = 3;
   status = GE310(&s_pt_1,&m_pt_1,&e_pt_1,NULL,&arc1,seg_1,alt_arc);
   if ( status < 0 ) return (status);
   status = GE310(&s_pt_2,&m_pt_2,&e_pt_2,NULL,&arc2,seg_2,alt_arc);
   if ( status < 0 ) return (status);
/*
*** Calculate the points for the output line (2D problem)
*/
  status= GE208(arc1.x_a,arc1.y_a,arc1.r_a,arc1.v1_a,arc1.v2_a,
                arc2.x_a,arc2.y_a,arc2.r_a,arc2.v1_a,arc2.v2_a,
                alt, &start, &end, &n_lin  );
  if ( status < 0 ) return(status);
/*
*** Transformate line points to the global system
*/
   status = GEtfpos_to_basic(&start, &csys_arcs, &start);
   if ( status < 0 ) return (status);
   status = GEtfpos_to_basic(&end  , &csys_arcs, &end  );
   if ( status < 0 ) return (status);

   goto create_lin;
/*
*** Calculate the points for the output line (2D problem)
*/
centers_ok:; /* Label: Center points checked and OK */
arcs_2d:;    /* Label: Input 2D circles */
  status= GE208(pa1->x_a,pa1->y_a,pa1->r_a,pa1->v1_a,pa1->v2_a,
                pa2->x_a,pa2->y_a,pa2->r_a,pa2->v1_a,pa2->v2_a,
                alt, &start, &end, &n_lin  );
  if ( status < 0 ) return(status);
/*
*** Create the output line
*/
create_lin:; /* Label: Line created with new circles  */

  status = GE200(&start,&end,plin);
      if( status < 0 ) return(erpush("GE2162","GE206"));

      return(0);
}

/********************************************************************/

/*!******************************************************************/
/*  File: geintpos.c                                                */
/*  ================                                                */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*  GEintersect_pos()  Compute coordinates of intersect position    */
/*  GEintersect_pv()   Compute parameter value of intersect pos     */
/*  GEintersect_npos() Compute the number of possible intersects    */
/*                                                                  */
/*  This file is part of the VARKON Geometry Library.               */
/*  URL:  http://varkon.sourceforge.net                             */
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

        DBstatus GEintersect_pos(
        DBAny    *pstr1,
        char     *pdat1,
        DBAny    *pstr2,
        char     *pdat2,
        DBTmat   *pc,
        short     intnr,
        DBVector *pvec)

/*      Main entry for calculation of intersect positions in R3.
 *
 *      In: 
 *          pstr1 = First entity
 *          pdat1 = Optional segment/patches
 *          pstr2 = Second entity
 *          pdat2 = Optional segments/patches
 *          pc    = Active coordinate system
 *          intnr = Requested solution +/-
 *
 *      Out: *pvec = R3 Coordinates
 *
 *      (C)microform ab 22/12/91 Helt omskriven, J. Kjellander 
 *
 *      6/9/95  Ytor, J. Kjellander
 *      1999-05-07 Rewritten, J.Kjellander
 *
 **************************************************************!*/

{
   DBetype typ1,typ2;
   DBfloat u1,u2;
   char    errbuf[80];

/*
***Compute the parametric u-values for the requested intersection.
*/
   if ( GEintersect_pv(pstr1,pdat1,pstr2,pdat2,pc,intnr,&u1,&u2) < 0 )
     {
     sprintf(errbuf,"%d",intnr);
     return(erpush("GE7012",errbuf));
     }
/*
***Map from GE-range (1-2) to MBS parametric range (0-1).
*/
     u1 -= 1.0;
     u2 -= 1.0;
/*
***Calculate corresponding coordinates. In 2D any of the two
***entities can be used. In 3D the first entity is normallu used.
***If the first entity is a plane however the second entity
***is used. In that case GEintersect_pv() returns the para-
***metric u-value in u1 and not in u2 !
*/
   typ1 = pstr1->hed_un.type;
   typ2 = pstr2->hed_un.type;

   if ( typ1 == CSYTYP  ||  typ1 == BPLTYP )
     return(GEposition(pstr2,pdat2,u1,(DBfloat)0.0,pvec));
/*
***If one ot the entities is a surface GEintersect_pv()
***returns the value in the surface UV-plane.
*/
   else if ( typ1 == SURTYP )
     return(GEposition(pstr1,pdat1,u1,u2,pvec));
   else if ( typ2 == SURTYP )
     return(GEposition(pstr2,pdat2,u1,u2,pvec));
/*
***In all other cases we use the first entity.
*/
   else return(GEposition(pstr1,pdat1,u1,(DBfloat)0.0,pvec));

   return(0);
}

/********************************************************************/
/********************************************************************/

        DBstatus GEintersect_pv(
        DBAny   *pstr1,
        char    *pdat1,
        DBAny   *pstr2,
        char    *pdat2,
        DBTmat  *pc,
        short    intnr,
        DBfloat *pu1,
        DBfloat *pu2)

/*      Computes the parameter values of an intersect.
 *
 *      In: 
 *          pstr1 = First entity
 *          pdat1 = Optional segment/patches
 *          pstr2 = Second entity
 *          pdat2 = Optional segments/patches
 *          pc    = Active coordinate system
 *          intnr = Requested solution +/-
 *
 *      Out: 
 *           *pu1 = Parametric value relative to first entity
 *           *pu2 = Parametric value relative to second entity
 *
 *      (C)microform ab 6/9/95 Helt omskriven, J. Kjellander 
 *
 *       1996-01-10 GE724 tillagd, Gunnar Liden
 *       1999-05-07 Rewritten, J.Kjellander
 *
 *****************************************************************!*/

  {
    short   status,noint;
    DBetype typ1,typ2;

/*
***uout1 and uout2 temporarily need to hold all the intersects
***and therefore must be dimensioned for the "worst case".
*/
    DBfloat uout1[GMMXSG*INTMAX];
    DBfloat uout2[GMMXSG*INTMAX];
/*
***What type of entities ?
*/
   typ1 = pstr1->hed_un.type;
   typ2 = pstr2->hed_un.type;
/*
***The requested intersect is ABS(intnr). The total number of
***intersects found is noint. If noint < ABS(intnr) or noint
***is zero the requested intersect does not exist. If intnr
***is negative, lines are extended to infinity and arcs are
***treated as full circles (0-360 degrees).
*/
   noint = intnr;
   if ( intnr < 0 ) intnr = -intnr;
/*
***Line - line.
*/
   if ( typ1 == LINTYP && typ2 == LINTYP)
     {
     if ( GE705(pstr1,pstr2,pc,&noint,uout1,uout2) < 0 )
       return(erpush("GE7083",""));
     uout1[0] = uout1[0] + 1.0;
     uout2[0] = uout2[0] + 1.0;
     }
/*
***Line/curve - surface. Result is always returned in uout1.
*/
   else if ( typ1 == LINTYP  &&  typ2 == SURTYP )
     {
     status = GE723((DBLine *)pstr1,(DBSurf *)pstr2,(DBPatch *)pdat2,
                                                      &noint,uout1,uout2);
     if ( status < 0 ) return(erpush("GE7112",""));
     }
   else if ( typ2 == LINTYP  &&  typ1 == SURTYP )
     {
     status = GE723((DBLine *)pstr2,(DBSurf *)pstr1,(DBPatch *)pdat1,
                                                      &noint,uout1,uout2);
     if ( status < 0 ) return(erpush("GE7112",""));
     }
   else if ( typ2 == CURTYP  &&  typ1 == SURTYP )
     {
     status = GE724((DBCurve *)pstr2,(GMSEG *)pdat2,
                    (DBSurf *)pstr1,(DBPatch *)pdat1,&noint,uout1,uout2);
     if ( status < 0 ) return(erpush("GE7412",""));
     }
   else if ( typ1 == CURTYP  &&  typ2 == SURTYP )
     {
     status = GE724((DBCurve *)pstr1,(GMSEG *)pdat1,
                    (DBSurf *)pstr2,(DBPatch *)pdat2,&noint,uout1,uout2);
     if ( status < 0 ) return(erpush("GE7412",""));
     }
   else if ( typ1 == SURTYP  ||  typ2 == SURTYP ) return(erpush("GE7422",""));
/*
***(Line, arc or curve) - (csy or bplane). Result is returned
***in uout1.
*/
   else if ( typ1 == CSYTYP  ||  typ1 == BPLTYP )
     {
     status = GE715(pstr2,(GMSEG *)pdat2,pstr1,&noint,uout1);
     if ( status < 0 ) return(status);
     uout2[0] = 1.0;
     }
   else if ( typ2 == CSYTYP  ||  typ2 == BPLTYP )
     {
     status = GE715(pstr1,(GMSEG *)pdat1,pstr2,&noint,uout1);
     if ( status < 0 ) return(status);
     uout2[0] = 1.0;
     }
/*
***Line - (arc or curve).
*/
   else if ( typ1 == LINTYP || typ2 == LINTYP )
     { 
     if ( GE710(pstr1,(GMSEG *)pdat1,pstr2,(GMSEG *)pdat2,
                                            pc,&noint,uout1,uout2) < 0 )
         return(erpush("GE7092",""));
     }
/*
***(Arc or curve ) - (arc or curve).
*/
   else
     {
     if ( GE720(pstr1,(GMSEG *)pdat1,pstr2,(GMSEG *)pdat2,
                                            pc,&noint,uout1,uout2) < 0 )
        return(erpush("GE7102",""));
     }
/*
***Select solution. If no solution exists return -1.
*/
   if ( noint < intnr )
     {
    *pu1 = 0.0;
    *pu2 = 0.0;
     return(-1);
     }
   else
     {
    *pu1 = uout1[intnr-1];
    *pu2 = uout2[intnr-1];
     return(0);
     }
 } 

/********************************************************************/
/*!******************************************************************/

      DBstatus GEintersect_npos(
      DBAny   *pstr1,
      char    *pdat1,
      DBAny   *pstr2,
      char    *pdat2,
      DBTmat  *pc,
      short    extend,
      short   *numint)

/*      Check the number of possible intersects between
 *      a wireframe entitiy and a wireframe/surface entity.
 *
 *      In:  pstr1  = First entity
 *           pdat1  = Optional segment/patchdata
 *           pstr2  = Second entity
 *           pdat2  = Optional segment/patchdata
 *           pc     = Active coordinate system
 *           extend = -1 if extrapolation requested
 *
 *      Out: *numint = The number of intersects >= 0.
 *
 *      (C)microform ab 22/1/92 J. Kjellander
 *
 *      1996-08-19 Skärning kurva/yta, Gunnar Liden
 *      1999-05-17 Rewritten, J.Kjellander
 *      1999-12-18 sur164->varkon_sur_curint  surint.h erased  Liden
 *
 ******************************************************************!*/

  {
   short    status,noint;
   DBetype  typ1,typ2;
   DBfloat  uout1[GMMXSG*INTMAX];
   DBfloat  uout2[GMMXSG*INTMAX];
   DBint    ocase,acc,sol,nsol;
   DBVector start,r3_pt,uv_pt,all_uv[25],all_xyz[25];
   DBfloat  all_u[SMAX];    /* SMAX, See surint.h */
   DBfloat  uvalue;

/*
***What kind of entities ?
*/
   typ1 = pstr1->hed_un.type;
   typ2 = pstr2->hed_un.type;
/*
***Number of intersects is noint.
*/
   noint = extend;
/*
***Init for curve/surface  1996-08-19 Gunnar Liden
***No startpoint.
*/
   start.x_gm =  0.0;
   start.y_gm =  0.0;
   start.z_gm = -1.0;
/*
***Sort (ocase) relative to curve start.
***acc = all patches. 
*/
   ocase = 1 + 1000;
   acc   = 1;
   sol   = 1;
   nsol  = 0;
/*
***Line - line.
*/
   if ( typ1 == LINTYP && typ2 == LINTYP)
     {
     if ( GE705(pstr1,pstr2,pc,&noint,uout1,uout2) < 0 )
       {
       erinit();
       *numint = 0;
       return(0);
       }
     }
/*
***Curve - surface.
*/
   else if ( typ1 == CURTYP  &&  typ2 == SURTYP )
     {
     status = varkon_sur_curint(
                    (DBSurf*)pstr2,(GMPAT *)pdat2,
                    (DBCurve*)pstr1,(GMSEG *)pdat1,
                    &start,ocase,acc,sol,&nsol,&r3_pt,&uv_pt,
                    &uvalue,all_u,all_uv,all_xyz);
     if ( status < 0 ) return(status);
     *numint = (short)nsol;
     return(0);
     }
   else if ( typ1 == SURTYP  &&  typ2 == CURTYP )
     {
     status = varkon_sur_curint(
                    (DBSurf*)pstr1,(GMPAT *)pdat1,
                    (DBCurve*)pstr2,(GMSEG *)pdat2,
                    &start,ocase,acc,sol,&nsol,&r3_pt,&uv_pt,
                    &uvalue,all_u,all_uv,all_xyz);
     if ( status < 0 ) return(status);
     *numint = (short)nsol;
     return(0);
     }
/*
***Line - surface.
*/
   else if ( typ1 == LINTYP  &&  typ2 == SURTYP )
     {
     status = GE723((DBLine *)pstr1,(DBSurf *)pstr2,(DBPatch *)pdat2,
                                                   &noint,uout1,uout2);
     if ( status < 0 )
       {
       erinit();
       *numint = 0;
       return(0);
       }
     }
   else if ( typ2 == LINTYP  &&  typ1 == SURTYP )
     {
     status = GE723((DBLine *)pstr2,(DBSurf *)pstr1,(DBPatch *)pdat1,
                                                   &noint,uout1,uout2);
     if ( status < 0 )
       {
       erinit();
       *numint = 0;
       return(0);
       }
     }
   else if ( typ1 == SURTYP  ||  typ2 == SURTYP )
     {
     return(erpush("GE7112",""));
     }
/*
***(Line, arc or curve) - (csy or bplane)
*/
   else if ( typ1 == CSYTYP  ||  typ1 == BPLTYP )
     {
     if ( GE715(pstr2,(GMSEG *)pdat2,pstr1,&noint,uout1) < 0 )
       {
       erinit();
       *numint = 0;
       return(0);
       }
     }
   else if ( typ2 == CSYTYP  ||  typ2 == BPLTYP )
     {
     if ( GE715(pstr1,(GMSEG *)pdat1,pstr2,&noint,uout1) < 0 )
       {
       erinit();
       *numint = 0;
       return(0);
       }
     }
/*
***Line - (arc or curve).
*/
   else if ( typ1 == LINTYP || typ2 == LINTYP )
     { 
     if ( GE710(pstr1,(GMSEG *)pdat1,pstr2,(GMSEG *)pdat2,pc,
                                            &noint,uout1,uout2) < 0 )
       {
       erinit();
       *numint = 0;
       return(0);
       }
     }
/*
***(Arc or curve ) - (arc or curve).
*/
   else
     {
     if ( GE720(pstr1,(GMSEG *)pdat1,pstr2,(GMSEG *)pdat2,pc,
                                            &noint,uout1,uout2) < 0 )
       {
       erinit();
       *numint = 0;
       return(0);
       }
     }
/*
***End.
*/
   *numint = noint;

    return(0);
 } 

/********************************************************************/

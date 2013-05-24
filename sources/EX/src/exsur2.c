/*!******************************************************************
*
*    exsur2.c
*    ========
*
*    EXsswp();       Create SUR_SWEEP
*    EXscyl();       Create SUR_CYL
*    EXsrot();       Create SUR_ROT
*    EXsrul();       Create SUR_RULED 
*
*    This file is part of the VARKON Execute  Library.
*    URL:  http://varkon.sourceforge.net
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
*
*
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../include/EX.h"

#ifdef DEBUG
#include "../../IG/include/debug.h"
#endif

extern DBTmat *lsyspk;     /* Active coord. system                  */
extern DBTmat  lklsyi;     /* Active coord. system, inverted        */

/* varkon_cur_fromarc       * Convert a circle to curve segment(s)  */
/* varkon_cur_fromlin       * Convert a line to a curve segment     */
/* varkon_sur_rot           * Create a rotational surface SUR_ROT   */
/* varkon_sur_ruled         * Create a ruled      surface SUR_RULED */
/* varkon_sur_sweep         * Create a swept      surface SUR_SWEEP */
/* varkon_sur_cylind        * Create a cylindric  surface SUR_CYL   */



/*!******************************************************/

        short     EXsswp(
        DBId     *sur_id,
        DBId     *cur_id,
        DBId     *spine_id,
        DBint     reverse,
        DBVector *p_yaxis,
        V2NAPA   *pnp)

/*      Creates SUR_SWEEP
 *
 *      In:  sur_id    => Output surface identity
 *           spine_id  => Input sweeping curve 
 *           cur_id    => Input section  curve 
 *           reverse   => Reverse  case                            
 *                        Eq.  1: Input curve will be V= 0
 *                        Eq. -1: Input curve will be V= 1
 *                        Eq.  2: Input curve will be reversed
 *                        Eq.  3: Input curve will be reversed and
 *                                input curve will be V= 1
 *           p_yaxis   => Defines Y axis in local coordinate system
 *                        for section curve. ? axis is the tangent 
 *                        to the spine.
 *           pnp       => Attribute for sur_id 
 *
 *      Ut:  Inget.
 *
 *      FV:       0 => Ok.
 *
 *      1997-05-02 Gunnar Liden 
 *      1997-05-05 spine_id<->cur_id Gunnar Liden 
 *      1997-05-26 Debug             Gunnar Liden
 *
 ******************************************************!*/

{
  bool     alloc1,alloc2;
  short    status;
  DBptr    la;
  DBetype  typ;
  char     errbuf[V3STRLEN+1];
  DBLine    lin;
  DBArc    arc;
  DBSeg    arcseg1[4];
  DBSeg    arcseg2[4];
  DBCurve    spine;        /* Spine curve                             */
  DBSeg   *p_spineseg;   /* Spine segments                    (ptr) */
  DBCurve    cur;          /* Section curve                           */
  DBSeg   *p_seg;        /* Section curve segments            (ptr) */
  DBSurf   sur;          /* Sweep    surface                        */
  DBint    s_case;       /* Sweeping case   Not yet used            */
  DBPatch *p_pat;        /* Sweep    surface patches          (ptr) */

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),"exe23*EXsswp Enter SUR_SWEEP interface fctn\n");
fflush(dbgfil(EXEPAC)); 
}
#endif

/*
***Initializations
*/
    status = 0;

    alloc1 = alloc2 = FALSE;

    p_spineseg = NULL;
    p_seg      = NULL;
    p_pat      = NULL;


/*
*** Transformation of the yaxis to the basic system
*/

   if ( (p_yaxis->x_gm != 0.0  ||
         p_yaxis->y_gm != 0.0  ||
         p_yaxis->z_gm != 0.0) &&
        lsyspk != NULL ) GEtfvec_to_local(p_yaxis,&lklsyi,p_yaxis);




/*
*** Get geometry data for the spine line/curve   
*/
    if ( DBget_pointer('I',spine_id,&la,&typ) < 0 ) return(erpush("EX1402",""));

    switch ( typ )
      {
      case LINTYP:
      DBread_line(&lin, la);
      DBread_line(&lin, la);
      varkon_cur_fromlin(&lin,&spine,arcseg1);
      p_spineseg = arcseg1;
      break;
      
      case ARCTYP:
      DBread_arc(&arc, arcseg1, la);
      varkon_cur_fromarc(&arc,&spine,arcseg1);
      p_spineseg = arcseg1;
      break;

      case CURTYP:
      DBread_curve(&spine,NULL,&p_spineseg,la);
      alloc1 = TRUE;
      break;

      default:
      IGidst(spine_id,errbuf);
      return(erpush("EX1412",errbuf));
      }

/*
*** Get geometry data for the section curve    
*/
    if ( DBget_pointer('I',cur_id,&la,&typ) < 0 ) return(erpush("EX1402",""));

    switch ( typ )
      {
      case LINTYP:
      DBread_line(&lin, la);
      varkon_cur_fromlin(&lin,&cur,arcseg2);
      p_seg = arcseg2;
      break;
      
      case ARCTYP:
      DBread_arc(&arc, arcseg2, la);
      varkon_cur_fromarc(&arc,&cur,arcseg2);
      p_seg = arcseg2;
      break;

      case CURTYP:
      DBread_curve(&cur,NULL,&p_seg,la);
      alloc2 = TRUE;
      break;

      default:
      IGidst(cur_id,errbuf);
      return(erpush("EX1412",errbuf));
      }
/*
*** Create the surface
*/
    s_case = I_UNDEF;

    status = varkon_sur_sweep( &spine, p_spineseg, &cur,p_seg, 
                      p_yaxis, s_case, reverse,
                      &sur,&p_pat); 
    if ( status < 0 )
        {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),"exe23*EXsswp varkon_sur_cylind failed \n");
fflush(dbgfil(EXEPAC));
}
#endif
        if  (  p_pat == NULL )
          {
          goto  err2;
          }
         else
          {
          goto  err1;
          }
        }

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),"exe23*EXsswp Store surface in DB\n");
fflush(dbgfil(EXEPAC)); 
}
#endif

/*
***Lagra i DB och rita.
*/
    status = EXesur(sur_id,&sur,p_pat,NULL,NULL,pnp);
/*
***Lämna tillbaks dynamiskt allokerat minne.
*/
err1:;
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),"exe23*EXsswp Deallocate memory\n");
fflush(dbgfil(EXEPAC)); 
}
#endif

    if ( p_pat != NULL ) DBfree_patches(&sur,p_pat);

err2:
   if ( alloc1) DBfree_segments(p_spineseg);
   if ( alloc2) DBfree_segments(p_seg);
/*
***Slut.
*/

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
{
fprintf(dbgfil(EXEPAC),"exe23*EXsswp Exit status= %d\n",(int)status );
fflush(dbgfil(EXEPAC)); 
}
#endif



return(status);

}


/*!******************************************************/

        short EXscyl(
        DBId     *sur_id,
        DBId     *cur_id,
        DBfloat   dist,
        DBint     reverse,
        DBVector *p_dir,
        V2NAPA   *pnp)

/*      Creates SUR_CYL
 *
 *      In:  sur_id  => Output surface identity
 *           cur_id  => Input section curve 
 *           dist    => Sweeping distance
 *           reverse => Sweeping case        
 *                      Eq.  1: Input curve will be V= 0
 *                      Eq. -1: Input curve will be V= 1
 *                      Eq.  2: Input curve will be reversed
 *                      Eq.  3: Input curve will be reversed and
 *                              input curve will be V= 1
 *           p_dir   => Sweeping direction            (ptr)
 *                      *p_dir= (0,0,0) ==> Curve plane will be used
 *           pnp     => Attribute for sur_id 
 *
 *      Ut:  Inget.
 *
 *      FV:       0 => Ok.
 *
 *      1997-04-27 Gunnar Liden 
 *      1997-04-29 Reverse      Gunnar Liden 
 *      1997-04-30 GEtfvec_to_local       Gunnar Liden 
 *
 ******************************************************!*/

  {
  bool     allocs;
  short    status;
  DBptr    la;
  DBetype  typ;
  char     errbuf[V3STRLEN+1];
  DBLine    lin;
  DBArc    arc;
  DBSeg    arcseg[4];
  DBCurve    cur;          /* Section  curve                          */
  DBSeg   *p_seg;        /* Section  curve segments           (ptr) */
#ifdef  TILLSVIDARE
   DBint   s_type;       /* Surface output type                     */
                         /* Eq.  1: SUR_SWEEP, spine= straight line */
                         /* Eq.  2: SUR_RULED, profile transformed  */
#endif
  DBSurf   sur;          /* Cylinder surface                        */
  DBPatch *p_pat;        /* Cylinder surface patches          (ptr) */

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"exe23*EXscyl Enter dist= %f reverse %d\n",
              dist, (int)reverse );
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"exe23*EXscyl p_dir %f %f %f\n", 
        p_dir->x_gm, p_dir->y_gm, p_dir->z_gm );
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

/*
***Initializations
*/
    status = 0;

/*
***Ännu är inget minne för kurvsegment allokerat.
*/

    allocs = FALSE;


/*
***Get geometry for the section curve
*/
    if ( DBget_pointer('I',cur_id,&la,&typ) < 0 ) return(erpush("EX1402",""));

    switch ( typ )
      {
      case LINTYP:
      DBread_line(&lin, la);
      varkon_cur_fromlin(&lin,&cur,arcseg);
      p_seg = arcseg;
      break;
      
      case ARCTYP:
      DBread_arc(&arc, arcseg, la);
      varkon_cur_fromarc(&arc,&cur,arcseg);
      p_seg = arcseg;
      break;

      case CURTYP:
      DBread_curve(&cur,NULL,&p_seg,la);
      allocs = TRUE;
      break;

      default:
      IGidst(cur_id,errbuf);
      return(erpush("EX1412",errbuf));
      }
/*
*** Transformation of dir to the basic system
*/
   if ( (p_dir->x_gm != 0.0  ||
         p_dir->y_gm != 0.0  ||
         p_dir->z_gm != 0.0) &&
        lsyspk != NULL ) GEtfvec_to_local(p_dir,&lklsyi,p_dir);
/*
*** Create the surface
*/


  status = varkon_sur_cylind ( &cur,p_seg, dist, p_dir, reverse, 
                    &sur,&p_pat); 
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"exe23*EXscyl varkon_sur_cylind failed \n");
  fflush(dbgfil(EXEPAC));
  }
#endif
      if  (  p_pat == NULL )
        {
        goto  err2;
        }
       else
        {
        goto  err1;
        }
      }
/*
***Lagra i DB och rita.
*/
    status = EXesur(sur_id,&sur,p_pat,NULL,NULL,pnp);
/*
***Lämna tillbaks dynamiskt allokerat minne.
*/
err1:;
    if ( p_pat != NULL ) DBfree_patches(&sur,p_pat);

err2:
   if ( allocs) DBfree_segments(p_seg);
/*
***Slut.
*/

#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),"exe23*EXscyl Exit status= %d\n",(int)status );
  fflush(dbgfil(EXEPAC)); /* To file from buffer      */
  }
#endif

    return(status);
  }
  
  
/********************************************************/
/*!******************************************************/

       short     EXsrot(
       DBId     *id,
       DBId     *rid,
       DBVector *p1,
       DBVector *p2,
       DBfloat   v1,
       DBfloat   v2,
       DBint     dir_in,
       DBint     reverse,
       V2NAPA   *pnp)

/*      Cretae SUR_ROT().
 *
 *      In:  id   => Pekare till ytans identitet.
 *           rid  => Pekare till ID för refererad kurva.
 *           p1   => 1:a positionen.
 *           p2   => 2:a positionen.
 *           v1   => Startvinkel.
 *           v2   => Slutvinkel.
 *           dir  => Rotationsriktning +/- 1
 *           pnp  => Pekare till attribut.
 *
 *
 *      Return:   0 => Ok.
 *           EX1402 => Refererad storhet finns ej.
 *           EX1412 => Refererad storhet har fel typ.
 *
 *      (C)microform ab 1997
 *
 *      1997-03-09  sur_type, dir_in   Gunnar Liden
 *      1997-03-21  sur716             Gunnar Liden
 *      1998-03-20  Initializations    Gunnar Liden
 *
 ******************************************************!*/

  {
    short    status;
    DBptr    la;
    DBetype  typ;
    char     errbuf[V3STRLEN+1];
    DBCurve  rotcur;
    DBSeg   *rotseg;
    DBSeg    cirlin[4];
    DBLine   lin;       /* For a rotation curve which is a line      */
    DBArc    arc;       /* For a rotation curve which is a circle    */
    DBint    f_alloc;   /* =0: No allocation  =1: Curve allocation   */
    DBSurf   rotsur;
    DBPatch *rotpat;
    DBint    sur_type;  /* =1: LFT_SUR p_flag=2 =2: LFT_SUR p_flag=3 */
    DBint    dir;       /* =1: positive =-1 Negative                 */


/*
***Get the curve data.
*/
    if ( DBget_pointer('I',rid,&la,&typ) < 0 ) return(erpush("EX1402",""));

    if      ( typ == CURTYP )
      {
      f_alloc = 1;
      DBread_curve(&rotcur,NULL,&rotseg,la);
      }
    else if ( typ == LINTYP )
      {
      f_alloc = 0;
      DBread_line(&lin, la);
      status = varkon_cur_fromlin(&lin, &rotcur, &cirlin[0] );
      rotseg = &cirlin[0];
      }
    else if ( typ == ARCTYP )
      {
      f_alloc = 0;
      DBread_arc(&arc, cirlin, la);
      status = varkon_cur_fromarc(&arc, &rotcur, &cirlin[0] );
      rotseg = &cirlin[0];
      }
    else
      {
      IGidst(rid,errbuf);
      return(erpush("EX1412",errbuf));
      }
/*
***Transform p1 and p2 to basic.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(p1,&lklsyi,p1);
    if ( lsyspk != NULL ) GEtfpos_to_local(p2,&lklsyi,p2);
/*
***Create the surface.
***The SUR_ROT function should have a parameter which makes it
***possible to create an ordinary (P-value) LFT_SUR surface.
*/
    if      ( dir_in ==  1001 )
      {
      dir      =  1;
      sur_type =  2;
      }
    else if ( dir_in == -1001 )
      {
      dir      = -1;
      sur_type =  2;
      }
    else
      {
      dir      = dir_in;
      sur_type = 1;
      }

    status = varkon_sur_rot(&rotcur,rotseg,dir,
              p1,p2,v1,v2,sur_type,&rotsur,&rotpat);

    if       ( status < 0  &&  rotpat == NULL ) goto err2;
    else if ( status < 0 ) goto err1;
/*
***Save in DB and display.
*/
   status = EXesur(id,&rotsur,rotpat,NULL,NULL,pnp);
/*
***Release mallocated C memory.
*/
err1:
    DBfree_patches(&rotsur,rotpat);
err2:
    if ( f_alloc == 1 ) DBfree_segments(rotseg);
/*
***The end.
*/
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short EXsrul(
       DBId    *id,
       DBId    *ref1,
       DBId    *ref2,
       DBint    reverse,
       DBint    par,
       V2NAPA  *pnp)

/*      Skapar SUR_RULED.
 *
 *      In:  id      => Pekare till ytans identitet.
 *           ref1    => Pekare till Kurva 1.
 *           ref2    => Pekare till Kurva 2.
 *           reverse => Reverse of parameter directions
 *           par     => Type of parameterization         
 *           pnp     => Pekare till attribut.
 *
 *      Ut:  Inget.
 *
 *      FV:       0 => Ok.
 *
 *      (C)microform ab 1997-04-21       J.Kjellander
 *       1997-04-29 reverse, ...         G. Liden
 *       1997-05-11 r_case numbers       G. Liden
 *       1998-10-10 Developable added    G. Liden
 *
 ******************************************************!*/

  {
  bool     alloc1,alloc2;
  short    status;
  DBptr    la;
  DBetype  typ;
  char     errbuf[V3STRLEN+1];
  DBLine    lin;
  DBArc    arc;
  DBSeg    arcseg1[4];
  DBSeg    arcseg2[4];
  DBCurve  cur1;         /* Boundary curve 1                        */
  DBSeg   *p_seg1;       /* Boundary curve 1 segments         (ptr) */
  DBCurve  cur2;         /* Boundary curve 2                        */
  DBSeg   *p_seg2;       /* Boundary curve 2 segments         (ptr) */
  DBint    r_case;       /* Ruled surface case:                     */
                         /* Eq. 1: Arclength                        */
                         /*        Output surface type LFT_SUR      */
                         /* Eq. 2: Use input curve segments         */
                         /*        Output surface type RAT_SUR or   */
                         /*        output surface type CUB_SUR      */
                         /* Eq. 3: As 1 but add curve segments      */
                         /*        Output surface type RAT_SUR or   */
                         /*        output surface type CUB_SUR      */
                         /* Eq. 4: Developable                      */
                         /*        Output surface type LFT_SUR      */
                         /* Eq. 5: Input curves and a spine         */
                         /*        Output surface type LFT_SUR      */
  DBfloat  idpoint;      /* Identical points tolerance              */
  DBfloat  idangle;      /* Identical angle  tolerance              */
  DBCurve *p_spine;      /* Spine curve                       (ptr) */
  DBSeg   *p_spineseg;   /* Spine segments                    (ptr) */
  DBSurf   sur;          /* Ruled surface                           */
  DBPatch *p_pat;        /* Ruled surface patches             (ptr) */

/*
***Ännu är inget minne för kurvsegment allokerat.
*/
    alloc1 = alloc2 = FALSE;
/*
***Hämta geometri-data för första "kurvan".
*/
    if ( DBget_pointer('I',ref1,&la,&typ) < 0 ) return(erpush("EX1402",""));

    switch ( typ )
      {
      case LINTYP:
      DBread_line(&lin, la);
      varkon_cur_fromlin(&lin,&cur1,arcseg1);
      p_seg1 = arcseg1;
      break;
      
      case ARCTYP:
      DBread_arc(&arc, arcseg1, la);
      varkon_cur_fromarc(&arc,&cur1,arcseg1);
      p_seg1 = arcseg1;
      break;

      case CURTYP:
      DBread_curve(&cur1,NULL,&p_seg1,la);
      alloc1 = TRUE;
      break;

      default:
      IGidst(ref1,errbuf);
      return(erpush("EX1412",errbuf));
      }
/*
***Hämta geometri-data för andra "kurvan".
*/
    if ( DBget_pointer('I',ref2,&la,&typ) < 0 ) return(erpush("EX1402",""));

    switch ( typ )
      {
      case LINTYP:
      DBread_line(&lin, la);
      varkon_cur_fromlin(&lin,&cur2,arcseg2);
      p_seg2 = arcseg2;
      break;
      
      case ARCTYP:
      DBread_arc(&arc, arcseg2, la);
      varkon_cur_fromarc(&arc,&cur2,arcseg2);
      p_seg2 = arcseg2;
      break;

      case CURTYP:
      DBread_curve(&cur2,NULL,&p_seg2,la);
      alloc2 = TRUE;
      break;

      default:
      IGidst(ref2,errbuf);
      return(erpush("EX1412",errbuf));
      }
/*
***Skapa ytan.
*/

  p_spine    = NULL;
  p_spineseg = NULL;
  idpoint    = F_UNDEF;
  idangle    = F_UNDEF;
  r_case     = I_UNDEF;
  if        ( par == 1 ) 
    {
    r_case     = 1;
    }
  else if   ( par == 2 ) 
    {
    r_case     = 4;
    }
  else
     {
     erinit();
     sprintf(errbuf,"%d%%exe23", (int)par);
     return(erpush("SU8123",errbuf));
     }

   status = varkon_sur_ruled ( &cur1,p_seg1, &cur2,p_seg2,r_case,
               p_spine,p_spineseg,reverse, idpoint,idangle, 
                    &sur,&p_pat); 
  if ( status < 0 )
      {
#ifdef DEBUG
if ( dbglev(EXEPAC) == 1 )
  {
  fprintf(dbgfil(EXEPAC),
        "exe23*EXsrul varkon_sur_ruled failed reverse= %d\n",
                          reverse);
  fflush(dbgfil(EXEPAC)); 
  }
#endif
      if  (  p_pat == NULL )
        {
        goto  err2;
        }
       else
        {
        goto  err1;
        }
      }
/*
***Lagra i DB och rita.
*/
    status = EXesur(id,&sur,p_pat,NULL,NULL,pnp);
/*
***Lämna tillbaks dynamiskt allokerat minne.
*/
err1:;
    if ( p_pat != NULL ) DBfree_patches(&sur,p_pat);

err2:
   if ( alloc1 ) DBfree_segments(p_seg1);
   if ( alloc2 ) DBfree_segments(p_seg2);
/*
***Slut.
*/
    return(status);
  }
  
/********************************************************/


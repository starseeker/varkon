/*!*******************************************************
*
*    excsy.c
*    =======
*
*    EXecsy();    Create coordinatesystem
*    EXcs3p();    Create CSYS_3P
*    EXcs1p();    Create CSYS_1P
*    EXcsud();    Create CSYS_USRDEF
*    EXpcatm();   Create a tf matrix by array of points
*
*    EXmoba();    Interface routine for MODE_BASIC
*    EXmogl();    Interface routine for MODE_GLOBAL
*    EXmolo();    Interface routine for MODE_LOCAL
*    EXmlla();    Activates coordinatsystem by DBptr
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
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../../WP/include/WP.h"

#include "../include/EX.h"

extern char    actcnm[];
extern char    modstk[];
extern char   *mspek;
extern DBTmat  modsys;
extern DBTmat *msyspk;
extern DBptr   msysla;

/*
***Current coordinate system.
*/
DBTmat  lklsys;      /* Active (local) system matrix. */
DBTmat  lklsyi;      /* Inverted version of lklsys for speed */
DBTmat *lsyspk;      /* C-ptr to active matrix. NULL => BASIC */
DBptr   lsysla;      /* DB pointer to active local system. */
                     /* lsysla = msysla => Current module system is active */
                     /* DBNULL => BASIC */

/*!******************************************************/

       short EXecsy(
       DBId    *id,
       DBCsys  *crdpek,
       DBTmat  *pmat,
       V2NAPA  *pnp)

/*      Lagrar i GM och ritar ett koordinatsystem.
 *
 *      In: id     => Pekare till identitet.
 *          crdpek => Pekare till GM-stuktur.
 *          pmat   => Pekare plan-matris.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          EX1392 => Kan ej lagra koordinatsystem
 *
 *      (C)microform ab 15/11/85 B. Doverud
 *
 *      15/10/86 SAVE, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      1/2/95   Multifönster, J. Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;

/*
***Fyll i namnparameterdata.
*/
    crdpek->hed_pl.blank = pnp->blank;
    crdpek->hed_pl.pen   = pnp->pen;
    crdpek->hed_pl.level = pnp->level;
    crdpek->pcsy_pl      = lsysla;
/*
***Lagra i GM.
*/
    if ( pnp->save )
      {
      crdpek->hed_pl.hit = pnp->hit;
      if ( DBinsert_csys(crdpek,pmat,id,&la) < 0 ) 
              return(erpush("EX1392",""));
      }
    else
      {
      crdpek->hed_pl.hit = 0;
      }
/*
***Rita.
*/
    WPdrcs(crdpek,la,GWIN_ALL);

    return(0);

  }
  
/********************************************************/
/*!******************************************************/

       short EXcs3p(
       DBId     *id,
       char     *str,
       DBVector *p1,
       DBVector *p2,
       DBVector *p3,
       V2NAPA   *pnp)

/*      Skapa koordinatsystem med 2 eller 3 positioner.
 *
 *      In: id     => Pekare till identitet.
 *          str    => Pekare till namnsträng.
 *          p1     => Pekare till 1:a punkten.
 *          p2     => Pekare till 2:a punkten.
 *          p3     => Pekare till 3:e punkten eller NULL.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      Felkod: EX1382 = Can't calculate csys data
 *
 *      (C)microform ab 9/2/85 J.Kjellander
 *
 *      14/10/85   Header-data. J.Kjellander
 *      20/11/85   Anrop till EXecsy. B.Doverud
 *      27/12/86   hit, J.Kjellander
 *      1996-07-09 Y-axel optionell, J.Kjellander
 *
 ******************************************************!*/

  {
    DBCsys   csy;
    DBTmat  pmat;

/*
***Transformera positionerna till basic. 
*/
    if ( lsyspk != NULL )
      {
      GEtfpos_to_local(p1,&lklsyi,p1);
      GEtfpos_to_local(p2,&lklsyi,p2);
      if ( p3 != NULL ) GEtfpos_to_local(p3,&lklsyi,p3);
      }
/*
***Skapa matrisen.
*/
    if ( GEmktf_2p(p1,p2,p3,&pmat) < 0 ) return(erpush("EX1382",""));
/*
***Fyll i namnet.
*/
    *(str+JNLGTH) = '\0';
    strcpy(csy.name_pl,str);
/*
***Lagra i GM och rita.
*/
    return ( EXecsy(id,&csy,&pmat,pnp)); 
  }
  
/********************************************************/
/*!******************************************************/

       short EXcs1p(
       DBId     *id,
       char     *str,
       DBVector *p,
       DBfloat   v1,
       DBfloat   v2,
       DBfloat   v3,
       V2NAPA   *pnp)

/*      Skapa koordinatsystem med 1 position och vinklar.
 *
 *      In: id     => Pekare till identitet.
 *          str    => Pekare till namnsträng.
 *          p      => Pekare till origo.
 *          v1     => Vridning runt X.
 *          v2     => Vridning runt Y.
 *          v3     => Vridning runt Z.
 *          pnp    => Pekare till namnparameterblock.
 *
 *      Ut: Inget.
 *
 *      FV:      0 => Ok.
 *          EX1382 => Kan ej beräkna koordinatsystem-data
 *
 *      (C)microform ab 30/9/87 J. Kjellander
 *
 ******************************************************!*/

  {
    DBCsys   csy;
    DBTmat  pmat;

/*
***Skapa matrisen.
*/
    if ( GEmktf_1p(p,v1,v2,v3,lsyspk,&pmat) < 0 )
              return(erpush("EX1382",""));
/*
***Fyll i namnet.
*/
    *(str+JNLGTH) = '\0';
    strcpy(csy.name_pl,str);
/*
***Lagra i GM och rita.
*/
    return ( EXecsy(id,&csy,&pmat,pnp)); 
  }
  
/********************************************************/
/*!******************************************************/

       short EXcsud(
       DBId     *id,
       char     *str,
       DBTmat   *tmat,
       V2NAPA   *pnp)

/*      Create a user defined coordinate system.
 *
 *      In: id     => Pointer to identity. 
 *          str    => Pointer to name string.
 *          tmat   => Pointer to 4x4 matrix.
 *          pnp    => pointer to attributes.
 *
 *      Out: None.
 *
 *      Return:     0 => Ok.
 *             EX1382 => Can't calculate csys data
 *
 *      (C) Örebo university 26/05/2008 M. Rahayem
 *
 ********************************************************/

  {
    DBVector vx,vy,vz;
    DBTmat   tmati;
    DBTmat   tcsud;
    DBTmat   tmpmat;
    DBCsys   csy;

/*
***Extract X-axis vector from the given 4x4 matrix.
*/
    vx.x_gm = tmat->g11;
    vx.y_gm = tmat->g12;
    vx.z_gm = tmat->g13;
/*
***Extract y-axis vector from the given 4x4 matrix.
*/
    vy.x_gm = tmat->g21;
    vy.y_gm = tmat->g22;
    vy.z_gm = tmat->g23;
/*
***Extract z-axis vector from the given 4x4 matrix.
*/
    vz.x_gm = tmat->g31;
    vz.y_gm = tmat->g32;
    vz.z_gm = tmat->g33;
/*
***Normalisation and check of axes vector lengths.
*/
   if ( GEnormalise_vector3D(&vx,&vx) < 0 )
     return(erpush("EX4332","X"));         /* EX4332 The length of X axis vector is Zero */

   if ( GEnormalise_vector3D(&vy,&vy) < 0 )
     return(erpush("EX4332","Y"));         /* EX4332 The length of Y axis vector is Zero */

   if ( GEnormalise_vector3D(&vz,&vz) < 0 )
     return(erpush("EX4332","Z"));         /* EX4332 The length of Y axis vector is Zero */
/*
***Check if x and y axes are perpendicular to each other.
*/
   if ( GEscalar_product3D(&vx,&vy) > COMPTOL )
     return(erpush("EX4342","X%Y"));       /* EX4332 X&Y NOT Perp. */
/*
***Check if x and z axes are perpendicular to each other.
*/
   if ( GEscalar_product3D(&vx,&vz) > COMPTOL )
     return(erpush("EX4342","X%Z"));       /* EX4332 X&Z NOT Perp. */ 
/*
***Check if y and z axes are perpendicular to each other.
*/
   if ( GEscalar_product3D(&vy,&vz) > COMPTOL )
     return(erpush("EX4342","Y%Z"));       /* EX4332 Z&Y NOT Perp. */
/*
***Create user defined coordinate system matrix.
*/
   if ( lsyspk != NULL )
     {
     GEtform_inv(tmat,&tmati); 
     GEtform_mult(&tmati,&lklsys,&tmpmat);
     V3MOME(&tmpmat,&tcsud,sizeof(DBTmat));
     }
   else
     {
     GEtform_inv(tmat,&tmati);
     V3MOME(&tmati,&tcsud,sizeof(DBTmat));
     }
/*
***Fill in the name.
*/
    *(str+JNLGTH) = '\0';
    strncpy(csy.name_pl,str,JNLGTH);
/*
***Store csys_usrdef 4x4 matrix into DB and visualize.
*/
    return(EXecsy(id,&csy,&tcsud,pnp));
  }
  
/********************************************************/
/*!******************************************************/

       short EXpcatm(
       DBVector *ppts,
       DBint     npoi,
       DBTmat   *tmat)

/*      Create a transformation matrix by array of points.
 *
 *      In: npoi   => Number of points 
 *          ppts   => Pointer to points.
 *          tmat   => Pointer to the created 4x4 matrix.
 *
 *      Out: nothing.
 *
 *
 *     (C) Örebo university 26/05/2008 M. Rahayem
 *
 ******************************************************!*/

  {
    short    status;
    DBfloat  varxy, varxz, varyz;
    DBVector eigenvalues;
    DBTmat   tmat_inv;

/*
***Create the matrix
*/
    status = GEmktf_pca(ppts,npoi,&eigenvalues,&tmat_inv);
    GEtform_inv(&tmat_inv,tmat);
/*
***Calculate the variance by check the differences between the calculated eigen values.
*/
    varxy = fabs( eigenvalues.x_gm) - fabs( eigenvalues.y_gm );
    varxz = fabs( eigenvalues.x_gm) - fabs( eigenvalues.z_gm );
    varyz = fabs( eigenvalues.y_gm) - fabs( eigenvalues.z_gm );
/*
***Check if the input points are identical.
*/
    if ( varxy <= 0.00001 && varxz <= 0.00001 && varyz <= 0.00001 )
      {
      status = -2;
      }
/*
***Check if the input points are on straight line in any orientation.
*/
    else if ( varxy <= 0.00001 || varxz <= 0.00001 || varyz <= 0.00001) 
      {
      status = -3;
      }
/*
*** Return status.
*/
    return(status);
  }

/********************************************************/
/*!******************************************************/

       short EXmoba()

/*      Executes the MODE_BASIC() procedure.
 *
 *      (C)microform ab 2/3/95 J. Kjellander
 *
 *      2007-03-18 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
/*
***No coordinate system at all is now active.
*/
    lsyspk = NULL;
    lsysla = DBNULL;
/*
***If the active module is active, set csys-name = BASIC.
*/
    if ( mspek == modstk ) strcpy(actcnm,"BASIC");

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short EXmogl()

/*      Executes the MODE_GLOBAL() procedure.
 *
 *      (C)microform ab 9/2/85 J. Kjellander
 *
 *      7/11/85  Bugfix, J. Kjellander
 *      16/4/86  Bytt geo607 mot 612, J. Kjellander
 *      20/3/92  lsysla, J. Kjellander
 *      2007-03-18 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
/*
***If the active module is active, this effectively is the
***same thing as making the BASIC system active but the
***name = GLOBAL.
*/
    if ( mspek == modstk )
      {
      msyspk = NULL;
      msysla = DBNULL;
      lsyspk = NULL;
      lsysla = DBNULL;
      strcpy(actcnm,"GLOBAL");
      }
/*
***If we are not in the active module, make the global system of
***the current module active.
*/
    else 
      {
      if ( msyspk == NULL )
        {
        lsyspk = NULL;
        lsysla = DBNULL;
        }
      else
        {
        lsyspk = &modsys;
        GEtform_inv(&modsys,&lklsyi);
        lsysla = msysla;
        }
      }

    return(0);
  }

/********************************************************/
/*!******************************************************/

       short EXmolo(DBId *idvek)

/*      Executes the MODE_LOCAL(#n) procedure.
 *
 *      In: idvek => ID of coordinate system.
 *
 *      Error: EX1402 = Csys does not exist
 *
 *      (C)microform ab 9/2/85 J. Kjellander
 *
 *      16/4/86  Bytt geo607 mot 612, J. Kjellander
 *      20/3/92  EXmlla(), J. Kjellander
 *      2007-03-18 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    DBptr   la;
    DBetype typ;

/*
***Get csys DBptr.
*/
    if ( DBget_pointer('I',idvek,&la,&typ) < 0  ||  typ != CSYTYP )
         return(erpush("EX1402",""));
/*
***Activate.
*/
    return(EXmlla(la));
  }

/********************************************************/
/*!******************************************************/

       short EXmlla(DBptr la)

/*      Activate local coordinate system.
 *
 *      In: la = DBptr to csys.
 *
 *      (C)microform ab 20/3/92 J. Kjellander
 *
 *      2007-03-18 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    DBCsys csy;

/*
***Read the csys from DB and save in lklsys.
*/
    DBread_csys(&csy,&lklsys,la);
    lsyspk = &lklsys;
/*
***Also create the inverted version and save in lklsyi.
*/
    GEtform_inv(&lklsys,&lklsyi);
/*
***Finally update lsysla to point to the new csys.
*/
    lsysla = la;
/*
***If the active module is active, uppdate csys-name
***for the csys of the active module.
*/
    if ( mspek == modstk ) strcpy(actcnm,csy.name_pl);

    return(0);
  }

/********************************************************/


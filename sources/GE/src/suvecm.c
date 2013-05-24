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

/*!******************************************************************/
/*  File: suvecm.c !!Old geo900, but with long names                */
/*  =============                                                   */
/*                                                                  */
/*  Table of contents                                               */
/*                                                                  */
/*  varkon_normv       Normalisation of a vector                    */
/*  varkon_lengv       The lentgh of a vector                       */
/*  varkon_scalar      The scalar product                           */
/*  varkon_xprod       The vector product                           */
/*  varkon_nxprod      The vector product, normalised               */
/*  varkon_triscl      The triple scalar product                    */
/*  varkon_angr        The angle between two vectors in radians     */
/*  varkon_angd        The angle between two vectors in degrees     */
/*                                                                  */
/*                                                                  */
/* Description of all the functions                                 */
/* --------------------------------                                 */
/* Refer to /D1/GEODOC/geo900.txt                                   */
/*                                                                  */
/*  Gunnar Liden 1985-01-13                                         */
/*                                                                  */
/*  Revised 1985-01-22                                              */
/*          1985-07-10 Error system  B. Doverud                     */
/*          1985-09-21 angr: dot=1 => *pang=0 (and not PI) Liden    */
/*          1986-05-11 extern short, B. Doverud                     */
/*  1999-12-05   Free source code modifications                     */
/*                                                                  */
/******************************************************************!*/

/********************************************************************/
/* Error messages and warnings for (the value of) the functions     */
/* -------------------------------------------------------------    */
/*                                                                  */
/*      GE9001 = geo900 The input vector to normv() has zero        */
/*               length (TOL1)                                      */
/*                                                                  */
/*      GE9011 = geo900 First input vector to scalar() has zero     */
/*               length (TOL1)                                      */
/*                                                                  */
/*      GE9021 = geo900 Second input vector to scalar() has zero    */
/*               length (TOL1)                                      */
/*                                                                  */
/*      GE9031 = geo900 The input vector to lengv() has zero        */
/*               length (TOL1)                                      */
/*                                                                  */
/*      GE9041 = geo900 First input vector to xprod() has zero      */
/*               length (TOL1)                                      */
/*                                                                  */
/*      GE9051 = geo900 Second input vector to xprod() has zero     */
/*               length (TOL1)                                      */
/*                                                                  */
/*      GE9061 = geo900 Function xprod failed in nxprod()           */
/*                                                                  */
/*      GE9071 = geo900 Function normv failed in nxprod()           */
/*                                                                  */
/*      GE9081 = geo900 First input vector to triscl() has zero     */
/*               length (TOL1)                                      */
/*                                                                  */
/*      GE9091 = geo900 Second input vector to triscl() has zero    */
/*               length (TOL1)                                      */
/*                                                                  */
/*      GE9101 = geo900 Third input vector to triscl() has zero     */
/*               length (TOL1)                                      */
/*                                                                  */
/*      GE9111 = geo900 First input vector to angr() has zero       */
/*               length (TOL1)                                      */
/*                                                                  */
/*      GE9121 = geo900 Second input vector to angr() has zero      */
/*               length (TOL1)                                      */
/*                                                                  */
/*      GE9131 = geo900 Function angr failed in angd()              */
/*                                                                  */
/********************************************************************/



/*!  The normalisation of a vector                                  */
/*  -----------------------------                                   */

    DBstatus  varkon_normv (

/* Argument declarations                                            */
/* ......................                                           */
/* In and Out:                                                      */

   DBfloat v[] ) /* Pointer to the vector which will be normalised !*/

{ /* Start of function */

/* Internal variables                                               */
/* ...................                                              */

DBfloat sum;     /* The length of the vector                        */
short i;         /* Loop index                                      */

/* ----------------end-of-declarations------------------------------*/

/* computation of length and test of status from lengv*/
if(varkon_lengv(v,&sum) < 0 ) return(varkon_erpush("GE9001","geo900"));

for (i=0;i<=2;i=i+1)
    {
    v[i]= v[i]/sum;
    }

return(SUCCED);
} /* End of function */

/********************************************************************/


/*! The length of a vector                                          */
/*   --------------------                                           */
    DBstatus  varkon_lengv (

/* Argument declarations                                            */
/* ......................                                           */
/* In:                                                              */
       DBfloat v[],       /* Pointer to the input vector            */
       DBfloat *pleng )   /* Pointer to the length of the vector    */

/* Out:  The length of the vector filed away to pleng              !*/

{ /* Start of function */

/* Internal variables                                               */
/* ...................                                              */

       DBfloat sum;     /* The length of the vector                 */

/* ----------------end-of-declarations------------------------------*/

/* Check of the vector length. Warning if the length = 0 */

sum = v[0]*v[0] + v[1]*v[1] +v[2]*v[2];
sum = SQRT(sum);

if(sum <= TOL1) 
   {
   *pleng = 0.0;
   return(varkon_erpush("GE9031","geo900"));
   }
*pleng = sum;

return(SUCCED);

} /* End of function  */

/******************************************************************/

/*! Scalar product                                                  */
/*  ---------------                                                 */

    DBstatus  varkon_scalar (

/* Argument declarations                                            */
/* ......................                                           */
/* In:                                                              */
        DBfloat u[],
        DBfloat v[],        /* Pointers to two input vectors        */
        DBfloat *pdot )     /* Pointer to the scalar product        */

/* Out: The dot product filed away to pdot                         !*/

{ /* Start of function */


/* Internal variables                                               */
/* ...................                                              */


DBfloat sum; /* The length of the vector */



/* ----------------end-of-declarations------------------------------*/

/* Check of vector lengths. Warning if length = 0 */

if( varkon_lengv(u,&sum) < 0 ) 
    {
    *pdot = 0.0;
    return(varkon_erpush("GE9011","geo900"));
    }
if( varkon_lengv(v,&sum) < 0 )
    {
    *pdot = 0.0;
    return(varkon_erpush("GE9021","geo900"));
    }
*pdot = u[0]*v[0]+u[1]*v[1]+u[2]*v[2];


return(SUCCED);

} /* End of function */

/******************************************************************/


/*! Vector product                                                  */
/*  ---------------                                                 */


    DBstatus  varkon_xprod (

/* Argument declarations                                            */
/* ......................                                           */
/* In:                                                              */
      DBfloat u[],
      DBfloat v[],      /* Pointers to two input vectors            */
      DBfloat cross[] ) /* Pointer to the cross product vector      */

/* Out:  The vector components of the cross vector filed away       */
/*       to cross                                                   */

{ /* Start of function */

/* Internal variables                                               */
/* ...................                                              */
       DBfloat sum;   /* The length of the vector                   */


/* ----------------end-of-declarations------------------------------*/


/* Check of vector lengths. Warning if length = 0 */

if( varkon_lengv(u,&sum) < 0 ) 
    {
    cross[0]=0.0;
    cross[1]=0.0;
    cross[2]=0.0;
    return(varkon_erpush("GE9041","geo900"));
    }
if( varkon_lengv(v,&sum)  < 0  )
    {
    cross[0]=0.0;
    cross[1]=0.0;
    cross[2]=0.0;
    return(varkon_erpush("GE9051","geo900"));
    }


cross[0] = u[1]*v[2]-u[2]*v[1];
cross[1] = u[2]*v[0]-u[0]*v[2];
cross[2] = u[0]*v[1]-u[1]*v[0];

return(SUCCED);

} /* End of function  */

/******************************************************************/


/*! Vector product, normalised                                      */
/*  --------------------------                                      */


    DBstatus  varkon_nxprod (

/* Argument declarations                                            */
/* ......................                                           */
/* In:                                                              */
      DBfloat u[],
      DBfloat v[],      /* Pointers to two input vectors            */
      DBfloat cross[] ) /* Pointer to the cross product vector      */

/* Out:  The vector components of the cross vector filed away       */
/*       to cross                                                   */

{ /* Start of function */

/* Internal variables                                               */
/* ...................                                              */



/* ----------------end-of-declarations------------------------------*/

/* Computation of the cross product */
/* Error if input vector has length 0 */

  if ( varkon_xprod(u,v,cross) < 0 ) 
        return(varkon_erpush("GE9061","geo900"));

/* Normalise           */

  if ( varkon_normv(cross) < 0 ) 
        return(varkon_erpush("GE9071","geo900"));

return(SUCCED);

} /* End of function  */

/******************************************************************/

/*! The triple scalar product                                      */
/*  ---------------------------                                    */

      DBstatus varkon_triscl(

/* Argument declarations                                            */
/* ......................                                           */
/* In:                                                              */
       DBfloat v1[],
       DBfloat v2[],
       DBfloat v3[],           /* Pointers to three input vectors   */
       DBfloat *res )          /* Pointer where to put the result   */
                               /*  from the triple scalar product   */
                               /* computation                       */

{ /* Start of function  */


/* Internal variables                                               */
/* ...................                                              */


       DBfloat sum;             /* Vector length                    */


/* ----------------end-of-declarations------------------------------*/
if(varkon_lengv(v1,&sum) < 0 )
   {
   *res = 0.0;
   return(varkon_erpush("GE9081","geo900"));
   }

if(varkon_lengv(v2,&sum) < 0 )
   {
   *res = 0.0;
   return(varkon_erpush("GE9091","geo900"));
   }

if(varkon_lengv(v3,&sum) < 0 )
   {
   *res = 0.0;
   return(varkon_erpush("GE9101","geo900"));
   }

*res = v1[0] * (v2[1] * v3[2] - v2[2] * v3[1]);
*res = *res + (v1[1] * (v2[2] * v3[0] - v2[0] * v3[2]));
*res = *res + (v1[2] * (v2[0] * v3[1] - v2[1] * v3[0]));

return(SUCCED);
} /* End of function  */

/*******************************************************************/


/*! The angle between two vectors in radians                        */
/*  -----------------------------------------                       */

    DBstatus  varkon_angr (

/* Argument declarations                                            */
/* ......................                                           */
/* In:                                                              */
        DBfloat u[],
        DBfloat v[],        /* Pointers to two input vectors        */
        DBfloat *pang )     /* Pointer to the angle                 */

/* Out: The angle filed away to pang                               !*/

{ /* Start of function */


/* Internal variables                                               */
/* ...................                                              */


       DBfloat  dot;    /* The scalar product                       */
/* ----------------end-of-declarations------------------------------*/

/* Normalise the input vectors and check if length # 0.0  */

  if ( varkon_normv(u) < 0 ) return(varkon_erpush("GE9111","geo900"));
  if ( varkon_normv(v) < 0 ) return(varkon_erpush("GE9121","geo900"));

/* Compute the scalar product of the two normalised      */
/* vectors = cosinus of the angle between the vectors    */

   varkon_scalar(u,v,&dot); /* No error, vectors have been checked  */

/*  Compute the angle in radians                       */

    if( ABS(dot) >= 1.0 ) 
         {
         *pang = 0.0;
         if( dot <= -1.0 ) *pang = PI;
         }

    if ( ABS(dot) < 1.0 ) *pang = DACOS(dot);




return(SUCCED);

} /* End of function */

/*******************************************************************/


/*! The angle between two vectors in degrees                        */
/*  -----------------------------------------                       */

    DBstatus  varkon_angd (

/* Argument declarations                                            */
/* ......................                                           */
/* In:                                                              */
        DBfloat u[],
        DBfloat v[],        /* Pointers to two input vectors        */
        DBfloat *pang )     /* Pointer to the angle                 */

/* Out: The angle filed away to pang                               !*/

{ /* Start of function */


/* Internal variables                                               */
/* ...................                                              */

       short  status;  /* Status of angr                            */
       DBfloat  rad;   /* Angle in radians                          */


/* ----------------end-of-declarations------------------------------*/

/*  Angle in radians. Error if vector is zero  */

    status = varkon_angr(u,v,&rad);

    if ( status < 0 ) return(varkon_erpush("GE9131","geo900"));

/*  Angle in degrees                           */

    *pang = rad*180.0/PI;


return(SUCCED);

} /* End of function */


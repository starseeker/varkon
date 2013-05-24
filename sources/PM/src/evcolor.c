/**********************************************************************
*
*    evcolor.c
*    =========
*
*    This file is part of the VARKON Program Module Library.
*    URL: http://varkon.sourceforge.net
*
*    This file includes the following routines:
*
*    evltvi();     Evaluates CRE_LIGHT()
*    evlton();     Evaluates LIGHT_ON()
*    evltof();     Evaluates LIGHT_OFF()
*    evgtlt();     Evaluates GET_LIGHT()
*
*    evcrco();     Evaluates CRE_COLOR()
*    evgtco();     Evaluates GET_COLOR()
*    evrgbhsv();   Evaluates RGB_TO_HSV()
*    evhsvrgb();   Evaluates HSV_TO_RGB()
*
*    evcrmt();     Evaluates CRE_MATERIAL()
*    evgtmt();     Evaluates GET_MATERIAL()
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
***********************************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../WP/include/WP.h"
#include <X11/Xcms.h>

extern PMPARVA *proc_pv;  /* Access structure for MBS procedures */
extern short    proc_pc;  /* Number of actual procedure parameters */
extern PMPARVA *func_pv;  /* Access structure for MBS functions */
extern short    func_pc;  /* Number of actual function parameters */
extern PMLITVA *func_vp;  /* Ptr to function return value */

/********************************************************/

        short evltvi()

/*      Evaluates MBS procedure CRE_LIGHT().
 *
 *      (C)2007-11-30 J.Kjellander
 *
 ********************************************************/

 {
   DBVector dir;
   DBfloat  spotang,focus;

/*
***Only light number and light direction supplied.
*/
   if ( proc_pc == 2 )
     {
     dir.x_gm = 0.0;
     dir.y_gm = 0.0;
     dir.z_gm = 1.0;
     spotang  = 180.0;
     focus    = 0.0;
     }
/*
***Spot dir supplied. This means that par 2 is spot position.
*/
   else if ( proc_pc == 3 )
     {
     dir.x_gm = proc_pv[3].par_va.lit.vec_va.x_val;
     dir.y_gm = proc_pv[3].par_va.lit.vec_va.y_val;
     dir.z_gm = proc_pv[3].par_va.lit.vec_va.z_val;
     spotang  = 180.0;
     focus    = 0.0;
     }
/*
***Spot angle supplied.
*/
   else if ( proc_pc == 4 )
     {
     dir.x_gm = proc_pv[3].par_va.lit.vec_va.x_val;
     dir.y_gm = proc_pv[3].par_va.lit.vec_va.y_val;
     dir.z_gm = proc_pv[3].par_va.lit.vec_va.z_val;
     spotang  = proc_pv[4].par_va.lit.float_va;
     focus    = 0.0;
     }
/*
***Spot focus supplied.
*/
   else
     {
     dir.x_gm = proc_pv[3].par_va.lit.vec_va.x_val;
     dir.y_gm = proc_pv[3].par_va.lit.vec_va.y_val;
     dir.z_gm = proc_pv[3].par_va.lit.vec_va.z_val;
     spotang  = proc_pv[4].par_va.lit.float_va;
     focus    = proc_pv[5].par_va.lit.float_va;
     }
/*
***Execute.
*/
   return(WPcreate_light(proc_pv[1].par_va.lit.int_va,
    (DBVector *)&proc_pv[2].par_va.lit.vec_va,
                &dir,spotang,focus));
 }

/********************************************************/
/********************************************************/

        short evlton()

/*      Evaluates MBS procedure LIGHT_ON().
 *
 *      (C)2008-01-23 J.Kjellander
 *
 ********************************************************/

 {
   int tmode;

/*
***If optional parameter light transformation mode is not
***supplied, set to 0 => No light transformation.
*/
   if ( proc_pc == 2 ) tmode = 0;
   else                tmode = proc_pv[3].par_va.lit.int_va;
/*
***Activate.
*/
   return(WPactivate_light(proc_pv[1].par_va.lit.int_va,
                           proc_pv[2].par_va.lit.float_va,
                           tmode,
                           TRUE));
 }

/********************************************************/
/********************************************************/

        short evltof()

/*      Evaluates MBS procedure LIGHT_OFF().
 *
 *      (C)2007-11-30 J.Kjellander
 *
 ********************************************************/

 {
   return(WPactivate_light(proc_pv[1].par_va.lit.int_va,
                           0.0,0,FALSE));
 }

/********************************************************/
/********************************************************/

        short evgtlt()

/*      Evaluates MBS procedure GET_LIGHT().
 *
 *      (C)2008-01-23 J.Kjellander
 *
 ********************************************************/

 {
   short   status;
   int     i;
   bool    defined,on,follow_model;
   DBfloat intensity;
   PMLITVA litval[3];
/*
***Get data for this light source.
*/
   status = WPget_light(func_pv[1].par_va.lit.int_va,
                       &defined,&on,&intensity,&follow_model);
   if ( status < 0 ) return(status);
/*
***Is the light source defined ?
*/
   if ( defined )
     {
/*
***Yes, return 0 and copy light data to PMLITVA.
*/
     func_vp->lit.int_va = 0;

     if ( on ) litval[0].lit.int_va = 1;
     else      litval[0].lit.int_va = 0;

     litval[1].lit.float_va = intensity;

     if ( follow_model ) litval[2].lit.int_va = 1;
     else                litval[2].lit.int_va = 0;
/*
***Write to MBS variables.
*/
     for ( i=0; i<3; ++i )
       {
       inwvar(func_pv[i+2].par_ty, func_pv[i+2].par_va.lit.adr_va,
              0, NULL, &litval[i]);
       }
     }
/*
**This light source is not defined.
*/
   else func_vp->lit.int_va = -1;
/*
***The end.
*/
   return(0);
 }

/********************************************************/
/********************************************************/

        short evcrco()

/*      Evaluates MBS procedure CRE_COLOR().
 *
 *      (C)2007-11-30 J.Kjellander
 *
 ********************************************************/

 {
   int pen,val1,val2,val3;

/*
***Pen number and RGB values.
*/
   pen  = proc_pv[1].par_va.lit.int_va;
   val1 = proc_pv[2].par_va.lit.int_va;
   val2 = proc_pv[3].par_va.lit.int_va;
   val3 = proc_pv[4].par_va.lit.int_va;
/*
***Create the color.
*/
   return(WPccol(pen,val1,val2,val3));
 }

/********************************************************/
/********************************************************/

        short evgtco()

/*      Evaluates MBS procedure GET_COLOR().
 *
 *      (C)2008-01-19 J. Kjellander
 *
 ********************************************************/

 {
   short   status;
   int     i,red,green,blue;
   bool    defined;
   PMLITVA litval[3];

/*
***Get the RGB values for this pen.
*/
   status = WPgpen(func_pv[1].par_va.lit.int_va,&defined,&red,&green,&blue);
   if ( status < 0 ) return(status);
/*
***Is the pen defined ?
*/
   if ( defined )
     {
/*
***Yes, return 0 and copy RGB values to PMLITVA.
*/
     func_vp->lit.int_va = 0;

     litval[0].lit.int_va = red;
     litval[1].lit.int_va = green;
     litval[2].lit.int_va = blue;
/*
***Write to MBS variables.
*/
     for ( i=0; i<3; ++i )
       {
       inwvar(func_pv[i+2].par_ty,func_pv[i+2].par_va.lit.adr_va,
              0, NULL, &litval[i]);
       }
     }
/*
**This pen is not defined.
*/
   else func_vp->lit.int_va = -1;
/*
***The end.
*/
   return(0);
 }

/********************************************************/
/********************************************************/

        short evrgbhsv()

/*      Evaluates MBS procedure RGB_TO_HSV().
 *
 *      Error: IN5842 = Red value out of range
 *             IN5852 = Green value out of range
 *             IN5862 = Blue value out of range
 *             IN5872 = R=G=B=0
 *
 *      (C)2008-02-22 J.Kjellander
 *
 ********************************************************/

 {
   double  hue,sat,val,red,green,blue,max,min,diff;
   int     i;
   PMLITVA litval[3];

/*
***Red, green and blue.
*/
   red   = proc_pv[1].par_va.lit.int_va/255.0;
   green = proc_pv[2].par_va.lit.int_va/255.0;
   blue  = proc_pv[3].par_va.lit.int_va/255.0;
/*
***Range check.
*/
   if ( red   < 0.0 || red   > 1.0 ) return(erpush("IN5842",""));
   if ( green < 0.0 || green > 1.0 ) return(erpush("IN5852",""));
   if ( blue  < 0.0 || blue  > 1.0 ) return(erpush("IN5862",""));
/*
***Home made RGB_to_HSV algorithm.
*/
   if ( red >= green  &&  red >= blue )
     {
     max = red;
     if ( green < blue ) min = green;
     else                min = blue;
     }
   else if ( green >= red  &&  green >= blue )
     {
     max = green;
     if ( red < blue ) min = red;
     else              min = blue;
     }
   else
     {
     max = blue;
     if ( red < green ) min = red;
     else               min = green;
     }

   val = max;

   diff = max - min;

   if ( max > 0.0 ) sat = diff/max;
   else return(erpush("IN5872",""));

   if      ( max == red )   hue = 60.0*((green - blue)/diff);
   else if ( max == green ) hue = 60.0*(2.0 + (blue - red)/diff);
   else                     hue = 60.0*(4.0 + (red - green)/diff);

   if ( hue < 0.0 ) hue += 360.0;
/*
***Copy HSV values to PMLITVA.
*/
   litval[0].lit.int_va = hue;
   litval[1].lit.int_va = sat*100.0;
   litval[2].lit.int_va = val*100.0;
/*
***Write to MBS variables.
*/
   for ( i=0; i<3; ++i )
     {
     inwvar(proc_pv[i+4].par_ty,proc_pv[i+4].par_va.lit.adr_va,
            0, NULL, &litval[i]);
     }
/*
***The end.
*/
   return(0);
 }

/********************************************************/
/********************************************************/

        short evhsvrgb()

/*      Evaluates MBS procedure HSV_TO_RGB().
 *
 *      Error: IN5812 = Hue out of range
 *             IN5822 = Saturation out of range
 *             IN5832 = Value out of range
 *
 *      (C)2008-02-22 J.Kjellander
 *
 ********************************************************/

 {
   double  hue,sat,val,red,green,blue,k1,k2,k3,k4;
   int     i,sextant;
   PMLITVA litval[3];

/*
***Hue, saturation and value.
*/
   hue = proc_pv[1].par_va.lit.int_va;
   sat = proc_pv[2].par_va.lit.int_va/100.0;
   val = proc_pv[3].par_va.lit.int_va/100.0;
/*
***Range check.
*/
   if ( hue < 0.0 || hue > 360.0 ) return(erpush("IN5812",""));
   if ( sat < 0.0 || sat >   1.0 ) return(erpush("IN5822",""));
   if ( val < 0.0 || val >   1.0 ) return(erpush("IN5832",""));
/*
***Homemade HSV_to_RGB algorithm.
*/
   if ( sat == 0.0 )
     {
     red   = val;
     green = val;
     blue  = val;
     }
   else
     {
     if ( hue > 359.999999 ) hue = 359.999999;

     hue = hue/60.0;
     sextant = (int)hue;
     k1 = hue - sextant;
     k2 = val*(1.0 - sat);
     k3 = val*(1.0 - sat*k1);
     k4 = val*(1.0 - sat + sat*k1);

     switch ( sextant )
       {
       case 0: red = val; green = k4;  blue = k2;  break;
       case 1: red = k3;  green = val; blue = k2;  break;
       case 2: red = k2;  green = val; blue = k4;  break;
       case 3: red = k2;  green = k3;  blue = val; break;
       case 4: red = k4;  green = k2;  blue = val; break;
       case 5: red = val; green = k2;  blue = k3;  break;
       }
     }
/*
***Copy RGB values to PMLITVA.
*/
   litval[0].lit.int_va = red*255.0;
   litval[1].lit.int_va = green*255.0;
   litval[2].lit.int_va = blue*255.0;
/*
***Write to MBS variables.
*/
   for ( i=0; i<3; ++i )
     {
     inwvar(proc_pv[i+4].par_ty,proc_pv[i+4].par_va.lit.adr_va,
            0, NULL, &litval[i]);
     }
/*
***The end.
*/
   return(0);
 }

/********************************************************/
/********************************************************/

        short evcrmt()

/*      Evaluates MBS procedure CRE_MATERIAL().
 *
 *      (C)2007-11-30 J.Kjellander
 *
 ********************************************************/

 {
   return(WPcmat(proc_pv[1].par_va.lit.int_va,
                 proc_pv[2].par_va.lit.int_va,
                 proc_pv[3].par_va.lit.int_va,
                 proc_pv[4].par_va.lit.int_va,
                 proc_pv[5].par_va.lit.int_va,
                 proc_pv[6].par_va.lit.int_va,
                 proc_pv[7].par_va.lit.int_va,
                 proc_pv[8].par_va.lit.int_va,
                 proc_pv[9].par_va.lit.int_va,
                 proc_pv[10].par_va.lit.int_va,
                 proc_pv[11].par_va.lit.int_va,
                 proc_pv[12].par_va.lit.int_va,
                 proc_pv[13].par_va.lit.int_va,
                 proc_pv[14].par_va.lit.int_va));
 }

/********************************************************/
/********************************************************/

        short evgtmt()

/*      Evaluates MBS procedure GET_MATERIAL().
 *
 *      (C)2008-01-19 J. Kjellander
 *
 ********************************************************/

 {
   short   status;
   int     i,ar,ag,ab,dr,dg,db,sr,sg,sb,er,eg,eb,s;
   bool    defined;
   PMLITVA litval[13];

/*
***Get the material values for this pen.
*/
   status = WPgmat(func_pv[1].par_va.lit.int_va,&defined,&ar,&ag,&ab,
                   &dr,&dg,&db,&sr,&sg,&sb,&er,&eg,&eb,&s);
   if ( status < 0 ) return(status);
/*
***Is the pen defined ?
*/
   if ( defined )
     {
/*
***Yes, return 0 and copy material values to PMLITVA.
*/
     func_vp->lit.int_va = 0;

     litval[0].lit.int_va = ar;
     litval[1].lit.int_va = ag;
     litval[2].lit.int_va = ab;

     litval[3].lit.int_va = dr;
     litval[4].lit.int_va = dg;
     litval[5].lit.int_va = db;

     litval[6].lit.int_va = sr;
     litval[7].lit.int_va = sg;
     litval[8].lit.int_va = sb;

     litval[9].lit.int_va  = er;
     litval[10].lit.int_va = eg;
     litval[11].lit.int_va = eb;

     litval[12].lit.int_va = s;
/*
***Write to MBS variables.
*/
     for ( i=0; i<13; ++i )
       {
       inwvar(func_pv[i+2].par_ty, func_pv[i+2].par_va.lit.adr_va,
              0, NULL, &litval[i]);
       }
     }
/*
**This pen is not defined.
*/
   else func_vp->lit.int_va = -1;
/*
***The end.
*/
   return(0);
 }

/********************************************************/

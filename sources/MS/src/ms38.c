/**********************************************************************
*
*    ms38.c
*    ======
*
*    This file is part of the VARKON MS-library including
*    Microsoft WIN32 specific parts of the Varkon
*    WindowPac library.
*
*    This file includes:
*
*     mscbmm();    Handle mouse movments
*     msvsline();  Handle view settings for line
*     msvsfill();  Handle view settings for fill
*     msvslight(); Handle view settings for light
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
*    (C) 1984-1999, Johan Kjellander, Microform AB
*    (C) 200-2004, Johan Kjellander, Örebro university
*
***********************************************************************/

#include "../../../sources/DB/include/DB.h"
#include "../../../sources/IG/include/IG.h"
#include "../../../sources/WP/include/WP.h"
#include <commctrl.h>
#include <commdlg.h>

/*!*****************************************************/

   void  mscbmm(WPRWIN *rwinpt,
                POINTS *ptold,
                POINTS *ptnew)

/*       Hantering av musrörelse   
 *       
 *       (C)microform ab 1998-11-20  R.Bäckgren,M.Lindberg
 *       
 *
 *****************************************************!*/

{
	POINTS ptdiff;
	ptdiff.x = ptnew->x - ptold->x;
	ptdiff.y = ptnew->y - ptold->y; 
      
/*
*** rotation
*/
	
   if(SendMessage(rwinpt->tb_id,TB_ISBUTTONCHECKED,ID_ROT,0))
   {
		if( abs(ptdiff.x) > 1 || abs(ptdiff.y) > 1)
      {
         rwinpt->roty = ptdiff.x/4.0;
		   rwinpt->rotx = ptdiff.y/4.0; 
         ptold->x = ptnew->x;
	      ptold->y = ptnew->y;	
         wpsodl(rwinpt,(GLuint)1);
      }
	}

/*
*** panorering
*/		
	if(SendMessage(rwinpt->tb_id,TB_ISBUTTONCHECKED,ID_PAN,0)) 
	{
	   if( abs(ptdiff.x) > 1 || abs(ptdiff.y) > 1)
      {
         rwinpt->movx += (double)(ptold->x-ptnew->x)/rwinpt->geo.dx*
                         (rwinpt->xmax - rwinpt->xmin)*
                         rwinpt->scale;
         rwinpt->movy += (double)(ptnew->y-ptold->y)/rwinpt->geo.dy*
                         (rwinpt->ymax - rwinpt->ymin)*
                         rwinpt->scale;
         ptold->x = ptnew->x;
		   ptold->y = ptnew->y;	
         wpsodl(rwinpt,(GLuint)1);
      }         
	} 

/*
*** zoomning
*/
	if(SendMessage(rwinpt->tb_id,TB_ISBUTTONCHECKED,ID_ZOOM,0)) 
   {
	   if(abs(ptdiff.y) > 1)
		{
			rwinpt->scale *= 1.0+(double)(ptnew->y-ptold->y)/
                          rwinpt->geo.dy;
         if(rwinpt->scale < 1E-10) rwinpt->scale = 1E-10;
         if(rwinpt->scale > 1E+10) rwinpt->scale = 1E+10;
         ptold->x = ptnew->x;
		   ptold->y = ptnew->y;
         wpsodl(rwinpt,(GLuint)1);
      }	
   }
}
/*******************************************************/
/*!*****************************************************/

   void  msvsline()

/*     Hantering av view-settings,line  
 *       
 *     (C)microform ab 1998-11-27  R.Bäckgren,M.Lindberg
 *       
 *
 *****************************************************!*/
{
   glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
}
/*******************************************************/
/*!*****************************************************/

   void  msvsfill()

/*     Hantering av view-settings,fill  
 *       
 *     (C)microform ab 1998-11-27  R.Bäckgren,M.Lindberg
 *       
 *
 *****************************************************!*/
{
   glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}
/*******************************************************/
/*!*****************************************************/

   void  msvslight(WPRWIN *rwinpt)

/*       Hantering av ljusintensitet   
 *       
 *       (C)microform ab 1998-11-30  R.Bäckgren,M.Lindberg
 *       
 *
 *****************************************************!*/

{
    GLfloat light_ambient[4],light_diffuse[4],light_specular[4];
    double intensity;
    int i;
    intensity = rwinpt->light;
    for(i=0;i<3;i++)
    {  
       light_ambient[i] = (GLfloat)(intensity / 100.0 * rwinpt->ambient[i]);
       light_diffuse[i] = (GLfloat)(intensity / 100.0 * rwinpt->diffuse[i]);
       light_specular[i] = (GLfloat)(intensity / 100.0 * rwinpt->specular[i]);
    }
    light_ambient[3] = 1.0;
    light_diffuse[3] = 1.0;
    light_specular[3] = 1.0;
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}
/********************************************************/

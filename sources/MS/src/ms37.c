/**********************************************************************
*
*    ms37.c
*    ======
*
*    This file is part of the VARKON MS-library including
*    Microsoft WIN32 specific parts of the Varkon
*    WindowPac library.
*
*    This file includes:
*
*     msprWC();  Handle toolbar
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


/*!*****************************************************/

       void    msprWC(WPRWIN *rwinpt, UINT wParam)
/*
 *
 *****************************************************!*/

{
   int exit; 

   switch(wParam)
   {
      case ID_EXIT:
         exit=MessageBox(rwinpt->id.ms_id, "Are you sure?", "EXIT", MB_OKCANCEL);
         if ( exit == IDOK ) mswdel((v2int)rwinpt->id.w_id);
      break;

      case ID_VIEW:
         msdl10(rwinpt);
         wpsodl(rwinpt, (GLuint)1);
         BitBlt(rwinpt->bmdc, 0, 0, rwinpt->geo.dx, rwinpt->geo.dy,
                rwinpt->dc, 0, 0, SRCCOPY);     
      break; 

      case ID_LIGHT:
         msdl11(rwinpt); 
         wpsodl(rwinpt, (GLuint)1);
         BitBlt(rwinpt->bmdc, 0, 0, rwinpt->geo.dx, rwinpt->geo.dy,
                rwinpt->dc, 0, 0, SRCCOPY);
      break;
         
      case ID_CLIP:
         msdl12(rwinpt);
         wpsodl(rwinpt, (GLuint)1);
         BitBlt(rwinpt->bmdc, 0, 0, rwinpt->geo.dx, rwinpt->geo.dy,
                rwinpt->dc, 0, 0, SRCCOPY);
      break;

      case ID_HELPP:
         ighelp();
      break;
   }
}

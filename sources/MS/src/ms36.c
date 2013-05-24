/**********************************************************************
*
*    ms36.c
*    ======
*
*    This file is part of the VARKON MS-library including
*    Microsoft WIN32 specific parts of the Varkon
*    WindowPac library.
*
*    This file includes:
*
*     msdl10();  View settings box
*     msdl11();  Light settings box
*     msdl12();  Clip settings box
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
#include "../../../sources/EX/include/EX.h"
#include <commctrl.h>
#include <string.h>

BOOL APIENTRY cbkdmsdl10(HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY cbkdmsdl11(HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY cbkdmsdl12(HWND, UINT, WPARAM, LPARAM);

static WPRWIN *act_rwinpt;
static int modelbutton=2;  

/*!******************************************************/

        int msdl10(WPRWIN *rwinpt)        
      
/*      Dialogbox för vyer.
 *
 *      (C) 01/12/98 R.Bäckgren, M.Lindberg
 *
 ******************************************************!*/
{ 
   act_rwinpt = rwinpt;
   DialogBox (ms_inst, MAKEINTRESOURCE(IDD_DIALOG10),
              act_rwinpt->id.ms_id, (DLGPROC)cbkdmsdl10);
   rwinpt = act_rwinpt;
   return(0);
} 

/*!******************************************************/

        BOOL APIENTRY cbkdmsdl10(dlg, message, wp, lp)
        HWND dlg;
        UINT message;
        WPARAM wp;
        LPARAM lp; 

/*      Callback för msdl10(), vysättning.
 *
 *      In: dlg     = Dialogboxens fönsterid.
 *          message = Typ av meddelande.
 *          wp,lp   = Ytterligare data.
 *
 *      Ut: Inget.
 *
 *      (C) 01/12/98 R.Bäckgren, M.Lindberg
 *
 ******************************************************!*/
{
   char    *nparr[100];
   int      x, y, mdx, mdy, ddx, ddy, nnam, i;
   RECT     mr, dr;
   MSG      tmpmes;
   static int viewbutton=1;
   static int persbutton=2;
   static int edittal=0;
   static int tempedittal=0;
   static int sliderpos=0;
   static int tempsliderpos=0;
   char buff[3];
   static char dflt[30] = "Default";

   switch ( message )
   {
      case WM_INITDIALOG:
/*
***Placering mitt i huvudfönstret.
*/
         GetWindowRect(ms_main, &mr);
         mdx = mr.right - mr.left;
         mdy = mr.bottom - mr.top;
         GetWindowRect(dlg, &dr);
         ddx = dr.right - dr.left;
         ddy = dr.bottom - dr.top;

         x = (mdx - ddx)/2;
         y = (mdy - ddy)/2;
         SetWindowPos(dlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
        
         SendDlgItemMessage(dlg, IDC_PER_SLIDER, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)sliderpos);
         sprintf(buff, "%7d", edittal); 
         SetDlgItemText(dlg, IDC_PER_EDIT, buff);        

         if(SendMessage(act_rwinpt->tb_id, TB_ISBUTTONCHECKED, ID_ROT, 0))
            viewbutton=1;
         if(SendMessage(act_rwinpt->tb_id, TB_ISBUTTONCHECKED, ID_PAN, 0))
            viewbutton=2;
         if(SendMessage(act_rwinpt->tb_id, TB_ISBUTTONCHECKED, ID_ZOOM, 0))
            viewbutton=3; 

         if(viewbutton == 1)
            SendMessage(GetDlgItem(dlg, IDC_ROT), BM_SETCHECK, 1, 0L);    
         else if(viewbutton == 2)
            SendMessage(GetDlgItem(dlg, IDC_PAN), BM_SETCHECK, 1, 0L);
         else
            SendMessage(GetDlgItem(dlg, IDC_ZOOM), BM_SETCHECK, 1, 0L);

         if(persbutton == 1)        
            SendMessage(GetDlgItem(dlg, IDC_PER_ON), BM_SETCHECK, 1, 0L);
         else
            SendMessage(GetDlgItem(dlg, IDC_PER_OFF), BM_SETCHECK, 1, 0L);   

          if(modelbutton == 1)
            SendMessage(GetDlgItem(dlg, IDC_LINE), BM_SETCHECK, 1, 0L);      
         else
            SendMessage(GetDlgItem(dlg, IDC_FILL), BM_SETCHECK, 1, 0L);      
         /*
         ***Fyll listan med vynamn.
         */
         EXgvnl(nparr, &nnam);
         for ( i=0; i<nnam; ++i )
         SendDlgItemMessage(dlg, IDC_COMBOBOX, CB_ADDSTRING,
                            (WPARAM)0, (LPARAM)nparr[i]);
         SendDlgItemMessage(dlg, IDC_COMBOBOX, WM_SETTEXT, 0,
             				    (LPARAM)dflt); 
      return(TRUE);

      case WM_HSCROLL: 
         while(PeekMessage(&tmpmes, dlg, WM_HSCROLL,WM_HSCROLL, PM_REMOVE) != 0);
         tempsliderpos=SendDlgItemMessage(dlg, IDC_PER_SLIDER, TBM_GETPOS, (WPARAM)0, (LPARAM)0);
         tempedittal = tempsliderpos;
         sprintf(buff, "%7d", tempedittal); 
         SetDlgItemText(dlg, IDC_PER_EDIT, buff);
      break; 

/*
***Clickning i OK eller CANCEL-rutan.
*/
      case WM_COMMAND:
      switch ( LOWORD(wp) )
      {
         case IDC_HELP_D1:
            ighelp();   
         return(TRUE);     

         case IDC_SAVE_D1:
            MessageBox(ms_main, "SAVE", "DialogBox 1", MB_OK);     
         return(TRUE); 
                
         case IDOK:
            if(IsDlgButtonChecked(dlg, IDC_ROT) == 1)
            {
               SendMessage(act_rwinpt->tb_id, TB_CHECKBUTTON, ID_ROT, 1);
               viewbutton=1;
            }
            else if(IsDlgButtonChecked(dlg, IDC_PAN) == 1)
            {
               SendMessage(act_rwinpt->tb_id, TB_CHECKBUTTON, ID_PAN, 1);
               viewbutton=2;
            }
            else
            {
               SendMessage(act_rwinpt->tb_id, TB_CHECKBUTTON, ID_ZOOM, 1);
               viewbutton=3;
            }  
       
            if(IsDlgButtonChecked(dlg, IDC_PER_ON) == 1)
            {
               persbutton=1;
               act_rwinpt->pfactor = tempedittal;
            }
            else
            {
               persbutton=2;
               act_rwinpt->pfactor = 0.0;
            }              

            if(IsDlgButtonChecked(dlg, IDC_LINE) == 1)
            {
               modelbutton=1;
               msvsline();
            }
            
            else
            {
               modelbutton=2;
               msvsfill();  
            }

            edittal=tempedittal;
            sliderpos=tempsliderpos;
            SendDlgItemMessage(dlg, IDC_COMBOBOX, WM_GETTEXT, sizeof(dflt), 
                              (LPARAM) dflt);

            EndDialog(dlg, wp);
         return(FALSE);

         case IDCANCEL:
            EndDialog(dlg, wp);
         return(TRUE);
      }       
   }
   return(FALSE);
}

/*!******************************************************/

        int     msdl11(WPRWIN *rwinpt)
      
/*      Dialogbox för ljusstyrka.
 *
 *      (C) 01/12/98 R.Bäckgren, M.Lindberg
 *
 ******************************************************!*/
{
   act_rwinpt = rwinpt;
   DialogBox (ms_inst, MAKEINTRESOURCE(IDD_DIALOG11),
              act_rwinpt->id.ms_id, (DLGPROC)cbkdmsdl11);
   return(0);
} 

/*!******************************************************/

        BOOL APIENTRY cbkdmsdl11(dlg, message, wp, lp)
        HWND dlg;
        UINT message;
        WPARAM wp;
        LPARAM lp; 

/*      Callback för msdl11(), ljus sättning.
 *
 *      In: dlg     = Dialogboxens fönsterid.
 *          message = Typ av meddelande.
 *          wp,lp   = Ytterligare data.
 *
 *      Ut: Inget.
 *
 *      (C) 01/12/98 R.Bäckgren, M.Lindberg
 *
 ******************************************************!*/
{
   int      x, y, mdx, mdy, ddx, ddy;
   RECT     mr, dr;
   static int aliasingbutton=2;
   static int edittal=50;
   static int tempedittal=50;
   static int sliderpos=50;
   static int tempsliderpos=50;
   char buff[3]; 

   switch ( message )
   {
      case WM_INITDIALOG:
/*
***Placering mitt i huvudfönstret.
*/
         GetWindowRect(ms_main, &mr);
         mdx = mr.right - mr.left;
         mdy = mr.bottom - mr.top;
         GetWindowRect(dlg, &dr);
         ddx = dr.right - dr.left;
         ddy = dr.bottom - dr.top;

         x = (mdx - ddx)/2;
         y = (mdy - ddy)/2;
         SetWindowPos(dlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
         
         SendDlgItemMessage(dlg, IDC_INT_SLIDER, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)sliderpos);
         sprintf(buff, "%5d", edittal); 
         SetDlgItemText(dlg, IDC_INT_EDIT, buff);

         if(aliasingbutton == 1)
            SendMessage(GetDlgItem(dlg, IDC_ALI_ON), BM_SETCHECK, 1, 0L);
         else
            SendMessage(GetDlgItem(dlg, IDC_ALI_OFF), BM_SETCHECK, 1, 0L);           
      return(TRUE);

      case WM_HSCROLL:     
         tempsliderpos=SendDlgItemMessage(dlg, IDC_INT_SLIDER, TBM_GETPOS, (WPARAM)0, (LPARAM)0);
         tempedittal = tempsliderpos;
         sprintf(buff, "%5d", tempedittal); 
         SetDlgItemText(dlg, IDC_INT_EDIT, buff);
      break;

/*
***Clickning i OK eller CANCEL-rutan.
*/
      case WM_COMMAND:

      switch (LOWORD(wp) )
      {
         case IDC_HELP_D2:
            ighelp();
            wpsodl(act_rwinpt, (GLuint)1);     
         return(TRUE); 

         case IDOK:         
            if(IsDlgButtonChecked(dlg, IDC_ALI_ON) == 1)
            {
               aliasingbutton=1;
               glEnable(GL_POLYGON_SMOOTH);
               glEnable(GL_LINE_SMOOTH);
               glEnable(GL_BLEND);
               glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }
            else
            {
               aliasingbutton=2;
               glDisable(GL_POLYGON_SMOOTH);
               glDisable(GL_LINE_SMOOTH);
               glDisable(GL_BLEND);
            }

            edittal=tempedittal;
            act_rwinpt->light=tempedittal; 
            if(sliderpos != tempsliderpos)
            {
               msvslight(act_rwinpt);
               modelbutton=2;
            }
            sliderpos=tempsliderpos;          

            EndDialog(dlg, wp);
         return(FALSE);

         case IDCANCEL:
            EndDialog(dlg, wp);
         return(TRUE);
      }
   }
   return(FALSE);
}

/*!******************************************************/

        int     msdl12(WPRWIN *rwinpt)
      
/*      Dialogbox för z-klippning.
 *
 *      (C) 01/12/98 R.Bäckgren, M.Lindberg
 *
 *       1999-01-05 GL_Enable() mm. J.Kjellander
 *
 ******************************************************!*/
{
   act_rwinpt = rwinpt;
   DialogBox (ms_inst, MAKEINTRESOURCE(IDD_DIALOG12),
              act_rwinpt->id.ms_id, (DLGPROC)cbkdmsdl12);
   return(0);
} 

/*!******************************************************/

        BOOL APIENTRY cbkdmsdl12(dlg, message, wp, lp)
        HWND dlg;
        UINT message;
        WPARAM wp;
        LPARAM lp; 

/*      Callback för msdl12(), z-klipp sättning.
 *
 *      In: dlg     = Dialogboxens fönsterid.
 *          message = Typ av meddelande.
 *          wp,lp   = Ytterligare data.
 *
 *      Ut: Inget.
 *
 *      (C) 01/12/98 R.Bäckgren, M.Lindberg
 *
 ******************************************************!*/
{
   int      x, y, mdx, mdy, ddx, ddy;
   RECT     mr, dr;
   static int edittal=0;
   static int tempedittal=0;
   static int sliderpos=50;
   static int tempsliderpos=50;
   char buff[3]; 
   GLdouble plane[] = {0.0, 0.0, 1.0, -20.0};
   switch (message)
   {
      case WM_INITDIALOG:
/*
***Placering mitt i huvudfönstret.
*/
         GetWindowRect(ms_main, &mr);
         mdx = mr.right - mr.left;
         mdy = mr.bottom - mr.top;
         GetWindowRect(dlg, &dr);
         ddx = dr.right - dr.left;
         ddy = dr.bottom - dr.top;

         x = (mdx - ddx)/2;
         y = (mdy - ddy)/2;
         SetWindowPos(dlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);

         SendDlgItemMessage(dlg, IDC_ZSLIDER, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)sliderpos);
         sprintf(buff, "%4d", edittal); 
         SetDlgItemText(dlg, IDC_Z_EDIT, buff);
         
         if(act_rwinpt->zclip)        
            SendMessage(GetDlgItem(dlg, IDC_Z_ON), BM_SETCHECK, 1, 0L);
         else
            SendMessage(GetDlgItem(dlg, IDC_Z_OFF), BM_SETCHECK, 1, 0L);

      return(TRUE);

      case WM_HSCROLL:     
         tempsliderpos=SendDlgItemMessage(dlg, IDC_ZSLIDER, TBM_GETPOS, (WPARAM)0, (LPARAM)0);
         tempedittal = 2*(tempsliderpos-50);
         sprintf(buff, "%4d", tempedittal); 
         SetDlgItemText(dlg, IDC_Z_EDIT, buff);
      break;

/*
***Clickning i OK eller CANCEL-rutan.
*/
      case WM_COMMAND:

      switch (LOWORD(wp))
      {
         case IDC_HELP_D3:
            ighelp();    
         return(TRUE); 

         case IDOK:
            if(IsDlgButtonChecked(dlg, IDC_Z_ON) == 1)
              {
              act_rwinpt->zclip=TRUE;
              act_rwinpt->zfactor=tempsliderpos;
              glEnable(GL_CLIP_PLANE0);
              }
            else
              {
              act_rwinpt->zclip=FALSE;
              glDisable(GL_CLIP_PLANE0);
              }
            edittal=tempedittal;           
            sliderpos=tempsliderpos;
            EndDialog(dlg, wp);
         return(FALSE);

         case IDCANCEL:
            EndDialog(dlg, wp);
         return(TRUE);
      }
   }
   return(FALSE);
}

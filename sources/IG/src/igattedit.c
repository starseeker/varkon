/*!******************************************************************/
/*  igattedit.c                                                     */
/*  ===========                                                     */
/*                                                                  */
/*  This file includes:                                             */
/*                                                                  */
/*   IGset_actatts(); Set active attributes                         */
/*   IGcpen();        Edit pen                                      */
/*   IGclevel();      Edit level                                    */
/*   IGcwdt();        Edit linewidth                                */
/*   IGcdal();        Edit dashlength                               */
/*   IGcfs();         Set font to solid                             */
/*   IGcfd();         Set font to dashed                            */
/*   IGcfc();         Set font to centreline                        */
/*   IGcff();         Set font to filled                            */
/*   IGctsz();        Edit text size                                */
/*   IGctwd();        Edit text width                               */
/*   IGctsl();        Edit text slant                               */
/*   IGctfn();        Edit text font                                */
/*   IGctpm();        Edit text planemode                           */
/*   IGcdts();        Edit dimension digitsize                      */
/*   IGcdas();        Edit dimension arrowsize                      */
/*   IGcdnd();        Edit dimension number of digits               */
/*   IGcda0();        Edit dimension auto = off                     */
/*   IGcda1();        Edit dimension auto = on                      */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
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
/********************************************************************/

#include "../../DB/include/DB.h"
#include "../../GE/include/geotol.h"
#include "../include/IG.h"
#include "../../EX/include/EX.h"
#include "../../WP/include/WP.h"

extern short   sysmode;
extern DBptr   lsysla;

static short igcfnt(int font);
static short igcdau(int dauto);

/********************************************************/

        short IGset_actatts()

/*      Interactive function "Set active attributes".
 *
 *      Return: 0  = OK
 *
 *      (C)2007-11-14 J. Kjellander
 *
 ******************************************************!*/

  {
/*
***Call the attributes dialog in WP.
*/
   WPatdi();
/*
***The end.
*/
   return(0);
  }

/********************************************************/
/********************************************************/

        short IGcpen()

/*      Interactive function "Edit pen".
 *
 *      Return: 0  = OK
 *          REJECT = Exit
 *          GOMAIN = Main menu
 *
 *      Errors: IG3562 = Entity belongs to a part
 *              IG3702 = Illegal pen number
 *
 *      (C)microform ab 12/4/86 J. Kjellander
 *
 *      6/10/86  GOMAIN, B. Doverud
 *      11/11/86 Test om storhet ingår i part, J. Kjellander
 *      15/3/88  Ritpaketet, J. Kjellander
 *      18/11/88 IGgmid(), J. Kjellander
 *      2007-08-10 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    int      i,nid;
    DBetype  typ,typvek[IGMAXID];
    DBptr    la;
    pm_ptr   exnpt,retla,valtyp;
    char     istr[V3STRLEN+1],mesbuf[V3STRLEN];
    PMREFVA  idmat[IGMAXID][MXINIV];
    DBHeader hed;
    PMLITVA  val;

    static char dstr[V3STRLEN+1] = "";

/*
***Select one or more entities.
*/
loop:
    WPaddmess_mcwin(IGgtts(150),WP_PROMPT);
    typvek[0] = ALLTYP;
    nid       = IGMAXID;
    if ( (status=IGgmid(idmat,typvek,&nid)) < 0 ) goto exit;
/*
***In generic mode entities may not belong to a part.
*/
    if ( sysmode & GENERIC )
      {
      for ( i=0; i<nid; ++i )
        if ( idmat[i][0].p_nextre != NULL )
          {
          erpush("IG3562","");
          errmes();
          WPerhg();
          goto loop;
         }
      }
/*
***Ask for new pen number.
*/
    pmmark();
    if ( (status=IGcint(16,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
/*
***Interpret.
*/
    inevev(exnpt,&val,&valtyp);
/*
***Check for illegal value.
*/
    if ( val.lit.int_va < 0 || val.lit.int_va > 32767 )
      {
      erpush("IG3702","");
      errmes();
      WPerhg();
      goto loop;
      }
/*
***Update all entities.
*/
    for ( i=0; i<nid; ++i )
      {
/*
***In generic mode update PM.
*/
      if ( sysmode & GENERIC ) pmchnp(idmat[i],PMPEN,exnpt,&retla);
/*
***Always update DB and WPGWIN's.
*/
      DBget_pointer('I',idmat[i],&la,&typ);
      DBread_header(&hed,la);
      if ( hed.pen != (short)val.lit.int_va )
        {
        EXdren(la,typ,FALSE,GWIN_ALL);
        hed.pen = (short)val.lit.int_va;
        DBupdate_header(&hed,la);
        EXdren(la,typ,TRUE,GWIN_ALL);
        }
      }
/*
***Update WPRWIN's.
*/
    WPrepaint_RWIN(RWIN_ALL,FALSE);
/*
***Confirmational message.
*/
    if ( nid == 1 ) strcpy(mesbuf,IGgtts(63));
    else           sprintf(mesbuf,"%hd %s",nid,IGgtts(64));
    WPaddmess_mcwin(mesbuf,WP_MESSAGE);
/*
***The end.
*/
exit:
    WPerhg();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGclevel()

/*      Interactive function "Edit level".
 *
 *      Return: 0  = OK
 *          REJECT = Exit
 *          GOMAIN = Main menu
 *
 *      Errors: IG3212 = Inavlid level number
 *              IG3562 = An entity belongs to a part
 *
 *      (C)microform ab 12/4/86 J. Kjellander
 *
 *      30/9/86  Ny nivï¿½hantering, J. Kjellander
 *      6/10/86  GOMAIN, B. Doverud
 *      11/11/86 Test om storhet ingï¿½r i part, J. Kjellander
 *      15/3/88  Ritpaketet, J. Kjellander
 *      18/11/88 IGgmid(), J. Kjellander
 *      1998-01-12 Aktivt csys, J.Kjellander
 *      2007-08-10 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    int      i,nid;
    DBetype  typ,typvek[IGMAXID];
    DBptr    la;
    char     istr[V3STRLEN+1],mesbuf[V3STRLEN];
    pm_ptr   exnpt,retla,valtyp;
    PMREFVA  idmat[IGMAXID][MXINIV];
    DBHeader hed;
    PMLITVA  val;

    static char dstr[V3STRLEN+1] = "";

/*
***Select entities.
*/
loop:
    WPaddmess_mcwin(IGgtts(151),WP_PROMPT);
    typvek[0] = ALLTYP;
    nid       = IGMAXID;
    if ( (status=IGgmid(idmat,typvek,&nid)) < 0 ) goto exit;
/*
***In generic mode entities may not belong to a part.
*/
    if ( sysmode & GENERIC )
      {
      for ( i=0; i<nid; ++i )
        if ( idmat[i][0].p_nextre != NULL )
          {
          erpush("IG3562","");
          errmes();
          WPerhg();
          goto loop;
         }
      }
/*
***Ask for new level number.
*/
    pmmark();
    if ( (status=IGcint(227,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
/*
***Interpret.
*/
    inevev(exnpt,&val,&valtyp);
/*
***Check that the value is valid.
*/
    if ( val.lit.int_va < 0 || val.lit.int_va > WP_NIVANT-1 )
      {
      erpush("IG3212","");
      errmes();
      WPerhg();
      goto loop;
      }
/*
***Update.
*/
    for ( i=0; i<nid; ++i )
      {
/*
***In generic mode update PM.
*/
      if ( sysmode & GENERIC ) pmchnp(idmat[i],PMLEVEL,exnpt,&retla);
/*
***Update DB and WPGWIN's.
*/
      DBget_pointer('I',idmat[i],&la,&typ);
      DBread_header(&hed,la);
      if ( hed.level != (short)val.lit.int_va )
        {
        EXdren(la,typ,FALSE,GWIN_ALL);
        hed.level = (short)val.lit.int_va;
        DBupdate_header(&hed,la);
        EXdren(la,typ,TRUE,GWIN_ALL);
        if ( la == lsysla ) IGupcs(lsysla,V3_CS_ACTIVE);
        }
      }
/*
***Update WPRWIN's.
*/
    WPrepaint_RWIN(RWIN_ALL,FALSE);
/*
***Confirmational message.
*/
    if ( nid == 1 ) strcpy(mesbuf,IGgtts(63));
    else           sprintf(mesbuf,"%hd %s",nid,IGgtts(64));
    WPaddmess_mcwin(mesbuf,WP_MESSAGE);
/*
***The end.
*/
exit:
    WPerhg();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGcwdt()

/*      Interactive function "Edit linewidth".
 *
 *      Return: 0  = OK
 *          REJECT = Exit
 *          GOMAIN = Main menu
 *
 *      Errors: IG3562 = Entity belongs to part
 *              IG3702 = Invalid linewidth
 *
 *      (C)microform ab 1998-11-25 J. Kjellander
 *
 *      1999-03-09, Bugfix pen, J.Kjellander
 *      2004-07-27 B_plane, J.Kjellander, ï¿½rebro university
 *      2007-08-10 1.19, J.Kjellander
 *      2007-09-01 Hatch and Dims, J.Kjellander
 *
 ******************************************************!*/

  {
    short    status;
    int      i,nid;
    DBetype  typ,typvek[IGMAXID];
    DBptr    la;
    DBfloat  width;
    pm_ptr   exnpt,retla,valtyp;
    char     istr[V3STRLEN+1],mesbuf[V3STRLEN];
    DBPoint  poi;
    DBLine   lin;
    DBArc    arc;
    DBCurve  cur;
    DBSurf   sur;
    DBText   txt;
    DBHatch  xht;
    DBLdim   ldm;
    DBCdim   cdm;
    DBRdim   rdm;
    DBAdim   adm;
    DBBplane bpl;
    PMREFVA  idmat[IGMAXID][MXINIV];
    DBHeader hed;
    PMLITVA  val;

    static char dstr[V3STRLEN+1] = "0";

/*
***Make multiple selection.
*/
loop:
    WPaddmess_mcwin(IGgtts(1633),WP_PROMPT);
    typvek[0] = POITYP+LINTYP+ARCTYP+CURTYP+SURTYP+TXTTYP+XHTTYP+LDMTYP+
                CDMTYP+RDMTYP+ADMTYP+BPLTYP;
    nid = IGMAXID;
    if ( (status=IGgmid(idmat,typvek,&nid)) < 0 ) goto exit;
/*
***In generic mode entities may not belong to a part.
*/
    if ( sysmode & GENERIC )
      {
      for ( i=0; i<nid; ++i )
        if ( idmat[i][0].p_nextre != NULL )
          {
          erpush("IG3562","");
          errmes();
          WPerhg();
          goto loop;
         }
      }
/*
***Ask for new linewidth.
*/
askwdt:
    pmmark();
    if ( (status=IGcflt(1632,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
/*
***Interpret.
*/
    inevev(exnpt,&val,&valtyp);

    if ( val.lit_type == C_INT_VA ) width = (DBfloat)val.lit.int_va;
    else width = val.lit.float_va;
/*
***Check for validity.
*/
    if ( width < 0.0 )
      {
      pmrele();
      erpush("IG3872","");
      errmes();
      goto askwdt;
      }
/*
***Update.
*/
    for ( i=0; i<nid; ++i )
      {
/*
***In generic mode update PM.
*/
      if ( sysmode & GENERIC ) pmchnp(idmat[i],PMWIDTH,exnpt,&retla);
/*
***Update DB and graphical windows.
*/
      DBget_pointer('I',idmat[i],&la,&typ);
      DBread_header(&hed,la);
      EXdren(la,typ,FALSE,GWIN_ALL);

      if ( typ == POITYP )
        {
        DBread_point(&poi,la);
        poi.wdt_p = width;
        DBupdate_point(&poi,la);
        }
      else if ( typ == LINTYP )
        {
        DBread_line(&lin,la);
        lin.wdt_l = width;
        DBupdate_line(&lin,la);
        }
      else if ( typ == ARCTYP )
        {
        DBread_arc(&arc,NULL,la);
        arc.wdt_a = width;
        DBupdate_arc(&arc,NULL,la);
        }
      else if ( typ == CURTYP )
        {
        DBread_curve(&cur,NULL,NULL,la);
        cur.wdt_cu = width;
        DBupdate_curve(&cur,NULL,la);
        }
      else if ( typ == SURTYP )
        {
        DBread_surface(&sur,la);
        sur.wdt_su = width;
        DBupdate_surface(&sur,la);
        }
      else if ( typ == TXTTYP )
        {
        DBread_text(&txt,NULL,la);
        txt.wdt_tx = width;
        DBupdate_text(&txt,NULL,la);
        }
      else if ( typ == XHTTYP )
        {
        DBread_xhatch(&xht,NULL,NULL,la);
        xht.wdt_xh = width;
        DBupdate_xhatch(&xht,NULL,la);
        }
      else if ( typ == LDMTYP )
        {
        DBread_ldim(&ldm,NULL,la);
        ldm.wdt_ld = width;
        DBupdate_ldim(&ldm,la);
        }
      else if ( typ == CDMTYP )
        {
        DBread_cdim(&cdm,NULL,la);
        cdm.wdt_cd = width;
        DBupdate_cdim(&cdm,la);
        }
      else if ( typ == RDMTYP )
        {
        DBread_rdim(&rdm,NULL,la);
        rdm.wdt_rd = width;
        DBupdate_rdim(&rdm,la);
        }
      else if ( typ == ADMTYP )
        {
        DBread_adim(&adm,NULL,la);
        adm.wdt_ad = width;
        DBupdate_adim(&adm,la);
        }
      else if ( typ == BPLTYP )
        {
        DBread_bplane(&bpl,la);
        bpl.wdt_bp = width;
        DBupdate_bplane(&bpl,la);
        }

      EXdren(la,typ,TRUE,GWIN_ALL);
      }
/*
***Update WPRWIN's.
*/
    WPrepaint_RWIN(RWIN_ALL,FALSE);
/*
***Confirmational message.
*/
    if ( nid == 1 ) strcpy(mesbuf,IGgtts(63));
    else           sprintf(mesbuf,"%hd %s",nid,IGgtts(64));
    WPaddmess_mcwin(mesbuf,WP_MESSAGE);
/*
***The end.
*/
exit:
    WPerhg();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGcdal()

/*      Interactive function to edit dashlength.
 *
 *      FV: 0      = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3502 = Storheten ingï¿½r i en part.
 *              IG3802 = Storheten ï¿½r heldragen.
 *              IG3862 = Otillï¿½ten strecklï¿½ngd.
 *
 *      (C)microform ab 25/8/87 J. Kjellander
 *
 *      17/3/88  Snitt, J. Kjellander
 *      12/1/92  Kurvor, J. Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    short   font;
    DBetype typ;
    DBptr   la;
    DBfloat dashl;
    bool    end,right;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt,retla,valtyp;
    PMREFVA idvek[MXINIV];
    DBLine  lin;
    DBArc   arc;
    DBCurve cur;
    DBHatch xht;
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "";

/*
***Get entity ID.
*/
loop:
    WPaddmess_mcwin(IGgtts(268),WP_PROMPT);
    typ = LINTYP+ARCTYP+CURTYP+XHTTYP;
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
/*
***In generic mode the entity may not belong to a part.
*/
    if ( sysmode & GENERIC  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Check that entity is not solid.
*/
    DBget_pointer('I',idvek,&la,&typ);

    if ( typ == LINTYP )
      {
      DBread_line(&lin,la);
      font = lin.fnt_l;
      dashl = lin.lgt_l;
      }
    else if ( typ == ARCTYP )
      {
      DBread_arc(&arc,NULL,la);
      font = arc.fnt_a;
      dashl = arc.lgt_a;
      }
    else if ( typ == CURTYP )
      {
      DBread_curve(&cur,NULL,NULL,la);
      font = cur.fnt_cu;
      dashl = cur.lgt_cu;
      }
    else
      {
      DBread_xhatch(&xht,NULL,NULL,la);
      font = xht.fnt_xh;
      dashl = xht.lgt_xh;
      }

    if ( font == 0 )
      {
      erpush("IG3802","");
      errmes();
      WPerhg();
      goto loop;
      }
/*
***Ask for new dashlength.
*/
askdsh:
    sprintf(strbuf,"%s%g  %s",IGgtts(12),dashl,IGgtts(248));
    IGplma(strbuf,IG_INP);

    pmmark();
    if ( (status=IGcflt(0,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
/*
***Interpret.
*/
    inevev(exnpt,&val,&valtyp);

    if ( val.lit_type == C_INT_VA ) dashl = (DBfloat)val.lit.int_va;
    else dashl = val.lit.float_va;
/*
***Check validity.
*/
    if ( dashl < 2*TOL2 )
      {
      pmrele();
      erpush("IG3862","");
      errmes();
      goto askdsh;
      }
/*
***Update.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);

    if ( typ == LINTYP )
      {
      if ( sysmode & GENERIC ) pmchnp(idvek,PMLDASHL,exnpt,&retla);
      lin.lgt_l = dashl;
      DBupdate_line(&lin,la);
      }
    else if ( typ == ARCTYP )
      {
      if ( sysmode & GENERIC ) pmchnp(idvek,PMADASHL,exnpt,&retla);
      arc.lgt_a = dashl;
      DBupdate_arc(&arc,NULL,la);
      }
    else if ( typ == CURTYP )
      {
      if ( sysmode & GENERIC ) pmchnp(idvek,PMCDASHL,exnpt,&retla);
      cur.lgt_cu = dashl;
      DBupdate_curve(&cur,NULL,la);
      }
    else
      {
      if ( sysmode & GENERIC ) pmchnp(idvek,PMXDASHL,exnpt,&retla);
      xht.lgt_xh = dashl;
      DBupdate_xhatch(&xht,NULL,la);
      }

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Update WPRWIN's.
*/
    WPrepaint_RWIN(RWIN_ALL,FALSE);
/*
***Confirmational message.
*/
    WPaddmess_mcwin(IGgtts(63),WP_MESSAGE);
/*
***Again.
*/
    goto loop;
/*
***The end.
*/
exit:
    WPclear_mcwin();
    WPerhg();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGcfs()

/*      Interactive edit entity font to 0 = solid.
 *
 *      (C)microform ab 25/8/87 J. Kjellander
 *
 *      17/3/88  Snitt, J. Kjellander
 *
 ******************************************************!*/

   {
   return(igcfnt(0));
   }

/********************************************************/
/*!******************************************************/

        short IGcfd()

/*      Interactive edit entity font to 1 = dashed.
 *
 *      (C)microform ab 25/8/87 J. Kjellander
 *
 *      17/3/88  Snitt, J. Kjellander
 *
 ******************************************************!*/

   {
   return(igcfnt(1));
   }

/********************************************************/
/*!******************************************************/

        short IGcfc()

/*      Interactive edit entity font to 2 = centerline.
 *
 *      (C)microform ab 25/8/87 J. Kjellander
 *
 *      17/3/88  Snitt, J. Kjellander
 *
 ******************************************************!*/

   {
   return(igcfnt(2));
   }

/********************************************************/
/*!******************************************************/

        short IGcff()

/*      Interactive edit entity font to 3 = filled.
 *
 *      (C)microform ab 1999-12-15 J. Kjellander
 *
 ******************************************************!*/

   {
   return(igcfnt(3));
   }

/********************************************************/
/*!******************************************************/

static short igcfnt(int font)

/*      Edit the font of a point, line, arc, curve or hatch.
 *
 *      In: font = 0,1 or 2.
 *
 *      Return: 0  = OK
 *          REJECT = avsluta
 *          GOMAIN = huvudmenyn
 *
 *      Felkod: IG3502 = Entity belongs to a part.
 *
 *      (C)microform ab 25/8/87 J. Kjellander
 *
 *      17/3/88  Snitt, J. Kjellander
 *      12/1/92  Kurvor, J. Kjellander
 *      2007-08-10 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype typ;
    DBptr   la;
    bool    end,right;
    pm_ptr  exnpt,retla;
    PMREFVA idvek[MXINIV];
    DBPoint poi;
    DBLine  lin;
    DBArc   arc;
    DBCurve cur;
    DBHatch xht;
    PMLITVA val;

/*
***Get entity ID.
*/
loop:
    WPaddmess_mcwin(IGgtts(268),WP_PROMPT);
    typ = POITYP+LINTYP+ARCTYP+CURTYP+XHTTYP;
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
/*
***In generic mode the entity may not belong to a part.
*/
    if ( sysmode & GENERIC  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Update.
*/
    DBget_pointer('I',idvek,&la,&typ);

    EXdren(la,typ,FALSE,GWIN_ALL);

    val.lit_type = C_INT_VA;
    val.lit.int_va = font;
    pmclie(&val,&exnpt);

    if       ( typ == POITYP )
      {
      if ( sysmode & GENERIC ) pmchnp(idvek,PMPFONT,exnpt,&retla);
      DBread_point(&poi,la);
      poi.fnt_p = font;
      DBupdate_point(&poi,la);
      }
    else if ( typ == LINTYP )
      {
      if ( sysmode & GENERIC ) pmchnp(idvek,PMLFONT,exnpt,&retla);
      DBread_line(&lin,la);
      lin.fnt_l = font;
      DBupdate_line(&lin,la);
      }
    else if ( typ == ARCTYP )
      {
      if ( sysmode & GENERIC ) pmchnp(idvek,PMAFONT,exnpt,&retla);
      DBread_arc(&arc,NULL,la);
      arc.fnt_a = font;
      DBupdate_arc(&arc,NULL,la);
      }
    else if ( typ == CURTYP )
      {
      if ( sysmode & GENERIC ) pmchnp(idvek,PMCFONT,exnpt,&retla);
      DBread_curve(&cur,NULL,NULL,la);
      cur.fnt_cu = font;
      DBupdate_curve(&cur,NULL,la);
      }
    else
      {
      if ( sysmode & GENERIC ) pmchnp(idvek,PMXFONT,exnpt,&retla);
      DBread_xhatch(&xht,NULL,NULL,la);
      xht.fnt_xh = font;
      DBupdate_xhatch(&xht,NULL,la);
      }

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Update WPRWIN's.
*/
    WPrepaint_RWIN(RWIN_ALL,FALSE);
/*
***Confirmational message.
*/
    WPaddmess_mcwin(IGgtts(63),WP_MESSAGE);
/*
***Again.
*/
    goto loop;
/*
***The end.
*/
exit:
    WPerhg();
    WPclear_mcwin();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGctsz()

/*      Interactive function to edit text size.
 *
 *      Return: 0  = OK
 *          REJECT = Exit
 *          GOMAIN = Main menu
 *
 *      Error: IG3502 = Text belongs to part
 *
 *      (C)microform ab 25/8/87 J. Kjellander
 *
 *      2007-08-10 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype   typ;
    DBptr   la;
    bool    end,right;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt,retla,valtyp;
    PMREFVA idvek[MXINIV];
    DBText   txt;
    char    str[V3STRLEN+1];
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "";

/*
***Select text.
*/
loop:
    WPaddmess_mcwin(IGgtts(268),WP_PROMPT);
    typ = TXTTYP;
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
/*
***In generic mode it may not belong to a part.
*/
    if ( sysmode & GENERIC  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Get text from DB.
*/
    DBget_pointer('I',idvek,&la,&typ);
    DBread_text(&txt,str,la);
/*
***Ask for new size
*/
    sprintf(strbuf,"%s%g  %s",IGgtts(11),txt.h_tx,IGgtts(248));
    IGplma(strbuf,IG_INP);

    pmmark();
    if ( (status=IGcflt(0,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }

    strcpy(dstr,istr);
/*
***Interpret.
*/
    inevev(exnpt,&val,&valtyp);
/*
***Update.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);

    if ( sysmode & GENERIC ) pmchnp(idvek,PMTSIZE,exnpt,&retla);
    if ( val.lit_type == C_INT_VA ) txt.h_tx = val.lit.int_va;
    else txt.h_tx = val.lit.float_va;
    DBupdate_text(&txt,str,la);

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Update WPRWIN's.
*/
    WPrepaint_RWIN(RWIN_ALL,FALSE);
/*
***Confirmational message.
*/
    WPaddmess_mcwin(IGgtts(63),WP_MESSAGE);
/*
***Again.
*/
    goto loop;
/*
***The end.
*/
exit:
    WPerhg();
    WPclear_mcwin();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGctwd()

/*      Interactive function to edit text width.
 *
 *      Return: 0  = OK
 *          REJECT = Exit
 *          GOMAIN = Main menu
 *
 *      Error: IG3502 = Text belongs to a part
 *
 *      (C)microform ab 25/8/87 J. Kjellander
 *
 *      2007-08-10, 1.19 J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype typ;
    DBptr   la;
    bool    end,right;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt,retla,valtyp;
    PMREFVA idvek[MXINIV];
    DBText  txt;
    char    str[V3STRLEN+1];
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "";

/*
***Get text ID.
*/
loop:
    WPaddmess_mcwin(IGgtts(268),WP_PROMPT);
    IGptma(268,IG_MESS);
    typ = TXTTYP;
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
    IGrsma();
/*
***In generic mode the text may not belong to a part.
*/
    if ( sysmode & GENERIC  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Get text from DB.
*/
    DBget_pointer('I',idvek,&la,&typ);
    DBread_text(&txt,str,la);
/*
***Ask for new width.
*/
    sprintf(strbuf,"%s%g  %s",IGgtts(13),txt.b_tx,IGgtts(248));
    IGplma(strbuf,IG_INP);

    pmmark();
    if ( (status=IGcflt(0,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
/*
***Interpret.
*/
    inevev(exnpt,&val,&valtyp);
/*
***Update.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);

    if ( sysmode & GENERIC ) pmchnp(idvek,PMTWIDTH,exnpt,&retla);
    if ( val.lit_type == C_INT_VA ) txt.b_tx = val.lit.int_va;
    else txt.b_tx = val.lit.float_va;
    DBupdate_text(&txt,str,la);

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Update WPRWIN's.
*/
    WPrepaint_RWIN(RWIN_ALL,FALSE);
/*
***Confirmational message.
*/
    WPaddmess_mcwin(IGgtts(63),WP_MESSAGE);
/*
***Again.
*/
    goto loop;
/*
***The end.
*/
exit:
    WPerhg();
    WPclear_mcwin();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGctsl()

/*      Interactive function to edit text slant.
 *
 *      Return: 0  = OK
 *          REJECT = Exit
 *          GOMAIN = Main menu
 *
 *      Error: IG3502 = The text belongs to a part
 *
 *      (C)microform ab 25/8/87 J. Kjellander
 *
 *      2007-08-10 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype typ;
    DBptr   la;
    bool    end,right;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt,retla,valtyp;
    PMREFVA idvek[MXINIV];
    DBText  txt;
    char    str[V3STRLEN+1];
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "";

/*
***Get text ID.
*/
loop:
    WPaddmess_mcwin(IGgtts(268),WP_PROMPT);
    typ = TXTTYP;
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
/*
***In generic mode the text may not belong to a part.
*/
    if ( sysmode & GENERIC  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Get text from DB.
*/
    DBget_pointer('I',idvek,&la,&typ);
    DBread_text(&txt,str,la);
/*
***Ask for new text slant.
*/
    sprintf(strbuf,"%s%g  %s",IGgtts(15),txt.l_tx,IGgtts(248));
    IGplma(strbuf,IG_INP);

    pmmark();
    if ( (status=IGcflt(0,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
/*
***Interpret.
*/
    inevev(exnpt,&val,&valtyp);
/*
***Update.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);

    if ( sysmode & GENERIC ) pmchnp(idvek,PMTSLANT,exnpt,&retla);
    if ( val.lit_type == C_INT_VA ) txt.l_tx = val.lit.int_va;
    else txt.l_tx = val.lit.float_va;
    DBupdate_text(&txt,str,la);

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Update WPRWIN's.
*/
    WPrepaint_RWIN(RWIN_ALL,FALSE);
/*
***Confirmational message.
*/
    WPaddmess_mcwin(IGgtts(63),WP_MESSAGE);
/*
***Again.
*/
    goto loop;
/*
***The end.
*/
exit:
    WPerhg();
    WPclear_mcwin();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGctfn()

/*      Interactive function to edit text font.
 *
 *      Return: 0  = OK
 *          REJECT = Exit
 *          GOMAIN = Main menu
 *
 *      Error: IG3502 = The text belongs to a part
 *
 *      (C)microform ab 28/12/92 J. Kjellander
 *
 *      2007-08-10, 1.19 J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype typ;
    DBptr   la;
    bool    end,right;
    pm_ptr  exnpt,retla,valtyp;
    int     font;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    PMREFVA idvek[MXINIV];
    DBText  txt;
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "1";

/*
***Get text ID.
*/
loop:
    WPaddmess_mcwin(IGgtts(268),WP_PROMPT);
    typ = TXTTYP;
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
/*
***In generic mode the text may not belong to a part.
*/
    if ( sysmode & GENERIC  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Get text from DB.
*/
    DBget_pointer('I',idvek,&la,&typ);
    DBread_text(&txt,NULL,la);
    font = txt.fnt_tx;
/*
***Ask for new font.
*/
    sprintf(strbuf,"%s%d  %s",IGgtts(43),font,IGgtts(109));
    IGplma(strbuf,IG_INP);

    pmmark();
    if ( (status=IGcint(0,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
/*
***Interpret.
*/
    inevev(exnpt,&val,&valtyp);
    font = val.lit.int_va;
/*
***Update.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);

    val.lit_type = C_INT_VA;
    val.lit.int_va = font;
    pmclie(&val,&exnpt);

    if ( sysmode & GENERIC ) pmchnp(idvek,PMTFONT,exnpt,&retla);
    txt.fnt_tx = font;
    DBupdate_text(&txt,NULL,la);

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Update WPRWIN's.
*/
    WPrepaint_RWIN(RWIN_ALL,FALSE);
/*
***Confirmational message.
*/
    WPaddmess_mcwin(IGgtts(63),WP_MESSAGE);
/*
***Again.
*/
    goto loop;
/*
***The end.
*/
exit:
    WPerhg();
    WPclear_mcwin();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGctpm()

/*      Interactive function to edit text position mode.
 *
 *      Return: 0  = OK
 *          REJECT = Exit
 *          GOMAIN = Main menu
 *
 *      Error: IG3502 = The text belongs to a part
 *
 *      (C)microform ab 1998-10-01, J. Kjellander
 *
 *      2007-08-10 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype typ;
    DBptr   la;
    bool    end,right;
    pm_ptr  exnpt,retla,valtyp;
    int     pmode;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    PMREFVA idvek[MXINIV];
    DBText  txt;
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "1";

/*
***Get text ID.
*/
loop:
    WPaddmess_mcwin(IGgtts(268),WP_PROMPT);
    typ = TXTTYP;
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
/*
***In generic mode the text may not belong to a part.
*/
    if ( sysmode & GENERIC  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Get text from DB.
*/
    DBget_pointer('I',idvek,&la,&typ);
    DBread_text(&txt,NULL,la);
    pmode = txt.pmod_tx;
/*
***Ask for new pmode.
*/
    sprintf(strbuf,"%s%d  %s",IGgtts(43),pmode,IGgtts(1631));
    IGplma(strbuf,IG_INP);

    pmmark();
    if ( (status=IGcint(0,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
/*
***Interpret.
*/
    inevev(exnpt,&val,&valtyp);
    pmode = val.lit.int_va;
/*
***Update.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);

    val.lit_type = C_INT_VA;
    val.lit.int_va = pmode;
    pmclie(&val,&exnpt);

    if ( sysmode & GENERIC ) pmchnp(idvek,PMTPMODE,exnpt,&retla);
    txt.pmod_tx = pmode;
    DBupdate_text(&txt,NULL,la);

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Update WPRWIN's.
*/
    WPrepaint_RWIN(RWIN_ALL,FALSE);
/*
***Confirmational message.
*/
    WPaddmess_mcwin(IGgtts(63),WP_MESSAGE);
/*
***Again.
*/
    goto loop;
/*
***The end.
*/
exit:
    WPerhg();
    WPclear_mcwin();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGcdts()

/*      Interactive function to edit dim text size.
 *
 *      Return: 0  = OK
 *          REJECT = Exit
 *          GOMAIN = Main menu
 *
 *      Errors: IG3502 = The dim belongs to a part
 *              IG3812 = The dim has no digits
 *
 *      (C)microform ab 17/3/88 J. Kjellander
 *
 *      2007-08-10 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    tbool   dauto;
    DBptr   la;
    DBetype typ;
    DBfloat tsiz;
    bool    end,right;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt,retla,valtyp;
    PMREFVA idvek[MXINIV];
    DBLdim  ldm;
    DBCdim  cdm;
    DBRdim  rdm;
    DBAdim  adm;
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "";

/*
***Get dim ID.
*/
loop:
    WPaddmess_mcwin(IGgtts(268),WP_PROMPT);
    typ = LDMTYP+CDMTYP+ADMTYP+RDMTYP;
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
/*
***In generic mode the dim may not belong to a part.
*/
    if ( sysmode & GENERIC  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Check that dim has digits (auto on).
*/
    DBget_pointer('I',idvek,&la,&typ);

    if ( typ == LDMTYP )
      {
      DBread_ldim(&ldm,NULL,la);
      dauto = ldm.auto_ld;
      tsiz = ldm.tsiz_ld;
      }
    else if ( typ == CDMTYP )
      {
      DBread_cdim(&cdm,NULL,la);
      dauto = cdm.auto_cd;
      tsiz = cdm.tsiz_cd;
      }
    else if ( typ == RDMTYP )
      {
      DBread_rdim(&rdm,NULL,la);
      dauto = rdm.auto_rd;
      tsiz = rdm.tsiz_rd;
      }
    else
      {
      DBread_adim(&adm,NULL,la);
      dauto = adm.auto_ad;
      tsiz = adm.tsiz_ad;
      }

    if ( !dauto )
      {
      erpush("IG3812","");
      errmes();
      goto loop;
      }
/*
***Ask for new digit size.
*/
    sprintf(strbuf,"%s%g  %s",IGgtts(43),tsiz,IGgtts(129));
    IGplma(strbuf,IG_INP);

    pmmark();
    if ( (status=IGcflt(0,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
/*
***Interpret.
*/
    inevev(exnpt,&val,&valtyp);

    if ( val.lit_type == C_INT_VA ) tsiz = val.lit.int_va;
    else tsiz = val.lit.float_va;
/*
***Update.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);
    if ( sysmode & GENERIC ) pmchnp(idvek,PMDTSIZE,exnpt,&retla);

    if ( typ == LDMTYP )
      {
      ldm.tsiz_ld = tsiz;
      DBupdate_ldim(&ldm,la);
      }
    else if ( typ == CDMTYP )
      {
      cdm.tsiz_cd = tsiz;
      DBupdate_cdim(&cdm,la);
      }
    else if ( typ == RDMTYP )
      {
      rdm.tsiz_rd = tsiz;
      DBupdate_rdim(&rdm,la);
      }
    else
      {
      adm.tsiz_ad = tsiz;
      DBupdate_adim(&adm,la);
      }

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Update WPRWIN's.
*/
    WPrepaint_RWIN(RWIN_ALL,FALSE);
/*
***Confirmational message.
*/
    WPaddmess_mcwin(IGgtts(63),WP_MESSAGE);
/*
***Again.
*/
    goto loop;
/*
***The end.
*/
exit:
    WPerhg();
    WPclear_mcwin();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGcdas()

/*      Interactive function to edit dim arrow size.
 *
 *      Return: 0  = OK
 *          REJECT = Exit
 *          GOMAIN = Main menu
 *
 *      Error: IG3502 = The dim belongs to a part
 *
 *      (C)microform ab 17/3/88 J. Kjellander
 *
 *      2007-08-10 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBptr   la;
    DBetype typ;
    DBfloat asiz;
    bool    end,right;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt,retla,valtyp;
    PMREFVA idvek[MXINIV];
    DBLdim  ldm;
    DBCdim  cdm;
    DBRdim  rdm;
    DBAdim  adm;
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "";

/*
***Get dim ID.
*/
loop:
    WPaddmess_mcwin(IGgtts(268),WP_PROMPT);
    typ = LDMTYP+CDMTYP+ADMTYP+RDMTYP;
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
/*
***In generic mode the dim may not belong to a part.
*/
    if ( sysmode & GENERIC  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Get current arrow size.
*/
    DBget_pointer('I',idvek,&la,&typ);

    if ( typ == LDMTYP )
      {
      DBread_ldim(&ldm,NULL,la);
      asiz = ldm.asiz_ld;
      }
    else if ( typ == CDMTYP )
      {
      DBread_cdim(&cdm,NULL,la);
      asiz = cdm.asiz_cd;
      }
    else if ( typ == RDMTYP )
      {
      DBread_rdim(&rdm,NULL,la);
      asiz = rdm.asiz_rd;
      }
    else
      {
      DBread_adim(&adm,NULL,la);
      asiz = adm.asiz_ad;
      }
/*
***Ask for new arrow size.
*/
    sprintf(strbuf,"%s%g  %s",IGgtts(43),asiz,IGgtts(128));
    IGplma(strbuf,IG_INP);

    pmmark();
    if ( (status=IGcflt(0,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
/*
***Interpret.
*/
    inevev(exnpt,&val,&valtyp);

    if ( val.lit_type == C_INT_VA ) asiz = val.lit.int_va;
    else asiz = val.lit.float_va;
/*
***Update.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);
    if ( sysmode & GENERIC ) pmchnp(idvek,PMDASIZE,exnpt,&retla);

    if ( typ == LDMTYP )
      {
      ldm.asiz_ld = asiz;
      DBupdate_ldim(&ldm,la);
      }
    else if ( typ == CDMTYP )
      {
      cdm.asiz_cd = asiz;
      DBupdate_cdim(&cdm,la);
      }
    else if ( typ == RDMTYP )
      {
      rdm.asiz_rd = asiz;
      DBupdate_rdim(&rdm,la);
      }
    else
      {
      adm.asiz_ad = asiz;
      DBupdate_adim(&adm,la);
      }

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Update WPRWIN's.
*/
    WPrepaint_RWIN(RWIN_ALL,FALSE);
/*
***Confirmational message.
*/
    WPaddmess_mcwin(IGgtts(63),WP_MESSAGE);
/*
***Again.
*/
    goto loop;
/*
***The end.
*/
exit:
    WPerhg();
    WPclear_mcwin();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGcdnd()

/*      Interactive function to edit dim ndigs.
 *
 *      Return: 0  = OK
 *          REJECT = Exit
 *          GOMAIN = Main menu
 *
 *      Errors: IG3502 = Dim belongs to a part
 *              IG3822 = Dim has no digits
 *
 *      (C)microform ab 17/3/88 J. Kjellander
 *
 *      2007-08-10 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    tbool   dauto;
    DBptr   la;
    DBetype typ;
    short   ndig;
    bool    end,right;
    char    istr[V3STRLEN+1];
    char    strbuf[V3STRLEN+1];
    pm_ptr  exnpt,retla,valtyp;
    PMREFVA idvek[MXINIV];
    DBLdim  ldm;
    DBCdim  cdm;
    DBRdim  rdm;
    DBAdim  adm;
    PMLITVA val;

    static char dstr[V3STRLEN+1] = "";

/*
***Get dim ID.
*/
loop:
    WPaddmess_mcwin(IGgtts(268),WP_PROMPT);
    typ = LDMTYP+CDMTYP+ADMTYP+RDMTYP;
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
/*
***In generic mode the dim may not belong to a part.
*/
    if ( sysmode & GENERIC  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Check that dim has digits (auto on).
*/
    DBget_pointer('I',idvek,&la,&typ);

    if ( typ == LDMTYP )
      {
      DBread_ldim(&ldm,NULL,la);
      dauto = ldm.auto_ld;
      ndig = ldm.ndig_ld;
      }
    else if ( typ == CDMTYP )
      {
      DBread_cdim(&cdm,NULL,la);
      dauto = cdm.auto_cd;
      ndig = cdm.ndig_cd;
      }
    else if ( typ == RDMTYP )
      {
      DBread_rdim(&rdm,NULL,la);
      dauto = rdm.auto_rd;
      ndig = rdm.ndig_rd;
      }
    else
      {
      DBread_adim(&adm,NULL,la);
      dauto = adm.auto_ad;
      ndig = adm.ndig_ad;
      }

    if ( !dauto )
      {
      erpush("IG3822","");
      errmes();
      goto loop;
      }
/*
***Ask for new digit size.
*/
    sprintf(strbuf,"%s%d  %s",IGgtts(43),ndig,IGgtts(130));
    IGplma(strbuf,IG_INP);

    pmmark();
    if ( (status=IGcint(0,dstr,istr,&exnpt)) < 0 )
      {
      pmrele();
      goto exit;
      }
    strcpy(dstr,istr);
/*
***Interpret.
*/
    inevev(exnpt,&val,&valtyp);

    ndig = val.lit.int_va;
/*
***Update.
*/
    EXdren(la,typ,FALSE,GWIN_ALL);

    if ( sysmode & GENERIC ) pmchnp(idvek,PMDNDIG,exnpt,&retla);

    if ( typ == LDMTYP )
      {
      ldm.ndig_ld = ndig;
      DBupdate_ldim(&ldm,la);
      }
    else if ( typ == CDMTYP )
      {
      cdm.ndig_cd = ndig;
      DBupdate_cdim(&cdm,la);
      }
    else if ( typ == RDMTYP )
      {
      rdm.ndig_rd = ndig;
      DBupdate_rdim(&rdm,la);
      }
    else
      {
      adm.ndig_ad = ndig;
      DBupdate_adim(&adm,la);
      }

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Update WPRWIN's.
*/
    WPrepaint_RWIN(RWIN_ALL,FALSE);
/*
***Confirmational message.
*/
    WPaddmess_mcwin(IGgtts(63),WP_MESSAGE);
/*
***Again.
*/
    goto loop;
/*
***The end.
*/
exit:
    WPerhg();
    WPclear_mcwin();
    return(status);
  }

/********************************************************/
/*!******************************************************/

        short IGcda0()

/*      Interactive function to set Auto off.
 *
 *      (C)microform ab 18/3/88 J. Kjellander
 *
 ******************************************************!*/

   {
   return(igcdau(0));
   }

/********************************************************/
/*!******************************************************/

        short IGcda1()

/*      Interactive function to set auto on.
 *
 *      (C)microform ab 18/3/88 J. Kjellander
 *
 ******************************************************!*/

   {
   return(igcdau(1));
   }

/********************************************************/
/*!******************************************************/

static short igcdau(int dauto)

/*      Edits a dim auto on/off.
 *
 *      In: auto = 0 or 1.
 *
 *      Return: 0  = OK
 *          REJECT = Exit
 *          GOMAIN = Main menu
 *
 *      Error: IG3502 = The dim belongs to a part.
 *
 *      (C)microform ab 25/8/87 J. Kjellander
 *
 *      2007-08-10 1.19, J.Kjellander
 *
 ******************************************************!*/

  {
    short   status;
    DBetype typ;
    DBptr   la;
    bool    end,right;
    pm_ptr  exnpt,retla;
    PMREFVA idvek[MXINIV];
    DBLdim  ldm;
    DBCdim  cdm;
    DBRdim  rdm;
    DBAdim  adm;
    PMLITVA val;

/*
***Get dim ID.
*/
loop:
    WPaddmess_mcwin(IGgtts(268),WP_PROMPT);
    typ = LDMTYP+CDMTYP+RDMTYP+ADMTYP;
    if ( (status=IGgsid(idvek,&typ,&end,&right,(short)0)) < 0 ) goto exit;
/*
***In generic mode the dim may not belong to a part.
*/
    if ( sysmode & GENERIC  &&  idvek[0].p_nextre != NULL )
      {
      erpush("IG3502","");
      errmes();
      goto loop;
      }
/*
***Update.
*/
    DBget_pointer('I',idvek,&la,&typ);

    EXdren(la,typ,FALSE,GWIN_ALL);

    if ( sysmode & GENERIC )
      {
      val.lit_type = C_INT_VA;
      val.lit.int_va = dauto;
      pmclie(&val,&exnpt);
      pmchnp(idvek,PMDAUTO,exnpt,&retla);
      }

    if ( typ == LDMTYP )
      {
      DBread_ldim(&ldm,NULL,la);
      ldm.auto_ld = dauto;
      DBupdate_ldim(&ldm,la);
      }
    else if ( typ == CDMTYP )
      {
      DBread_cdim(&cdm,NULL,la);
      cdm.auto_cd = dauto;
      DBupdate_cdim(&cdm,la);
      }
    else if ( typ == RDMTYP )
      {
      DBread_rdim(&rdm,NULL,la);
      rdm.auto_rd = dauto;
      DBupdate_rdim(&rdm,la);
      }
    else
      {
      DBread_adim(&adm,NULL,la);
      adm.auto_ad = dauto;
      DBupdate_adim(&adm,la);
      }

    EXdren(la,typ,TRUE,GWIN_ALL);
/*
***Update WPRWIN's.
*/
    WPrepaint_RWIN(RWIN_ALL,FALSE);
/*
***Confirmational message.
*/
    WPaddmess_mcwin(IGgtts(63),WP_MESSAGE);
/*
***Again.
*/
    goto loop;
/*
***The end.
*/
exit:
    WPerhg();
    WPclear_mcwin();
    return(status);
  }

/********************************************************/

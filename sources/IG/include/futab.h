/*!******************************************************************/
/*  File: futab.h                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
/*  URL:  http://www.tech.oru.se/cad/varkon                         */
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

/*
***C functions callable from menus, FBUTTON's or FICON's are defined
***in the global function table futab[]. Function numbers are
***also used by the help system to load the right doc page for
***each function. An entry in futab[] is defined as follows:
*/

typedef struct fuattr
{
short (*fnamn)();      /* C function to call */
bool    call;          /* Can be called while other function active */
char    mode;          /* Valid in these modes */
} FUATTR;

/*
***The size of futab[].
*/
#define FTABSIZ 242

/*
***futab[] itself.
*/

static FUATTR futab[] =
{
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f1   WPselect_sysmode() */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f2   IGselect_job() */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f3   - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f4   - */
 {IGrun_active,      FALSE, GENERIC},          /* f5   Run active module*/

 {IGtrim,            FALSE, EXPLICIT+GENERIC}, /* f6   Trim entity */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f7   - */
 {IGrun_named,       FALSE, EXPLICIT+GENERIC}, /* f8   Run a named module */
 {IGcsy_1p,          FALSE, EXPLICIT+GENERIC}, /* f9   Create CSYS_1P */
 {IGdelete_entity,   FALSE, EXPLICIT+GENERIC}, /* f10  Delete entity*/

 {IGcpen,            FALSE, EXPLICIT+GENERIC}, /* f11  Change the PEN of an entity */
 {IGclevel,          FALSE, EXPLICIT+GENERIC}, /* f12  Change the LEVEL of an entity */
 {IGdelete_group,    FALSE, EXPLICIT},         /* f13  Delete group by name */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f14  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f15  - */

 {IGset_actatts,     FALSE, EXPLICIT+GENERIC}, /* f16  Set active attributes */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f17  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f18  - */
 {IGpofr,            FALSE, EXPLICIT+GENERIC}, /* f19  Create POI_FREE */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f20  - */

 {IGpopr,            FALSE, EXPLICIT+GENERIC}, /* f21  Create POI_PROJ */
 {IGlifr,            FALSE, EXPLICIT+GENERIC}, /* f22  Create LIN_FREE */
 {IGliof,            FALSE, EXPLICIT+GENERIC}, /* f23  Create LIN_OFFS */
 {IGlipv,            FALSE, EXPLICIT+GENERIC}, /* f24  Create LIN_ANG */
 {IGpart,            FALSE, EXPLICIT+GENERIC}, /* f25  Create PART */

 {IGbpln,            FALSE, EXPLICIT+GENERIC}, /* f26  Create B_PLANE */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f27  - */
 {IGlipt,            FALSE, EXPLICIT+GENERIC}, /* f28  Create LIN_TAN1 */
 {IGli2t,            FALSE, EXPLICIT+GENERIC}, /* f29  Create LIN_TAN2 */
 {IGld0,             FALSE, EXPLICIT+GENERIC}, /* f30  Create LDIM horizontal */

 {IGld1,             FALSE, EXPLICIT+GENERIC}, /* f31  Create LDIM vertical */
 {IGld2,             FALSE, EXPLICIT+GENERIC}, /* f32  Create LDIM parallell */
 {IGcd0,             FALSE, EXPLICIT+GENERIC}, /* f33  Create CDIM horizontal */
 {IGcd1,             FALSE, EXPLICIT+GENERIC}, /* f34  Create CDIM vertical */
 {IGcd2,             FALSE, EXPLICIT+GENERIC}, /* f35  Create CDIM parallell */

 {IGrdim,            FALSE, EXPLICIT+GENERIC}, /* f36  Create RDIM */
 {IGadim,            FALSE, EXPLICIT+GENERIC}, /* f37  Create ADIM */
 {IGxht,             FALSE, EXPLICIT+GENERIC}, /* f38  Create XHATCH */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f39  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f40  - */

 {IGlipr,            FALSE, EXPLICIT+GENERIC}, /* f41  Create LIN_PROJ */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f42  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f43  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f44  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f45  - */

 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f46  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f47  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f48  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f49  - */
 {IGtext,            FALSE, EXPLICIT+GENERIC}, /* f50  Create TEXT */

 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f51  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f52  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f53  - */
 {IGlipe,            FALSE, EXPLICIT+GENERIC}, /* f54  Create LIN_PERP */
 {IGgrp,             FALSE, EXPLICIT+GENERIC}, /* f55  Create GROUP */

 {IGar1p,            FALSE, EXPLICIT+GENERIC}, /* f56  Create ARC_1POS */
 {IGar2p,            FALSE, EXPLICIT+GENERIC}, /* f57  Create ARC_2POS */
 {IGar3p,            FALSE, EXPLICIT+GENERIC}, /* f58  Create ARC_3POS */
 {IGarof,            FALSE, EXPLICIT+GENERIC}, /* f59  Create ARC_OFFS */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f60  - */

 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f61  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f62  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f63  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f64  - */
 {IGarfl,            FALSE, EXPLICIT+GENERIC}, /* f65  Create ARC_FIL */

 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f66  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f67  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f68  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f69  - */
 {IGcuft,            FALSE, EXPLICIT+GENERIC}, /* f70  Create CUR_SPLINE Ferg w tan */

 {IGcuct,            FALSE, EXPLICIT+GENERIC}, /* f71  Create CUR_SPLINE Chord w tan */
 {IGcuvt,            FALSE, EXPLICIT+GENERIC}, /* f72  Create CUR_SPLINE Stiff w tan */
 {IGcdal,            FALSE, EXPLICIT+GENERIC}, /* f73  Edit *DASHL */
 {IGcfs,             FALSE, EXPLICIT+GENERIC}, /* f74  Set *FONT to solid */
 {IGcomp,            FALSE, EXPLICIT+GENERIC}, /* f75  Create CUR_COMP */

 {IGcfd,             FALSE, EXPLICIT+GENERIC}, /* f76  Set *FONT to dashed */
 {IGcfc,             FALSE, EXPLICIT+GENERIC}, /* f77  Set *FONT to centerline */
 {IGctsz,            FALSE, EXPLICIT+GENERIC}, /* f78  Edit TSIZE */
 {IGctwd,            FALSE, EXPLICIT+GENERIC}, /* f79  Edit TWIDTH */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f80  - */

 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f81  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f82  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f83  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f84  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f85  - */

 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f86  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f87  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f88  - */
 {IGctsl,            FALSE, EXPLICIT+GENERIC}, /* f89  Edit TSLANT */
 {IGslvl,            FALSE, EXPLICIT+GENERIC}, /* f90  Set active level */

 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f91  - */
 {IGexport_DXF2D,    FALSE, EXPLICIT+GENERIC}, /* f92  Export DXF 2D */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f93  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f94  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f95  - */

 {IGspen,            FALSE, EXPLICIT+GENERIC}, /* f96  Set active pen */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f97  - */
 {IGset_cacc,        FALSE, EXPLICIT+GENERIC}, /* f98  Set curve accuracy */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f99  - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f100 - */

 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f101 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f102 WPview_dialog() */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f103 WPgrid_dialog() */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f104 - */
 {IGcsy_3p,          FALSE, EXPLICIT+GENERIC}, /* f105 Create coordinate system with 3 pos */

 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f106 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f107 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f108 - */
 {IGuppt,            FALSE, EXPLICIT+GENERIC}, /* f109 Update part */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f110 - */

 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f111 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f112 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f113 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f114 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f115 - */

 {IGcufn,            FALSE, EXPLICIT+GENERIC}, /* f116 Create CUR_SPLINE Ferg no tan */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f117 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f118 - */
 {IGsave_JOB_as,     FALSE, EXPLICIT+GENERIC}, /* f119 Save JOB-file as */
 {IGnjsn,            FALSE, EXPLICIT+GENERIC}, /* f120 New job, save nothing */

 {IGsave_RES_as,     FALSE, EXPLICIT+GENERIC}, /* f121 Save RES-file as */
 {IGexit_sa,         FALSE, EXPLICIT+GENERIC}, /* f122 Exit and save all */
 {IGexit_sn,         FALSE, EXPLICIT+GENERIC}, /* f123 Exit and save nothing */
 {IGnjsa,            FALSE, EXPLICIT+GENERIC}, /* f124 New job, save all */
 {IGshell,           FALSE, EXPLICIT+GENERIC}, /* f125 Issue a shell command */

 {IGsave_MBO_as,     FALSE, GENERIC},          /* f126 Save MBO-file as */
 {IGcatt,            FALSE, GENERIC},          /* f127 Edit module attribute */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f128 - */
 {IGcdnd,            FALSE, EXPLICIT+GENERIC}, /* f129 Edit DNDIG */
 {IGcda0,            FALSE, EXPLICIT+GENERIC}, /* f130 Set DAUTO off */

 {IGcda1,            FALSE, EXPLICIT+GENERIC}, /* f131 Set DAUTO on */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f132 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f133 ! Hide on display */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f134 ! Hide to file */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f135 ! Both */

 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f136 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f137 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f138 - */
 {IGanpm,            FALSE, EXPLICIT+GENERIC}, /* f139 PM status */
 {IGandb,            FALSE, EXPLICIT+GENERIC}, /* f140 DB status */

 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f141 */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f142 */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f143 */
 {IGsave_MBS_as,     FALSE, GENERIC},          /* f144 Save module in MBS format */
 {IGsymb,            FALSE, EXPLICIT+GENERIC}, /* f145 Import plotfile */

 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f146 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f147 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f148 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f149 - */
 {IGabout_help,      TRUE,  EXPLICIT+GENERIC}, /* f150 Display help about help */

 {IGexport_GKSM,     FALSE, EXPLICIT+GENERIC}, /* f151 Export GKS Metafile */
 {IGmacro,           FALSE, EXPLICIT+GENERIC}, /* f152 Execute a macro */
 {IGhelp,            TRUE,  EXPLICIT+GENERIC}, /* f153 Display help on current context */
 {IGcdts,            FALSE, EXPLICIT+GENERIC}, /* f154 Edit DTSIZE */
 {IGcdas,            FALSE, EXPLICIT+GENERIC}, /* f155 Edit DASIZE */

 {IGcucn,            FALSE, EXPLICIT+GENERIC}, /* f156 Create CUR_SPLINE Chord no tan */
 {IGcuvn,            FALSE, EXPLICIT+GENERIC}, /* f157 Create CUR_SPLINE Stiff no tan */
 {IGsuro,            FALSE, EXPLICIT+GENERIC}, /* f158 Create SUR_ROT */
 {IGsuof,            FALSE, EXPLICIT+GENERIC}, /* f159 Create SUR_OFFS */
 {IGsucy,            FALSE, EXPLICIT+GENERIC}, /* f160 Create SUR_CYL */

 {IGcucf,            FALSE, EXPLICIT+GENERIC}, /* f161 Create CUR_CONIC free */
 {IGcucp,            FALSE, EXPLICIT+GENERIC}, /* f162 Create CUR_CONIC proj */
 {IGcuro,            FALSE, EXPLICIT+GENERIC}, /* f163 Create CUR_OFFS */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f164 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f165 - */

 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f166 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f167 - */
 {IGedst,            FALSE, GENERIC},          /* f168 Edit MBS statement */
 {IGanrf,            FALSE, GENERIC},          /* f169 Analyze references */
 {IGcptw,            FALSE, EXPLICIT+GENERIC}, /* f170 Edit part parameters */

 {IGctfn,            FALSE, EXPLICIT+GENERIC}, /* f171 Edit TFONT */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f172 - */
 {IGexsd,            FALSE, GENERIC},          /* f173 Exit with decompilation */
 {IGnjsd,            FALSE, GENERIC},          /* f174 New job with decompilation */
 {IGsave_all,        FALSE, EXPLICIT+GENERIC}, /* f175 Save everything */

 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f176 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f177 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f178 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f179 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f180 - */

 {IGsusw,            FALSE, EXPLICIT+GENERIC}, /* f181 Create SUR_SWEEP */
 {IGsuru,            FALSE, EXPLICIT+GENERIC}, /* f182 Create SUR_RULED */
 {IGcuri,            FALSE, EXPLICIT+GENERIC}, /* f183 Create CUR_INT */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f184 - */
 {IGcwdt,            FALSE, EXPLICIT+GENERIC}, /* f185 Edit WIDTH */

 {IGswdt,            FALSE, EXPLICIT+GENERIC}, /* f186 Set active WIDTH */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f187 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f188 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f189 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f190 - */

 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f191 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f192 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f193 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f194 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f195 - */

 {IGcreate_gwin,     FALSE, EXPLICIT+GENERIC}, /* f196 Create new WPGWIN */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f197 WPlevel_dialog() */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f198 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f199 - */
 {IGcreate_rwin,     FALSE, EXPLICIT+GENERIC}, /* f200 Create new WPRWIN */

 {IGsave_all_as,     FALSE, EXPLICIT+GENERIC}, /* f201 Save everything as */
 {IGmv1,             FALSE, EXPLICIT},         /* f202 Drag entity */
 {IGrot1,            FALSE, EXPLICIT},         /* f203 Rotate entity */
 {IGcarr,            FALSE, EXPLICIT},         /* f204 Edit arc radius */
 {IGcar1,            FALSE, EXPLICIT},         /* f205 Edit arc start angle */

 {IGcar2,            FALSE, EXPLICIT},         /* f206 Edit arc end angle */
 {IGctxv,            FALSE, EXPLICIT},         /* f207 Edit text angle */
 {IGctxs,            FALSE, EXPLICIT},         /* f208 Edit text */
 {IGmven,            FALSE, EXPLICIT},         /* f209 Move entities */
 {IGcpy1,            FALSE, EXPLICIT},         /* f210 Copy entities */

 {IGmir1,            FALSE, EXPLICIT},         /* f211 Mirror entities */
 {IGchgr,            FALSE, EXPLICIT},         /* f212 Edit group */
 {IGblnk,            FALSE, EXPLICIT},         /* f213 Blank entity */
 {IGubal,            FALSE, EXPLICIT},         /* f214 Unblank all entities */
 {IGhtal,            FALSE, EXPLICIT},         /* f215 Make all selectable */

 {IGedit_active,     FALSE, GENERIC},          /* f216 Edit active module in MBS mode */
 {IGedit_MBS,        FALSE, EXPLICIT+GENERIC}, /* f217 Edit MBS file */
 {IGcff,             FALSE, EXPLICIT+GENERIC}, /* f218 Set *FONT to filled */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f219 - */
 {IGcompile_all,     FALSE, EXPLICIT+GENERIC}, /* f220 Compile all */

 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f221 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f222 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f223 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f224 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f225 - */

 {IGctpm,            FALSE, EXPLICIT+GENERIC}, /* f226 Edit TPMODE */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f227 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f228 - */
 {notimpl,           FALSE, EXPLICIT+GENERIC}, /* f229 - */
 {IGcuis,            FALSE, EXPLICIT+GENERIC}, /* f230 Create CUR_ISO */

 {IGcura,            FALSE, EXPLICIT+GENERIC}, /* f231 Create CUR_APPROX */
 {IGcurt,            FALSE, EXPLICIT+GENERIC}, /* f232 Create CUR_TRIM */
 {IGcusi,            FALSE, EXPLICIT+GENERIC}, /* f233 Create CUR_SIL */
 {IGsurt,            FALSE, EXPLICIT+GENERIC}, /* f234 Create SUR_TRIM */
 {IGsura,            FALSE, EXPLICIT+GENERIC}, /* f235 Create SUR_APPROX */

 {IGsuco,            FALSE, EXPLICIT+GENERIC}, /* f236 Create SUR_COMP */
 {IGsuex,            FALSE, EXPLICIT+GENERIC}, /* f237 Create SUR_EXDEF */
 {IGsulo,            FALSE, EXPLICIT+GENERIC}, /* f238 Create SUR_CONIC */
 {IGtfmo,            FALSE, EXPLICIT+GENERIC}, /* f239 Create TFORM_MOVE */
 {IGtfro,            FALSE, EXPLICIT+GENERIC}, /* f240 Create TFORM_ROTL */

 {IGtfmi,            FALSE, EXPLICIT+GENERIC}, /* f241 Create TFORM_MIRR*/
 {IGtcpy,            FALSE, EXPLICIT+GENERIC}, /* f242 Create TCOPY */
};

/******************************************************!*/

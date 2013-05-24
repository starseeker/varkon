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
***Ett entry i funktionstabellen futab.
*/

typedef struct fuattr
{
short (*fnamn)();      /* C function to call */
tbool snabb;           /* Can be called in other function TRUE/FALSE */
char  modul;           /* Logical AND between XXX_MOD codes */
} FUATTR;

/*
***Följande är unika för WIN32.
*/
#ifdef WIN32
extern short msamod(),msomod(),mscoal(),msneww(),
       mshlp1(),mshlp2();
#endif

/*
***Själva funktionstabellen.
*/

static FUATTR futab[] = 
{
 {IGexit, FALSE, TOP_MOD},                   /* f1   Sluta */
 {notimpl,FALSE, BAS_MOD},                   /* Fd. f2   Skapa parameter */
 {notimpl,FALSE, BAS_MOD},                   /* Fd. f3   Ändra parametervärde */
 {notimpl,FALSE, BAS_MOD},                   /* Fd. f4   Lista parametrar */
 {IGramo, FALSE, BAS_MOD},                   /* f5   Kör aktiv modul */

 {IGtrim, FALSE, RIT_MOD+BAS2_MOD},          /* f6   Trimma storhet */
 {wpunik,  TRUE, RIT_MOD+BAS_MOD},           /* f7   Scale WPRWIN */
 {IGrnmo, FALSE, RIT_MOD+BAS_MOD},           /* f8   Kör namngiven modul */
 {IGcs1p, FALSE, RIT_MOD+BAS2_MOD},          /* f9   Koordinatsystem 1 pos */
 {IGdlen, FALSE, RIT_MOD+BAS_MOD},           /* f10  Ta bort storhet */

 {IGcpen, FALSE, RIT_MOD+BAS_MOD},           /* f11  Ändra penna */
 {IGcniv, FALSE, RIT_MOD+BAS_MOD},           /* f12  Ändra nivå */
 {IGdlgp, FALSE, RIT_MOD+BAS_MOD},           /* f13  Ta bort grupp */
 {IGcs1p, FALSE, BAS3_MOD},                  /* f14  Koordinatsystem 1 pos */
 {IGdlls, FALSE, BAS_MOD},                   /* f15  Ta bort sista sats */

 {WPatdi,  TRUE, RIT_MOD+BAS_MOD},           /* f16 Attribute dialogue */
 {notimpl, TRUE, RIT_MOD+BAS_MOD},           /* Fd. f17  Delning X */
 {notimpl, TRUE, RIT_MOD+BAS_MOD},           /* Fd. f18  Delning Y */
 {IGpofr, FALSE, BAS3_MOD},                  /* f19  Punkt fri */
 {IGpofr, FALSE, RIT_MOD+BAS2_MOD},          /* f20  Punkt */

 {IGpopr, FALSE, BAS3_MOD},                  /* f21  Punkt projicerad */
 {IGlifr, FALSE, BAS3_MOD},                  /* f22  Linje fri */
 {IGliof, FALSE, BAS3_MOD},                  /* f23  Linje parallell */
 {IGlipv, FALSE, BAS3_MOD},                  /* f24  Linje pos. och vinkel */
 {IGpart, FALSE, RIT_MOD+BAS_MOD},           /* f25  Skapa part */

 {IGbpln, FALSE, BAS3_MOD},                  /* f26  B-Plan */
 {IGbpln, FALSE, RIT_MOD+BAS2_MOD},          /* f27  B-Plan */
 {IGlipt, FALSE, BAS3_MOD},                  /* f28  Linje tang. en storhet */
 {IGli2t, FALSE, BAS3_MOD},                  /* f29  Linje tang. 2 cirklar */
 { IGld0, FALSE, RIT_MOD+BAS2_MOD},          /* f30  Längdmått horisontellt */

 { IGld1, FALSE, RIT_MOD+BAS2_MOD},          /* f31  Längdmått vertikalt */
 { IGld2, FALSE, RIT_MOD+BAS2_MOD},          /* f32  Längdmått parallellt */
 { IGcd0, FALSE, RIT_MOD+BAS2_MOD},          /* f33  Diametermått horisont. */
 { IGcd1, FALSE, RIT_MOD+BAS2_MOD},          /* f34  Diametermått vertikalt */
 { IGcd2, FALSE, RIT_MOD+BAS2_MOD},          /* f35  Diametermått parallellt */

 {IGrdim, FALSE, RIT_MOD+BAS2_MOD},          /* f36  Radiemått */
 {IGadim, FALSE, RIT_MOD+BAS2_MOD},          /* f37  Vinkelmått */
 { IGxht, FALSE, RIT_MOD+BAS2_MOD},          /* f38  Snittmarkering */
 {IGlipe, FALSE, RIT_MOD+BAS2_MOD},          /* f39  Linje v-rät mot annan */
 {IGlifr, FALSE, RIT_MOD+BAS2_MOD},          /* f40  Linje mellan 2 pos */

 {IGlipr, FALSE, BAS3_MOD},                  /* f41  Linje projicerad */
 {IGliof, FALSE, RIT_MOD+BAS2_MOD},          /* f42  Linje parallell */
 {IGlipv, FALSE, RIT_MOD+BAS2_MOD},          /* f43  Linje pos. och vinkel */
 {IGlipt, FALSE, RIT_MOD+BAS2_MOD},          /* f44  Linje tang. en storhet */
 {IGli2t, FALSE, RIT_MOD+BAS2_MOD},          /* f45  Linje tangent 2 cirklar */

 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f46  Heldragen linjetyp */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f47  Streckad linjetyp */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f48  Streckprickad linjetyp */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f49  Linjers strecklängd */
 {IGtext, FALSE, RIT_MOD+BAS_MOD},           /* f50  Text */

 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f51  Ändra aktiv texthöjd */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f52  Ändra aktiv textbredd */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f53  Ändra aktiv textlutning */
 {IGlipe, FALSE, BAS3_MOD},                  /* f54  Linje v-rät mot annan */
 { IGgrp, FALSE, RIT_MOD+BAS_MOD},           /* f55  Skapa grupp */

 {IGar1p, FALSE, BAS3_MOD},                  /* f56  Cirkelbåge 1 pos */
 {IGar2p, FALSE, BAS3_MOD},                  /* f57  Cirkelbåge 2 pos */
 {IGar3p, FALSE, BAS3_MOD},                  /* f58  Cirkelbåge 3 pos */
 {IGarof, FALSE, BAS3_MOD},                  /* f59  Cirkelbåge offset */
 {IGar1p, FALSE, RIT_MOD+BAS2_MOD},          /* f60  Cirkelbåge 1 pos */

 {IGar2p, FALSE, RIT_MOD+BAS2_MOD},          /* f61  Cirkelbåge 2 pos */
 {IGar3p, FALSE, RIT_MOD+BAS2_MOD},          /* f62  Cirkelbåge 3 pos */
 {IGarof, FALSE, RIT_MOD+BAS2_MOD},          /* f63  Cirkelbåge offset */
 {IGarfl, FALSE, RIT_MOD+BAS2_MOD},          /* f64  Hörnradie */
 {IGarfl, FALSE, BAS3_MOD},                  /* f65  Hörnradie */

 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f66  Heldragen cirkeltyp */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f67  Streckad cirkeltyp */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f68  Streckprickad cirkeltyp */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f69  Cirklars strecklängd */
 {IGcuft, FALSE, RIT_MOD+BAS_MOD},           /* f70  Ferguson med tang */

 {IGcuct, FALSE, RIT_MOD+BAS_MOD},           /* f71  Chord med tang */
 {IGcuvt, FALSE, RIT_MOD+BAS_MOD},           /* f72  Stiffness med tang */
 {IGcdal, FALSE, RIT_MOD+BAS_MOD},           /* f73  Ändra strecklängd */
 { IGcfs, FALSE, RIT_MOD+BAS_MOD},           /* f74  Ändra till heldragen */
 {IGcomp, FALSE, RIT_MOD+BAS_MOD},           /* f75  Sammansatt kurva */

 { IGcfd, FALSE, RIT_MOD+BAS_MOD},           /* f76  Ändra till streckad */
 { IGcfc, FALSE, RIT_MOD+BAS_MOD},           /* f77  Ändra till streckprickad */
 {IGctsz, FALSE, RIT_MOD+BAS_MOD},           /* f78  Ändra texthöjd */
 {IGctwd, FALSE, RIT_MOD+BAS_MOD},           /* f79  Ändra textbredd */
 {notimpl,FALSE, RIT_MOD+BAS2_MOD},          /* f80  Fd. Ändra aktiv sifferstorl */

 {notimpl,FALSE, RIT_MOD+BAS2_MOD},          /* Fd. f81  Ändra aktiv pilstorlek */
 {notimpl,FALSE, RIT_MOD+BAS2_MOD},          /* Fd. f82  Ändra aktiv antal dec */
 {notimpl,FALSE, RIT_MOD+BAS2_MOD},          /* Fd. f83  Auto på */
 {notimpl,FALSE, RIT_MOD+BAS2_MOD},          /* Fd. f84  Ej auto */
 {notimpl,FALSE, RIT_MOD+BAS2_MOD},          /* Fd. f85  Heldragen snittyp */

 {notimpl,FALSE, RIT_MOD+BAS2_MOD},          /* Fd. f86  Streckad snittyp */
 {notimpl,FALSE, RIT_MOD+BAS2_MOD},          /* Fd. f87  Streckprickad snittyp */
 {notimpl,FALSE, RIT_MOD+BAS2_MOD},          /* Fd. f88  Snittlinjers längd */
 {IGctsl, FALSE, RIT_MOD+BAS_MOD},           /* f89  Ändra textlutning */
 {IGslvl, FALSE, RIT_MOD+BAS_MOD},           /* f90  Byt aktiv nivå */

 {wpunik,  TRUE, RIT_MOD+BAS_MOD},           /* f91  Pan WPRWIN */
 {IGcdxf, FALSE, RIT_MOD+BAS_MOD},           /* f92  Skriv ut DXF-fil */
 {wpunik,  TRUE, RIT_MOD+BAS_MOD},           /* f93  Rotate WPRWIN */
 {wpunik,  TRUE, RIT_MOD+BAS_MOD},           /* f94  Perspective WPRWIN */
 {wpunik,  TRUE, RIT_MOD+BAS_MOD},           /* f95  Light WPRWIN */

 {IGspen, FALSE, RIT_MOD+BAS_MOD},           /* f96  Byt aktivt pennummer */
 {wpunik,  TRUE, RIT_MOD+BAS_MOD},           /* f97  Clip WPRWIN */
 {IGcnog,  TRUE, RIT_MOD+BAS_MOD},           /* f98  Kurvnoggrannhet */
 {notimpl,FALSE, BAS3_MOD},                  /* Fd. f99  Skapa vy med ksys */
 {wpunik,  TRUE, RIT_MOD+BAS_MOD},           /* f100 Wireframe mode WPRWIN */

 {wpunik,  TRUE, RIT_MOD+BAS_MOD},           /* f101 Shaded mode WPRWIN */
 {wpunik,  TRUE, RIT_MOD+BAS_MOD},           /* f102 The view dialouge */
 {wpunik,  TRUE, RIT_MOD+BAS_MOD},           /* f103 The grid dialogue */
 {IGcs3p, FALSE, BAS3_MOD},                  /* f104 Koordinatsystem */
 {IGcs3p, FALSE, RIT_MOD+BAS2_MOD},          /* f105 Koordinatsystem */

 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f106 Aktivera lokalt ksys */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f107 Aktivera globala */
 {notimpl, TRUE, RIT_MOD+BAS_MOD},           /* Fd. f108 Ritningsskala */
 {IGuppt, FALSE, RIT_MOD+BAS_MOD},           /* f109 Updatera part */
 {notimpl, TRUE, RIT_MOD+BAS_MOD+NO_X11_MOD},/* Fd. f110 Byt vy */

 {notimpl, TRUE, RIT_MOD+BAS_MOD},           /* Fd. f111 Skapa vy */
 {notimpl, TRUE, RIT_MOD+BAS_MOD},           /* Fd. f112 Ta bort vy */
 {notimpl, TRUE, RIT_MOD+BAS_MOD},           /* Fd. f113 Lista vyer */
 {notimpl, TRUE, RIT_MOD+BAS_MOD},           /* Fd. f114 Raster */
 {notimpl, TRUE, RIT_MOD+BAS_MOD},           /* Fd. f115 Släck raster */

 {IGcufn, FALSE, RIT_MOD+BAS_MOD},           /* f116 Ferguson utan tang. */
 {notimpl, TRUE, RIT_MOD+BAS_MOD},           /* Fd. f117 Pos-menyn av */
 {notimpl, TRUE, RIT_MOD+BAS_MOD},           /* Fd. f118 Pos-menyn på */
 {IGsjbn, FALSE, RIT_MOD+BAS_MOD},           /* f119 Lagra jobfil */
 {IGnjsn, FALSE, RIT_MOD+BAS_MOD},           /* f120 Lagra ej och nytt job */

 {IGsgmn, FALSE, RIT_MOD+BAS_MOD},           /* f121 Lagra resultatfil */
 {IGexsa, FALSE, RIT_MOD+BAS_MOD},           /* f122 Lagra allt och sluta */
 {IGexsn, FALSE, RIT_MOD+BAS_MOD},           /* f123 Lagra ej och sluta */
 {IGnjsa, FALSE, RIT_MOD+BAS_MOD},           /* f124 Lagra allt och nytt job */
 {IGshll,  TRUE, RIT_MOD+BAS_MOD},           /* f125 Kommando till OS */

 {IGspmn, FALSE, BAS_MOD},                   /* f126 Lagra modul */
 {IGcatt, FALSE, BAS_MOD},                   /* f127 Ändra modulens attribut */
 {notimpl,FALSE, BAS_MOD},                   /* Fd. f128 Ändra modulens skyddskod */
 {IGcdnd, FALSE, RIT_MOD+BAS2_MOD},          /* f129 Ändra antal dec. i mått */
 {IGcda0, FALSE, RIT_MOD+BAS2_MOD},          /* f130 Ändra mått auto av */

 {IGcda1, FALSE, BAS2_MOD},                  /* f131 Ändra mått auto på */
 {notimpl,FALSE, BAS3_MOD+NO_X11_MOD},       /* Fd. f132 Perspektiv */
 {IGhid1,  TRUE, BAS3_MOD},                  /* f133 Dolda konturer skärm */
 {IGhid2,  TRUE, BAS3_MOD},                  /* f134 Dolda konturer fil */
 {IGhid3,  TRUE, BAS3_MOD},                  /* f135 Dolda konturer båda */

 {notimpl,FALSE, RIT_MOD+BAS2_MOD},          /* Fd. f136 Kalibrera digitizer */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f137 Status */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f138 Ej status */
 {IGanpm, FALSE, RIT_MOD+BAS_MOD},           /* f139 Analysera PM */
 {IGangm, FALSE, RIT_MOD+BAS_MOD},           /* f140 Analysera GM */

 {IGlitb,  TRUE, RIT_MOD+BAS_MOD},           /* f141 Lista ID-tab */
 {IGrgmp,  TRUE, RIT_MOD+BAS_MOD},           /* f142 Läs i GM */
 {IGlsyd,  TRUE, RIT_MOD+BAS_MOD},           /* f143 Systemdata */
 {IGprtm, FALSE, BAS_MOD},                   /* f144 Lista modul till fil */
 {IGsymb, FALSE, RIT_MOD+BAS2_MOD},          /* f145 Läs in plotfil */

 {notimpl,FALSE, BAS_MOD},                   /* Fd. f146 Lista modul till skärm */
 {notimpl, TRUE, RIT_MOD+BAS_MOD},           /* Fd. f147 Hela menyer */
 {notimpl, TRUE, RIT_MOD+BAS_MOD},           /* Fd. f148 Bara rubriker */
 {notimpl, TRUE, RIT_MOD+BAS_MOD},           /* Fd. f149 Inga menyer */
 {wpunik,  TRUE, RIT_MOD+BAS_MOD},           /* f150 The print dialogue */

 {IGcgkm,  TRUE, RIT_MOD+BAS_MOD},           /* f151 Skapa plotfil */
 {IGmfun, FALSE, RIT_MOD+BAS_MOD},           /* f152 Kör MACRO */
 {IGhelp,  TRUE, TOP_MOD+RIT_MOD+BAS_MOD},   /* f153 Hjälp */
 {IGcdts, FALSE, RIT_MOD+BAS2_MOD},          /* f154 Ändra mått siff.storl. */
 {IGcdas, FALSE, RIT_MOD+BAS2_MOD},          /* f155 Ändra mått pilstorl. */

 {IGcucn, FALSE, RIT_MOD+BAS_MOD},           /* f156 Chord utan tang. */
 {IGcuvn, FALSE, RIT_MOD+BAS_MOD},           /* f157 Stiffness utan tang. */
 {IGsuro, FALSE, BAS3_MOD},                  /* f158 Rotationsyta */
 {IGsuof, FALSE, BAS3_MOD},                  /* f159 Offsetyta */
 {IGsucy, FALSE, BAS3_MOD},                  /* f160 Cylinderyta */

 {IGcucf, FALSE, RIT_MOD+BAS_MOD},           /* f161 Kägelsnitt fri */
 {IGcucp, FALSE, BAS3_MOD},                  /* f162 Kägelsnitt proj */
 {IGcuro, FALSE, RIT_MOD+BAS_MOD},           /* f163 Offset kurva */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f164 Heldragen kurvtyp */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f165 Streckad kurvtyp */

 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f166 Streckprickad kurvtyp */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f167 Kurvors strecklängd */
 {IGedst, FALSE, BAS_MOD},                   /* f168 Editera sats */
 {IGanrf, FALSE, BAS_MOD},                   /* f169 Analysera referenser */
 {IGcptw, FALSE, RIT_MOD+BAS_MOD},           /* f170 X11/WIN32-ändra part */

 {IGctfn, FALSE, RIT_MOD+BAS_MOD},           /* f171 Ändra text:s font */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f172 Sätt aktiv textfont */
 {IGexsd, FALSE, BAS_MOD},                   /* f173 Sluta med dekomp. */
 {IGnjsd, FALSE, BAS_MOD},                   /* f174 Nytt jobb med dekomp. */
 {IGsjsa, FALSE, RIT_MOD+BAS_MOD},           /* f175 Lagra allt sluta ej */

 {IGchpr, FALSE, RIT_MOD+BAS_MOD},           /* f176 Byt projekt */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f177 Aktivera basic */
 {IGlspr, FALSE, RIT_MOD+BAS_MOD},           /* f178 Lista projekt */
 {IGdlpr, FALSE, RIT_MOD+BAS_MOD},           /* f179 Ta bort projekt */
 {IGlsjb, FALSE, RIT_MOD+BAS_MOD},           /* f180 Lista jobb */

 {IGsusw, FALSE, BAS3_MOD},                  /* f181 Svept yta */
 {IGsuru, FALSE, BAS3_MOD},                  /* f182 Regelyta */
 {IGcuri, FALSE, BAS3_MOD},                  /* f183 Skärningskurva */
 {IGdljb, FALSE, RIT_MOD+BAS_MOD},           /* f184 Ta bort jobb */
 {IGcwdt, FALSE, RIT_MOD+BAS_MOD},           /* f185 Ändra storhets bredd */

 {IGswdt, FALSE, RIT_MOD+BAS_MOD},           /* f186 Storheters bredd */
 {IGmvrr, FALSE, TOP_MOD},                   /* f187 Kopiera RES->RIT */
 {wpunik, FALSE, NONE_MOD},                  /* f188 X-Ändra skala */
 {wpunik, FALSE, NONE_MOD},                  /* f189 X-Panorera */
 {wpunik, FALSE, NONE_MOD},                  /* f190 X-Perspektiv */

 {wpunik, FALSE, NONE_MOD},                  /* f191 X-Föregående vy */
 {wpunik, FALSE, NONE_MOD},                  /* f192 X-version av status */
 {wpunik, FALSE, NONE_MOD},                  /* f193 X-version av ZOOM */
 {wpunik, FALSE, NONE_MOD},                  /* f194 X-version av AutoZOOM */
 {notimpl,FALSE, NONE_MOD},                  /* Fd. f195 X-Byt vy */
#ifdef WIN32
 {msneww, TRUE,  RIT_MOD+BAS_MOD},           /* f196 WIN32-Skapa nytt fönster */
#else
 {WPneww, TRUE,  RIT_MOD+BAS_MOD+X11_MOD},   /* f196 X-Skapa nytt fönster */
#endif
 {wpunik, FALSE, NONE_MOD},                  /* f197 The levels dialogue */
 {notimpl,TRUE,  BAS3_MOD},                  /* Fd. f198 Flat shading */
 {notimpl,TRUE,  BAS3_MOD},                  /* Fd. f199 Smooth shading */
 {IGrenw, FALSE, NONE_MOD},                  /* f200 Render */

 {IGsaln, FALSE, RIT_MOD+BAS_MOD},           /* f201 Lagra allt med nytt namn */
 { IGmv1, FALSE, RIT_MOD},                   /* f202 Dra storhet */
 {IGrot1, FALSE, RIT_MOD},                   /* f203 Rotera storhet */
 {IGcarr, FALSE, RIT_MOD},                   /* f204 Ändra cirkel radie */
 {IGcar1, FALSE, RIT_MOD},                   /* f205 Ändra startvinkel*/

 {IGcar2, FALSE, RIT_MOD},                   /* f206 Ändra slutvinkel */
 {IGctxv, FALSE, RIT_MOD},                   /* f207 Ändra text vinkel */
 {IGctxs, FALSE, RIT_MOD},                   /* f208 Ändra text */
 {IGmven, FALSE, RIT_MOD},                   /* f209 Flytta storheter */
 {IGcpy1, FALSE, RIT_MOD},                   /* f210 Kopiera storheter */

 {IGmir1, FALSE, RIT_MOD},                   /* f211 Spegla storhet */
 {IGchgr, FALSE, RIT_MOD},                   /* f212 Ändra grupp */
 {IGblnk, FALSE, RIT_MOD},                   /* f213 Släck storhet */
 {IGubal, FALSE, RIT_MOD},                   /* f214 Tänd alla släckta */
 {IGhtal, FALSE, RIT_MOD},                   /* f215 Gör allt pekbart */

/*
***Några funktioner som har samma nummer i WIN32
***och X11 men i själva verket är olika.
*/
#ifdef WIN32

{ msamod, FALSE, BAS_MOD},                   /* f216 MBS:a aktiv modul WIN32 */
{ msomod, FALSE, BAS_MOD},                   /* f217 MBS:a annan modul WIN32 */
{notimpl, FALSE, NONE_MOD},                  /* f218  */
{notimpl, FALSE, NONE_MOD},                  /* f219  */
{ mscoal, FALSE, BAS_MOD},                   /* f220 Kompilera alla WIN32 */

{ mshlp1, TRUE,  RIT_MOD+BAS_MOD},           /* f221 Hjälp om hjälp WIN32 */
{ mshlp2, TRUE,  RIT_MOD+BAS_MOD},           /* f222 Hjälpindex     WIN32 */
{notimpl, FALSE, NONE_MOD},                  /* f223  */
{notimpl, FALSE, NONE_MOD},                  /* f224  */
{notimpl, FALSE, NONE_MOD},                  /* f225  */

#else

{ WPamod, FALSE, BAS_MOD+X11_MOD},           /* f216 MBS:a aktiv modul X11 */
{ WPomod, FALSE, BAS_MOD+X11_MOD},           /* f217 MBS:a annan modul X11 */
 { IGcff, FALSE, RIT_MOD+BAS_MOD},           /* f218 Edit entity font = 3 */
{notimpl, FALSE, NONE_MOD},                  /* f219  */
{ WPcoal, FALSE, BAS_MOD+X11_MOD},           /* f220 Kompilera alla X11 */

{notimpl,  TRUE, NONE_MOD},                  /* f221 Delete WPRWIN */
{notimpl, FALSE, NONE_MOD},                  /* f222  */
{notimpl, FALSE, NONE_MOD},                  /* f223  */
{notimpl, FALSE, NONE_MOD},                  /* f224  */
{notimpl, FALSE, NONE_MOD},                  /* f225  */

#endif

{ IGctpm, FALSE, BAS3_MOD},                  /* f226 Ändra texts TPMODE */
{notimpl, FALSE, BAS3_MOD},                  /* Fd. f227 Sätt aktivt TPMODE */
{notimpl, FALSE, NONE_MOD},                  /* f228  */
{notimpl, FALSE, NONE_MOD},                  /* f229  */
{ IGcuis, FALSE, BAS3_MOD},                  /* f230 Isoparameter kurva */

{ IGcura, FALSE, BAS2_MOD+BAS3_MOD},         /* f231 Approximera kurva */
{ IGcurt, FALSE, BAS2_MOD+BAS3_MOD},         /* f232 Trimma kurva */
{ IGcusi, FALSE, BAS3_MOD},                  /* f233 Silhuette kurva */
{ IGsurt, FALSE, BAS3_MOD},                  /* f234 Trimma yta */
{ IGsura, FALSE, BAS3_MOD},                  /* f235 Approximera yta */

{ IGsuco, FALSE, BAS3_MOD},                  /* f236 Sammansatt yta */
{ IGsuex, FALSE, BAS3_MOD},                  /* f237 Importerad yta */
{ IGsulo, FALSE, BAS3_MOD},                  /* f238 Loftad yta */
{ IGtfmo, FALSE, BAS3_MOD},                  /* f239 Translation */
{ IGtfro, FALSE, BAS3_MOD},                  /* f240 Rotation */

{ IGtfmi, FALSE, BAS3_MOD},                  /* f241 Spegling */
{ IGtcpy, FALSE, BAS3_MOD},                  /* f242 Kopiering */

};

/*
***Kom ihåg att sätta futab's storlek till rätt värde nedan om
***storleken ändras.
*/
#define FTABSIZ 242

/******************************************************!*/

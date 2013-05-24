/*!******************************************************************/
/*  File: futab.h                                                   */
/*  =============                                                   */
/*                                                                  */
/*  This file is part of the VARKON IG Library.                     */
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
/********************************************************************/

/*
***Ett entry i funktionstabellen futab.
*/

typedef struct fuattr
{
short (*fnamn)();      /* Funktionens namn */
tbool snabb;           /* Kan anropas som snabbval TRUE/FALSE */
char  modul;           /* Logiskt AND mellan XXX_MOD-koder. */
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
 {v3exit, FALSE, TOP_MOD},                   /* f1   Sluta */
 {crepar, FALSE, BAS_MOD},                   /* f2   Skapa parameter */
 {chapar, FALSE, BAS_MOD},                   /* f3   Ändra parametervärde */
 {lstpar, FALSE, BAS_MOD},                   /* f4   Lista parametrar */
 {igramo, FALSE, BAS_MOD},                   /* f5   Kör aktiv modul */

 {trimpm, FALSE, RIT_MOD+BAS2_MOD},          /* f6   Trimma storhet */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f7   Ändra part, VT100 */
 {igrnmo, FALSE, RIT_MOD+BAS_MOD},           /* f8   Kör namngiven modul */
 {cs1ppm, FALSE, RIT_MOD+BAS2_MOD},          /* f9   Koordinatsystem 1 pos */
 {igdlen, FALSE, RIT_MOD+BAS_MOD},           /* f10  Ta bort storhet */

 {igcpen, FALSE, RIT_MOD+BAS_MOD},           /* f11  Ändra penna */
 {igcniv, FALSE, RIT_MOD+BAS_MOD},           /* f12  Ändra nivå */
 {igdlgp, FALSE, RIT_MOD+BAS_MOD},           /* f13  Ta bort grupp */
 {cs1ppm, FALSE, BAS3_MOD},                  /* f14  Koordinatsystem 1 pos */
 {igdlls, FALSE, BAS_MOD},                   /* f15  Ta bort sista sats */

 {igcror,  TRUE, RIT_MOD+BAS_MOD},           /* f16  Raster läge */
 {igcrdx,  TRUE, RIT_MOD+BAS_MOD},           /* f17  Delning X */
 {igcrdy,  TRUE, RIT_MOD+BAS_MOD},           /* f18  Delning Y */
 {pofrpm, FALSE, BAS3_MOD},                  /* f19  Punkt fri */
 {pofrpm, FALSE, RIT_MOD+BAS2_MOD},          /* f20  Punkt */

 {poprpm, FALSE, BAS3_MOD},                  /* f21  Punkt projicerad */
 {lifrpm, FALSE, BAS3_MOD},                  /* f22  Linje fri */
 {liofpm, FALSE, BAS3_MOD},                  /* f23  Linje parallell */
 {lipvpm, FALSE, BAS3_MOD},                  /* f24  Linje pos. och vinkel */
 {partpm, FALSE, RIT_MOD+BAS_MOD},           /* f25  Skapa part */

 {bplnpm, FALSE, BAS3_MOD},                  /* f26  B-Plan */
 {bplnpm, FALSE, RIT_MOD+BAS2_MOD},          /* f27  B-Plan */
 {liptpm, FALSE, BAS3_MOD},                  /* f28  Linje tang. en storhet */
 {li2tpm, FALSE, BAS3_MOD},                  /* f29  Linje tang. 2 cirklar */
 { ld0pm, FALSE, RIT_MOD+BAS2_MOD},          /* f30  Längdmått horisontellt */

 { ld1pm, FALSE, RIT_MOD+BAS2_MOD},          /* f31  Längdmått vertikalt */
 { ld2pm, FALSE, RIT_MOD+BAS2_MOD},          /* f32  Längdmått parallellt */
 { cd0pm, FALSE, RIT_MOD+BAS2_MOD},          /* f33  Diametermått horisont. */
 { cd1pm, FALSE, RIT_MOD+BAS2_MOD},          /* f34  Diametermått vertikalt */
 { cd2pm, FALSE, RIT_MOD+BAS2_MOD},          /* f35  Diametermått parallellt */

 {rdimpm, FALSE, RIT_MOD+BAS2_MOD},          /* f36  Radiemått */
 {adimpm, FALSE, RIT_MOD+BAS2_MOD},          /* f37  Vinkelmått */
 { xhtpm, FALSE, RIT_MOD+BAS2_MOD},          /* f38  Snittmarkering */
 {lipepm, FALSE, RIT_MOD+BAS2_MOD},          /* f39  Linje v-rät mot annan */
 {lifrpm, FALSE, RIT_MOD+BAS2_MOD},          /* f40  Linje mellan 2 pos */

 {liprpm, FALSE, BAS3_MOD},                  /* f41  Linje projicerad */
 {liofpm, FALSE, RIT_MOD+BAS2_MOD},          /* f42  Linje parallell */
 {lipvpm, FALSE, RIT_MOD+BAS2_MOD},          /* f43  Linje pos. och vinkel */
 {liptpm, FALSE, RIT_MOD+BAS2_MOD},          /* f44  Linje tang. en storhet */
 {li2tpm, FALSE, RIT_MOD+BAS2_MOD},          /* f45  Linje tangent 2 cirklar */

 {igslfs, FALSE, RIT_MOD+BAS_MOD},           /* f46  Heldragen linjetyp */
 {igslfd, FALSE, RIT_MOD+BAS_MOD},           /* f47  Streckad linjetyp */
 {igslfc, FALSE, RIT_MOD+BAS_MOD},           /* f48  Streckprickad linjetyp */
 {igsldl, FALSE, RIT_MOD+BAS_MOD},           /* f49  Linjers strecklängd */
 {textpm, FALSE, RIT_MOD+BAS_MOD},           /* f50  Text */

 {igstsz, FALSE, RIT_MOD+BAS_MOD},           /* f51  Ändra aktiv texthöjd */
 {igstwd, FALSE, RIT_MOD+BAS_MOD},           /* f52  Ändra aktiv textbredd */
 {igstsl, FALSE, RIT_MOD+BAS_MOD},           /* f53  Ändra aktiv textlutning */
 {lipepm, FALSE, BAS3_MOD},                  /* f54  Linje v-rät mot annan */
 { grppm, FALSE, RIT_MOD+BAS_MOD},           /* f55  Skapa grupp */

 {ar1ppm, FALSE, BAS3_MOD},                  /* f56  Cirkelbåge 1 pos */
 {ar2ppm, FALSE, BAS3_MOD},                  /* f57  Cirkelbåge 2 pos */
 {ar3ppm, FALSE, BAS3_MOD},                  /* f58  Cirkelbåge 3 pos */
 {arofpm, FALSE, BAS3_MOD},                  /* f59  Cirkelbåge offset */
 {ar1ppm, FALSE, RIT_MOD+BAS2_MOD},          /* f60  Cirkelbåge 1 pos */

 {ar2ppm, FALSE, RIT_MOD+BAS2_MOD},          /* f61  Cirkelbåge 2 pos */
 {ar3ppm, FALSE, RIT_MOD+BAS2_MOD},          /* f62  Cirkelbåge 3 pos */
 {arofpm, FALSE, RIT_MOD+BAS2_MOD},          /* f63  Cirkelbåge offset */
 {arflpm, FALSE, RIT_MOD+BAS2_MOD},          /* f64  Hörnradie */
 {arflpm, FALSE, BAS3_MOD},                  /* f65  Hörnradie */

 {igsafs, FALSE, RIT_MOD+BAS_MOD},           /* f66  Heldragen cirkeltyp */
 {igsafd, FALSE, RIT_MOD+BAS_MOD},           /* f67  Streckad cirkeltyp */
 {igsafc, FALSE, RIT_MOD+BAS_MOD},           /* f68  Streckprickad cirkeltyp */
 {igsadl, FALSE, RIT_MOD+BAS_MOD},           /* f69  Cirklars strecklängd */
 {cuftpm, FALSE, RIT_MOD+BAS_MOD},           /* f70  Ferguson med tang */

 {cuctpm, FALSE, RIT_MOD+BAS_MOD},           /* f71  Chord med tang */
 {cuvtpm, FALSE, RIT_MOD+BAS_MOD},           /* f72  Stiffness med tang */
 {igcdal, FALSE, RIT_MOD+BAS_MOD},           /* f73  Ändra strecklängd */
 { igcfs, FALSE, RIT_MOD+BAS_MOD},           /* f74  Ändra till heldragen */
 {comppm, FALSE, RIT_MOD+BAS_MOD},           /* f75  Sammansatt kurva */

 { igcfd, FALSE, RIT_MOD+BAS_MOD},           /* f76  Ändra till streckad */
 { igcfc, FALSE, RIT_MOD+BAS_MOD},           /* f77  Ändra till streckprickad */
 {igctsz, FALSE, RIT_MOD+BAS_MOD},           /* f78  Ändra texthöjd */
 {igctwd, FALSE, RIT_MOD+BAS_MOD},           /* f79  Ändra textbredd */
 {igsdts, FALSE, RIT_MOD+BAS2_MOD},          /* f80  Ändra aktiv sifferstorl */

 {igsdas, FALSE, RIT_MOD+BAS2_MOD},          /* f81  Ändra aktiv pilstorlek */
 {igsdnd, FALSE, RIT_MOD+BAS2_MOD},          /* f82  Ändra aktiv antal dec */
 {igsda1, FALSE, RIT_MOD+BAS2_MOD},          /* f83  Auto på */
 {igsda0, FALSE, RIT_MOD+BAS2_MOD},          /* f84  Ej auto */
 {igsxfs, FALSE, RIT_MOD+BAS2_MOD},          /* f85  Heldragen snittyp */

 {igsxfd, FALSE, RIT_MOD+BAS2_MOD},          /* f86  Streckad snittyp */
 {igsxfc, FALSE, RIT_MOD+BAS2_MOD},          /* f87  Streckprickad snittyp */
 {igsxdl, FALSE, RIT_MOD+BAS2_MOD},          /* f88  Snittlinjers längd */
 {igctsl, FALSE, RIT_MOD+BAS_MOD},           /* f89  Ändra textlutning */
 {igslvl, FALSE, RIT_MOD+BAS_MOD},           /* f90  Byt aktiv nivå */

 {notimpl, TRUE, RIT_MOD+BAS_MOD+NO_X11_MOD},/* Fd. f91 Föregående vy */
 {igcdxf, FALSE, RIT_MOD+BAS_MOD},           /* f92  Skriv ut DXF-fil */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f93  Namnge nivå */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f94  Lista nivåer */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f95  Ta bort nivånamn */

 {igspen, FALSE, RIT_MOD+BAS_MOD},           /* f96  Byt aktivt pennummer */
 {notimpl, TRUE, RIT_MOD+BAS_MOD+NO_X11_MOD},/* f97  Fd. Auto-ZOOM */
 {igcnog,  TRUE, RIT_MOD+BAS_MOD},           /* f98  Kurvnoggrannhet */
 {igcrvc, FALSE, BAS3_MOD},                  /* f99  Skapa vy med ksys */
 {notimpl, TRUE, RIT_MOD+BAS_MOD+NO_X11_MOD},/* Fd. f100 ZOOM */

 {notimpl, TRUE, RIT_MOD+BAS_MOD+NO_X11_MOD},/* Fd. f101 Skala */
 {notimpl, TRUE, RIT_MOD+BAS_MOD+NO_X11_MOD},/* Fd. f102 Centrum */
 {notimpl, TRUE, RIT_MOD+BAS_MOD+NO_X11_MOD},/* Fd. f103 Generera bild */
 {cs3ppm, FALSE, BAS3_MOD},                  /* f104 Koordinatsystem */
 {cs3ppm, FALSE, RIT_MOD+BAS2_MOD},          /* f105 Koordinatsystem */

 {modlpm, FALSE, RIT_MOD+BAS_MOD},           /* f106 Aktivera lokalt ksys */
 {modgpm, FALSE, RIT_MOD+BAS_MOD},           /* f107 Aktivera globala */
 {notimpl, TRUE, RIT_MOD+BAS_MOD},           /* Fd. f108 Ritningsskala */
 {iguppt, FALSE, RIT_MOD+BAS_MOD},           /* f109 Updatera part */
 {notimpl, TRUE, RIT_MOD+BAS_MOD+NO_X11_MOD},/* Fd. f110 Byt vy */

 {igcrvp,  TRUE, RIT_MOD+BAS_MOD},           /* f111 Skapa vy */
 {dlview,  TRUE, RIT_MOD+BAS_MOD},           /* f112 Ta bort vy */
 {notimpl, TRUE, RIT_MOD+BAS_MOD},           /* Fd. f113 Lista vyer */
 {igtndr,  TRUE, RIT_MOD+BAS_MOD},           /* f114 Tänd raster */
 {igslkr,  TRUE, RIT_MOD+BAS_MOD},           /* f115 Släck raster */

 {cufnpm, FALSE, RIT_MOD+BAS_MOD},           /* f116 Ferguson utan tang. */
 {igpmof,  TRUE, RIT_MOD+BAS_MOD},           /* f117 Pos-menyn av */
 {igpmon,  TRUE, RIT_MOD+BAS_MOD},           /* f118 Pos-menyn på */
 {igsjbn, FALSE, RIT_MOD+BAS_MOD},           /* f119 Lagra jobfil */
 {ignjsn, FALSE, RIT_MOD+BAS_MOD},           /* f120 Lagra ej och nytt job */

 {igsgmn, FALSE, RIT_MOD+BAS_MOD},           /* f121 Lagra resultatfil */
 {igexsa, FALSE, RIT_MOD+BAS_MOD},           /* f122 Lagra allt och sluta */
 {igexsn, FALSE, RIT_MOD+BAS_MOD},           /* f123 Lagra ej och sluta */
 {ignjsa, FALSE, RIT_MOD+BAS_MOD},           /* f124 Lagra allt och nytt job */
 {igshll,  TRUE, RIT_MOD+BAS_MOD},           /* f125 Kommando till OS */

 {igspmn, FALSE, BAS_MOD},                   /* f126 Lagra modul */
 {igcatt, FALSE, BAS_MOD},                   /* f127 Ändra modulens attribut */
 {igcmpc, FALSE, BAS_MOD},                   /* f128 Ändra modulens skyddskod */
 {igcdnd, FALSE, RIT_MOD+BAS2_MOD},          /* f129 Ändra antal dec. i mått */
 {igcda0, FALSE, RIT_MOD+BAS2_MOD},          /* f130 Ändra mått auto av */

 {igcda1, FALSE, BAS2_MOD},                  /* f131 Ändra mått auto på */
 {notimpl,FALSE, BAS3_MOD+NO_X11_MOD},       /* Fd. f132 Perspektiv */
 {ighid1,  TRUE, BAS3_MOD},                  /* f133 Dolda konturer skärm */
 {ighid2,  TRUE, BAS3_MOD},                  /* f134 Dolda konturer fil */
 {ighid3,  TRUE, BAS3_MOD},                  /* f135 Dolda konturer båda */

 {notimpl,FALSE, RIT_MOD+BAS2_MOD},          /* Fd. f136 Kalibrera digitizer */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f137 Status */
 {notimpl,FALSE, RIT_MOD+BAS_MOD},           /* Fd. f138 Ej status */
 {iganpm, FALSE, RIT_MOD+BAS_MOD},           /* f139 Analysera PM */
 {igangm, FALSE, RIT_MOD+BAS_MOD},           /* f140 Analysera GM */

 {lstitb,  TRUE, RIT_MOD+BAS_MOD},           /* f141 Lista ID-tab */
 {rdgmpk,  TRUE, RIT_MOD+BAS_MOD},           /* f142 Läs i GM */
 {lstsyd,  TRUE, RIT_MOD+BAS_MOD},           /* f143 Systemdata */
 {prtmod, FALSE, BAS_MOD},                   /* f144 Lista modul till fil */
 {symbpm, FALSE, RIT_MOD+BAS2_MOD},          /* f145 Läs in plotfil */

 {lstmod, FALSE, BAS_MOD},                   /* f146 Lista modul till skärm */
 {notimpl, TRUE, RIT_MOD+BAS_MOD},           /* Fd. f147 Hela menyer */
 {notimpl, TRUE, RIT_MOD+BAS_MOD},           /* Fd. f148 Bara rubriker */
 {notimpl, TRUE, RIT_MOD+BAS_MOD},           /* Fd. f149 Inga menyer */
 {igplot,  TRUE, RIT_MOD+BAS_MOD},           /* f150 Plotta */

 {igcgkm,  TRUE, RIT_MOD+BAS_MOD},           /* f151 Skapa plotfil */
 {igmfun, FALSE, RIT_MOD+BAS_MOD},           /* f152 Kör MACRO */
 {ighelp,  TRUE, TOP_MOD+RIT_MOD+BAS_MOD},   /* f153 Hjälp */
 {igcdts, FALSE, RIT_MOD+BAS2_MOD},          /* f154 Ändra mått siff.storl. */
 {igcdas, FALSE, RIT_MOD+BAS2_MOD},          /* f155 Ändra mått pilstorl. */

 {cucnpm, FALSE, RIT_MOD+BAS_MOD},           /* f156 Chord utan tang. */
 {cuvnpm, FALSE, RIT_MOD+BAS_MOD},           /* f157 Stiffness utan tang. */
 {suropm, FALSE, BAS3_MOD},                  /* f158 Rotationsyta */
 {suofpm, FALSE, BAS3_MOD},                  /* f159 Offsetyta */
 {sucypm, FALSE, BAS3_MOD},                  /* f160 Cylinderyta */

 {cucfpm, FALSE, RIT_MOD+BAS_MOD},           /* f161 Kägelsnitt fri */
 {cucppm, FALSE, BAS3_MOD},                  /* f162 Kägelsnitt proj */
 {curopm, FALSE, RIT_MOD+BAS_MOD},           /* f163 Offset kurva */
 {igscfs, FALSE, RIT_MOD+BAS_MOD},           /* f164 Heldragen kurvtyp */
 {igscfd, FALSE, RIT_MOD+BAS_MOD},           /* f165 Streckad kurvtyp */

 {igscfc, FALSE, RIT_MOD+BAS_MOD},           /* f166 Streckprickad kurvtyp */
 {igscdl, FALSE, RIT_MOD+BAS_MOD},           /* f167 Kurvors strecklängd */
 {igedst, FALSE, BAS_MOD},                   /* f168 Editera sats */
 {iganrf, FALSE, BAS_MOD},                   /* f169 Analysera referenser */
 {igcptw, FALSE, RIT_MOD+BAS_MOD},           /* f170 X11/WIN32-ändra part */

 {igctfn, FALSE, RIT_MOD+BAS_MOD},           /* f171 Ändra text:s font */
 {igstfn, FALSE, RIT_MOD+BAS_MOD},           /* f172 Sätt aktiv textfont */
 {igexsd, FALSE, BAS_MOD},                   /* f173 Sluta med dekomp. */
 {ignjsd, FALSE, BAS_MOD},                   /* f174 Nytt jobb med dekomp. */
 {igsjsa, FALSE, RIT_MOD+BAS_MOD},           /* f175 Lagra allt sluta ej */

 {igchpr, FALSE, RIT_MOD+BAS_MOD},           /* f176 Byt projekt */
 {modbpm, FALSE, RIT_MOD+BAS_MOD},           /* f177 Aktivera basic */
 {iglspr, FALSE, RIT_MOD+BAS_MOD},           /* f178 Lista projekt */
 {igdlpr, FALSE, RIT_MOD+BAS_MOD},           /* f179 Ta bort projekt */
 {iglsjb, FALSE, RIT_MOD+BAS_MOD},           /* f180 Lista jobb */

 {suswpm, FALSE, BAS3_MOD},                  /* f181 Svept yta */
 {surupm, FALSE, BAS3_MOD},                  /* f182 Regelyta */
 {curipm, FALSE, BAS3_MOD},                  /* f183 Skärningskurva */
 {igdljb, FALSE, RIT_MOD+BAS_MOD},           /* f184 Ta bort jobb */
 {igcwdt, FALSE, RIT_MOD+BAS_MOD},           /* f185 Ändra storhets bredd */

 {igswdt, FALSE, RIT_MOD+BAS_MOD},           /* f186 Storheters bredd */
 {igmvrr, FALSE, TOP_MOD},                   /* f187 Kopiera RES->RIT */
 {wpunik, FALSE, NONE_MOD},                  /* f188 X-Ändra skala */
 {wpunik, FALSE, NONE_MOD},                  /* f189 X-Panorera */
 {wpunik, FALSE, NONE_MOD},                  /* f190 X-Perspektiv */

 {wpunik, FALSE, NONE_MOD},                  /* f191 X-Föregående vy */
 {wpunik, FALSE, NONE_MOD},                  /* f192 X-version av status */
 {wpunik, FALSE, NONE_MOD},                  /* f193 X-version av ZOOM */
 {wpunik, FALSE, NONE_MOD},                  /* f194 X-version av AutoZOOM */
 {wpunik, FALSE, NONE_MOD},                  /* f195 X-Byt vy */
#ifdef WIN32
 {msneww, TRUE,  RIT_MOD+BAS_MOD},           /* f196 WIN32-Skapa nytt fönster */
#else
 {WPneww, TRUE,  RIT_MOD+BAS_MOD+X11_MOD},   /* f196 X-Skapa nytt fönster */
#endif
 {wpunik, FALSE, NONE_MOD},                  /* f197 X-Nivåer */
 {igshd0, TRUE,  BAS3_MOD},                  /* f198 Flat shading */
 {igshd1, TRUE,  BAS3_MOD},                  /* f199 Smooth shading */
 {igrenw, FALSE, NONE_MOD},                  /* f200 Render */

 {igsaln, FALSE, RIT_MOD+BAS_MOD},           /* f201 Lagra allt med nytt namn */
 { mv1gm, FALSE, RIT_MOD},                   /* f202 Dra storhet */
 {roengm, FALSE, RIT_MOD},                   /* f203 Rotera storhet */
 {igcarr, FALSE, RIT_MOD},                   /* f204 Ändra cirkel radie */
 {igcar1, FALSE, RIT_MOD},                   /* f205 Ändra startvinkel*/

 {igcar2, FALSE, RIT_MOD},                   /* f206 Ändra slutvinkel */
 {igctxv, FALSE, RIT_MOD},                   /* f207 Ändra text vinkel */
 {igctxs, FALSE, RIT_MOD},                   /* f208 Ändra text */
 {mvengm, FALSE, RIT_MOD},                   /* f209 Flytta storheter */
 {cpengm, FALSE, RIT_MOD},                   /* f210 Kopiera storheter */

 {mrengm, FALSE, RIT_MOD},                   /* f211 Spegla storhet */
 {chgrgm, FALSE, RIT_MOD},                   /* f212 Ändra grupp */
 {igblnk, FALSE, RIT_MOD},                   /* f213 Släck storhet */
 {igubal, FALSE, RIT_MOD},                   /* f214 Tänd alla släckta */
 {ightal, FALSE, RIT_MOD},                   /* f215 Gör allt pekbart */

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
 { igcff, FALSE, RIT_MOD+BAS_MOD},           /* f218 Edit entity font = 3 */
{notimpl, FALSE, NONE_MOD},                  /* f219  */
{ WPcoal, FALSE, BAS_MOD+X11_MOD},           /* f220 Kompilera alla X11 */

{notimpl, FALSE, NONE_MOD},                  /* f221  */
{notimpl, FALSE, NONE_MOD},                  /* f222  */
{notimpl, FALSE, NONE_MOD},                  /* f223  */
{notimpl, FALSE, NONE_MOD},                  /* f224  */
{notimpl, FALSE, NONE_MOD},                  /* f225  */

#endif

{ igctpm, FALSE, BAS3_MOD},                  /* f226 Ändra texts TPMODE */
{ igstpm, FALSE, BAS3_MOD},                  /* f227 Sätt aktivt TPMODE */
{notimpl, FALSE, NONE_MOD},                  /* f228  */
{notimpl, FALSE, NONE_MOD},                  /* f229  */
{ cuispm, FALSE, BAS3_MOD},                  /* f230 Isoparameter kurva */

{ curapm, FALSE, BAS2_MOD+BAS3_MOD},         /* f231 Approximera kurva */
{ curtpm, FALSE, BAS2_MOD+BAS3_MOD},         /* f232 Trimma kurva */
{ cusipm, FALSE, BAS3_MOD},                  /* f233 Silhuette kurva */
{ surtpm, FALSE, BAS3_MOD},                  /* f234 Trimma yta */
{ surapm, FALSE, BAS3_MOD},                  /* f235 Approximera yta */

{ sucopm, FALSE, BAS3_MOD},                  /* f236 Sammansatt yta */
{ suexpm, FALSE, BAS3_MOD},                  /* f237 Importerad yta */
{ sulopm, FALSE, BAS3_MOD},                  /* f238 Loftad yta */
{ tfmopm, FALSE, BAS3_MOD},                  /* f239 Translation */
{ tfropm, FALSE, BAS3_MOD},                  /* f240 Rotation */

{ tfmipm, FALSE, BAS3_MOD},                  /* f241 Spegling */
{ tcpypm, FALSE, BAS3_MOD},                  /* f242 Kopiering */

};

/*
***Kom ihåg att sätta futab's storlek till rätt värde nedan om
***storleken ändras.
*/
#define FTABSIZ 242

/******************************************************!*/

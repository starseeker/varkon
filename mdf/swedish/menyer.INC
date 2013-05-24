
! Varkons standardmenyer
! Senast uppdaterade för version 1.18A
! Johan Kjellander, Örebro universitet
!
m2  = "HUVUDMENY-2D"
      "skapa",m13
      "ändra",m15
      "ta bort",m22
      "variera",m34
      "analys",m75
      "grafik",m66
      "nivåer",m65
      "attribut",m14
      "mbs",m58
      "arkiv",m5;

m3  = "HUVUDMENY-3D"
      "skapa",m11
      "ändra",m15
      "ta bort",m22
      "variera",m34
      "analys",m75  
      "grafik",m66 
      "nivåer",m65
      "attribut",m14
      "mbs",m58
      "arkiv",m5;

m4  = "RITMODUL"
      "skapa",m13
      "redigera",m49
      "ta bort",m80
      "analys",m75  
      "grafik",m66 
      "nivåer",m65
      "attribut",m14
      "arkiv",m6;

m5  = "ARKIV"
      "öppna",m47
      "spara",f175
      "spara som",m45
      "ta bort",f184
      "projekt",m7
      "skriv ut",m71
      "inställningar",m8
      "macro",f152
      "kom till OS",f125
      "sluta",m46;

m6  = "ARKIV"
      "öppna",m47
      "spara",f175
      "spara som",m48
      "ta bort",f184
      "projekt",m7
      "skriv ut",m71
      "macro",f152
      "kom till OS",f125
      "sluta",m46;

m7  = "PROJEKT"
      "byt projekt",f176
      "inställningar",f178
      "ta bort projekt",f179;

m8  = "INSTÄLLNINGAR"
      "attribut",f127
      "skyddskod",f128;

m10 = "TCOPY"
      "translation",f239
      "rotation",f240
      "spegling",f241
      "kopiering",f242;

! Skapa för Basmodul-3D

m11 = "SKAPA"
      "punkt",m20
      "linje",m40
      "cirkelbåge",m55
      "kurvor",m60
      "ytor",m12
      "koordsystem",m26
      "text",f50
      "tcopy",m10
      "grupp",f55
      "parter",m43;

m12 = "YTOR"
      "bplan",f26
      "rotation",f158
      "cylinder",f160
      "svept",f181
      "regel",f182
      "loftad",f238
      "offset",f159
      "approx",f235
      "trimmad",f234
      "sammansatt",f236
      "importera",f237;

! Skapa för Basmodul-2D och Ritmodul

m13 = "SKAPA"
      "punkt",f20
      "linje",m41
      "cirkelbåge",m56
      "kurvor",m61
      "text",m23
      "måttsättning",m50
      "koordsystem",m25
      "grupp",f55
      "parter",m39;

m14 = "ATTRIBUT"
      "penna",f96
      "nivå",f90
      "linjebredd",f186
      "linjer",m42
      "cirklar",m57
      "kurvor",m21
      "text",m51
      "mått",m31
      "snitt",m32;

m15 = "ÄNDRA"
      "penna",f11
      "nivå",f12
      "linjebredd",f185
      "attribut",m16
      "storhet",M117
      "trimma",f6;

m16 = "ATTRIBUT"
      "linje",m17
      "cirkel",m17
      "kurva",m90
      "text",m18
      "mått",m19
      "snitt",m17;

m17 = "ATTRIBUT"
      "heldragen",f74
      "streckad",f76
      "centrum",f77
      "ny längd",f73;

m18 = "TEXT"
      "höjd",f78
      "bredd",f79
      "lutning",f89
      "typsnitt",f171
      "tpmode",f226;

m19 = "MÅTT"
      "sifferstorlek",f154
      "pilstorlek",f155
      "decimaler",f129
      "auto",f131
      "ej auto",f130;

m20 = "PUNKT"
      "fri",f19
      "projicerad",f21;

m21 = "KURVOR"
      "heldragna",f164
      "streckade",f165
      "centrum",f166
      "strecklängd",f167;

m22 = "TA BORT"
      "en storhet",f10
      "grupp",f13
      "sista sats",f15;

m23 = "TEXT"
      "vänster",f50
      "höger",M113
      "centrerad",M114
      "2 positioner",M115;
!      "font-editor",m93;

m25 = "KOORDSYSTEM"
      "3 pos",f105
      "pos/vinkel",f9
      "aktivera",m27;

m26 = "KOORDSYSTEM"
      "3 pos",f104
      "pos/vinklar",f14
      "aktivera",m27;

m27 = "AKTIVERA"
      "lokalt",f106
      "globala",f107
      "basic",f177;

m30 = "LÄNGDMÅTT"
      "horisontellt",f30
      "vertikalt",f31
      "parallellt",f32;

m31 = "MÅTT"
      "sifferstorlek",f80
      "pilstorlek",f81
      "decimaler",f82
      "auto",f83
      "ej auto",f84;

m32 = "SNITT"
      "heldragna",f85
      "streckade",f86
      "centrum",f87
      "strecklängd",f88;

m33 = "DIAMETERMÅTT"
      "horisontellt",f33
      "vertikalt",f34
      "parallellt",f35;

m34 = "VARIERA"
      "part",m35
      "aktiv modul",m36;

m35 = "PART"
      "ändra",f170
      "uppdatera",f109;

m36 = "AKTIV MODUL"
      "parametrar",m37
      "lista",m38
      "editera",f168
      "kör",f5;

m37 = "PARAMETRAR"
      "skapa",f2
      "ändra",f3
      "lista",f4;

m38 = "LISTA MODUL"
      "skärm",f146
      "fil",f144;

m39 = "PARTER"
      "fast",f25
      "temporär",f8
      "iges",p1624
      "dxf",m44
      "plotfil",m52;

m40 = "LINJE"
      "2f två pos",f22
      "2p projicerad",f41
      "pl parallell",f23
      "v  vinkelrät",f54
      "pv pos/vinkel",f24
      "pt pos/tan",f28
      "t  tan/tan",f29;

m41 = "LINJE"
      "2  två pos",f40
      "pl parallell",f42
      "v  vinkelrät",f39
      "pv pos/vinkel",f43
      "pt pos/tan",f44
      "t  tan/tan",f45;

m42 = "LINJER"
      "heldragna",f46
      "streckade",f47
      "centrum",f48
      "strecklängd",f49;

m43 = "PARTER"
      "fast",f25
      "temporär",f8
      "iges",p1624
      "dxf",m53
      "stl",m87;

m44 = "DXF-2D"
      "läs in",p388
      "skriv ut",f92;

m45 = "SPARA SOM"
      "alla filer",f201
      "JOB-fil",f119
      "MBO-fil",f126
      "MBS-fil",f144
      "RES-fil",f121;

m46 = "SLUTA"
      "allt lagras",f122
      "inget lagras",f123;

m47 = "ÖPPNA"
      "lagra först",f124
      "inget lagras",f120;

m48 = "SPARA SOM"
      "alla filer",f201
      "JOB-fil",f119
      "RIT-fil",f121;

m49 = "REDIGERA"
      "ändra",m81
      "trimma",f6
      "dra",f202
      "flytta",f209
      "spegla",f211
      "rotera",f203
      "kopiera",f210;

m50 = "MÅTTSÄTTNING"
      "längdmått",m30
      "diametermått",m33
      "radiemått",f36
      "vinkelmått",f37
      "snitt",f38;

m51 = "TEXTER"
      "höjd",f51
      "bredd",f52
      "lutning",f53
      "typsnitt",f172
      "tpmode",f227;

m52 = "PLOTFIL"
      "läs in",f145
      "skriv ut",f151;

m53 = "DXF-3D"
      "läs in",p389
      "2D-skriv ut",f92
      "3D-skriv ut",M398;

m55 = "CIRKELBÅGE"
      "1 pos/vinklar",f56
      "2 pos/radie",f57
      "3 pos",f58
      "offset",f59
      "hörnradie",f65;

m56 = "CIRKELBÅGE"
      "1 pos/vinklar",f60
      "2 pos/radie",f61
      "3 pos",f62
      "offset",f63
      "hörnradie",f64;

m57 = "CIRKLAR"
      "heldragna",f66
      "streckade",f67
      "centrum",f68
      "strecklängd",f69;

m58 = "MBS"
      "aktiv modul",f216
      "part",m59;

m59 = "PART"
      "editera",f217
      "uppdatera",f109
      "kompilera alla",f220;

m60 = "KURVOR"
      "spline",m89
      "kägelsnitt",m84
      "offset",f163
      "sammansatt",f75
      "trimmad",f232
      "isoparam",f230
      "skärning",f183
      "siluett",f233
      "approx",f231;

m61 = "KURVOR"
      "spline",m89
      "kägelsnitt",f161
      "offset",f163
      "sammansatt",f75
      "trimmad",f232
      "approx",f231;

m62 = "DOLDA KONT"
      "skärm",f133
      "fil",f134
      "båda",f135;

m63 = "FERGUSON"
      "med tang.",f70
      "utan tang.",f116;

m64 = "CHORD LENGTH"
      "med tang.",f71
      "utan tang.",f156;

m65 = "NIVÅER"
      "byt aktiv",f90
      "nivånamn",m88;

m66 = "GRAFIK"
      "vyer",m67 
      "raster",m69 
      "plottning",m71
      "diverse",m72;

! Meny-66 för teckenbaserade terminaler
!m66 = "GRAFIK"
!      "vyer",m67 
!      "raster",m69 
!      "plottning",m71
!      "skala",f101
!      "centrum",f102
!      "generera",f103
!      "föregående",f91
!      "zoom",m74
!      "auto-zoom",f97
!      "diverse",m72;

m67 = "VYER"
      "skapa vy",m70
      "ta bort vy",f112
      "lista vyer",f113
      "nytt fönster",f196
      "skuggning",m85
      "dolda kont",m62;

m68 = "SKÄRMLAYOUT"
      "hela menyer",f147
      "bara rubriker",f148
      "inga menyer",f149
      "status",f137
      "ej status",f138
      "pos-menyn",m73;

m69 = "RASTER"
      "läge",f16
      "x-delning",f17
      "y-delning",f18
      "tänd raster",f114
      "släck raster",f115;

m70 = "SKAPA VY"
      "betr. pos.",f111
      "koord.sys.",f99
      "8 vyer",M112;

m71 = "PLOTTNING"
      "skapa plotfil",f151
      "plotta",f150;

m72 = "DIV"
      "kurv-noggr",f98
      "skärmlayout",m68 
      "allt pekbart",f215
      "kalibrera dig",f136
      "ritningsskala",f108;

m73 = "POS-MENYN"
      "på",f118
      "av",f117;

m74 = "ZOOM"
      "hårkors",f100
      "pos./längd",M394
      "nytt centrum",M395;

m75 = "ANALYS"
      "formel",M393
      "geometri",m76 
      "part/grupp",m79
      "status",m78 
      "systemdata",m77;

m76 = "GEOMETRI"
      "en storhet",M356
      "position",M363
      "avstånd",M364
      "vinkel",M365
      "area",m86
      "tyngdpunkt",M417
      "kurvor",m91;

m77 = "SYSTEMDATA"
      "gm-data",f140
      "pm-data",f139
      "systemdata",M184
      "övrigt",m95;

m78 = "STATUS"
      "jobb",M368
      "grafik",M369
      "attribut",M370;

m79 = "PART/GRUPP"
      "en part",M379
      "anropskedja",M418
      "referenser",f169
      "part medlemmar",M366
      "partlista",M378
      "strukturträd",p419
      "gm-lista",M420
      "grupp medlemmar",M360
      "grupplista",M361;

m80 = "TA BORT"
      "en storhet",f10
      "grupp",f13
      "släck",f213
      "tänd alla",f214;

m81 = "ÄNDRA"
      "penna",f11
      "nivå",f12
      "linjebredd",f185
      "attribut",m16
      "cirkel",m82
      "text",m83
      "grupp",f212
      "part",m35;

m82 = "CIRKEL"
      "radie",f204
      "startvinkel",f205
      "slutvinkel",f206;

m83 = "TEXT"
      "vinkel",f207
      "text",f208;

m84 = "KÄGELSNITT"
      "fri",f161
      "projicerad",f162;

m85 = "SKUGGNING"
      "snabb",f198
      "mjuk",f199
      "dynamisk",f200
      "kurv-noggr",f98;

m86 = "YTA"
      "2D-area",M416
      "kurva",M1591
      "yta",M1589;

m87 = "STL"
      "läs in",p1623
      "skriv ut",M1625;

m88 = "NIVÅNAMN"
      "skapa",f93
      "ta bort",f95
      "lista",f94;

m89 = "SPLINE"
      "ferguson",m63
      "chord length",m64
      "variable stiffness",m97;

m90 = "KURVATTRIBUT"
      "heldragen",f74
      "streckad",f76
      "centrum",f77
      "fylld",f218
      "ny längd",f73;

m91 = "KURVOR"
      "längd",M431
      "tangent",M432
      "normal",M434
      "bi-normal",M435
      "krökning",M433
      "inverser",m92;

m92 = "INVERSER"
      "position",M436
      "tangent",M437
      "krökning",M438
      "längd",M439;

m93 = "FONT_EDITOR"
      "mall",p176
      "punkter",f20
      "rita",M177
      "flytta",p178
      "generera fil",m94;

m94 = "GENERERA FIL"
       "detta tecken",M179
       "alla tecken",M180;

m95 = "ÖVRIGT"
      "idtab-ej dok",f141
      "läs-ej dok",f142
      "sydata-ej dok",f143;

m97 = "VAR STIFFNESS"
      "med tang.",f72
      "utan tang.",f157;

m100 = "2 POSITIONER"
       "1 startpos",a1
       "2 slutpos",a2;

m101 = "PARALLELL"
       "linje",a1
       "avstånd",a2;

m102 = "POS-VINKEL"
       "startposition",a1
       "vinkel",a2
       "längd",a3;

m103 = "VINKELRÄT"
       "startposition",a1
       "linje",a2
       "längd",a3;

m104 = "POS-TANGENT"
       "startposition",a1
       "cirkel/kurva",a2
       "alternativ",a3;

m105 = "TAN-TAN"
       "1:a cirkel",a1
       "2:a cirkel",a2
       "alternativ",a3;

m106 = "1 POSITION"
       "origo",a1
       "radie",a2
       "startvinkel",a3
       "slutvinkel",a4;

m107 = "TVÅ POS"
       "1 startpos",a1
       "2 slutpos",a2
       "radie",a3;

m108 = "TRE POS"
       "1 startpos",a1
       "2 mittpos",a2
       "3 slutpos",a3;

m109 = "HÖRNRADIE"
       "1:a storhet",a1
       "2:a storhet",a2
       "radie",a3
       "alternativ",a4;

m110 = "OFFSET"
       "cirkel",a1
       "avstånd",a2;

m111 = "LÄNGDMÅTT"
       "1 startpos",a1
       "2 slutpos",a2
       "textpos",a3
       "alternativ",a4;

m112 = "DIAMETERMÅTT"
       "cirkel",a1
       "textpos",a2
       "alternativ",a3;

m113 = "RADIEMÅTT"
       "cirkel",a1
       "brytpos",a2
       "textpos",a3;

m114 = "VINKELMÅTT"
       "1:a linjen",a1
       "2:a linjen",a2
       "textpos",a3
       "alternativ",a4;

m115 = "TEXT"
       "position",a1
       "vinkel",a2
       "text",a3;

m116 = "SNITT"
       "avstånd",a1
       "vinkel",a2;

m117 = "KSYS 1P"
       "namn",a1
       "origo",a2
       "x-vinkel",a3
       "y-vinkel",a4
       "z-vinkel",a5;

m118 = "KSYS 3P"
       "namn",a1
       "origo",a2
       "x-axel",a3
       "y-axel",a4;

m119 = "B_PLAN"
       "1:a hörnet",a1
       "2:a hörnet",a2
       "3:e hörnet",a3
       "4:e hörnet",a4
       "reversera",a5;

m120 = "KÄGELSNITT"
       "1 startpos",a1
       "2 starttang",a2
       "3 P-värde",a3
       "4 slutpos",a4
       "5 sluttang",a5;

m121 = "OFFSET"
       "kurva",a1
       "offset",a2;

m122 = "SPLINE"
       "1 startpos",a1
       "2 starttang",a2
       "3 slutpos",a3
       "4 sluttang",a4;

m123 = "SKÄRNING"
       "yta",a1
       "plan",a2;

m134= "GRUPP"
      "namn",a3
      "ta bort",a4
      "lägg till",a5;

m140= "POSITION"
      "absolut",a1
      "relativt",a2
      "hårkors",a3
      "ände",a4
      "på storhet",a5
      "centrum",a6
      "skärning",a7;

m141= "POSITION"
      "absolut",a1
      "relativt",a2
      "hårkors",a3
      "mbs",a4
      "ände",a5
      "på storhet",a6
      "centrum",a7
      "skärning",a8;

m143= "PARAMETERTYP"
      "heltal",a1
      "flyttal",a2
      "sträng",a3
      "vektor",a4;

m144= "MODULTYP"
      "geometri",a1
      "ritning",a2;

m145= "ATTRIBUT"
      "lokal",a1
      "global",a2
      "basic",a3;

m146= "SKAPA PLOTFIL"
      "hela modellen",a1
      "allt på skärm",a2
      "del av skärm",a3
      "annan vy",a4;

m147= "SKAPA DXF-FIL"
      "hela modellen",a1
      "allt på skärm",a2
      "del av skärm",a3
      "annan vy",a4;

m150= "OFFSET"
      "yta",a1
      "offset",a2;

m151= "ROTATION"
      "kurva",a1
      "startvinkel",a2
      "slutvinkel",a3
      "1:a position",a4
      "2:a position",a5;

!Ej klar      "reversera",a6;

m152= "CYLINDER"
      "kurva",a1
      "avstånd",a2
      "reversera",a3;

m153= "SVEPYTA"
      "profilkurva",a1
      "styrkurva",a2
      "reversera",a3;

m154= "REGELYTA"
      "1:a kurvan",a1
      "2:a kurvan",a2
      "reversera",a3;

!Ej klar      "båglängd",a4
!Ej klar      "ursprunglig",a5;
      

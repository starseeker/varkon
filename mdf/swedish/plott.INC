!
!*Include-fil för plottning.
!*
!*Meny 135 med alla plotteralternativen är installationsberoende
!*och ligger på VARKON_PLT.
!*

#include "$VARKON_PLT/m135.INC"

t396 = "p_plott"
t399 = "p_pwin"
t441 = "p_autop"

m71 = "PLOTTNING"
      "skapa plotfil",f151
      "manuell plot",M396
      "fönster",M399
      "autoplot",M441;
 
m136 = "PARAMETRAR"
       "skala",a3
       "vridning",a4
       "xmin",a5
       "ymin",a6
       "visa",a2
       "återställ",a1;

m138 = "MANUELL PLOT"
       "parametrar",a1
       "starta",a2;

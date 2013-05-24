
!
! Include-file for VARKON plotting subsytem.
!

#include "$VARKON_PLT/m135.INC"
!
t396 = "p_plott"
t399 = "p_pwin"
t441 = "p_autop"

m71 = "PLOTTING"
      "create plotfile",f151
      "manual plot",M396
      "window",M399
      "autoplot",M441;
 
m136 = "PARAMETERS"
       "scale",a3
       "rotation",a4
       "xmin",a5
       "ymin",a6
       "show",a2
       "reset",a1;

m138 = "MANUAL PLOT"
       "parameters",a1
       "start plot",a2;

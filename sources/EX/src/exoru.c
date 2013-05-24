/*!****************************************************************************
*
*    exoru.c
*    ========
*
*    EXorurstimediff();   Get time difference between scanner and robot
*
*    This file is part of the VARKON Execute  Library.
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
*    (C) 2004-05-15 Sören Larsson, Örebro University                             
*
*******************************************************************************/


#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/EX.h"

extern short erpush();

#ifdef V3ORU
#include <unistd.h>   /*usleep*/
#include <sys/time.h>
#include "rap.h"   
#endif



/*!****************************************************************************/

        short   EXorurstimediff(
        DBint   Pclient,
        DBint   ssocket,
        DBfloat *diff,
        DBfloat *robtime,
        DBint   *pstat)

/*      
 *      In:   Pclient   =  Handle to client (DBint)
 *            name      =  Variable name
 *
 *      Out:  *data      =  Float data to read
 *            *pstat     =  Status
 *
 *      (C) 2004-04-26 Sören Larsson, Örebro University
 *
 ****************************************************************************!*/

{
#ifdef V3ORU

DBfloat rtime;
DBfloat stime;
DBfloat tdiff;
long sdiff,udiff;
struct timeval timev1;
struct timeval timev2;

DBint  status;
int oru_rcommand;
int confirm;

#define TRIGG_STIME 1
#define TRIGG_RTIME 3

/*
*** Check functionality of scanner computer 'timesync' program
*/
status=EXtcpoutint(ssocket,4,0,0,TRIGG_STIME,pstat);
if ( status < 0 ) return(status);
status=EXtcpinint(ssocket,4,0,0,&confirm,pstat);
status=EXtcpinfloat(ssocket,8,0,0,&stime,pstat);

/*
*** set rapid 'oru_timestamp' = 0
*/
rtime=0;
status=EXrapoutfloat(Pclient, "oru_timestamp", 0, pstat);
if ( status < 0 ) return(status);

/*
*** Wait untill rapid retrieve task is ready
*** This may be needed if this MBS-statment was preceeded by another 
*** command to the robot's 'retrieve' task.
*/
do  {status=EXrapinint(Pclient, "oru_rcommand", &oru_rcommand, pstat); }
while ( oru_rcommand > 0 );

/*
*** START TIME CRITICAL, get robot time + local time 1
*/
status=EXrapoutint(Pclient, "oru_rcommand", TRIGG_RTIME, pstat);
while (rtime<0.001){status=EXrapinfloat(Pclient,"oru_timestamp",&rtime,pstat);}
status=gettimeofday(&timev1,NULL); 
  

/*
*** Trigg scanner time, get confirmation + local time2
*/
status=EXtcpoutint(ssocket,4,0,0,TRIGG_STIME,pstat);
status=EXtcpinint(ssocket,4,0,0,&confirm,pstat);
status=gettimeofday(&timev2,NULL);

/*
*** END TIME CRITICAL, Get scanner time (64 bit float)
*/
status=EXtcpinfloat(ssocket,8,0,0,&stime,pstat);
          
/*
*** Calculate difference
*/
sdiff=timev2.tv_sec-timev1.tv_sec;
udiff=timev2.tv_usec-timev1.tv_usec;
tdiff=(DBfloat)(sdiff+udiff*0.000001);


*diff=(DBfloat)stime-rtime-tdiff;
*robtime=rtime;
*pstat=0;
return(0);

#else
return(erpush("EX6042","")); 
#endif
}


/*!*******************************************************
*
*    exsymb.c
*    ========
*
*    EXsymb();     Create (SYMB) plotfile-part
*
*    This file is part of the VARKON Execute Library.
*    URL:  http://varkon.sourceforge.net
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
*********************************************************/

#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../../GE/include/GE.h"
#include "../../WP/include/WP.h"

#include "../include/EX.h"
#include <string.h>

extern DBTmat lklsyi,*lsyspk;
extern char   jobdir[];

/*!******************************************************/

       short     EXsymb(
       DBId     *id,
       char     *name,
       DBVector *pos,
       DBfloat   size,
       DBfloat   ang,
       V2NAPA   *pnp)

/*      Create a part from plot (GKSM) data.
 *
 *      In: id     => ID.
 *          nam    => Plot file name.
 *          pos    => Base position.
 *          size   => Scale factor.
 *          ang    => Rotation in the XY-plane.
 *          pnp    => Attributes.
 *
 *      Return:   0 = Ok.
 *           EX1582 = File %s does not exist.
 *
 *      (C)microform ab 6/12/85 J. Kjellander
 *
 *      27/12/85 Ny konv. av flyttal R. Svedin
 *      29/1/86  Ny inläsning från fil, J. Kjellander
 *      2/1/86   Koll av antal vektorer, J. Kjellander
 *      23/3/86  Bytt namn, J. Kjellander.
 *      30/9/86  asydir, J. Kjellander
 *      19/10/86 Sumbolens origo, J. Kjellander
 *      27/12/86 hit, J. Kjellander
 *      7/5/87   Plot-origo, J. Kjellander
 *      25/4/88  Bug flcose(), J. Kjellander
 *      28/2/89  Part, J. Kjellander
 *      15/12/89 Pennummer, J. Kjellander
 *      13/2/91  Bytt riktn. på linjer, J. Kjellander
 *      99-02-26 Linjebredd, J.Kjellander
 *      2007-11-30 2.0, J.Kjellander
 *
 ******************************************************!*/

  {
    char    sfnam[132];
    DBfloat crdvek[4];
    DBfloat *crdpek;
    DBfloat localx,localy,rotx,roty;
    DBfloat cosv,sinv,width;
    DBfloat origox,origoy;
    char    inbuf[128];
    char    str1[81],str2[81];
    char   *bufpek;
    int     ntkn,ncrd,i;
    double  x,y,nx,ny,mxmin,mymin,mxmax,mymax;
    short   status=0,npnum;
    FILE   *fp;

    DBLine  lin;
    DBId    lid;
    DBPart  prt;
    DBPdat  dat;
    V2NAPA  attr;

/*
***Transform pos to basic.
*/
    if ( lsyspk != NULL ) GEtfpos_to_local(pos,&lklsyi,pos);
/*
***Create a part struct.
*/
    prt.hed_pt.level = pnp->level;
    prt.hed_pt.pen = pnp->pen;
    prt.hed_pt.blank = pnp->blank;
    prt.hed_pt.hit = pnp->hit;
   *(name+JNLGTH) = '\0';
    strcpy(prt.name_pt,name);
    prt.its_pt = 3;
    dat.mtyp_pd = 3;
    dat.matt_pd = BASIC;
    dat.npar_pd = 0;
/*
***Create a line struct.
*/
   lid.seq_val = 1;
   lid.ord_val = 1;
   lid.p_nextre = NULL;
   lin.crd1_l.z_gm = lin.crd2_l.z_gm = 0.0;
/*
***Create an attributes struct.
*/
   V3MOME(pnp,&attr,sizeof(V2NAPA));
   attr.lfont  = 0;
   attr.ldashl = 0.0;
   attr.width  = 0.0;
/*
***Try to open the PLT-file. It must reside in jobdir.
*/
    strcpy(sfnam,jobdir);
    strcat(sfnam,name);
    strcat(sfnam,SYMEXT);
    if ( (fp=fopen(sfnam,"r")) == NULL )
      {
      return(erpush("EX1582",name));
      }
/*
***File exists. Open new part in DB.
*/
    if ( pnp->save == 1 )
      {
      status = EXoppt(id,NULL,&prt,&dat,NULL,NULL);
      if ( status < 0 ) goto end;
      }
/*
***Inits for faster computations.
*/
    cosv   = COS(ang*DGTORD);
    sinv   = SIN(ang*DGTORD);
    origox = origoy = 0.0;
/*
***Get vectors from file and make lines.
*/
    while (fgets(inbuf,127,fp) != NULL)
        {
        if (strncmp (inbuf,"GKSM",4) == 0)
            {
            bufpek = inbuf+4;
/*
***Polyline, GKSM  11.
*/
            if ( strncmp(bufpek,"  11",4) == 0)
                {
                crdpek = crdvek;
                bufpek += 4;
                strncpy(str1,bufpek,6);
                bufpek += 6;
                strncpy(str2,bufpek,6);
                bufpek += 6;
                str1[6] = str2[6] ='\0';
                sscanf(str1,"%d",&ntkn);
                sscanf(str2,"%d",&ncrd);

                for ( i=0; i<ncrd; ++i )
                   {
                   sscanf(bufpek,"%lf",&x);
                   bufpek += 10;
                   sscanf(bufpek,"%lf",&y);
                   bufpek += 10;
/*
***Transform from normalized GKSM coords to model coords.
*/
                   localx = size*(x*(mxmax-mxmin)/nx-origox);
                   localy = size*(y*(mymax-mymin)/ny-origoy);

                   rotx = localx*cosv - localy*sinv;
                   roty = localy*cosv + localx*sinv;

                   *crdpek = pos->x_gm + rotx; ++crdpek;
                   *crdpek = pos->y_gm + roty; ++crdpek;

                   if ( i != 0 )
                     {
                     lin.crd2_l.x_gm = *(crdpek-4);
                     lin.crd2_l.y_gm = *(crdpek-3);
                     lin.crd1_l.x_gm = *(crdpek-2);
                     lin.crd1_l.y_gm = *(crdpek-1);
                     *(crdpek-4) = *(crdpek-2);
                     *(crdpek-3) = *(crdpek-1);
                     crdpek -= 2;
/*
***Save the line in DB.
*/
                     if ( pnp->save == 1 )
                       if ( (status=EXelin(&lid,&lin,&attr)) < 0 )
                         goto end;
                     }
/*
***This line may continue on the next line (long polyline).
*/
                   if ( i != ncrd-1 && (int)(bufpek-inbuf+1) >= (int)strlen(inbuf) )
                     {
                     fgets(inbuf,512,fp);
                     bufpek = inbuf;
                     }
                   }
                }
/*
***Model window, GKSM 175.
*/
            else if ( strncmp(bufpek," 175",4) == 0)
                {
                bufpek += 4;
                sscanf(bufpek,"%6c",str1);
                str1[6] = '\0';
                sscanf(str1,"%d",&ntkn);

                bufpek += 6;
                sscanf(bufpek,"%10c%10c",str1,str2);
                str1[10] = str2[10] = '\0';
                sscanf(str1,"%lf",&mxmin);
                sscanf(str2,"%lf",&mxmax);

                bufpek += 20;
                sscanf(bufpek,"%10c%10c",str1,str2);
                str1[10] = str2[10] = '\0';
                sscanf(str1,"%lf",&mymin);
                sscanf(str2,"%lf",&mymax);

                if ( *(bufpek+21) != '\0' )
                  {
                  bufpek += 20;
                  sscanf(bufpek,"%10c%10c",str1,str2);
                  str1[10] = str2[10] = '\0';
                  sscanf(str1,"%lf",&origox);
                  sscanf(str2,"%lf",&origoy);
                  }
/*
***Calculate xmax/ymax.
*/
                nx = mxmax - mxmin;
                ny = mymax - mymin;

                if  ( nx < ny ) 
                   { nx /= ny; ny = 1.0; }
                else
                   { ny /= nx; nx = 1.0; }
                }
/*
***Line width, GKSM 174.
*/
            else if ( strncmp(bufpek," 174",4) == 0)
                {
                bufpek += 4;
                sscanf(bufpek,"%6c%10c",str1,str2);
                str1[6] = str2[10] = '\0';
                sscanf(str1,"%d",&ntkn);
                sscanf(str2,"%lf",&width);
                attr.width = width;
                }
/*
***Pen number, GKSM 173.
*/
            else if ( strncmp(bufpek," 173",4) == 0)
                {
                bufpek += 4;
                sscanf(bufpek,"%6c%6c",str1,str2);
                str1[6] = str2[6] = '\0';
                sscanf(str1,"%d",&ntkn);
                sscanf(str2,"%hd",&npnum);
                attr.pen = npnum;
                }
            }
        }
/*
***The end.
*/
end:
    if ( pnp->save == 1 ) EXclpt();
    fclose(fp);
    return(status);
  }

/********************************************************/

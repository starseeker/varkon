/*!*****************************************************************************
*
*    extcp.c
*    ======
*
*    EXtcpco();        Interface routine for TCP_Connect
*    EXtcpli();        Interface routine for TCP_Listen
*    EXtcpcl();        Interface routine for TCP_Close
*    EXtcpoutint();    Interface routine for TCP_Outint
*    EXtcpinint();     Interface routine for TCP_Inint
*    EXtcpoutfloat();  Interface routine for TCP_Outfloat
*    EXtcpinfloat();   Interface routine for TCP_Infloat
*    EXtcpoutvec();    Interface routine for TCP_Outvec
*    EXtcpinvec();     Interface routine for TCP_Invec
*    EXtcpgetlocal();  Interface routine for TCP_Getlocal
*    EXtcpgetremote(); Interface routine for TCP_Getremote
*
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
*    (C) 2003-01-27 Sören Larsson, Örebro University       
*
*******************************************************************************/


#include "../../DB/include/DB.h"
#include "../../IG/include/IG.h"
#include "../include/EX.h"

#include <math.h>
#include <memory.h> /* for memset */

#ifdef WIN32
#include <winsock.h> 
#endif

#ifdef UNIX
#include <sys/socket.h> 
#include <netdb.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>


/* Error codes returned by TCP-functions,                  */
/* These definitions is made to be compatible with winsock */

#define WSANOTINITIALISED 0     /* Not appl. in UNIX           */
#define SOCKET_ERROR      -1    /* Not included in UNIX error.h*/
#define INVALID_SOCKET    -1    /* Not included in UNIX error.h*/

#define WSAENETDOWN       ENETDOWN
#define WSAEADDRINUSE     EADDRINUSE
#define WSAEADDRNOTAVAIL  EADDRNOTAVAIL
#define WSAEISCONN        EISCONN
#define WSAENOTSOCK       ENOTSOCK
#define WSAENOTCONN       ENOTCONN
#define WSAETIMEDOUT      ETIMEDOUT
#define WSAECONNRESET     ECONNRESET
#define WSAEINVAL         EINVAL
#define WSAECONNREFUSED   ECONNREFUSED
#define WSAEINTR          EINTR
#define WSAEBADF          EBADF
#endif /*END UNIX*/ 

extern short erpush();
#define SD_SEND         0x01
#define TIMEOUT_CLOSE   10 /* Time between shutdown and close */ 

/*---------------------------------------*/ 
/* Error codes reurned by TCP-functions, */
/* see also the varkon manual            */
/*---------------------------------------*/ 

/* TCP_listen, TCP_outint, TCP_inint */
#define V3ERRTCP_TIMEOUT         -1  /* Timeout value expired */

/* All varkon TCP-functions  except TCP_connect and TCP_listen */
#define V3ERRTCP_ENOTSOCK       -2  /* The descriptor is not a socket. */
#define V3ERRTCP_NOTINITIALISED -2  /* The socket is not initalised 
                                        (WSA_startup()) */
/* As above except TCP_getlocal */
#define V3ERRTCP_ENOTCONN       -2  /* The socket is not connected */
#define V3ERRTCP_EBADF          -2  /* Bad file (socket) number */

/* TCP_getlocal */
#define V3ERRTCP_EINVAL         -2  /* The socket has not been bound to an 
                                        adress with bind(), or ADDR_ANY is 
                                        specified in bind() and connection
                                        has not yet been established */
/* All varkon TCP-functions  */
#define V3ERRTCP_ENETDOWN       -3   /* The network subsystem or the associated 
                                        service provider has failed. */ 

/* TCP_connect, TCP_listen */
#define V3ERRTCP_SYSNOTREADY       -3  /* The network subsystem is not ready 
                                          for networkcommunication. */
#define V3ERRTCP_STARTUP           -4  /* WSAstartup() failed. Could not 
                                          initiate the use of WS2_32.DLL.*/
#define V3ERRTCP_SOCKET            -5  /* Socket() failed. A socket could not 
                                          be opened. */
#define V3ERRTCP_EADDRINUSE        -6  /* Local address/port already in use.*/
#define V3ERRTCP_EADDRNOTAVAIL_LOC -7  /* The local address/port is not valid.*/
#define V3ERRTCP_BIND              -8  /* Bind() failed.Could not bind to 
                                          specified loc.port.*/
/* TCP_connect */
#define V3ERRTCP_HOSTADRESS         -9 /* Can not resolve given host adress.*/
#define V3ERRTCP_EADDRNOTAVAIL_REM -10 /* The remote address is not valid.*/ 
#define V3ERRTCP_ECONNREFUSED      -11 /* The attempt to connect was rejected.*/ 
#define V3ERRTCP_CONNECT           -12 /* Connect() failed. Could not connect.*/

/* TCP_listen */
#define V3ERRTCP_EISCONN           -13  /* The socket is already connected*/
#define V3ERRTCP_LISTEN            -14  /* Listen() failed.*/
#define V3ERRTCP_SELECT_LI         -15  /* Select() failed (when listening)*/
#define V3ERRTCP_ACCEPT            -16  /* Accept() failed*/

/* TCP_close */
#define V3ERRTCP_CLOSE             -17  /* closesocket() failed*/

/* TCP_outint, TCP_outfloat */
#define V3ERRTCP_SELECT_SE         -18  /* select() failed before sending*/
#define V3ERRTCP_SEND              -19  /* send() failed  */

/* TCP_inint, TCP_infloat */
#define V3ERRTCP_SELECT_RE         -20  /* select() failed before receiving*/
#define V3ERRTCP_RCV               -21  /* receive() failed  */

/* TCP_inint, TCP_outint, TCP_infloat, TCP_outfloat */
#define V3ERRTCP_ECONNRESET   -22 /*The virtual circuit was reset by the remote
                                  side executing a "hard" or "abortive" close.*/
#define V3ERRTCP_ETIMEDOUT    -23 /*The connection has been dropped because of  
                                    a network failure or because the system on  
                                    the other end went down without notice.*/
/* TCP_getremote */
#define V3ERRTCP_GETPEERNAME  -24 /*getpeername() failed*/

/* TCP_getlocal */
#define V3ERRTCP_GETSOCKET    -25 /*getsocket() failed*/




/*!****************************************************************************/

        short  EXtcpco(
        char  *server,
        DBint   port,
        DBint   localport,
        DBint *sock,
        DBint *pstat)
 
/*      Interface-routine for TCP_CONNECT.
 *
 *      In:  server     = Name or ip-adress
 *           port       = Port on server to connect
 *           localport  = Local port number to use for connection 
 *
 *      Out: *sock  = Socket id 
 *           *pstat = Status 
 *
 *      (C) 2003-01-27 Sören Larsson, Örebro University
 *
 ****************************************************************************!*/

  {
   struct sockaddr_in server_addr, local;
   struct hostent *hp;
   #ifdef WIN32
   WSADATA wsaData;
   SOCKET msgsock;
   #endif
   #ifdef UNIX
   int msgsock;
   #endif
   int sock_opt;
   int retval;
   char *name=NULL;
   unsigned int addr;
   char *inter_face= NULL;
 
   #ifdef WIN32
   /*-----------------------------------*/ 
   /* Initiate the use of WS2_32.DLL   */
   /*-----------------------------------*/ 
   if (WSAStartup(0x202,&wsaData) == SOCKET_ERROR) 
      {
      errno = WSAGetLastError();
      WSACleanup();
      if (errno == WSASYSNOTREADY) *pstat=V3ERRTCP_SYSNOTREADY;
      else                         *pstat=V3ERRTCP_STARTUP;
      return 0;
      }
   #endif

   /*------------------------------*/ 
   /* Resolve host adress          */
   /*------------------------------*/ 
   name=server;
   if (isalpha(name[0])) /* server address is a name */
      {   
      hp = gethostbyname(name);
      }
   else  /* Convert nnn.nnn address to a usable one */

      { 
      addr = inet_addr(name);
      hp = gethostbyaddr((char *)&addr,4,AF_INET);
      }
   if (hp == NULL )
      {
      #ifdef WIN32
      WSACleanup();
      #endif
      *pstat=V3ERRTCP_HOSTADRESS;
      return 0;
      }
   /*-----------------------------------------------------------------*/ 
   /* Copy the resolved information into the 'sockaddr_in' structure  */
   /*-----------------------------------------------------------------*/ 
   memset(&server_addr,0,sizeof(server_addr)); 
   memcpy(&(server_addr.sin_addr),hp->h_addr_list[0],hp->h_length);
   server_addr.sin_family = hp->h_addrtype;
   server_addr.sin_port = htons((unsigned short)port);
                          /* Port must be in n.w.byte order */

   /*-------------------------------*/ 
   /* Open a socket                 */
   /*-------------------------------*/ 
   msgsock = socket(AF_INET,SOCK_STREAM,0); 
   if (msgsock <0 ) 
      {
      #ifdef WIN32
      errno = WSAGetLastError();
      WSACleanup();
      #endif
      if (errno == WSAENETDOWN)     *pstat=V3ERRTCP_ENETDOWN;
      else  *pstat=V3ERRTCP_SOCKET;
      return 0;
      }
   /*-----------------------------------------------------------*/ 
   /* Bind to specified port if not zero                        */
   /*-----------------------------------------------------------*/ 
   if (localport!=0)
      {
      /*-----------------------------------------------------------*/ 
      /* Fill in data in the 'local'-structure */
      /*-----------------------------------------------------------*/ 
      memset(&local,0,sizeof(local));
      local.sin_family = AF_INET;
      local.sin_addr.s_addr = (!inter_face)?INADDR_ANY:inet_addr(inter_face); 
      local.sin_port = htons((unsigned short)localport); 
      
      /*----------------------------------------------------------*/ 
      /* bind() associates a local address and port combination   */
      /* with the socket just created. (our 'local' structure)    */             
      /*----------------------------------------------------------*/ 	              
      sock_opt=1;
      if(setsockopt(msgsock,SOL_SOCKET,SO_REUSEADDR,(void *)&sock_opt,
         sizeof (sock_opt))== SOCKET_ERROR)
         {
         *pstat=V3ERRTCP_BIND;
         return(0);
         };  
      if (bind(msgsock,(struct sockaddr*)&local,sizeof(local)) 
		   == SOCKET_ERROR) 
         {
         #ifdef WIN32
         errno = WSAGetLastError();
         WSACleanup();
         #endif
         if      (errno == WSAENETDOWN)      *pstat=V3ERRTCP_ENETDOWN;
         else if (errno == WSAEADDRINUSE)    *pstat=V3ERRTCP_EADDRINUSE;
         else if (errno == WSAEADDRNOTAVAIL) *pstat=V3ERRTCP_EADDRNOTAVAIL_LOC;
         else                                *pstat=V3ERRTCP_BIND;
         #ifdef UNIX
         (void)close(msgsock);
         #endif
         return 0;
         }
      }
   /*-------------------------------*/ 
   /* Connect to socket             */
   /*-------------------------------*/          
    #ifdef WIN32
    retval = connect(msgsock,(struct sockaddr*)&server_addr,
                                              sizeof(server_addr));
    #endif
    #ifdef UNIX
    do{retval = connect(msgsock,(struct sockaddr*)&server_addr,
                                               sizeof(server_addr));}
    while((retval==-1) && (errno == EINTR));                      
    #endif
    if (retval==SOCKET_ERROR)  
      {
      #ifdef WIN32
      errno = WSAGetLastError();
      WSACleanup();
      #endif
      if      (errno == WSAENETDOWN)      *pstat=V3ERRTCP_ENETDOWN;
      else if (errno == WSAEADDRINUSE)    *pstat=V3ERRTCP_EADDRINUSE;
      else if (errno == WSAEADDRNOTAVAIL) *pstat=V3ERRTCP_EADDRNOTAVAIL_REM;
      else if (errno == WSAECONNREFUSED)  *pstat=V3ERRTCP_ECONNREFUSED;
      else                                *pstat=V3ERRTCP_CONNECT;
      #ifdef UNIX
      (void)close(msgsock);
      #endif
      return 0;
      }
   *sock=msgsock;
   *pstat = 0;
   return(0);
  }


/*!****************************************************************************/

        short    EXtcpli(
        DBint    port,
        DBfloat  timeout,
        DBint   *sock,
        DBint   *pstat)
 
/*      Interface-routine for TCP_LISTEN.
 *
 *      In:  port    = Local port number to use for listen
 *           timeout = Time to wait for incomming connection
 *
 *      Out: *socket = Socket id 
 *           *pstat  = Status 
 *
 *      Error codes: EX3153 = Not implemented in UNIX-VARKON
 *                   
 *
 *      (C) 2003-01-27 Sören Larsson, Örebro University
 *
 ****************************************************************************!*/
  {
   #ifdef WIN32
   SOCKET listen_socket,msgsock;
   WSADATA wsaData;
   #endif   
   #ifdef UNIX   
   int listen_socket,msgsock;
   #endif     
   struct sockaddr_in local,from;
   struct timeval time_out;
   int retval;
   int sock_opt;
   unsigned int fromlen;
   char *inter_face= NULL;
   fd_set readfds;

   /*-----------------------------------*/ 
   /* Fill in the timeval-structure     */
   /*-----------------------------------*/ 

   time_out.tv_sec=(int)floor(timeout);
   time_out.tv_usec=(int)floor((timeout-floor(timeout))*1000000 + 0.5);

   /*-----------------------------------*/ 
   /* Initiate the use of WS2_32.DLL    */
   /*-----------------------------------*/ 
   #ifdef WIN32   
   if (WSAStartup(0x202,&wsaData) == SOCKET_ERROR) 
      {
      errno = WSAGetLastError();
      WSACleanup();
      if (errno == WSASYSNOTREADY) *pstat=V3ERRTCP_SYSNOTREADY;
      else                         *pstat=V3ERRTCP_STARTUP;
      return 0;
      }      
   #endif

   /*------------------------------------------*/ 
   /* Fill in data in the 'local'-structure    */
   /*------------------------------------------*/ 
   local.sin_family = AF_INET;
   local.sin_addr.s_addr = (!inter_face)?INADDR_ANY:inet_addr(inter_face); 
   local.sin_port = htons((unsigned short)port); 

   /*-------------------------------*/ 
   /* Open a socket                 */
   /*-------------------------------*/ 
   listen_socket = socket(AF_INET,SOCK_STREAM,0); 
   if (listen_socket <0 ) 
      {
      #ifdef WIN32
      errno = WSAGetLastError();
      WSACleanup();
      #endif
      if (errno == WSAENETDOWN) *pstat=V3ERRTCP_ENETDOWN;
      else  *pstat=V3ERRTCP_SOCKET;
      return 0;
      }
	
   /*----------------------------------------------------------*/ 
   /* bind() associates a local address and port combination   */
   /* with the socket just created. (our 'local' structure)    */             
   /*----------------------------------------------------------*/ 	              
   
   sock_opt=1;
   if(setsockopt(listen_socket,SOL_SOCKET,SO_REUSEADDR,(void *)&sock_opt,
                                    sizeof (sock_opt))== SOCKET_ERROR)
      {
      *pstat=V3ERRTCP_BIND;
      return(0);
      };  
   
   if (bind(listen_socket,(struct sockaddr*)&local,sizeof(local))==SOCKET_ERROR) 
      {
      #ifdef WIN32
      errno = WSAGetLastError();
      WSACleanup();
      #endif
      if      (errno == WSAENETDOWN)      *pstat=V3ERRTCP_ENETDOWN;
      else if (errno == WSAEADDRINUSE)    *pstat=V3ERRTCP_EADDRINUSE;
      else if (errno == WSAEADDRNOTAVAIL) *pstat=V3ERRTCP_EADDRNOTAVAIL_LOC;
      else                                *pstat=V3ERRTCP_BIND;
      #ifdef UNIX
      (void) close(listen_socket);
      #endif
      return 0;
      }
 
   /*----------------------------------------------------------*/ 
   /* listen to the socket                                     */
   /*----------------------------------------------------------*/ 	              
   if (listen(listen_socket,5) == SOCKET_ERROR) 
      {
      #ifdef WIN32
      errno = WSAGetLastError();
      WSACleanup();
      #endif
      if      (errno == WSAENETDOWN)      *pstat=V3ERRTCP_ENETDOWN;
      else if (errno == WSAEADDRINUSE)    *pstat=V3ERRTCP_EADDRINUSE;
      else if (errno == WSAEISCONN)       *pstat=V3ERRTCP_EISCONN;
      else                                *pstat=V3ERRTCP_LISTEN;
      #ifdef UNIX
      (void) close(listen_socket);
      #endif
      return 0;
      }
   
   /*----------------------------------------------------------*/ 
   /* Check for incomming connections,  (select() is used)     */
   /*----------------------------------------------------------*/ 
     
   FD_ZERO(&readfds);
   FD_SET(listen_socket,&readfds);
   
   if (time_out.tv_sec==0 && time_out.tv_usec==0)
      {
      #ifdef WIN32
      retval = select(listen_socket+1,&readfds,NULL,NULL,NULL);
      #endif
      #ifdef UNIX
      do{retval = select(listen_socket+1,&readfds,NULL,NULL,NULL);}
      while((retval==-1) && (errno == EINTR));
      #endif
      }                                           
   else
      {
      #ifdef WIN32
      retval = select(listen_socket+1,&readfds,NULL,NULL,&time_out);
      #endif
      #ifdef UNIX
      do{retval = select(listen_socket+1,&readfds,NULL,NULL,&time_out);}
      while((retval==-1) && (errno == EINTR));
      #endif
      }
   if (retval == SOCKET_ERROR) 
      {
      #ifdef WIN32
      errno = WSAGetLastError();
      WSACleanup();
      #endif
      if      (errno == WSAENETDOWN)     *pstat=V3ERRTCP_ENETDOWN;
      else                               *pstat=V3ERRTCP_SELECT_LI;
      #ifdef UNIX
      (void) close(listen_socket);
      #endif
      return 0;
      }
   if (retval == 0)
      {
      #ifdef WIN32
      WSACleanup();
      #endif
      *pstat=V3ERRTCP_TIMEOUT;
      *sock=0;
      #ifdef UNIX
      (void) close(listen_socket);
      #endif
      return 0;
      }
   /* Be sure we are talking about our connection */
      if (FD_ISSET(listen_socket, &readfds) != 1) 
      {
      *pstat=V3ERRTCP_SELECT_LI;
      *sock=0;
      #ifdef WIN32
      WSACleanup();
      #endif
      #ifdef UNIX
      (void) close(listen_socket);
      #endif
      return 0;
      }
 
   /*-------------------------------*/ 
   /* accept connection             */
   /*-------------------------------*/ 	              
      fromlen =sizeof(from);/*from is our 'sockaddr_in' structure */
      
      msgsock = accept(listen_socket,(struct sockaddr*)&from, &fromlen);
	
      if (msgsock == INVALID_SOCKET) 
         {
         #ifdef WIN32
         errno = WSAGetLastError();
         WSACleanup();
         #endif
         if      (errno == WSAENETDOWN)  *pstat=V3ERRTCP_ENETDOWN;
         else                            *pstat=V3ERRTCP_ACCEPT;
         }
      else
         {
         *sock=msgsock;
         *pstat = 0;
         }
      #ifdef UNIX
      (void)close(listen_socket);
      #endif
      #ifdef WIN32
      (void)closesocket(listen_socket);
      #endif     
      return(0);
      }


/*!****************************************************************************/

        short  EXtcpcl(
        int sock,
        DBint *pstat)
 
/*      Interface-routine for TCP_CLOSE.
 *
 *      In: sock     = Id of the socket to close.
 *
 *      Out:  *pstat = Status. 
 *
 *      (C) 2003-01-27 Sören Larsson, Örebro University
 *
 ****************************************************************************!*/

  {
   char data_8;  /*always 8 bit  (1byte)*/
   int retval;
   struct timeval time_out;
   fd_set readfds;


   /*-------------------------------*/ 
   /* shutdown                      */
   /*-------------------------------*/ 
   retval= shutdown (sock,SD_SEND);
   if (retval == SOCKET_ERROR) 
      {   
      #ifdef WIN32
      errno = WSAGetLastError();
      #endif
      if      (errno == WSAENOTCONN)       *pstat=V3ERRTCP_ENOTCONN;
      else if (errno == WSAENOTSOCK)
         {
         *pstat=V3ERRTCP_ENOTSOCK;
         return(0);
         }
      else if (errno == WSANOTINITIALISED) *pstat=V3ERRTCP_NOTINITIALISED;
      }
   
   /*------------------------------------------------*/ 
   /* Dropp all incomming data within TIMEOUT_CLOSE  */
   /*------------------------------------------------*/ 
   
   time_out.tv_sec=TIMEOUT_CLOSE;
   time_out.tv_usec=0;

   FD_ZERO(&readfds);   
   FD_SET(sock,&readfds);

   #ifdef WIN32
   do
      {
      retval=0;
      if((select(sock+1,&readfds,NULL,NULL,&time_out)>0) && 
                                     (FD_ISSET (sock, &readfds) == 1)) 
         { 
         retval=recv (sock,(char *)&data_8,1,0);
         }
      }
   while(retval==1);
   #endif
   
   #ifdef UNIX
   do
      {
      do{retval = select(sock+1,&readfds,NULL,NULL,&time_out);}
      while((retval==-1) && (errno == EINTR));
      if(retval>0 && (FD_ISSET (sock, &readfds) == 1)) 
         { 
         do{retval=recv (sock,(char *)&data_8,1,0);}
         while((retval==-1) && (errno= EINTR));
         }
      }
   while(retval==1);
   #endif

   /*-------------------------------*/ 
   /* close socket                  */
   /*-------------------------------*/ 
   #ifdef WIN32
   retval=closesocket(sock);
   #endif
   #ifdef UNIX
   retval=close(sock);
   #endif

   if (retval == SOCKET_ERROR) 
      {
      #ifdef WIN32
      errno = WSAGetLastError();
      WSACleanup();
      #endif
      if      (errno == WSAENETDOWN)       *pstat=V3ERRTCP_ENETDOWN;
      else if (errno == WSAENOTSOCK)       *pstat=V3ERRTCP_ENOTSOCK;
      else if (errno == WSANOTINITIALISED) *pstat=V3ERRTCP_NOTINITIALISED;
      else if (errno == WSAEBADF)          *pstat=V3ERRTCP_EBADF;
      else                                 *pstat=V3ERRTCP_CLOSE;
      return 0;
      }
   *pstat = 0;
   
   #ifdef WIN32
   WSACleanup();
   #endif
   return(0);
  }


/*!****************************************************************************/

        short    EXtcpoutint(
        DBint    sock,
        DBint    bytecount,
        DBint    byteorder,
        DBfloat  timeout,
        DBint    data,
        DBint   *pstat)
/*     
 *
 *      In: sock      = Id of the socket to use for sending.
 *          bytecount = The nuber of bytes to send (1, 2 or 4).
 *          byteorder = Conversion to network byte order 0/1
 *          timeout   = Time to wait before return -1
 *          data      = The data to be sent
 *
 *      Ut: *pstat = Status 
 *
 *      (C) 2003-01-27 Sören Larsson, Örebro University
 *
 ****************************************************************************!*/

  {
   struct timeval time_out;
   int retval;
   DBint   data_32;  /*always signed 32 bit (4byte)*/
   DBshort data_16;  /*always signed 16 bit (2byte)*/
   DBchar  data_8;   /*always signed 8 bit  (1byte)*/

   fd_set writefds;

   /*-----------------------------------*/ 
   /* Fill in the timeval-structure     */
   /*-----------------------------------*/ 

   if (timeout<0) return(erpush("EX6002","negative timeout not allowed"));
   time_out.tv_sec=(int)floor(timeout);
   time_out.tv_usec=(int)floor((timeout-floor(timeout))*1000000 + 0.5);

   /*----------------------------------------------------------*/ 
   /* Check if possible to send                                */
   /*----------------------------------------------------------*/ 
   FD_ZERO(&writefds);   
   FD_SET(sock,&writefds);

   if (time_out.tv_sec==0 && time_out.tv_usec==0)
      {
      #ifdef WIN32
      retval = select(sock+1,NULL,&writefds,NULL,NULL);   
      #endif
      #ifdef UNIX
      do{retval = select(sock+1,NULL,&writefds,NULL,NULL);}
      while((retval==-1) && (errno == EINTR));
      #endif
      }           
   else
      {
      #ifdef WIN32
      retval = select(sock+1,NULL,&writefds,NULL,&time_out);  
      #endif
      #ifdef UNIX
      do{retval = select(sock+1,NULL,&writefds,NULL,&time_out);}
      while((retval==-1) && (errno == EINTR));
      #endif
      }
        
   if (retval == SOCKET_ERROR) 
      {
      #ifdef WIN32
      errno = WSAGetLastError();
      #endif
      
      if       (errno == WSAENETDOWN)  *pstat=V3ERRTCP_ENETDOWN;
      else if  (errno == WSAENOTSOCK)  *pstat=V3ERRTCP_ENOTSOCK;
      else if  (errno == WSANOTINITIALISED)  *pstat=V3ERRTCP_NOTINITIALISED;
      else                             *pstat=V3ERRTCP_SELECT_SE;
      return 0;
      }
 
   if (retval == 0)
      {
      *pstat=V3ERRTCP_TIMEOUT;
      return 0;
      }

    /* be sure we are talking about our connection */
    if (FD_ISSET(sock, &writefds) != 1) 
      {
      *pstat=V3ERRTCP_SELECT_SE;
      return 0;
      }
   
   /*-----------------------------------*/ 
   /* send()                            */
   /*-----------------------------------*/ 

   if (bytecount==4)
      {
      if (byteorder==0) data_32=(DBint)data;
      else data_32=htonl((DBint)data);
      
      #ifdef WIN32
      retval = send(sock,(char *)&data_32,4,0); 
      #endif
      #ifdef UNIX
      do{retval = send(sock,(char *)&data_32,4,0);}
      while((retval==-1) && (errno == EINTR));
      #endif
      
      if (retval == SOCKET_ERROR) 
         {
         #ifdef WIN32
         errno = WSAGetLastError();
         #endif
         if       (errno == WSAENETDOWN)   *pstat=V3ERRTCP_ENETDOWN;
         else if  (errno == WSAENOTSOCK)   *pstat=V3ERRTCP_ENOTSOCK;
         else if  (errno == WSAENOTCONN)   *pstat=V3ERRTCP_ENOTCONN;
         else if  (errno == WSAETIMEDOUT)  *pstat=V3ERRTCP_ETIMEDOUT;
         else if  (errno == WSAECONNRESET) *pstat=V3ERRTCP_ECONNRESET;
         else                              *pstat=V3ERRTCP_SEND;
         return 0;
         }
      *pstat = retval;
      return(0);
      }
   else if (bytecount==1)
      {
      if (byteorder==0) data_8=(DBchar)data;
      else return(erpush("EX6002",
                          "changing of byteorder not possible for 1 byte!"));

      #ifdef WIN32
      retval = send(sock,(char *)&data_8,1,0);
      #endif
      #ifdef UNIX
      do{retval = send(sock,(char *)&data_8,1,0);}
      while((retval==-1) && (errno == EINTR));
      #endif
            
      if (retval == SOCKET_ERROR) 
         {
         #ifdef WIN32
         errno = WSAGetLastError();
         #endif
         if       (errno == WSAENETDOWN)   *pstat=V3ERRTCP_ENETDOWN;
         else if  (errno == WSAENOTSOCK)   *pstat=V3ERRTCP_ENOTSOCK;
         else if  (errno == WSAENOTCONN)   *pstat=V3ERRTCP_ENOTCONN;
         else if  (errno == WSAETIMEDOUT)  *pstat=V3ERRTCP_ETIMEDOUT;
         else if  (errno == WSAECONNRESET) *pstat=V3ERRTCP_ECONNRESET;
         else                              *pstat=V3ERRTCP_SEND;
         return 0;
      }
      *pstat = retval;
      return(0);
      }

   else if (bytecount==2)
      {
      if (byteorder==0) data_16=(DBshort)data;
      else data_16=htons((DBshort)data);

      #ifdef WIN32
      retval = send(sock,(char *)&data_16,2,0);
      #endif
      #ifdef UNIX
      do{retval = send(sock,(char *)&data_16,2,0);}
      while((retval==-1) && (errno == EINTR));
      #endif

      if (retval == SOCKET_ERROR) 
         {
         #ifdef WIN32
         errno = WSAGetLastError();
         #endif
         if       (errno == WSAENETDOWN)   *pstat=V3ERRTCP_ENETDOWN;
         else if  (errno == WSAENOTSOCK)   *pstat=V3ERRTCP_ENOTSOCK;
         else if  (errno == WSAENOTCONN)   *pstat=V3ERRTCP_ENOTCONN;
         else if  (errno == WSAETIMEDOUT)  *pstat=V3ERRTCP_ETIMEDOUT;
         else if  (errno == WSAECONNRESET) *pstat=V3ERRTCP_ECONNRESET;
         else                              *pstat=V3ERRTCP_SEND;
         return 0;
         }
      *pstat = retval;
      return(0);
      }
   else
      {
      return(erpush("EX6002","Only 1, 2 or 4 bytes are allowed for integers"));
      }
   }



/*!****************************************************************************/

        short    EXtcpinint(
        DBint    sock,
        DBint    bytecount,
        DBint    byteorder,
        DBfloat  timeout,
        DBint   *data,
        DBint   *pstat)
/*     
 *      In: sock      = Id of the socket to receive data from.
 *          bytecount = The nuber of bytes to receive (1, 2 or 4).
 *          byteorder = Conversion from network byte order 0/1
 *          timeout   = Time to wait for incomming data before return -1
 *
 *      Ut: *data     =  A variable to retrieve the data
 *          *pstat    = Status 
 *
 *      (C) 2003-01-27 Sören Larsson, Örebro University
 *
 ****************************************************************************!*/

  {
   struct timeval time_out;
   int retval;
   DBint   data_32;    /*always 32 bit (4byte)*/
   DBshort data_16;    /*always 16 bit (2byte)*/
   DBchar  data_8;     /*always 8 bit  (1byte)*/
   fd_set readfds;

   /*-----------------------------------*/ 
   /* Fill in the timeval-structure     */
   /*-----------------------------------*/ 
   if (timeout<0) return(erpush("EX6002","negative timeout not allowed"));
   time_out.tv_sec=(int)floor(timeout);
   time_out.tv_usec=(int)floor((timeout-floor(timeout))*1000000 + 0.5);

   /*----------------------------------------------------------*/ 
   /* Check if anything to receive                             */
   /*----------------------------------------------------------*/ 
   FD_ZERO(&readfds);   
   FD_SET(sock,&readfds);

   if (time_out.tv_sec==0 && time_out.tv_usec==0)
      {
      #ifdef WIN32
      retval = select(sock+1,&readfds,NULL,NULL,NULL); 
      #endif
      #ifdef UNIX
      do{retval = select(sock+1,&readfds,NULL,NULL,NULL);}
      while((retval==-1) && (errno == EINTR));
      #endif
      }           
   else
      {
      #ifdef WIN32
      retval = select(sock+1,&readfds,NULL,NULL,&time_out);  
      #endif
      #ifdef UNIX
      do{retval = select(sock+1,&readfds,NULL,NULL,&time_out);}
      while((retval==-1) && (errno == EINTR));
      #endif
      }
        
   if (retval == SOCKET_ERROR) 
      {
      #ifdef WIN32
      errno = WSAGetLastError();
      #endif
      if       (errno == WSAENETDOWN)       *pstat=V3ERRTCP_ENETDOWN;
      else if  (errno == WSAENOTSOCK)       *pstat=V3ERRTCP_ENOTSOCK;
      else if  (errno == WSANOTINITIALISED) *pstat=V3ERRTCP_NOTINITIALISED;
      else                                  *pstat=V3ERRTCP_SELECT_RE;
      return 0;
      }

   if (retval == 0)
      {
      *pstat=V3ERRTCP_TIMEOUT;
      return 0;
      }

    /* be sure we are talking about our connection */
    if (FD_ISSET (sock, &readfds) != 1) 
      {
      *pstat=V3ERRTCP_SELECT_RE;
      return 0;
      }

   /*----------------------------------------------------------*/ 
   /* Receive                                                  */
   /*----------------------------------------------------------*/ 
   if (bytecount==4)
      {
      #ifdef WIN32
      retval = recv(sock,(char *)&data_32,4,0 );
      #endif
      #ifdef UNIX
      do{retval = recv(sock,(char *)&data_32,4,0 );}
      while((retval==-1) && (errno == EINTR));
      #endif
      if (retval == SOCKET_ERROR) 
         {
         #ifdef WIN32
         errno = WSAGetLastError();
         #endif
         if       (errno == WSAENETDOWN)   *pstat=V3ERRTCP_ENETDOWN;
         else if  (errno == WSAENOTSOCK)   *pstat=V3ERRTCP_ENOTSOCK;
         else if  (errno == WSAENOTCONN)   *pstat=V3ERRTCP_ENOTCONN;
         else if  (errno == WSAETIMEDOUT)  *pstat=V3ERRTCP_ETIMEDOUT;
         else if  (errno == WSAECONNRESET) *pstat=V3ERRTCP_ECONNRESET;
         else                              *pstat=V3ERRTCP_RCV;
         return 0;
         }
      if (byteorder==0) *data = (DBint)data_32;
      else *data = (DBint)ntohl(data_32);
      *pstat = retval; /* no off bytes received */
      return(0);
      }
   
   else if (bytecount==1)
      {
      #ifdef WIN32
      retval = recv(sock,(char *)&data_8,1,0 );
      #endif
      #ifdef UNIX
      do{retval = recv(sock,(char *)&data_8,1,0 );}
      while((retval==-1) && (errno == EINTR));
      #endif
      if (retval == SOCKET_ERROR) 
         {
         #ifdef WIN32
         errno = WSAGetLastError();
         #endif
         if       (errno == WSAENETDOWN)   *pstat=V3ERRTCP_ENETDOWN;
         else if  (errno == WSAENOTSOCK)   *pstat=V3ERRTCP_ENOTSOCK;
         else if  (errno == WSAENOTCONN)   *pstat=V3ERRTCP_ENOTCONN;
         else if  (errno == WSAETIMEDOUT)  *pstat=V3ERRTCP_ETIMEDOUT;
         else if  (errno == WSAECONNRESET) *pstat=V3ERRTCP_ECONNRESET;
         else                              *pstat=V3ERRTCP_RCV;
         return 0;
         }
      if (byteorder==0) *data = (char)data_8;
      else return(erpush("EX6002",
                         "change of byteorder not possible fo 1 byte!"));
      *pstat = retval; /* no off bytes received */
      return(0);
      }


   else if (bytecount==2)
      {
      #ifdef WIN32
      retval = recv(sock,(char *)&data_16,2,0 );
      #endif
      #ifdef UNIX
      do{retval = recv(sock,(char *)&data_16,2,0 );}
      while((retval==-1) && (errno == EINTR));
      #endif
      if (retval == SOCKET_ERROR) 
         {
         #ifdef WIN32
         errno = WSAGetLastError();
         #endif
         if       (errno == WSAENETDOWN)   *pstat=V3ERRTCP_ENETDOWN;
         else if  (errno == WSAENOTSOCK)   *pstat=V3ERRTCP_ENOTSOCK;
         else if  (errno == WSAENOTCONN)   *pstat=V3ERRTCP_ENOTCONN;
         else if  (errno == WSAETIMEDOUT)  *pstat=V3ERRTCP_ETIMEDOUT;
         else if  (errno == WSAECONNRESET) *pstat=V3ERRTCP_ECONNRESET;
         else                              *pstat=V3ERRTCP_RCV;
         return 0;
         }
      if (byteorder==0) *data = (DBshort)data_16;
      else *data = (DBshort)ntohs(data_16);
      *pstat = retval; /* no off bytes received */
      return(0);
      }
    else
      {
      return(erpush("EX6002","Only 1, 2 or 4 bytes are allowed for integers"));
      }
   }


/*!****************************************************************************/

        short    EXtcpoutfloat(
        DBint    sock,
        DBint    bytecount,
        DBint    byteorder,
        DBfloat  timeout,
        DBfloat  data,
        DBint   *pstat)
/*     
 *      In: sock      = Id of the socket to use for sending.
 *          bytecount = The nuber of bytes to send (4 or 8).
 *          byteorder = Conversion to network byte order 0/1
 *          timeout   = Time to wait before return -1
 *          data      = The data to be sent
 *          
 *      Ut: *pstat = Status 
 *
 *      (C) 2003-01-30 Sören Larsson, Örebro University
 *
 ****************************************************************************!*/

  {
   struct timeval time_out;
   int retval;
   DBfloat  data_64;   /* 64 bit (8byte)*/
   float    data_32;   /* 32 bit (4byte)*/
   DBint    int_32;    /* 32 bit (4byte)*/
   fd_set writefds;

   /*-----------------------------------*/ 
   /* Fill in the timeval-structure     */
   /*-----------------------------------*/ 
   if (timeout<0) return(erpush("EX6002","negative timeout not allowed"));
   time_out.tv_sec=(int)floor(timeout);
   time_out.tv_usec=(int)floor((timeout-floor(timeout))*1000000 + 0.5);

   /*----------------------------------------------------------*/ 
   /* Check if possible to send                                */
   /*----------------------------------------------------------*/ 
	FD_ZERO(&writefds);   
   FD_SET(sock,&writefds);

   if (time_out.tv_sec==0 && time_out.tv_usec==0)
      {
      #ifdef WIN32
      retval = select(sock+1,NULL,&writefds,NULL,NULL);   
      #endif
      #ifdef UNIX
      do{retval = select(sock+1,NULL,&writefds,NULL,NULL);}
      while((retval==-1) && (errno == EINTR));
      #endif
      }           
   else
      {
      #ifdef WIN32
      retval = select(sock+1,NULL,&writefds,NULL,&time_out);  
      #endif
      #ifdef UNIX
      do{retval = select(sock+1,NULL,&writefds,NULL,&time_out);}
      while((retval==-1) && (errno == EINTR));
      #endif
      }
   if (retval == SOCKET_ERROR) 
      {
      #ifdef WIN32
      errno = WSAGetLastError();
      #endif      
      if       (errno == WSAENETDOWN)       *pstat=V3ERRTCP_ENETDOWN;
      else if  (errno == WSAENOTSOCK)       *pstat=V3ERRTCP_ENOTSOCK;
      else if  (errno == WSANOTINITIALISED) *pstat=V3ERRTCP_NOTINITIALISED;
      else                                  *pstat=V3ERRTCP_SELECT_SE;
      return 0;
      }
   if (retval == 0)
      {
      *pstat=V3ERRTCP_TIMEOUT;
      return 0;
      }
    /* be sure we are talking about our connection */
    if (FD_ISSET(sock, &writefds) != 1) 
      {
      *pstat=V3ERRTCP_SELECT_SE;
      return 0;
      }
   /*-----------------------------------*/ 
   /* send()                            */
   /*-----------------------------------*/ 
   if (bytecount==8)
   {
      if (byteorder==0) data_64=(DBfloat)data;
      else return(erpush("EX6002",
                    "Conversion of byteorder is not allowed for 8 byte float"));
      
      #ifdef WIN32
      retval = send(sock,(char *)(void *)&data_64,8,0);
      #endif
      #ifdef UNIX
      do{retval = send(sock,(char *)(void *)&data_64,8,0);}
      while((retval==-1) && (errno == EINTR));
      #endif
      
      if (retval == SOCKET_ERROR) 
         {
         #ifdef WIN32
         errno = WSAGetLastError();
         #endif
         if       (errno == WSAENETDOWN)   *pstat=V3ERRTCP_ENETDOWN;
         else if  (errno == WSAENOTSOCK)   *pstat=V3ERRTCP_ENOTSOCK;
         else if  (errno == WSAENOTCONN)   *pstat=V3ERRTCP_ENOTCONN;
         else if  (errno == WSAETIMEDOUT)  *pstat=V3ERRTCP_ETIMEDOUT;
         else if  (errno == WSAECONNRESET) *pstat=V3ERRTCP_ECONNRESET;
         else                              *pstat=V3ERRTCP_SEND;
         return 0;
         }
      *pstat = retval;
      return(0);
   }
    
   else if (bytecount==4)
   {
      if (byteorder==0)
         {
         data_32=(float)data;
         
         #ifdef WIN32
         retval = send(sock,(char *)(void *)&data_32,4,0);
         #endif
         #ifdef UNIX
         do{retval = send(sock,(char *)(void *)&data_32,4,0);}
         while((retval==-1) && (errno == EINTR));
         #endif
         
         }
      else
         {
         data_32=(float)data;
         int_32=*(DBint *)(void *)&data_32;
         int_32=htonl(int_32);
         
         #ifdef WIN32
         retval = send(sock,(char *)(void *)&int_32,4,0);
         #endif
         #ifdef UNIX
         do{retval = send(sock,(char *)(void *)&int_32,4,0);}
         while((retval==-1) && (errno == EINTR));
         #endif
         
         }
      
      if (retval == SOCKET_ERROR) 
         {
         #ifdef WIN32
         errno = WSAGetLastError();
         #endif
         if       (errno == WSAENETDOWN)   *pstat=V3ERRTCP_ENETDOWN;
         else if  (errno == WSAENOTSOCK)   *pstat=V3ERRTCP_ENOTSOCK;
         else if  (errno == WSAENOTCONN)   *pstat=V3ERRTCP_ENOTCONN;
         else if  (errno == WSAETIMEDOUT)  *pstat=V3ERRTCP_ETIMEDOUT;
         else if  (errno == WSAECONNRESET) *pstat=V3ERRTCP_ECONNRESET;
         else                              *pstat=V3ERRTCP_SEND;
         return 0;
         }
      *pstat = retval;
      return(0);
   }
   else
      {
      return(erpush("EX6002","Only 4 or 8 bytes are allowed for floats"));
      }
  }


/*!****************************************************************************/

        short    EXtcpinfloat(
        DBint    sock,
        DBint    bytecount,
        DBint    byteorder,
        DBfloat  timeout,
        DBfloat *data,
        DBint   *pstat)
/*     
 *      In: sock      = Id of the socket to receive data from.
 *          bytecount = The nuber of bytes to receive (4 or 8).
 *          byteorder = Conversion from network byte order 0/1
 *          timeout   = Time to wait for incomming data before return -1
 *
 *      Ut: *data     = A variable to retrieve the data
 *          *pstat    = Status 
 *
 *      (C) 2003-01-30 Sören Larsson, Örebro University
 *
 ****************************************************************************!*/

  {
   struct timeval time_out;
   int retval;
   DBfloat   data_64;    /*64 bit (8byte)*/
   float     data_32;    /*32 bit (4byte)*/
   DBint     int_32;     /*pointer to 4 byte*/

   fd_set readfds;

   /*-----------------------------------*/ 
   /* Fill in the timeval-structure     */
   /*-----------------------------------*/ 

   if (timeout<0) return(erpush("EX6002","negative timeout not allowed"));
   time_out.tv_sec=(int)floor(timeout);
   time_out.tv_usec=(int)floor((timeout-floor(timeout))*1000000 + 0.5);

   /*----------------------------------------------------------*/ 
   /* Check if anything to receive                             */
   /*----------------------------------------------------------*/ 
      
   FD_ZERO(&readfds);
   FD_SET(sock,&readfds);

   if (time_out.tv_sec==0 && time_out.tv_usec==0)
      {
      #ifdef WIN32
      retval = select(sock+1,&readfds,NULL,NULL,NULL);   
      #endif
      #ifdef UNIX
      do{retval = select(sock+1,&readfds,NULL,NULL,NULL);}
      while((retval==-1) && (errno == EINTR));
      #endif
      }           
   else
      {
      #ifdef WIN32
      retval = select(sock+1,&readfds,NULL,NULL,&time_out);  
      #endif
      #ifdef UNIX
      do{retval = select(sock+1,&readfds,NULL,NULL,&time_out);}
      while((retval==-1) && (errno == EINTR));
      #endif
      }
   if (retval == SOCKET_ERROR) 
      {
      #ifdef WIN32
      errno = WSAGetLastError();
      #endif
      if       (errno == WSAENETDOWN)       *pstat=V3ERRTCP_ENETDOWN;
      else if  (errno == WSAENOTSOCK)       *pstat=V3ERRTCP_ENOTSOCK;
      else if  (errno == WSANOTINITIALISED) *pstat=V3ERRTCP_NOTINITIALISED;
      else                                  *pstat=V3ERRTCP_SELECT_RE;
      return 0;
      }

   if (retval == 0)
      {
      *pstat=V3ERRTCP_TIMEOUT;
      return 0;
      }
    /* be sure we are talking about our connection */
    if (FD_ISSET(sock, &readfds) != 1) 
      {
      *pstat=V3ERRTCP_SELECT_RE;
       return 0;
      }

   /*----------------------------------------------------------*/ 
   /* Receive                                                  */
   /*----------------------------------------------------------*/ 
   if (bytecount==8)
      {
      
      #ifdef WIN32
      retval = recv(sock,(char *)&data_64,bytecount,0 );
      #endif
      #ifdef UNIX
      do{retval = recv(sock,(char *)&data_64,bytecount,0 );}
      while((retval==-1) && (errno == EINTR));
      #endif
      
      if (retval == SOCKET_ERROR) 
         {
         #ifdef WIN32
         errno = WSAGetLastError();
         #endif
         if       (errno == WSAENETDOWN)   *pstat=V3ERRTCP_ENETDOWN;
         else if  (errno == WSAENOTSOCK)   *pstat=V3ERRTCP_ENOTSOCK;
         else if  (errno == WSAENOTCONN)   *pstat=V3ERRTCP_ENOTCONN;
         else if  (errno == WSAETIMEDOUT)  *pstat=V3ERRTCP_ETIMEDOUT;
         else if  (errno == WSAECONNRESET) *pstat=V3ERRTCP_ECONNRESET;
         else                              *pstat=V3ERRTCP_RCV;
         return 0;
         }
      if (byteorder==0) *data = (DBfloat)data_64;
      else return(erpush("EX6002",
                  "Conversion of byteorder is not allowed for 8 byte float"));
      *pstat = retval; /* no off bytes received */
      return(0);
      }
   else if (bytecount==4)
      {
      
      #ifdef WIN32
      retval = recv(sock,(char *)&data_32,bytecount,0 );
      #endif
      #ifdef UNIX
      do{retval = recv(sock,(char *)&data_32,bytecount,0 );}
      while((retval==-1) && (errno == EINTR));
      #endif
      
      if (retval == SOCKET_ERROR) 
         {
         #ifdef WIN32
         errno = WSAGetLastError();
         #endif
         if       (errno == WSAENETDOWN)   *pstat=V3ERRTCP_ENETDOWN;
         else if  (errno == WSAENOTSOCK)   *pstat=V3ERRTCP_ENOTSOCK;
         else if  (errno == WSAENOTCONN)   *pstat=V3ERRTCP_ENOTCONN;
         else if  (errno == WSAETIMEDOUT)  *pstat=V3ERRTCP_ETIMEDOUT;
         else if  (errno == WSAECONNRESET) *pstat=V3ERRTCP_ECONNRESET;
         else                              *pstat=V3ERRTCP_RCV;
         return 0;
         }
      *data = 0.0;
      if (byteorder==0) *data = (float)data_32;
      else 
         {
         int_32= ntohl(*(DBint *)(void *)&data_32);
         *data=(DBfloat)*(float *)(void *)&int_32;
         }
      *pstat = retval; /* no off bytes received */
      return(0);
      }
  else
      {
      return(erpush("EX6002","Only 4 or 8 bytes are allowed for floats"));
      }
  }



/*!****************************************************************************/

        short    EXtcpoutvec(
        DBint    sock,
        DBint    bytecount,
        DBint    byteorder,
        DBfloat  timeout,
        DBfloat  datax,
        DBfloat  datay,
        DBfloat  dataz,
        DBint   *pstat)
/*     
 *      In: sock      = Id of the socket to use for sending.
 *          bytecount = The nuber of bytes to use for each dim. (4 or 8).
 *          byteorder = Conversion to network byte order 0/1
 *          timeout   = Time to wait before return -1
 *          datax     = Data to be sent
 *          datay     = Data to be sent
 *          dataz     = Data to be sent
 *          
 *      Ut: *pstat = Status 
 *
 *      (C) 2003-01-30 Sören Larsson, Örebro University
 *
 ****************************************************************************!*/

  {
   struct timeval time_out;
   int i;
   int retval[3];
   DBfloat  data_64;   /* 64 bit (8byte)*/
   float    data_32;   /* 32 bit (4byte)*/
   DBint    int_32;    /* 32 bit (4byte)*/
   DBfloat  data;
   fd_set writefds;

   /*-----------------------------------*/ 
   /* Fill in the timeval-structure     */
   /*-----------------------------------*/ 
   if (timeout<0) return(erpush("EX6002","negative timeout not allowed"));
   time_out.tv_sec=(int)floor(timeout);
   time_out.tv_usec=(int)floor((timeout-floor(timeout))*1000000 + 0.5);


   for (i=0;i<3;i++)
      {
      if (i==0) data=datax;
      else if (i==1) data=datay;
      else data=dataz;

      /*----------------------------------------------------------*/ 
      /* Check if possible to send                                */
      /*----------------------------------------------------------*/ 
	   FD_ZERO(&writefds);   
      FD_SET(sock,&writefds);

      if (time_out.tv_sec==0 && time_out.tv_usec==0)
         {
         #ifdef WIN32
         retval[i] = select(sock+1,NULL,&writefds,NULL,NULL);   
         #endif
         #ifdef UNIX
         do{retval[i] = select(sock+1,NULL,&writefds,NULL,NULL);}
         while((retval[i]==-1) && (errno == EINTR));
         #endif
         }           
      else
         {
         #ifdef WIN32
         retval[i] = select(sock+1,NULL,&writefds,NULL,&time_out);  
         #endif
         #ifdef UNIX
         do{retval[i] = select(sock+1,NULL,&writefds,NULL,&time_out);}
         while((retval[i]==-1) && (errno == EINTR));
         #endif
         }
      if (retval[i] == SOCKET_ERROR) 
         {
         #ifdef WIN32
         errno = WSAGetLastError();
         #endif      
         if       (errno == WSAENETDOWN)       *pstat=V3ERRTCP_ENETDOWN;
         else if  (errno == WSAENOTSOCK)       *pstat=V3ERRTCP_ENOTSOCK;
         else if  (errno == WSANOTINITIALISED) *pstat=V3ERRTCP_NOTINITIALISED;
         else                                  *pstat=V3ERRTCP_SELECT_SE;
         return 0;
         }
      if (retval[i] == 0)
         {
         *pstat=V3ERRTCP_TIMEOUT;
         return 0;
         }
       /* be sure we are talking about our connection */
       if (FD_ISSET(sock, &writefds) != 1) 
         {
         *pstat=V3ERRTCP_SELECT_SE;
         return 0;
         }
      /*-----------------------------------*/ 
      /* send()                            */
      /*-----------------------------------*/ 
      if (bytecount==8)
         {
         if (byteorder==0) data_64=(DBfloat)data;
         else return(erpush("EX6002",
                   "Conversion of byteorder is not allowed for 8 byte float"));
      
         #ifdef WIN32
         retval[i] = send(sock,(char *)(void *)&data_64,8,0);
         #endif
         #ifdef UNIX
         do{retval[i] = send(sock,(char *)(void *)&data_64,8,0);}
         while((retval[i]==-1) && (errno == EINTR));
         #endif
      
         if (retval[i] == SOCKET_ERROR) 
            {
            #ifdef WIN32
            errno = WSAGetLastError();
            #endif
            if       (errno == WSAENETDOWN)   *pstat=V3ERRTCP_ENETDOWN;
            else if  (errno == WSAENOTSOCK)   *pstat=V3ERRTCP_ENOTSOCK;
            else if  (errno == WSAENOTCONN)   *pstat=V3ERRTCP_ENOTCONN;
            else if  (errno == WSAETIMEDOUT)  *pstat=V3ERRTCP_ETIMEDOUT;
            else if  (errno == WSAECONNRESET) *pstat=V3ERRTCP_ECONNRESET;
            else                              *pstat=V3ERRTCP_SEND;
            return 0;
            }
         }
    
      else if (bytecount==4)
         {
         if (byteorder==0)
            {
            data_32=(float)data;
            #ifdef WIN32
            retval[i] = send(sock,(char *)(void *)&data_32,4,0);
            #endif
            #ifdef UNIX
            do{retval[i] = send(sock,(char *)(void *)&data_32,4,0);}
            while((retval[i]==-1) && (errno == EINTR));
            #endif
            }
         else
            {
            data_32=(float)data;
            int_32=*(DBint *)(void *)&data_32;
            int_32=htonl(int_32);
            #ifdef WIN32
            retval[i] = send(sock,(char *)(void *)&int_32,4,0);
            #endif
            #ifdef UNIX
            do{retval[i] = send(sock,(char *)(void *)&int_32,4,0);}
            while((retval[i]==-1) && (errno == EINTR));
            #endif
            }
         if (retval[i] == SOCKET_ERROR) 
            {
            #ifdef WIN32
            errno = WSAGetLastError();
            #endif
            if       (errno == WSAENETDOWN)   *pstat=V3ERRTCP_ENETDOWN;
            else if  (errno == WSAENOTSOCK)   *pstat=V3ERRTCP_ENOTSOCK;
            else if  (errno == WSAENOTCONN)   *pstat=V3ERRTCP_ENOTCONN;
            else if  (errno == WSAETIMEDOUT)  *pstat=V3ERRTCP_ETIMEDOUT;
            else if  (errno == WSAECONNRESET) *pstat=V3ERRTCP_ECONNRESET;
            else                              *pstat=V3ERRTCP_SEND;
            return 0;
            }
         }
      else
         {
         return(erpush("EX6002","Only 4 or 8 bytes are allowed for floats"));
         }
     }
     *pstat = retval[0] + retval[1] + retval[2];
     return(0);
   }


/*!****************************************************************************/

        short    EXtcpinvec(
        DBint    sock,
        DBint    bytecount,
        DBint    byteorder,
        DBfloat  timeout,
        DBfloat *datax,
        DBfloat *datay,
        DBfloat *dataz,
        DBint   *pstat)
/*     
 *      In:  sock      = Id of the socket to receive data from.
 *           bytecount = The nuber of bytes to receive (4 or 8).
 *           byteorder = Conversion from network byte order 0/1
 *           timeout   = Time to wait for incomming data before return -1
 *
 *      Out: *data     = A variable to retrieve the x-component.
 *           *data     = A variable to retrieve the y-component.
 *           *data     = A variable to retrieve the z-component.
 *           *pstat    = Status 
 *
 *      (C) 2003-01-30 Sören Larsson, Örebro University
 *
 ****************************************************************************!*/

  {
   struct timeval time_out;
   int retval[3];
   int i;
   DBfloat   data_64;    /*64 bit (8byte)*/
   float     data_32;    /*32 bit (4byte)*/
   DBint     int_32;     /*pointer to 4 byte*/
   DBfloat  *data;
   fd_set readfds;

   /*-----------------------------------*/ 
   /* Fill in the timeval-structure     */
   /*-----------------------------------*/ 

   if (timeout<0) return(erpush("EX6002","negative timeout not allowed"));
   time_out.tv_sec=(int)floor(timeout);
   time_out.tv_usec=(int)floor((timeout-floor(timeout))*1000000 + 0.5);


   for (i=0;i<3;i++)
      {

      if (i==0) data=datax;
      else if (i==1) data=datay;
      else data=dataz;

      /*----------------------------------------------------------*/ 
      /* Check if anything to receive                             */
      /*----------------------------------------------------------*/ 
      
	   FD_ZERO(&readfds);
      FD_SET(sock,&readfds);

      if (time_out.tv_sec==0 && time_out.tv_usec==0)
         {
         #ifdef WIN32
         retval[i] = select(sock+1,&readfds,NULL,NULL,NULL);   
         #endif
         #ifdef UNIX
         do{retval[i] = select(sock+1,&readfds,NULL,NULL,NULL);}
         while((retval[i]==-1) && (errno == EINTR));
         #endif
         }           
      else
         {
         #ifdef WIN32
         retval[i] = select(sock+1,&readfds,NULL,NULL,&time_out);  
         #endif
         #ifdef UNIX
         do{retval[i] = select(sock+1,&readfds,NULL,NULL,&time_out);}
         while((retval[i]==-1) && (errno == EINTR));
         #endif
         }
      if (retval[i] == SOCKET_ERROR) 
         {
         #ifdef WIN32
         errno = WSAGetLastError();
         #endif
         if       (errno == WSAENETDOWN)       *pstat=V3ERRTCP_ENETDOWN;
         else if  (errno == WSAENOTSOCK)       *pstat=V3ERRTCP_ENOTSOCK;
         else if  (errno == WSANOTINITIALISED) *pstat=V3ERRTCP_NOTINITIALISED;
         else                                  *pstat=V3ERRTCP_SELECT_RE;
         return 0;
         }

      if (retval[i] == 0)
         {
         *pstat=V3ERRTCP_TIMEOUT;
         return 0;
         }
       /* be sure we are talking about our connection */
       if (FD_ISSET(sock, &readfds) != 1) 
         {
         *pstat=V3ERRTCP_SELECT_RE;
          return 0;
         }

      /*----------------------------------------------------------*/ 
      /* Receive                                                  */
      /*----------------------------------------------------------*/ 
      if (bytecount==8)
         {
      
         #ifdef WIN32
         retval[i] = recv(sock,(char *)&data_64,bytecount,0 );
         #endif
         #ifdef UNIX
         do{retval[i] = recv(sock,(char *)&data_64,bytecount,0 );}
         while((retval[i]==-1) && (errno == EINTR));
         #endif
      
         if (retval[i] == SOCKET_ERROR) 
            {
            #ifdef WIN32
            errno = WSAGetLastError();
            #endif
            if       (errno == WSAENETDOWN)   *pstat=V3ERRTCP_ENETDOWN;
            else if  (errno == WSAENOTSOCK)   *pstat=V3ERRTCP_ENOTSOCK;
            else if  (errno == WSAENOTCONN)   *pstat=V3ERRTCP_ENOTCONN;
            else if  (errno == WSAETIMEDOUT)  *pstat=V3ERRTCP_ETIMEDOUT;
            else if  (errno == WSAECONNRESET) *pstat=V3ERRTCP_ECONNRESET;
            else                              *pstat=V3ERRTCP_RCV;
            return 0;
            }
         if (byteorder==0) *data = (DBfloat)data_64;
         else return(erpush("EX6002",
                    "Conversion of byteorder is not allowed for 8 byte float"));
         }
      else if (bytecount==4)
         {
      
         #ifdef WIN32
         retval[i] = recv(sock,(char *)&data_32,bytecount,0 );
         #endif
         #ifdef UNIX
         do{retval[i] = recv(sock,(char *)&data_32,bytecount,0 );}
         while((retval[i]==-1) && (errno == EINTR));
         #endif
      
         if (retval[i] == SOCKET_ERROR) 
            {
            #ifdef WIN32
            errno = WSAGetLastError();
            #endif
            if       (errno == WSAENETDOWN)   *pstat=V3ERRTCP_ENETDOWN;
            else if  (errno == WSAENOTSOCK)   *pstat=V3ERRTCP_ENOTSOCK;
            else if  (errno == WSAENOTCONN)   *pstat=V3ERRTCP_ENOTCONN;
            else if  (errno == WSAETIMEDOUT)  *pstat=V3ERRTCP_ETIMEDOUT;
            else if  (errno == WSAECONNRESET) *pstat=V3ERRTCP_ECONNRESET;
            else                              *pstat=V3ERRTCP_RCV;
            return 0;
            }
         *data = 0.0;
         if (byteorder==0) *data = (float)data_32;
         else 
            {
            int_32= ntohl(*(DBint *)(void *)&data_32);
            *data=(DBfloat)*(float *)(void *)&int_32;
            }
         }
      else
         {
         return(erpush("EX6002","Only 4 or 8 bytes are allowed for floats"));
         }
      }
   *pstat = retval[0] + retval[1] + retval[2]; /* no off bytes received */
   return(0);
   }

short  EXtcpoutchar(DBint sock, DBfloat timeout, char *data, DBint *pstat)
{return(0);}

short  EXtcpinchar(DBint sock, DBfloat timeout, char *data, DBint *pstat)
{return(0);}




/*!****************************************************************************/

        short  EXtcpgetlocal(
        DBint sock, 
        char  *name, 
        DBint *port, 
        DBint *pstat)
/*     
 *      In:  sock  = Id of the socket connection to check
 *           
 *
 *      Out: *name  = String to retrieve the local adress 
 *           *port  = Integer to retrieve the local port
 *           *pstat = Status 
 *
 *      (C) 2003-02-05 Sören Larsson, Örebro University
 *
 ****************************************************************************!*/

   {
   struct sockaddr_in local_addr;
   unsigned int namelen;
   int retval;
   /*-------------------------------*/ 
   /* getsockname                   */
   /*-------------------------------*/ 
   namelen=sizeof(local_addr);
   retval=getsockname ( sock, (struct sockaddr *)&local_addr,  &namelen );
   if (retval == SOCKET_ERROR) 
      {
      #ifdef WIN32
      errno = WSAGetLastError();
      #endif
      if      (errno == WSAENETDOWN)       *pstat=V3ERRTCP_ENETDOWN;
      else if (errno == WSAENOTSOCK)       *pstat=V3ERRTCP_ENOTSOCK;
      else if (errno == WSAEINVAL)         *pstat=V3ERRTCP_EINVAL;
      else if (errno == WSANOTINITIALISED) *pstat=V3ERRTCP_NOTINITIALISED;
      else                                 *pstat=V3ERRTCP_GETSOCKET;
      name="";
      *port=0;
      return 0;
      }
   strcpy(name,inet_ntoa(local_addr.sin_addr));
   *port=(DBint)htons(local_addr.sin_port);
   *pstat = 0;
   return(0);
   }


/*!****************************************************************************/

        short  EXtcpgetremote(
        DBint sock, 
        char  *name, 
        DBint *port, 
        DBint *pstat)
/*     
 *      In:  sock  = Id of the socket connection to check
 *
 *      Out: *name  = String to retrieve the remote adress 
 *           *port  = Integer to retrieve the remote port
 *           *pstat = Status 
 *
 *      (C) 2003-02-05 Sören Larsson, Örebro University
 *
 ****************************************************************************!*/
  {
   struct sockaddr_in server_addr;
   unsigned int namelen;
   int retval;
   /*-------------------------------*/ 
   /* getpeername                   */
   /*-------------------------------*/ 
   namelen=sizeof(server_addr);
   retval=getpeername ( sock, (struct sockaddr *)&server_addr,  &namelen );
   if (retval == SOCKET_ERROR) 
      {
      #ifdef WIN32
      errno = WSAGetLastError();
      #endif
      if      (errno == WSAENETDOWN)       *pstat=V3ERRTCP_ENETDOWN;
      else if (errno == WSAENOTSOCK)       *pstat=V3ERRTCP_ENOTSOCK;
      else if (errno == WSAENOTCONN)       *pstat=V3ERRTCP_ENOTCONN;
      else if (errno == WSANOTINITIALISED) *pstat=V3ERRTCP_NOTINITIALISED;
      else                                 *pstat=V3ERRTCP_GETPEERNAME;
      name="";
      *port=0;
      return 0;
      }
   strcpy(name,inet_ntoa(server_addr.sin_addr));
   *port=(DBint)htons(server_addr.sin_port);
   *pstat = 0;
   return(0);
  }







   





 
  
   
   
   
   
   
   
   
   
   

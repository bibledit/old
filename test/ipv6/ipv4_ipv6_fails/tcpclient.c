/*
 * tcpclient.c
 *
 * Example of BSD TCP sockets client application.
 * Paired with tcpserver.c (BSD tcp server).
 *
 * Protocol independent (working with both IPV4 and IPV6)
 *
 * OS: FreeBSD
 * Compiler: cc
 *      to compile: % cc -o tcpclient -g tcpclient.c
 * syntax: tcpclient remotehost portnumber
 *
 *      The server must be running.
 *      % tcpclient localhost 10000
 *      % tcpclient dfulton.cs.pdx.edu 10000
 *      % tcpclient 131.252.215.3  10000 
 *      % tcpclinet 2001:468:1f04:2f0:201:2ff:fe48:9659 10000
 *
 * NOREQUESTS (10) buffers of size BUFSIZE (1024) are sent to
 *      the server.  The server reads the packets and writes them
 *      back to the client.
 *
 * compiler note:
 *      This is K&R C, not ANSI C.  The "cc" above is the C compiler
 *      as found on a BSD UNIX system (Say SunOS 4.1.4).  You should
 *      probably use gcc on other systems.
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/times.h>
#include <resolv.h>
#include <arpa/nameser.h>
#include <string.h>

#define BUFSIZE         1024    /* size of buffer sent */
#define NOREQUESTS      10      /* no of buffer's sent */


/* main i/o buffer
*/
static char iobuf[BUFSIZE];

/*
 * main
 *      
 *      fireup socket
 *      connect to remote server
 *      doit - do i/o to remote server
 *      close socket
 */
main(argc,argv)
int argc;
char **argv;  
{
  int sockfd, n;
  struct addrinfo hints, *res, *ressave;
  char *remote;
  char *port; /* portice name or decimal port number*/

  
  fillBuf( iobuf, BUFSIZE);
  setbuf(stdout, NULL);
  if ( argc != 3 ) {
                fprintf(stderr,"tcpclient remote\n");
                exit(1);
        }
  remote = argv[1];
  printf("remote %s, \n",remote);
  port= argv[2];
  printf("port %s, \n", port);

  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_family=AF_UNSPEC;
  hints.ai_socktype=SOCK_STREAM;
  hints.ai_protocol=IPPROTO_TCP;

  if((n=getaddrinfo(remote, port, &hints, &res))!=0)
    printf("getaddrinfo error for %s, %s; %s", remote, port, gai_strerror(n));

  ressave=res;

  do{
   sockfd=socket(res->ai_family, res->ai_socktype, res->ai_protocol);
 
   if(sockfd<0)
     continue;  /*ignore this returned Ip addr*/
  
   if(connect(sockfd, res->ai_addr, res->ai_addrlen)==0)
    { doit(sockfd,iobuf);
      printf("connection ok!\n"); /* success*/
      break;
    }
   else  {
     perror("connecting stream socket");
   }


   close(sockfd);/*ignore this one*/
  } while ((res=res->ai_next)!= NULL);

freeaddrinfo(ressave);

} 

/* do read write on socket to remote server.
 *
 * only read/write calls here
 */
doit(sock,buf) 
int sock;
char *buf;
{ 

        /* echo tests. we write packet to remote.
         *             we then read packet back.
         */
        echoTest(sock,buf);
}


/* put data (readable ASCII) in buffer
*/
fillBuf(buf,size)
char *buf;
int size;
{
        char *spt;
        int len;
        int i;
        char current;

        current = 'a';
        spt = buf;

        /* loop through buffer of "size", if
         * chars mod 64, put in newline and set
         * current to next letter
         */
        for ( i = 0; i < size; i++) {
                if ( (i % 64) == 0 ){
                        *spt++ = '\n';
                        if ( i != 0 )
                                current++;
                        continue;
                }
                *spt++ = current;
        }
}

/*
 * write buffer to remote and read same amount back
 */
echoTest(sock,buf)
int sock;
char *buf;
{
        register int i;
        int rc;
        unsigned int writecsum,readcsum;
        int noerrs;

        noerrs = 0;
        /* for NOREQUESTS write/read iterations
        */
        for ( i = 0; i < NOREQUESTS; i++) {
#ifdef DEBUG
                printf("client: iteration %d, about to write\n", i);
#endif
                if ( (rc = write(sock, buf, BUFSIZE)) < 0 ) {
                        perror("client: writing on socket stream");
                        exit(1);
                }
                /* read data back from socket
                */
                doRead(sock, buf, BUFSIZE);
#ifdef DEBUG
                printf("client: iteration %d, readback complete\n", i);
#endif
                /* more debug. print data to stdout
                write(1,buf,BUFSIZE);
                */
        }
}


/* read from socket. TCP read call issued to kernel
 * may return with less data than you expected. This routine
 * must check that and force a complete read.
 */
doRead(sock, buf,amountNeeded)
int sock;
char *buf;
int amountNeeded;
{
        register int i;
        int rc;
        char *bpt;
        int count = amountNeeded;
        int amtread;

        bpt = buf;
        amtread = 0;
 
again:
        if ( (rc = read(sock,bpt,count)) < 0 ) {
                perror("doRead: reading socket stream");
                exit(1);
        }
        amtread += rc;

        if ( amtread < amountNeeded ) {
                count = count - rc;     
                bpt = bpt + rc;
                goto again;
        }
}

/* 
 * dump a buffer in hex
 */
printBuf(buf,size)
char *buf;
{
        int i;
        i = 16;
        while(size--) {
                printf("%x", (int)(*buf++&0xff));
                if ( i == 0 ) {
                        i = 16;
                        printf("\n");
                }
                else
                        i--;
        }
}
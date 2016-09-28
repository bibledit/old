/*
 * tcpserver.c
 *
 * Example BSD TCP socket server. 
 * Paired with tcpclient.c.
 *
 * Protocol independent(working with both IPV4 and IPV6)
 *
 * OS: FreeBSD
 * compiler: cc
 *      % cc -o tcpserver -g tcpserver.c
 *
 * To run:
 *      % tcpserver portnumber
 *      % tcpclient remotehost(machine name|IP address)  portnumber
 *
 * The server  accepts some number of requests from the client and turns around
 * and writes those buffers back to the client.
 *
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

#define BUFSIZE 1024            /* size of i/o buffer */
#define NOREADS 10              /* number of buffers transferred */

main(argc,argv)
int argc;
char **argv;  
{
   char *port;
   int listenfd, connfd, n;
   const int on=1;
   struct addrinfo hints, *res, *ressave;
   socklen_t addrlen;
   struct sockaddr *cliaddr;
   char* host=NULL;
   int i, rc;
   char buf[BUFSIZE];

   if ( argc != 2 ) {
       fprintf(stderr,"tcpserver port \n");
       exit(1);
    }
   port = argv[1];

   bzero(&hints, sizeof(struct addrinfo));
   hints.ai_flags=AI_PASSIVE;
   hints.ai_family=AF_UNSPEC;
   hints.ai_socktype=SOCK_STREAM;
   hints.ai_protocol=IPPROTO_TCP;

   if((n= getaddrinfo(host, port, &hints, &res)) !=0)
     printf("tcp listen error for %s, %s", host, port, gai_strerror(n));

   ressave=res;

   do{

     listenfd=socket(res->ai_family, res->ai_socktype, res->ai_protocol);
     
     if(listenfd<0)
       continue;   /*fail*/
     
     setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

     if(bind(listenfd, res->ai_addr, res->ai_addrlen)==0)
       break;     /*sucess*/

     close(listenfd);/*fail, try next one*/
  } while((res=res->ai_next) !=NULL);

  listen(listenfd, 5);

  if(&addrlen)
    addrlen=res->ai_addrlen;

  freeaddrinfo(ressave);
  
  cliaddr=malloc(addrlen);
     
  if((connfd=accept(listenfd, cliaddr, &addrlen))<0)
      { perror("accept");
        exit(1);
      }
     
     /* read and echo so many packets
     */
     for ( i = 0; i < NOREADS; i++) {
            doRead(connfd, buf, BUFSIZE);
            rc = write(connfd, buf, BUFSIZE); 
            if ( rc < 0 ) {
                  perror("write");
                  exit(1);
             }
     }
     close(connfd); 
     close(listenfd);
}


/* read from socket. Read call issued to kernel
 * may return with incomplete data. This routine
 * must check that and force a complete read.
 */
doRead(sock, buf, amountNeeded)
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
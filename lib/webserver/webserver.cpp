/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include <iostream>
#include <cstdlib>
#include <arpa/inet.h>
#include <signal.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <webserver/http.h>
#include <bootstrap/bootstrap.h>
#include <webserver/request.h>


using namespace std;


void webserver () 
{
  // Create a socket descriptor.
  int listenfd = socket (AF_INET, SOCK_STREAM, 0);
  int optval = 1;

  // Eliminate "Address already in use" error from bind.
  setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof (int));
  
  // Listenfd will be an endpoint for all requests to a port on any IP address for this host.
  typedef struct sockaddr SA;
  struct sockaddr_in serveraddr;
  memset (&serveraddr, 0, sizeof (serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)8080);
  bind (listenfd, (SA *) &serveraddr, sizeof (serveraddr));
      
  // Make it a listening socket ready to accept many connection requests.
  listen (listenfd, 100);
  
  // Ignore SIGPIPE signal: When the browser cancels the request, it won't kill Bibledit.
  signal (SIGPIPE, SIG_IGN);

  // Keep waiting for, accepting, and processing connections.
  while (true) {
      
    // Socket and file descriptor for the client connection.
    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof (clientaddr);
    int connfd = accept (listenfd, (SA *) &clientaddr, &clientlen);
    // &clientaddr contains client info.

    // The environment for this request.
    // It gets passed around from function to function during the entire request.
    // This provides thread-safety to the request.
    Webserver_Request * request = new Webserver_Request ();

    // Read the client's request.
    char buffer[65535];
    size_t bytes_read;
    bytes_read = read (connfd, buffer, sizeof (buffer));
    if (bytes_read) {};
    string input = buffer;

    // The page the browser requests via GET.
    http_get_header_get (input, request);

    // Assemble response.
    bootstrap_index (request);
    http_assemble_response (request);

    // Send response to browser.    
    const char * output = request->reply.c_str();
    size_t length = request->reply.size (); // The C function strlen () fails on null characters in the reply, so take .size()
    ssize_t written = write (connfd, output, length);
    if (written) {};

    // Clear memory.
    delete request;

    // Done: Close.
    close (connfd);
  }


}


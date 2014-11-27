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


#include <webserver/http.h>
#include <bootstrap/bootstrap.h>
#include <webserver/request.h>
#include <config/globals.h>
#ifdef WIN32
#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#endif
#include <database/logs.h>


using namespace std;


void webserver () 
{
  // Setup server behaviour.
  if (strcmp (CLIENT_INSTALLATION, "1") == 0) config_globals_client_prepared = true;
  if (strcmp (OPEN_INSTALLATION, "1") == 0) config_globals_open_installation = true;

#ifdef WIN32
  int iResult;

  // Initialize Winsock
  WSADATA wsaData;
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) cout << "WSAStartup failed with error: " << iResult << endl;
#endif

#ifdef WIN32

  struct addrinfo *result = NULL;
  struct addrinfo hints;
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  // Resolve the server address and port
  const char * portc = "8080";
  iResult = getaddrinfo(NULL, portc, &hints, &result);
  if (iResult != 0) cout << "getaddrinfo failed with error: " << iResult << endl;

  // Create a SOCKET for listening
  SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (ListenSocket == INVALID_SOCKET) cout << "socket failed with error: " << WSAGetLastError() << endl;

  // Setup the TCP listening socket
  iResult = bind(ListenSocket, (const sockaddr *) result->ai_addr, (int)result->ai_addrlen);
  if (iResult == SOCKET_ERROR) cout << "bind failed with error: " << WSAGetLastError() << endl;

  freeaddrinfo(result);

  iResult = listen(ListenSocket, SOMAXCONN);
  if (iResult == SOCKET_ERROR) cout << "listen failed with error: " << WSAGetLastError() << endl;

#else

  // Server port to listen on.
  unsigned short porti = 8080;

  // Create a listening socket.
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);

  // Eliminate "Address already in use" error from bind.
  int optval = 1;
  setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(int));

  // The listening socket will be an endpoint for all requests to a port on any IP address for this host.
  typedef struct sockaddr SA;
  struct sockaddr_in serveraddr;
  memset(&serveraddr, 0, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons(porti);
  bind(listenfd, (SA *)&serveraddr, sizeof(serveraddr));

  // Make it a listening socket ready to accept many connection requests.
  listen(listenfd, 100);

  // Ignore SIGPIPE signal: When the browser cancels the request, it won't kill Bibledit.
  signal(SIGPIPE, SIG_IGN);

#endif

  // Keep waiting for, accepting, and processing connections.
  config_globals_running = true;
  while (config_globals_running) {

    // The environment for this request.
    // It gets passed around from function to function during the entire request.
    // This provides thread-safety to the request.
    Webserver_Request * request = new Webserver_Request ();

#ifdef WIN32

    // Accept a client socket
    SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) cout << "accept failed with error: " << WSAGetLastError() << endl;
    // Set the remote address for Windows see below: No longer need for Cygwin.

#else

    // Socket and file descriptor for the client connection.
    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(clientaddr);
    int connfd = accept(listenfd, (SA *)&clientaddr, &clientlen);

    // The client's remote IPv4 address in dotted notation.
    char remote_address[256];
    inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, remote_address, sizeof(remote_address));
    request->remote_address = remote_address;

#endif
    try {
      if (config_globals_running) {
  
#define BUFLEN 65535
  
#ifdef WIN32
  
        char recvbuf[BUFLEN];
        ZeroMemory(&recvbuf, BUFLEN);
  
        int bytesRead = recv(ClientSocket, recvbuf, BUFLEN, 0);
        string input = recvbuf;
  
#else
  
        // Read the client's request.
        char buffer [BUFLEN];
        memset(&buffer, 0, BUFLEN); // Fix valgrind unitialized value message.
        size_t bytes_read;
        bytes_read = read(connfd, buffer, sizeof(buffer));
        if (bytes_read) {};
        string input = buffer;
  
#endif
    
        // Parse the browser's request's headers.
        http_parse_headers (input, request);
    
        // Assemble response.
        bootstrap_index (request);
        http_assemble_response (request);
    
        // Send response to browser.    
        const char * output = request->reply.c_str();
        size_t length = request->reply.size (); // The C function strlen () fails on null characters in the reply, so take string::size()
#ifdef WIN32
        iResult = send(ClientSocket, output, length, 0);
        if (iResult == SOCKET_ERROR) cout << "send failed with error: " << WSAGetLastError() << endl;
#else
        int written = write(connfd, output, length);
        if (written) {};
#endif
  
      }

    } catch (exception & e) {
      Database_Logs::log (e.what ());
    } catch (exception * e) {
      Database_Logs::log (e->what ());
    } catch (...) {
      Database_Logs::log ("A general internal error occurred in the web server");
    }

    // Clear memory.
    delete request;

    // Done: Close.
#ifdef WIN32
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) cout << "shutdown failed with error: " << WSAGetLastError() << endl;
    closesocket(ClientSocket);
#else
    close(connfd);
#endif
  }

#ifdef WIN32
  // No longer need server socket
  closesocket(ListenSocket);

  // Cleanup
  WSACleanup();
#endif

}


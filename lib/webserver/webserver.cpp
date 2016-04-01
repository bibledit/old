/*
Copyright (©) 2003-2016 Teus Benschop.

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
#include <database/logs.h>
#include <config/logic.h>
#include <webserver/io.h>
#include <filter/string.h>


mutex mutex_requests;


/**********************************************************************/
/* Get a line from a socket, whether the line ends in a newline,
 * carriage return, or a CRLF combination.  Terminates the string read
 * with a null character.  If no newline indicator is found before the
 * end of the buffer, the string is terminated with a null.  If any of
 * the above three line terminators is read, the last character of the
 * string will be a linefeed and the string will be terminated with a
 * null character.
 * Parameters: the socket file descriptor
 *             the buffer to save the data in
 *             the size of the buffer
 * Returns: the number of bytes stored (excluding null) */
/**********************************************************************/
int get_line (int sock, char *buf, int size)
{
  int i = 0;
  char c = '\0';
  int n;
  
  while ((i < size - 1) && (c != '\n'))
  {
    n = recv (sock, &c, 1, 0);
    if (n > 0)
    {
      if (c == '\r')
      {
        n = recv (sock, &c, 1, MSG_PEEK);
        if ((n > 0) && (c == '\n')) recv (sock, &c, 1, 0);
        else c = '\n';
      }
      buf[i] = c;
      i++;
    }
    else
    c = '\n';
  }
  buf[i] = '\0';
  
  return(i);
}


// Processes a single request from a web client.
void webserver_process_request (int connfd, string clientaddress)
{
  mutex_requests.lock ();
  config_globals_simultaneous_connection_count++;
  mutex_requests.unlock ();

  // The environment for this request.
  // It gets passed around from function to function during the entire request.
  // This provides thread-safety to the request.
  Webserver_Request request;

  // Store remote client address in the request.
  request.remote_address = clientaddress;
  
  try {
    if (config_globals_running) {
      
      // Connection health flag.
      bool connection_healthy = true;
      
      // Read the client's request.
      // With the HTTP protocol it is not possible to read the request till EOF,
      // because EOF does never come, because the browser keeps the connection open
      // for the response.
      // The HTTP protocol works per line.
      // Read one line of data from the client.
      // An empty line marks the end of the headers.
#define BUFFERSIZE 2048
      int bytes_read;
      bool header_parsed = true;
      char buffer [BUFFERSIZE];
      // Fix valgrind unitialized value message.
      memset (&buffer, 0, BUFFERSIZE);
      do {
        bytes_read = get_line (connfd, buffer, BUFFERSIZE);
        if (bytes_read <= 0) connection_healthy = false;
        // Parse the browser's request's headers.
        header_parsed = http_parse_header (buffer, &request);
      } while (header_parsed);
      
      if (connection_healthy) {
        
        // In the case of a POST request, more data follows: The POST request itself.
        // The length of that data is indicated in the header's Content-Length line.
        // Read that data, and parse it.
        string postdata;
        if (request.is_post) {
          bool done_reading = false;
          int total_bytes_read = 0;
          do {
            bytes_read = read (connfd, buffer, BUFFERSIZE);
            for (int i = 0; i < bytes_read; i++) {
              postdata += buffer [i];
            }
            // EOF indicates reading is ready.
            // An error also indicates that reading is ready.
            if (bytes_read <= 0) done_reading = true;
            if (bytes_read < 0) connection_healthy = false;
            // "Content-Length" bytes read: Done.
            total_bytes_read += bytes_read;
            if (total_bytes_read >= request.content_length) done_reading = true;
          } while (!done_reading);
          if (total_bytes_read < request.content_length) connection_healthy = false;
        }
        
        if (connection_healthy) {

          http_parse_post (postdata, &request);
          
          // Assemble response.
          bootstrap_index (&request);
          http_assemble_response (&request);
          
          // Send response to browser.
          const char * output = request.reply.c_str();
          // The C function strlen () fails on null characters in the reply, so take string::size()
          size_t length = request.reply.size ();
          int written = write (connfd, output, length);
          if (written) {};
          
        } else {
          // cerr << "Insufficient data received, closing connection" << endl;
        }

      } else {
        // cerr << "Unhealthy connection was closed" << endl;
      }
    }
  } catch (exception & e) {
    string message ("Internal error: ");
    message.append (e.what ());
    Database_Logs::log (message);
  } catch (exception * e) {
    string message ("Internal error: ");
    message.append (e->what ());
    Database_Logs::log (message);
  } catch (...) {
    Database_Logs::log ("A general internal error occurred");
  }
  
  // Done: Close.
  close (connfd);

  mutex_requests.lock ();
  config_globals_simultaneous_connection_count--;
  mutex_requests.unlock ();
}


void webserver ()
{
  // Setup server behaviour.
#ifdef CLIENT_PREPARED
  config_globals_client_prepared = true;
#else
  config_globals_client_prepared = false;
#endif
  if (strcmp (DEMO, "yes") == 0) config_globals_open_installation = true;

  // Create a listening socket.
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0) cerr << "Error opening socket: It returns a descriptor of " << listenfd << endl;

  // Eliminate "Address already in use" error from bind.
  int optval = 1;
  int result = setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, (const char *) &optval, sizeof (int));
  if (result != 0) cerr << "Error setting socket options" << endl;

  // The listening socket will be an endpoint for all requests to a port on any IP address for this host.
  typedef struct sockaddr SA;
  struct sockaddr_in serveraddr;
  memset (&serveraddr, 0, sizeof (serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl (INADDR_ANY);
  serveraddr.sin_port = htons (convert_to_int (config_logic_network_port ()));
  result = mybind (listenfd, (SA *) &serveraddr, sizeof (serveraddr));
  if (result != 0) cerr << "Error binding server to socket" << endl;

  // Make it a listening socket ready to accept many connection requests.
  result = listen (listenfd, 100);
  if (result != 0) cerr << "Error listening on socket" << endl;

  // Ignore SIGPIPE signal: When the browser cancels the request, it won't kill Bibledit.
  signal (SIGPIPE, SIG_IGN);

  // Keep waiting for, accepting, and processing connections.
  config_globals_running = true;
  while (config_globals_running) {

    // Socket and file descriptor for the client connection.
    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(clientaddr);
    int connfd = accept (listenfd, (SA *)&clientaddr, &clientlen);
    if (connfd > 0) {

      // Socket receive timeout.
      struct timeval tv;
      tv.tv_sec = 60;
      tv.tv_usec = 0;
      setsockopt (connfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
      
      // The client's remote IPv4 address in dotted notation.
      char remote_address[256];
      inet_ntop (AF_INET, &clientaddr.sin_addr.s_addr, remote_address, sizeof (remote_address));
      string clientaddress = remote_address;
      
      // Handle this request in a thread, enabling parallel requests.
      thread request_thread = thread (webserver_process_request, connfd, clientaddress);
      // Detach and delete thread object.
      request_thread.detach ();
      
    } else {
      cerr << "Error accepting connection on socket" << endl;
    }
  }
  
  // Close listening socket, freeing it for a possible subsequent server process.
  close (listenfd);
}


/*

 Notes about the network port and a proxy.
 
 In case a client can only connect through port 80, 
 then this may proxy a certain folder to another port:
 http://serverfault.com/questions/472482/proxypass-redirect-directory-url-to-non-standard-port
 
 Or to write our own server in C acting as a proxy 
 to forward incoming requests to the bibledit instances on localhost.
 
 But since there are URLs requested that start with a /, 
 that may not work with the proxy.
 That needs a fix first.
 
*/
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
#include <filter/url.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/certs.h>
#include <mbedtls/x509.h>
#include <mbedtls/ssl.h>
#include <mbedtls/net.h>
#include <mbedtls/error.h>
#include <mbedtls/ssl_cache.h>


// Gets a line from a socket.
// The line may end with a newline, a carriage return, or a CR-LF combination.
// It terminates the string read with a null character.
// If no newline indicator is found before the end of the buffer the string is terminated with a null.
// If any of the above three line terminators is read,
// the last character of the string will be a linefeed
// and the string will be terminated with a null character.
// Parameters: the socket file descriptor
//             the buffer to save the data to
//             the size of the buffer
// Returns: the number of bytes stored (excluding null).
int get_line (int sock, char *buf, int size)
{
  int i = 0;
  char c = '\0';
  int n = 0;
  while ((i < size - 1) && (c != '\n')) {
    n = recv (sock, &c, 1, 0);
    if (n > 0) {
      if (c == '\r') {
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
  // The environment for this request.
  // It gets passed around from function to function during the entire request.
  // This provides thread-safety to the request.
  Webserver_Request request;
  
  // This is the plain http server.
  request.secure = false;

  // Store remote client address in the request.
  request.remote_address = clientaddress;
  
  try {
    if (config_globals_http_running) {
      
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
}


void http_server ()
{
  // Whether the plain http server redirects to secure http.
  config_globals_enforce_https_browser = config_logic_enforce_https_browser ();
  config_globals_enforce_https_client = config_logic_enforce_https_client ();
  
  // Create a listening socket.
  int listenfd = socket (AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0) cerr << "Error opening socket: It returns a descriptor of " << listenfd << endl;

  // Eliminate "Address already in use" error from bind.
  int optval = 1;
  int result = setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, (const char *) &optval, sizeof (int));
  if (result != 0) cerr << "Error setting socket options" << endl;

  // The listening socket will be an endpoint for all requests to a port on this host.
  // When configured as a server it listens on any IP address.
  // When configured as a client, it listens on the loopback device.
  typedef struct sockaddr SA;
  struct sockaddr_in serveraddr;
  memset (&serveraddr, 0, sizeof (serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr =
#ifdef CLIENT_PREPARED
  htonl (INADDR_LOOPBACK);
#else
  htonl (INADDR_ANY);
#endif
  serveraddr.sin_port = htons (convert_to_int (config_logic_http_network_port ()));
  result = mybind (listenfd, (SA *) &serveraddr, sizeof (serveraddr));
  if (result != 0) cerr << "Error binding server to socket" << endl;

  // Make it a listening socket ready to accept many connection requests.
  result = listen (listenfd, 100);
  if (result != 0) cerr << "Error listening on socket" << endl;

  // Keep waiting for, accepting, and processing connections.
  config_globals_http_running = true;
  while (config_globals_http_running) {

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


// Processes a single request from a web client.
void secure_webserver_process_request (mbedtls_ssl_config * conf, mbedtls_net_context client_fd)
{
  // Socket receive timeout.
  struct timeval tv;
  tv.tv_sec = 60;
  tv.tv_usec = 0;
  setsockopt (client_fd.fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  
  // The environment for this request.
  // It gets passed around from function to function during the entire request.
  // This provides thread-safety to the request.
  Webserver_Request request;
  
  // This is the secure http server.
  request.secure = true;
  
  // SSL/TSL data.
  mbedtls_ssl_context ssl;
  mbedtls_ssl_init (&ssl);
  
  try {

    if (config_globals_https_running) {

      // The client's remote IPv4 address in dotted notation.
      struct sockaddr_in addr;
      socklen_t addr_size = sizeof(struct sockaddr_in);
      getpeername (client_fd.fd, (struct sockaddr *)&addr, &addr_size);
      char remote_address [256];
      inet_ntop (AF_INET, &addr.sin_addr.s_addr, remote_address, sizeof (remote_address));
      // Store it in the webserver request object.
      request.remote_address = remote_address;
      
      // This flag indicates a healthy connection: One that can proceed.
      bool connection_healthy = true;
      // Function results.
      int ret;
      
      if (connection_healthy) {
        ret = mbedtls_ssl_setup (&ssl, conf);
        if (ret != 0) {
          filter_url_display_mbed_tls_error (ret);
          connection_healthy = false;
        }
      }
      
      if (connection_healthy) {
        mbedtls_ssl_set_bio (&ssl, &client_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
      }
      
      // SSL / TLS handshake.
      while (connection_healthy && (ret = mbedtls_ssl_handshake (&ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
          if (config_globals_https_running) {
            // In case the secure server runs, display the error.
            // And in case the server is interrupted by e.g. Ctrl-C, don't display this error.
            filter_url_display_mbed_tls_error (ret);
          }
          connection_healthy = false;
        }
      }
      
      // Read the HTTP headers.
      bool header_parsed = true;
      string header_line;
      while (connection_healthy && header_parsed) {
        // Read the client's request.
        // With the HTTP protocol it is not possible to read the request till EOF,
        // because EOF does not always come,
        // since the browser may keep the connection open for the response.
        // The HTTP protocol works per line.
        // Read and parse one line of data from the client.
        // An empty line marks the end of the headers.
        unsigned char buffer [1];
        memset (&buffer, 0, 1);
        ret = mbedtls_ssl_read (&ssl, buffer, 1);
        if (ret == MBEDTLS_ERR_SSL_WANT_READ) continue;
        if (ret == MBEDTLS_ERR_SSL_WANT_WRITE) continue;
        if (ret == 0) header_parsed = false; // 0: EOF
        if (ret < 0) connection_healthy = false;
        if (connection_healthy && header_parsed) {
          char c = buffer [0];
          // The request contains a carriage return (\r) and a new line feed (\n).
          // The traditional order of this is \r\n.
          // Therefore when a \r is encountered, just disregard it.
          // A \n will follow to mark the end of the header line.
          if (c == '\r') continue;
          // At a new line, parse the received header line.
          if (c == '\n') {
            header_parsed = http_parse_header (header_line, &request);
            header_line.clear ();
          } else {
            header_line += c;
          }
        }
      }
      header_line.clear ();
      
      if (request.is_post) {
        // In the case of a POST request, more data follows:
        // The POST request itself.
        // The length of that data is indicated in the header's Content-Length line.
        // Read that data.
        string postdata;
        bool done_reading = false;
        int total_bytes_read = 0;
        while (connection_healthy && !done_reading) {
          unsigned char buffer [1];
          memset (&buffer, 0, 1);
          ret = mbedtls_ssl_read (&ssl, buffer, 1);
          if (ret == MBEDTLS_ERR_SSL_WANT_READ) continue;
          if (ret == MBEDTLS_ERR_SSL_WANT_WRITE) continue;
          if (ret == 0) done_reading = true; // 0: EOF
          if (ret < 0) connection_healthy = false;
          if (connection_healthy && !done_reading) {
            char c = buffer [0];
            postdata += c;
            total_bytes_read ++;
          }
          // "Content-Length" bytes read: Done.
          if (total_bytes_read >= request.content_length) done_reading = true;
        }
        if (total_bytes_read < request.content_length) connection_healthy = false;
        // Parse the POSTed data.
        if (connection_healthy) {
          http_parse_post (postdata, &request);
        }
      }
      
      // Assemble response.
      if (connection_healthy) {
        bootstrap_index (&request);
        http_assemble_response (&request);
      }
      
      // Write the response to the browser.
      const char * output = request.reply.c_str();
      const unsigned char * buf = (const unsigned char *) output;
      // The C function strlen () fails on null characters in the reply, so take string::size()
      size_t len = request.reply.size ();
      while (connection_healthy && (len > 0)) {
        // Function
        // int ret = mbedtls_ssl_write (&ssl, buf, len)
        // will do partial writes in some cases.
        // If the return value is non-negative but less than length,
        // the function must be called again with updated arguments:
        // buf + ret, len - ret
        // until it returns a value equal to the last 'len' argument.
        ret = mbedtls_ssl_write (&ssl, buf, len);
        if (ret > 0) {
          buf += ret;
          len -= ret;
        } else {
          // When it returns MBEDTLS_ERR_SSL_WANT_WRITE/READ,
          // it must be called later with the *same* arguments,
          // until it returns a positive value.
          if (ret == MBEDTLS_ERR_SSL_WANT_READ) continue;
          if (ret == MBEDTLS_ERR_SSL_WANT_WRITE) continue;
          filter_url_display_mbed_tls_error (ret);
          connection_healthy = false;
        }
      }

      // Close SSL/TLS connection.
      if (connection_healthy) {
        while ((ret = mbedtls_ssl_close_notify (&ssl)) < 0) {
          if (ret == MBEDTLS_ERR_SSL_WANT_READ) continue;
          if (ret == MBEDTLS_ERR_SSL_WANT_WRITE) continue;
          filter_url_display_mbed_tls_error (ret);
          connection_healthy = false;
          break;
        }
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
  
  // Close client network connection.
  mbedtls_net_free (&client_fd);
  
  // Done with the SSL context.
  mbedtls_ssl_free (&ssl);
}


void https_server ()
{
  // On clients, don't run the secure web server.
  // It is not possible to get a https certificate for https://localhost anyway.
  // Shutting down this secure server saves value system resources on low power devices.
#ifdef CLIENT_PREPARED
  return;
#endif
  // File descriptor for the listener.
  mbedtls_net_context listen_fd;
  mbedtls_net_init (&listen_fd);
  
  // The SSL configuration for the lifetime of the server.
  // This is done during initialisation of mbed TLS.
  mbedtls_ssl_config conf;
  mbedtls_ssl_config_init (&conf);
  
  mbedtls_ssl_cache_context cache;
  mbedtls_ssl_cache_init (&cache);

  // A single entropy source that is used in all the threads.
  mbedtls_entropy_context entropy;
  mbedtls_entropy_init (&entropy);

  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_ctr_drbg_init (&ctr_drbg);

  int ret;
  string path;
  
  // Load the private RSA server key.
  mbedtls_pk_context pkey;
  mbedtls_pk_init (&pkey);
  path = config_logic_server_key_path ();
  ret = mbedtls_pk_parse_keyfile (&pkey, path.c_str (), NULL);
  if (ret != 0) filter_url_display_mbed_tls_error (ret);
  
  // Server certificates store.
  mbedtls_x509_crt srvcert;
  mbedtls_x509_crt_init (&srvcert);
  
  // Load the server certificate.
  path = config_logic_server_certificate_path ();
  ret = mbedtls_x509_crt_parse_file (&srvcert, path.c_str ());
  if (ret != 0) filter_url_display_mbed_tls_error (ret);

  // Load the chain of certificates of the certificate authorities.
  path = config_logic_authorities_certificates_path ();
  ret = mbedtls_x509_crt_parse_file (&srvcert, path.c_str ());
  if (ret != 0) filter_url_display_mbed_tls_error (ret);

  // Seed the random number generator.
  const char *pers = "Bibledit Cloud";
  ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *) pers, strlen (pers));
  if (ret != 0) {
    filter_url_display_mbed_tls_error (ret);
    return;
  }
  
  // Setup the listening TCP socket.
  ret = mbedtls_net_bind (&listen_fd, NULL, convert_to_string (config_logic_https_network_port ()).c_str (), MBEDTLS_NET_PROTO_TCP);
  if (ret != 0) {
    filter_url_display_mbed_tls_error (ret);
    return;
  }
  
  // Setup SSL/TLS default values for the lifetime of the https server.
  ret = mbedtls_ssl_config_defaults (&conf, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
  if (ret != 0) {
    filter_url_display_mbed_tls_error (ret);
    return;
  }
  mbedtls_ssl_conf_rng (&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
  mbedtls_ssl_conf_session_cache (&conf, &cache, mbedtls_ssl_cache_get, mbedtls_ssl_cache_set);
  mbedtls_ssl_conf_ca_chain (&conf, srvcert.next, NULL);
  ret = mbedtls_ssl_conf_own_cert (&conf, &srvcert, &pkey);
  if (ret != 0) {
    filter_url_display_mbed_tls_error (ret);
    return;
  }
  
  // Keep preparing for, accepting, and processing client connections.
  config_globals_https_running = true;
  while (config_globals_https_running) {
    
    // Client connection file descriptor.
    mbedtls_net_context client_fd;
    mbedtls_net_init (&client_fd);
    
    // Wait until a client connects.
    ret = mbedtls_net_accept (&listen_fd, &client_fd, NULL, 0, NULL);
    if (ret != 0 ) {
      filter_url_display_mbed_tls_error (ret);
      continue;
    }
    
    // Handle this request in a thread, enabling parallel requests.
    thread request_thread = thread (secure_webserver_process_request, &conf, client_fd);
    // Detach and delete thread object.
    request_thread.detach ();
  }
  
  // Wait shortly to give sufficient time to let the connection fail,
  // before the local SSL/TLS variables get out of scope,
  // which would lead to a segmentation fault if those variables were still in use.
  this_thread::sleep_for (chrono::milliseconds (5));

  // Close listening socket, freeing it for a possible subsequent server process.
  mbedtls_net_free (&listen_fd);
  
  // Shutting down mbed TLS.
  mbedtls_x509_crt_free (&srvcert);
  mbedtls_pk_free (&pkey);
  mbedtls_ssl_config_free (&conf);
  mbedtls_ssl_cache_free (&cache);
  mbedtls_ctr_drbg_free (&ctr_drbg);
  mbedtls_entropy_free (&entropy);
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
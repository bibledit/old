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
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/certs.h>
#include <mbedtls/x509.h>
#include <mbedtls/ssl.h>
#include <mbedtls/net.h>
#include <mbedtls/error.h>
#include <mbedtls/ssl_cache.h>


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
  // The environment for this request.
  // It gets passed around from function to function during the entire request.
  // This provides thread-safety to the request.
  Webserver_Request request;

  // Store remote client address in the request.
  request.remote_address = clientaddress;
  
  try {
    if (config_globals_http_running) { // Todo also secure version.
      
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


void https_server_display_mbed_tls_error (int & ret)
{
  if (ret != 0) {
    char error_buf [100];
    mbedtls_strerror (ret, error_buf, 100);
    cerr << error_buf << " (" << ret << ")" << endl;
    ret = 0;
  }
}


// Processes a single request from a web client.
void secure_webserver_process_request (int connfd, string clientaddress) // Todo
{

  /* Todo sample threading server .

  mbedtls_threading_mutex_t debug_mutex;
  
  static void my_mutexed_debug( void *ctx, int level,
                               const char *file, int line,
                               const char *str )
  {
    long int thread_id = (long int) pthread_self();
    
    mbedtls_mutex_lock( &debug_mutex );
    
    ((void) level);
    mbedtls_fprintf( (FILE *) ctx, "%s:%04d: [ #%ld ] %s",
                    file, line, thread_id, str );
    fflush(  (FILE *) ctx  );
    
    mbedtls_mutex_unlock( &debug_mutex );
  }
  
  typedef struct {
    mbedtls_net_context client_fd;
    int thread_complete;
    const mbedtls_ssl_config *config;
  } thread_info_t;
  
  typedef struct {
    int active;
    thread_info_t   data;
    pthread_t       thread;
  } pthread_info_t;
  
  static thread_info_t    base_info;
  static pthread_info_t   threads[MAX_NUM_THREADS];
  
  static void *handle_ssl_connection( void *data )
  {
    int ret, len;
    thread_info_t *thread_info = (thread_info_t *) data;
    mbedtls_net_context *client_fd = &thread_info->client_fd;
    long int thread_id = (long int) pthread_self();
    unsigned char buf[1024];
    mbedtls_ssl_context ssl;
    
    // Make sure memory references are valid
    mbedtls_ssl_init( &ssl );
    
    mbedtls_printf( "  [ #%ld ]  Setting up SSL/TLS data\n", thread_id );
    
    // 4. Get the SSL context ready
    if( ( ret = mbedtls_ssl_setup( &ssl, thread_info->config ) ) != 0 )
    {
      mbedtls_printf( "  [ #%ld ]  failed: mbedtls_ssl_setup returned -0x%04x\n",
                     thread_id, -ret );
      goto thread_exit;
    }
    
    mbedtls_ssl_set_bio( &ssl, client_fd, mbedtls_net_send, mbedtls_net_recv, NULL );
    
    // 5. Handshake
    mbedtls_printf( "  [ #%ld ]  Performing the SSL/TLS handshake\n", thread_id );
    
    while( ( ret = mbedtls_ssl_handshake( &ssl ) ) != 0 )
    {
      if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
      {
        mbedtls_printf( "  [ #%ld ]  failed: mbedtls_ssl_handshake returned -0x%04x\n",
                       thread_id, -ret );
        goto thread_exit;
      }
    }
    
    mbedtls_printf( "  [ #%ld ]  ok\n", thread_id );
    
    // 6. Read the HTTP Request
    mbedtls_printf( "  [ #%ld ]  < Read from client\n", thread_id );
    
    do
    {
      len = sizeof( buf ) - 1;
      memset( buf, 0, sizeof( buf ) );
      ret = mbedtls_ssl_read( &ssl, buf, len );
      
      if( ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE )
        continue;
      
      if( ret <= 0 )
      {
        switch( ret )
        {
          case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
            mbedtls_printf( "  [ #%ld ]  connection was closed gracefully\n",
                           thread_id );
            goto thread_exit;
            
          case MBEDTLS_ERR_NET_CONN_RESET:
            mbedtls_printf( "  [ #%ld ]  connection was reset by peer\n",
                           thread_id );
            goto thread_exit;
            
          default:
            mbedtls_printf( "  [ #%ld ]  mbedtls_ssl_read returned -0x%04x\n",
                           thread_id, -ret );
            goto thread_exit;
        }
      }
      
      len = ret;
      mbedtls_printf( "  [ #%ld ]  %d bytes read\n=====\n%s\n=====\n",
                     thread_id, len, (char *) buf );
      
      if( ret > 0 )
        break;
    }
    while( 1 );
    
    // 7. Write the 200 Response
    mbedtls_printf( "  [ #%ld ]  > Write to client:\n", thread_id );
    
    len = sprintf( (char *) buf, HTTP_RESPONSE,
                  mbedtls_ssl_get_ciphersuite( &ssl ) );
    
    while( ( ret = mbedtls_ssl_write( &ssl, buf, len ) ) <= 0 )
    {
      if( ret == MBEDTLS_ERR_NET_CONN_RESET )
      {
        mbedtls_printf( "  [ #%ld ]  failed: peer closed the connection\n",
                       thread_id );
        goto thread_exit;
      }
      
      if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
      {
        mbedtls_printf( "  [ #%ld ]  failed: mbedtls_ssl_write returned -0x%04x\n",
                       thread_id, ret );
        goto thread_exit;
      }
    }
    
    len = ret;
    mbedtls_printf( "  [ #%ld ]  %d bytes written\n=====\n%s\n=====\n",
                   thread_id, len, (char *) buf );
    
    mbedtls_printf( "  [ #%ld ]  . Closing the connection...", thread_id );
    
    while( ( ret = mbedtls_ssl_close_notify( &ssl ) ) < 0 )
    {
      if( ret != MBEDTLS_ERR_SSL_WANT_READ &&
         ret != MBEDTLS_ERR_SSL_WANT_WRITE )
      {
        mbedtls_printf( "  [ #%ld ]  failed: mbedtls_ssl_close_notify returned -0x%04x\n",
                       thread_id, ret );
        goto thread_exit;
      }
    }
    
    mbedtls_printf( " ok\n" );
    
    ret = 0;
    
  thread_exit:
    
#ifdef MBEDTLS_ERROR_C
    if( ret != 0 )
    {
      char error_buf[100];
      mbedtls_strerror( ret, error_buf, 100 );
      mbedtls_printf("  [ #%ld ]  Last error was: -0x%04x - %s\n\n",
                     thread_id, -ret, error_buf );
    }
#endif
    
    mbedtls_net_free( client_fd );
    mbedtls_ssl_free( &ssl );
    
    thread_info->thread_complete = 1;
    
    return( NULL );
  }
  
  static int thread_create( mbedtls_net_context *client_fd )
  {
    int ret, i;
    
    // Find in-active or finished thread slot
    for( i = 0; i < MAX_NUM_THREADS; i++ )
    {
      if( threads[i].active == 0 )
        break;
      
      if( threads[i].data.thread_complete == 1 )
      {
        mbedtls_printf( "  [ main ]  Cleaning up thread %d\n", i );
        pthread_join(threads[i].thread, NULL );
        memset( &threads[i], 0, sizeof(pthread_info_t) );
        break;
      }
    }
    
    if( i == MAX_NUM_THREADS )
      return( -1 );
    
    // Fill thread-info for thread
    memcpy( &threads[i].data, &base_info, sizeof(base_info) );
    threads[i].active = 1;
    memcpy( &threads[i].data.client_fd, client_fd, sizeof( mbedtls_net_context ) );
    
    if( ( ret = pthread_create( &threads[i].thread, NULL, handle_ssl_connection,
                               &threads[i].data ) ) != 0 )
    {
      return( ret );
    }
    
    return( 0 );
  }
  
  int main( void )
  {
    int ret;
    mbedtls_net_context listen_fd, client_fd;
    const char pers[] = "ssl_pthread_server";
    
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt srvcert;
    mbedtls_x509_crt cachain;
    mbedtls_pk_context pkey;
#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
    unsigned char alloc_buf[100000];
#endif
#if defined(MBEDTLS_SSL_CACHE_C)
    mbedtls_ssl_cache_context cache;
#endif
    
#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
    mbedtls_memory_buffer_alloc_init( alloc_buf, sizeof(alloc_buf) );
#endif
    
#if defined(MBEDTLS_SSL_CACHE_C)
    mbedtls_ssl_cache_init( &cache );
#endif
    
    mbedtls_x509_crt_init( &srvcert );
    mbedtls_x509_crt_init( &cachain );
    
    mbedtls_ssl_config_init( &conf );
    mbedtls_ctr_drbg_init( &ctr_drbg );
    memset( threads, 0, sizeof(threads) );
    mbedtls_net_init( &listen_fd );
    mbedtls_net_init( &client_fd );
    
    mbedtls_mutex_init( &debug_mutex );
    
    base_info.config = &conf;
    
    // We use only a single entropy source that is used in all the threads.
    mbedtls_entropy_init( &entropy );
    
    // 1. Load the certificates and private RSA key
    mbedtls_printf( "\n  . Loading the server cert. and key..." );
    fflush( stdout );
    
    /
     * This demonstration program uses embedded test certificates.
     * Instead, you may want to use mbedtls_x509_crt_parse_file() to read the
     * server and CA certificates, as well as mbedtls_pk_parse_keyfile().
     /
    ret = mbedtls_x509_crt_parse( &srvcert, (const unsigned char *) mbedtls_test_srv_crt,
                                 mbedtls_test_srv_crt_len );
    if( ret != 0 )
    {
      mbedtls_printf( " failed\n  !  mbedtls_x509_crt_parse returned %d\n\n", ret );
      goto exit;
    }
    
    ret = mbedtls_x509_crt_parse( &cachain, (const unsigned char *) mbedtls_test_cas_pem,
                                 mbedtls_test_cas_pem_len );
    if( ret != 0 )
    {
      mbedtls_printf( " failed\n  !  mbedtls_x509_crt_parse returned %d\n\n", ret );
      goto exit;
    }
    
    mbedtls_pk_init( &pkey );
    ret =  mbedtls_pk_parse_key( &pkey, (const unsigned char *) mbedtls_test_srv_key,
                                mbedtls_test_srv_key_len, NULL, 0 );
    if( ret != 0 )
    {
      mbedtls_printf( " failed\n  !  mbedtls_pk_parse_key returned %d\n\n", ret );
      goto exit;
    }
    
    mbedtls_printf( " ok\n" );
    
    /
     * 1b. Seed the random number generator
     /
    mbedtls_printf( "  . Seeding the random number generator..." );
    
    if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
                                      (const unsigned char *) pers,
                                      strlen( pers ) ) ) != 0 )
    {
      mbedtls_printf( " failed: mbedtls_ctr_drbg_seed returned -0x%04x\n",
                     -ret );
      goto exit;
    }
    
    mbedtls_printf( " ok\n" );
    
    /
     * 1c. Prepare SSL configuration
     /
    mbedtls_printf( "  . Setting up the SSL data...." );
    
    if( ( ret = mbedtls_ssl_config_defaults( &conf,
                                            MBEDTLS_SSL_IS_SERVER,
                                            MBEDTLS_SSL_TRANSPORT_STREAM,
                                            MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 )
    {
      mbedtls_printf( " failed: mbedtls_ssl_config_defaults returned -0x%04x\n",
                     -ret );
      goto exit;
    }
    
    mbedtls_ssl_conf_rng( &conf, mbedtls_ctr_drbg_random, &ctr_drbg );
    mbedtls_ssl_conf_dbg( &conf, my_mutexed_debug, stdout );
    
    / mbedtls_ssl_cache_get() and mbedtls_ssl_cache_set() are thread-safe if
     * MBEDTLS_THREADING_C is set.
     /
#if defined(MBEDTLS_SSL_CACHE_C)
    mbedtls_ssl_conf_session_cache( &conf, &cache,
                                   mbedtls_ssl_cache_get,
                                   mbedtls_ssl_cache_set );
#endif
    
    mbedtls_ssl_conf_ca_chain( &conf, &cachain, NULL );
    if( ( ret = mbedtls_ssl_conf_own_cert( &conf, &srvcert, &pkey ) ) != 0 )
    {
      mbedtls_printf( " failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret );
      goto exit;
    }
    
    mbedtls_printf( " ok\n" );
    
    
    /
     * 2. Setup the listening TCP socket
     /
    mbedtls_printf( "  . Bind on https://localhost:4433/ ..." );
    fflush( stdout );
    
    if( ( ret = mbedtls_net_bind( &listen_fd, NULL, "4433", MBEDTLS_NET_PROTO_TCP ) ) != 0 )
    {
      mbedtls_printf( " failed\n  ! mbedtls_net_bind returned %d\n\n", ret );
      goto exit;
    }
    
    mbedtls_printf( " ok\n" );
    
  reset:
#ifdef MBEDTLS_ERROR_C
    if( ret != 0 )
    {
      char error_buf[100];
      mbedtls_strerror( ret, error_buf, 100 );
      mbedtls_printf( "  [ main ]  Last error was: -0x%04x - %s\n", -ret, error_buf );
    }
#endif
    
    /
     * 3. Wait until a client connects
     /
    mbedtls_printf( "  [ main ]  Waiting for a remote connection\n" );
    
    if( ( ret = mbedtls_net_accept( &listen_fd, &client_fd,
                                   NULL, 0, NULL ) ) != 0 )
    {
      mbedtls_printf( "  [ main ] failed: mbedtls_net_accept returned -0x%04x\n", ret );
      goto exit;
    }
    
    mbedtls_printf( "  [ main ]  ok\n" );
    mbedtls_printf( "  [ main ]  Creating a new thread\n" );
    
    if( ( ret = thread_create( &client_fd ) ) != 0 )
    {
      mbedtls_printf( "  [ main ]  failed: thread_create returned %d\n", ret );
      mbedtls_net_free( &client_fd );
      goto reset;
    }
    
    ret = 0;
    goto reset;
   */
  
  // The environment for this request.
  // It gets passed around from function to function during the entire request.
  // This provides thread-safety to the request.
  Webserver_Request request;
  
  // Store remote client address in the request.
  request.remote_address = clientaddress;
  
  try {
    if (config_globals_http_running) { // Todo also secure version.
      
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


void https_server () // Todo
{
  // File descriptors for the connections.
  mbedtls_net_context listen_fd;
  mbedtls_net_init (&listen_fd);

  mbedtls_net_context client_fd;
  mbedtls_net_init (&client_fd);
  
  // SSL/TSL configurations.
  mbedtls_ssl_context ssl;
  mbedtls_ssl_init (&ssl);

  mbedtls_ssl_config conf;
  mbedtls_ssl_config_init (&conf);
  
  mbedtls_ssl_cache_context cache;
  mbedtls_ssl_cache_init (&cache);

  mbedtls_x509_crt srvcert;
  mbedtls_x509_crt_init (&srvcert);
  
  mbedtls_pk_context pkey;
  mbedtls_pk_init (&pkey);

  mbedtls_entropy_context entropy;
  mbedtls_entropy_init (&entropy);

  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_ctr_drbg_init (&ctr_drbg);

  
  // Load the certificates and private RSA key.
  // This uses embedded test certificates.
  // Instead, you may want to use mbedtls_x509_crt_parse_file() to read the
  // server and CA certificates, as well as mbedtls_pk_parse_keyfile().
  int ret;
  ret = mbedtls_x509_crt_parse (&srvcert, (const unsigned char *) mbedtls_test_srv_crt, mbedtls_test_srv_crt_len);
  if (ret != 0) {
    https_server_display_mbed_tls_error (ret);
    return;
  }
  ret = mbedtls_x509_crt_parse (&srvcert, (const unsigned char *) mbedtls_test_cas_pem, mbedtls_test_cas_pem_len);
  if (ret != 0) {
    https_server_display_mbed_tls_error (ret);
    return;
  }
  ret =  mbedtls_pk_parse_key (&pkey, (const unsigned char *) mbedtls_test_srv_key, mbedtls_test_srv_key_len, NULL, 0);
  if (ret != 0) {
    https_server_display_mbed_tls_error (ret);
    return;
  }
  
  
  // Seed the random number generator.
  const char *pers = "Bibledit Cloud";
  ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *) pers, strlen (pers));
  if (ret != 0) {
    https_server_display_mbed_tls_error (ret);
    return;
  }

  
  // Setup the listening TCP socket.
  ret = mbedtls_net_bind (&listen_fd, NULL, convert_to_string (config_logic_https_network_port ()).c_str (), MBEDTLS_NET_PROTO_TCP);
  if (ret != 0) {
    https_server_display_mbed_tls_error (ret);
    return;
  }
  
  
  // Setup TLS stuff.
  ret = mbedtls_ssl_config_defaults (&conf, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
  if (ret != 0) {
    https_server_display_mbed_tls_error (ret);
    return;
  }
  mbedtls_ssl_conf_rng (&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
  // mbedtls_ssl_cache_get() and mbedtls_ssl_cache_set() are thread-safe if MBEDTLS_THREADING_C is set. Todo
  mbedtls_ssl_conf_session_cache (&conf, &cache, mbedtls_ssl_cache_get, mbedtls_ssl_cache_set);
  mbedtls_ssl_conf_ca_chain (&conf, srvcert.next, NULL);
  ret = mbedtls_ssl_conf_own_cert (&conf, &srvcert, &pkey);
  if (ret != 0) {
    https_server_display_mbed_tls_error (ret);
    return;
  }
  ret = mbedtls_ssl_setup (&ssl, &conf);
  if (ret != 0) {
    https_server_display_mbed_tls_error (ret);
    return;
  }
  
  
  // Keep preparing for, accepting, and processing client connections.
  config_globals_https_running = true;
  while (config_globals_https_running) {
    

    // This flag indicates a healthy connection, one that can proceed.
    bool connection_healthy = true;
    
    
    // Resetting before accepting client connection.
    mbedtls_net_free (&client_fd);
    mbedtls_ssl_session_reset (&ssl);

    
    // Wait until a client connects.
    ret = mbedtls_net_accept (&listen_fd, &client_fd, NULL, 0, NULL);
    if (ret != 0 ) {
      https_server_display_mbed_tls_error (ret);
      continue;
    }
    mbedtls_ssl_set_bio (&ssl, &client_fd, mbedtls_net_send, mbedtls_net_recv, NULL); // Todo

    
    // Socket receive timeout.
    struct timeval tv;
    tv.tv_sec = 60;
    tv.tv_usec = 0;
    setsockopt (client_fd.fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    
    // The client's remote IPv4 address in dotted notation.
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    getpeername (client_fd.fd, (struct sockaddr *)&addr, &addr_size);
    char remote_address [256];
    inet_ntop (AF_INET, &addr.sin_addr.s_addr, remote_address, sizeof (remote_address));
    string clientaddress = remote_address;
    
    
    // SSL / TLS handshake.
    while (connection_healthy && (ret = mbedtls_ssl_handshake (&ssl)) != 0) {
      if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
        if (config_globals_https_running) {
          // In case the secure server runs, display the error.
          // And in case the server is interrupted by e.g. Ctrl-C, don't display this error.
          https_server_display_mbed_tls_error (ret);
        }
        connection_healthy = false;
      }
    }
    if (!connection_healthy) continue;

    
    // Read the HTTP Request.
    int len;
    do {
      unsigned char buf [1024];
      len = sizeof (buf) - 1;
      memset (buf, 0, sizeof (buf));
      ret = mbedtls_ssl_read (&ssl, buf, len);
      
      if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) continue;
      
      if (ret <= 0) {
        switch (ret) {
          case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
            break;
          case MBEDTLS_ERR_NET_CONN_RESET:
            break;
          default:
            connection_healthy = false;
            https_server_display_mbed_tls_error (ret);
            break;
        }
        break;
      }
      
      len = ret;
      //cout << (char *) buf << endl;
      
      if (ret > 0) break;
    }
    while (connection_healthy);
    if (!connection_healthy) continue;


    // Write the response.
    /* Todo
     // Handle this request in a thread, enabling parallel requests.
     thread request_thread = thread (webserver_process_request, connfd, clientaddress);
     // Detach and delete thread object.
     request_thread.detach ();
    */

#define HTTP_RESPONSE \
"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n" \
"<h2>Bibledit secure server</h2>\r\n" \
"<p>Successful connection using: %s</p>\r\n"

    unsigned char buf[1024];
    len = sprintf ((char *) buf, HTTP_RESPONSE, mbedtls_ssl_get_ciphersuite (&ssl));
    while ((ret = mbedtls_ssl_write (&ssl, buf, len)) <= 0) {
      if (ret == MBEDTLS_ERR_NET_CONN_RESET) {
        https_server_display_mbed_tls_error (ret);
        continue;
      }
      if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
        https_server_display_mbed_tls_error (ret);
        continue;
      }
    }
    
    len = ret;
    //cout << (char *) buf << endl;
    
    while ((ret = mbedtls_ssl_close_notify (&ssl)) < 0) {
      if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
        https_server_display_mbed_tls_error (ret);
        break;
      }
    }
  }
  
  mbedtls_net_free (&client_fd);
  // Close listening socket, freeing it for a possible subsequent server process.
  mbedtls_net_free (&listen_fd);
  
  mbedtls_x509_crt_free (&srvcert);
  mbedtls_pk_free (&pkey);
  mbedtls_ssl_free (&ssl);
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
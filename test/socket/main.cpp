#include <cstdlib>
#include <unistd.h>
#include <utime.h>
#include <dirent.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/types.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <algorithm>
#include <set>
#include <chrono>
#include <iomanip>
#include <stdexcept>
#include <thread>
#include <cmath>
#include <mutex>
#include <numeric>
#include <random>
#include <limits>


using namespace std;


string filter_url_http_request_mbed ()
{
  bool secure = false;  
  bool connection_healthy = true;

  
  // Resolve the host.
  struct addrinfo hints;
  struct addrinfo * address_results = nullptr;
  bool address_info_resolved = false;
  if (!secure) {
    memset (&hints, 0, sizeof (struct addrinfo));
    // Allow IPv4 and IPv6.
    hints.ai_family = AF_UNSPEC;
    // TCP/IP socket.
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    // Select protocol that matches with the socket type.
    hints.ai_protocol = 0;
    // The 'service' is actually the port number.
    string service = "8080";
    // Get a list of address structures. There can be several of them.
    int res = getaddrinfo ("localhost", "8080", &hints, &address_results);
    if (res != 0) {
#ifdef HAVE_VISUALSTUDIO
      wchar_t * err = gai_strerrorW (res);
      cout << wstring2string (err) << endl;
#else
      cout << gai_strerror (res) << endl;
#endif
      connection_healthy = false;
    } else {
      address_info_resolved = true;
    }
  }
  
  
  // Plain connection setup.
  int sock = 0;
  if (!secure) {
    
    // Iterate over the list of address structures.
    vector <string> errors;
    struct addrinfo * rp = NULL;
    if (connection_healthy) {
      for (rp = address_results; rp != NULL; rp = rp->ai_next) {
        // Try to get a socket for this address structure.
        sock = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        // If it fails, try the next one.
        if (sock < 0) {
          cout << strerror (errno) << endl;
          continue;
        }
        // Try to connect.
        int res = connect (sock, rp->ai_addr, rp->ai_addrlen);
        // Test and record error.
        if (res < 0) {
          cout << strerror (errno) << endl;
        }
        // If success: Done.
        if (res != -1) break;
        // Failure: Socket should be closed.
        if (sock) close (sock);
        sock = 0;
      }
    }
    
    // Check whether no address succeeded.
    if (connection_healthy) {
      if (rp == NULL) {
        connection_healthy = false;
      }
    }
    
    // No longer needed: Only to be freed when the address was resolved.
    if (address_info_resolved) freeaddrinfo (address_results);
  }
  
  
  // A Bibledit client should work even over very bad networks,
  // so set a timeout on the network connection.
  if (connection_healthy) {
    // Socket, whether plain or secure http.
    int comm_sock = sock;
    if (secure) comm_sock = 0;
    // Make the timeout not too short, so it can support very slow networks.
#ifdef HAVE_VISUALSTUDIO
    // Windows: Timeout value is a DWORD in milliseconds, address passed to setsockopt() is const char *
    const char * tv = "600000";
#else
    // Linux: Timeout value is a struct timeval, address passed to setsockopt() is const void *
    struct timeval tv;
    tv.tv_sec = 600;
    tv.tv_usec = 0;
#endif
    // Check on the result of setting the socket options.
    // If it cannot be set, record it in the journal,
    // but still proceed with the connection, because this is not fatal.
    int ret;
#ifdef HAVE_VISUALSTUDIO
    ret = setsockopt (comm_sock, SOL_SOCKET, SO_RCVTIMEO, tv, sizeof (tv));
#else
    ret = setsockopt (comm_sock, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));
#endif
    if (ret != 0) cout << strerror (errno) << endl;
#ifdef HAVE_VISUALSTUDIO
    ret = setsockopt (comm_sock, SOL_SOCKET, SO_SNDTIMEO, tv, sizeof (tv));
#else
    ret = setsockopt (comm_sock, SOL_SOCKET, SO_SNDTIMEO, (struct timeval *)&tv, sizeof(struct timeval));
#endif
    if (ret != 0) cout << strerror (errno) << endl;
  }
  
  
  // Send the request.
  if (connection_healthy) {
    string request = "GET";
    request.append (" ");
    request.append ("/");
    request.append (" ");
    request.append ("HTTP/1.1");
    request.append ("\r\n");
    request.append ("Host: ");
    request.append ("localhost");
    request.append ("\r\n");
    // Close connection, else it's harder to locate the end of the response.
    request.append ("Connection: close");
    request.append ("\r\n");
    request.append ("\r\n");
    if (secure) {
      
    } else {

      // Send plain http.
      if (send (sock, request.c_str(), request.length(), 0) != (int) request.length ()) {
        cout << strerror (errno) << endl;
        connection_healthy = false;
      }
      
    }
  }
  
  
  // Read the response headers and body.
  string headers;
  string response;
  if (connection_healthy) {

    bool reading = true;
    bool reading_body = false;
    char prev = 0;
    char cur;

    do {
      int ret = 0;
      if (secure) {
      } else {
#ifdef HAVE_VISUALSTUDIO
        ret = recv(sock, &cur, 1, 0);
#else
        ret = read(sock, &cur, 1);
#endif
      }
      if (ret > 0) {
        if (reading_body) {
          response += cur;
        } else {
          if (cur == '\r') continue;
          headers += cur;
          if ((cur == '\n') && (prev == '\n')) reading_body = true;
          prev = cur;
        }
      } else if (!secure && (ret < 0)) {
        cout << strerror (errno) << endl;
        connection_healthy = false;
      } else {
        // Probably EOF.
        reading = false;
      }
    } while (reading && connection_healthy);
  }
  
  
  if (secure) {
  } else {
    // Only close the socket if it was open.
    // It used to close (0) in error,
    // and on Android and iOS, when this was done a couple of times, it would crash the app.
    if (sock > 0) {
      cout << "Closing socket " << sock << endl;
      close (sock);
    }
  }

  
  return response;
}


int main()
{
  for (int i = 0; i < 10; ++i) {
    filter_url_http_request_mbed ();
  }
}

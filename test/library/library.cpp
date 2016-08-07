/*
Copyright (©) 2003-2015 Teus Benschop.

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


#include <libraries.h>
#ifdef HAVE_MSYS
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#endif
#include <sys/types.h>


void bibledit_set_web_root (const char * directory)
{
    cout << directory << endl;
}


void bibledit_initialize_library ()
{
}


void bibledit_network ()
{
  int listenfd = socket (AF_INET, SOCK_STREAM, 0);
  cout << "File descriptor: " << listenfd << endl;

  int optval = 1;
  int result = setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, (const char *) &optval, sizeof (int));
  cout << "setsockupt: " << result << endl;
  
}

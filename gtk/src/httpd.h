/*
** Copyright (©) 2003-2013 Teus Benschop.
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**  
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#ifndef INCLUDED_HTTPD_H
#define INCLUDED_HTTPD_H


#include "libraries.h"
#ifndef WIN32
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#else
#include <ws2tcpip.h>
#endif
#include <stdio.h>


#define HTTPD_PORT 51516


class Httpd
{
public:
  Httpd (bool dummy);
  ~Httpd ();
  ustring search_whole_word;
  ustring difficult_url;
private:
  static void thread_start (gpointer data);
  void thread_main ();
  bool thread_stop;
  void handle_request (int fd);
  void log (const ustring & message);
  void sendline (int fd, const ustring & line);
  const char * getmimetype (char *name);
  void send_content_type (int fd, const ustring& filename);
  void send_file (int fd, const ustring& filename);
  void send_404 (int fd);
  void send_lines (int fd, const vector <ustring>& lines);
  void send_search (int fd, const ustring& filename, const ustring& command);
  void send_search_results (int fd, ustring searchword);
  int sock;
  int conn;
};


#endif

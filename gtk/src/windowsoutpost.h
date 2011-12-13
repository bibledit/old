/*
 ** Copyright (©) 2003-2012 Teus Benschop.
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


#ifndef INCLUDED_WINDOWSOUTPOST_H
#define INCLUDED_WINDOWSOUTPOST_H


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
#include "reference.h"


#define BIBLEDIT_WINDOWS_OUTPOST_EXE "bwoutpost.exe"


class WindowsOutpost
{
public:
  WindowsOutpost(bool dummy);
  ~WindowsOutpost();
  void Start();
  ustring BibleWorksReferenceGet ();
  void BibleWorksReferenceSet(const Reference& reference);
  ustring SantaFeFocusReferenceGet ();
  void SantaFeFocusReferenceSet(const Reference& reference);
  void SantaFeFocusWordSet(const ustring& word);
  bool online_bible_server_connected;
  void OnlineBibleReferenceSet (const Reference& reference);
  ustring OnlineBibleReferenceGet ();
  bool OnlineBibleDirectMode;
  ustring OnlineBibleDirectCommandResponseGet (const ustring& command);
private:
  struct sockaddr_in address;
  struct in_addr inaddr;
  struct hostent *host;
  int sock;
  static void thread_start(gpointer data);
  void thread_main();
  bool thread_run;
  bool thread_running;
  int stage;
  bool connected;
  void clear();
  void disconnect();
  void log(const ustring & message);
  ustring last_message;
  void telnet(const ustring& hostname);
  void send_line(const ustring & command);
  ustring Readln();
  ustring bibleworks_reference_set_value;
  ustring santafefocus_reference_set_value;
  ustring santafefocus_word_set_value;
  void online_bible_server_connect (bool connect);
  int online_bible_server_requested_action;
  ustring onlinebible_reference_set_value;
  ustring get_reference_command;
  ustring get_reference_reply;
};


void windowsoutpost_open_url(const ustring& url);
bool windowsoutpost_telnet(const ustring& hostname);
ustring windowsoutpost_path ();


#endif

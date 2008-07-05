/*
 ** Copyright (©) 2003-2008 Teus Benschop and Dan Dennison (WIN32).
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
 ** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */

#ifndef INCLUDED_IPC_H
#define INCLUDED_IPC_H

#include "libraries.h"
#include <gtk/gtkbutton.h>
#ifdef WIN32
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <sys/un.h>
#endif

enum IPCSocketType {ipcstBibleditBin, ipcstBibleditBibletime, ipcstBibleditGit};

enum IPCCallType {ipcctBibleTimeReference,
  ipcctBibleTimeShutdown,
  ipcctBibleTimeConnected,
  ipcctBibleTimeGetModules,
  ipcctBibleTimeBibles,
  ipcctBibleTimeCommentaries,
  ipcctBibleTimeReloadModules,
  ipcctBibleTimeSearchCommand,
  ipcctBibleTimeSearchDone,
  ipcctBibleTimeSearchResults,
  ipcctGitJobDescription,
  ipcctGitTaskDone,
  ipcctGitShutdown,
  ipcctEnd};

class InterprocessCommunication
{
public:
  InterprocessCommunication(IPCSocketType name);
  ~InterprocessCommunication();
  bool send(IPCSocketType name, IPCCallType method, const vector<ustring>& payload);
  vector <ustring> receive(IPCSocketType name, IPCCallType method, const vector<ustring>& payload, int timeout = 5);
  vector<ustring> get_payload(IPCCallType method);
  void erase_payload(IPCCallType method);
  void methodcall_add_signal(IPCCallType method);
  GtkWidget * method_called_signal;
  IPCCallType method_called_type;
  void methodcall_remove_all_signals();
private:
#ifdef WIN32
  // No UNIX sockets on pure Windows
  struct sockaddr_in address;
#else
  struct sockaddr_un address;
#endif
  int sock, conn;
  socklen_t addrLength;
  ustring socketname(IPCSocketType socket);
  IPCSocketType myname;
  static void listener_start(gpointer data);
  void listener_main();
  bool listener_run;
  bool listener_running;
  void log(const ustring& message, bool critical);
  map<IPCCallType, vector<ustring> > methodcalls;
  set<IPCCallType> signalling_methods;
};

#endif

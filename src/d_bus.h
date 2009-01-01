/*
** Copyright (©) 2003-2009 Teus Benschop.
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


#ifndef INCLUDED_D_BUS_H
#define INCLUDED_D_BUS_H


#include "libraries.h"
#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>
#include <gtk/gtkbutton.h>


enum DBusNameType {dbntNone, dbntOrgBibleditBin, dbntOrgBibleditBibletime, dbntOrgBibleditGit};

enum DBusMethodType {dbmtHello,
                     dbmtEnd};


class DBus
{
public:
  DBus (DBusNameType name);
  ~DBus ();
  void send (DBusNameType destination, DBusMethodType, const vector<ustring>& payload);
  vector<ustring> get_payload (DBusMethodType method);
  void erase_payload (DBusMethodType method);
  void methodcall_add_signal (DBusMethodType method);
  GtkWidget * method_called_signal;
  void methodcall_remove_all_signals ();
private:
  DBusConnection * connection;
  gchar * dbusname (DBusNameType dbname);
  bool name_in_use (DBusNameType dbname);
  gchar * dbuspath ();
  gchar * dbusinterface ();
  gchar * dbusmethod (DBusMethodType dbmethod);
  DBusMethodType dbusmethod (const char * dbmethod);
  void retrieve_message (DBusMessage *message);
  void retrieve_iter (DBusMessageIter *iter);
  int message_type;
  vector<ustring> string_reply;
  static void listener_start (gpointer data);
  void listener_main ();
  bool listener_run;
  bool listener_running;
  void log (const ustring& message, bool critical);
  void respond (DBusMessage* msg, const ustring& response);
  map<DBusMethodType, vector<ustring> > methodcalls;
  set<DBusMethodType> signalling_methods;
};


#endif

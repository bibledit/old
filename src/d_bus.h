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
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#ifndef INCLUDED_D_BUS_H
#define INCLUDED_D_BUS_H


#include "libraries.h"
#include <dbus/dbus.h>
#include <gtk/gtkbutton.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>


enum DBusMethodType {dbmtHello, dbmtEnd};


class DBus
{
public:
  DBus (int dummy);
  ~DBus ();
  void send_to_bibletime (const gchar * object, const gchar * interface, const gchar * method, const ustring& value);
private:
  DBusConnection *con;
	DBusGConnection *sigcon;
	DBusGProxy *proxy;
  void send (const gchar * bus_name, const gchar * object, const gchar * interface, const gchar * method, const ustring& payload);
  vector <ustring> method_call_wait_reply (const gchar * bus_name, const gchar * object, const gchar * interface, const gchar * method, bool silent);
  void retrieve_message (DBusMessage *message);
  void retrieve_iter (DBusMessageIter *iter);
  int message_type;
  vector<ustring> string_reply;
  void log (const ustring& message, bool critical);
  bool check_if_bibletime_bus_name (const gchar * bus_name);
  ustring bibletime_bus_name;
  static void on_name_acquired (DBusGProxy *proxy, const char *name, gpointer user_data);
  static void on_name_owner_changed (DBusGProxy *proxy, const char *name, const char *prev, const char *nw, gpointer user_data);
  void name_owner_changed ();
  static void on_name_lost (DBusGProxy *proxy, const char *name, gpointer user_data);
  guint event_id_rescan_bus;
  static bool on_rescan_bus_timeout(gpointer user_data);
  void on_rescan_bus();
};


#endif

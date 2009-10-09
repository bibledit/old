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


#include "libraries.h"
#include "d_bus.h"
#include <glib.h>
#include "gwrappers.h"


/*

Aids for dbus:

dbus-monitor
d-feet

*/


DBus::DBus(int dummy)
{
  // Initialize variables.
  event_id_rescan_bus = 0;

  // It was noticed that on a clean Ubuntu 9.04 system Bibledit crashes the first time,
  // and in gdb it was seen that this was related to the dbus.
  // The following two lines were added to prevent crashes, but this made no difference. 
  // dbus_threads_init_default ();
  // dbus_g_thread_init ();
  
	// Obtain a connection to the Session Bus.
	GError *error = NULL;
	sigcon = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if(sigcon) {
  	con = dbus_g_connection_get_connection(sigcon);
	} else {
    log (error->message, true);
		g_error_free(error);
  }

	// Request our name on the DBus.
  if (con) {
  	DBusError *dbuserror = NULL;
    int retval = dbus_bus_request_name(con, "org.bibledit", 0, dbuserror);
    if (retval != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
      if (dbus_error_is_set(dbuserror)) {
        log (dbuserror->message, true);
        dbus_error_free(dbuserror);
      }
    }
  }
  
  // Connect to a couple of signals that indicate applications have started or have exited.
  // Then a rescan of the bus can show whether BibleTime runs, or whether it have been exited.  
  proxy = NULL;
  if (sigcon)
  	proxy = dbus_g_proxy_new_for_name(sigcon, "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
  if (proxy) {
    dbus_g_proxy_add_signal(proxy, "NameAcquired", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(proxy, "NameAcquired", G_CALLBACK (on_name_acquired), gpointer (this), NULL);
    dbus_g_proxy_add_signal(proxy, "NameOwnerChanged", G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(proxy, "NameOwnerChanged", G_CALLBACK (on_name_owner_changed), gpointer (this), NULL);
    dbus_g_proxy_add_signal(proxy, "NameLost", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(proxy, "NameLost", G_CALLBACK (on_name_lost), gpointer (this), NULL);
  }
}


DBus::~DBus()
{
  // Destroy any pending timeout.
  gw_destroy_source(event_id_rescan_bus);
  
  // The connection obtained through dbus_g_connection_get_connection does not have its reference count incremented.
  // Therefore it should not be unreferenced.
  // dbus_connection_unref(con);

  if (proxy) {
    g_object_unref(proxy);
  }
}


vector <ustring> DBus::method_call_wait_reply (const gchar * bus_name, const gchar * object, const gchar * interface, const gchar * method, bool silent)
// This equals: dbus-send --print-reply --dest=bus_name object interface.method
// It calls the method, and returns the reply.
{
  // Assemble the method call.
  DBusMessage *dbus_message;
  dbus_message = dbus_message_new_method_call(bus_name, object, interface, method);
  dbus_message_set_auto_start(dbus_message, TRUE);

  // Send dbus_message and handle the reply.  
  DBusError error;
  dbus_error_init(&error);
  int timeout = 10; // Timeout in milliseconds.
  DBusMessage *dbus_reply;
  dbus_reply = dbus_connection_send_with_reply_and_block(con, dbus_message, timeout, &error);
  if (dbus_error_is_set(&error) && !silent) {
    ustring err(error.name);
    err.append(": ");
    err.append(error.message);
    log(err, true);
  }
  if (dbus_reply) {
    retrieve_message(dbus_reply);
    dbus_message_unref(dbus_reply);
  }

  // Free the message.
  dbus_message_unref(dbus_message);

  // Return reply.
  vector <ustring> method_reply (string_reply);
  string_reply.clear();
  return method_reply;
}


void DBus::retrieve_message(DBusMessage * message)
// Retrieve the payload of the message.
{
  DBusMessageIter iter;
  dbus_message_iter_init(message, &iter);
  message_type = dbus_message_iter_get_arg_type(&iter);
  retrieve_iter(&iter);
}


void DBus::retrieve_iter(DBusMessageIter * iter)
// Retrieve the payload of one iterator of the message.
{
  do {
    int type = dbus_message_iter_get_arg_type(iter);
    switch (type) {
    case DBUS_TYPE_INVALID:
      {
        break;
      }
    case DBUS_TYPE_STRING:
      {
        char *val;
        dbus_message_iter_get_basic(iter, &val);
        string_reply.push_back(val);
        break;
      }
    case DBUS_TYPE_SIGNATURE:
      {
        char *val;
        dbus_message_iter_get_basic(iter, &val);
        cout << "signature: " << val << endl;
        break;
      }
    case DBUS_TYPE_OBJECT_PATH:
      {
        char *val;
        dbus_message_iter_get_basic(iter, &val);
        cout << "object path: " << val << endl;
        break;
      }
    case DBUS_TYPE_INT16:
      {
        dbus_int16_t val;
        dbus_message_iter_get_basic(iter, &val);
        cout << "int16: " << val << endl;
        break;
      }
    case DBUS_TYPE_UINT16:
      {
        dbus_uint16_t val;
        dbus_message_iter_get_basic(iter, &val);
        cout << "uint16: " << val << endl;
        break;
      }
    case DBUS_TYPE_INT32:
      {
        dbus_int32_t val;
        dbus_message_iter_get_basic(iter, &val);
        cout << "int32: " << val << endl;
        break;
      }
    case DBUS_TYPE_UINT32:
      {
        dbus_uint32_t val;
        dbus_message_iter_get_basic(iter, &val);
        cout << "uint32: " << val << endl;
        break;
      }
    case DBUS_TYPE_INT64:
      {
        dbus_int64_t val;
        dbus_message_iter_get_basic(iter, &val);
        cout << "int64: " << val << endl;
        break;
      }
    case DBUS_TYPE_UINT64:
      {
        dbus_uint64_t val;
        dbus_message_iter_get_basic(iter, &val);
        cout << "uint64: " << val << endl;
        break;
      }
    case DBUS_TYPE_DOUBLE:
      {
        double val;
        dbus_message_iter_get_basic(iter, &val);
        cout << "double: " << val << endl;
        break;
      }
    case DBUS_TYPE_BYTE:
      {
        unsigned char val;
        dbus_message_iter_get_basic(iter, &val);
        cout << "byte: " << val << endl;
        break;
      }
    case DBUS_TYPE_BOOLEAN:
      {
        dbus_bool_t val;
        dbus_message_iter_get_basic(iter, &val);
        cout << "boolean: " << val << endl;
        break;
      }
    case DBUS_TYPE_VARIANT:
      {
        DBusMessageIter subiter;
        dbus_message_iter_recurse(iter, &subiter);
        cout << "variant" << endl;
        retrieve_iter(&subiter);
        break;
      }
    case DBUS_TYPE_ARRAY:
      {
        string_reply.clear();
        int current_type;
        DBusMessageIter subiter;
        dbus_message_iter_recurse(iter, &subiter);
        while ((current_type = dbus_message_iter_get_arg_type(&subiter)) != DBUS_TYPE_INVALID) {
          retrieve_iter(&subiter);
          dbus_message_iter_next(&subiter);
        }
        break;
      }
    case DBUS_TYPE_DICT_ENTRY:
      {
        DBusMessageIter subiter;
        dbus_message_iter_recurse(iter, &subiter);
        cout << "dict entry" << endl;
        retrieve_iter(&subiter);
        dbus_message_iter_next(&subiter);
        retrieve_iter(&subiter);
        cout << ")\n" << endl;
        break;
      }
    case DBUS_TYPE_STRUCT:
      {
        int current_type;
        DBusMessageIter subiter;
        dbus_message_iter_recurse(iter, &subiter);
        while ((current_type = dbus_message_iter_get_arg_type(&subiter)) != DBUS_TYPE_INVALID) {
          retrieve_iter(&subiter);
          dbus_message_iter_next(&subiter);
          if (dbus_message_iter_get_arg_type(&subiter) != DBUS_TYPE_INVALID)
            cout << "," << endl;
        }
        break;
      }
    }
  } while (dbus_message_iter_next(iter));
}


void DBus::log(const ustring & message, bool critical)
{
  ustring msg = "DBus: " + message;
  if (critical) {
    gw_critical(message);
  } else {
    gw_message (message);
  }
}


void DBus::send(const gchar * bus_name, const gchar * object, const gchar * interface, const gchar * method, const ustring& payload)
{
  // Assemble the method call.
  DBusMessage *message;
  message = dbus_message_new_method_call(bus_name, object, interface, method);
  dbus_message_set_auto_start(message, TRUE);

  // Add the payload to the message.
  DBusMessageIter args;
  dbus_message_iter_init_append(message, &args);
  const char *pl = payload.c_str();
  dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &pl);

  // Send the message and flush the connection.
  DBusError error;
  dbus_error_init(&error);
  dbus_uint32_t serial = 0;
  dbus_connection_send(con, message, &serial);
  dbus_connection_flush(con);

  // Clear memory.
  dbus_message_unref(message);
}


bool DBus::check_if_bibletime_bus_name (const gchar * bus_name)
// Checks whether the "bus_name" belongs to BibleTime:
// dbus-send --print-reply --dest=:1.502 / org.freedesktop.DBus.Introspectable.Introspect
// (The ":1.502" is the bus_name.
{
  vector <ustring> reply = method_call_wait_reply (bus_name, "/", "org.freedesktop.DBus.Introspectable", "Introspect", true);
  if (!reply.empty ()) {
    if (reply[0].find ("BibleTime") != string::npos) {
      return true;
    }
  }
  return false;
}


void DBus::send_to_bibletime (const gchar * object, const gchar * interface, const gchar * method, const ustring& value)
/*
Sends a message to BibleTime over the DBus

dbus-send --print-reply --dest=:1.638 /BibleTime org.freedesktop.DBus.Introspectable.Introspect
This gives BibleTime's methods.

To synchronize all verse based modules, do this:
dbus-send --print-reply --dest=:1.668 /BibleTime info.bibletime.BibleTime.syncAllVerseBasedModules "string:Gen 2.2"

*/
{
  // Bail out if BibleTime's name on the bus is not known.
  if (bibletime_bus_name.empty()) 
    return;
  // Send the message.
  send (bibletime_bus_name.c_str(), object, interface, method, value);
}


vector <ustring> DBus::receive_from_bibletime (const gchar * object, const gchar * interface, const gchar * method)
// Calls a method in BibleTime and wait for the reply.
{
  vector <ustring> reply;
  if (!bibletime_bus_name.empty()) 
    reply = method_call_wait_reply (bibletime_bus_name.c_str(), object, interface, method, false);
  return reply;
}


void DBus::on_name_acquired (DBusGProxy *proxy, const char *name, gpointer user_data)
{
  ((DBus *) user_data)->names_on_dbus_changed();
}


void DBus::on_name_owner_changed (DBusGProxy *proxy, const char *name, const char *prev, const char *nw, gpointer user_data)
{
  ((DBus *) user_data)->names_on_dbus_changed();
}


void DBus::on_name_lost (DBusGProxy *proxy, const char *name, gpointer user_data)
{
  ((DBus *) user_data)->names_on_dbus_changed();
}


void DBus::names_on_dbus_changed ()
{
  gw_destroy_source (event_id_rescan_bus);
  event_id_rescan_bus = g_timeout_add_full(G_PRIORITY_DEFAULT, 200, GSourceFunc(on_rescan_bus_timeout), gpointer(this), NULL);
}


bool DBus::on_rescan_bus_timeout(gpointer user_data)
{
  ((DBus *) user_data)->on_rescan_bus();
  return false;
}


void DBus::on_rescan_bus()
{
  // Clear event id.
  event_id_rescan_bus = 0;
  
  // Clear the relevant bus names.
  bibletime_bus_name.clear();
  xiphos_bus_name.clear();
  
  // Check the names currently available on the bus:
  // dbus-send --print-reply --dest=org.freedesktop.DBus /org/freedesktop/DBus org.freedesktop.DBus.ListNames
  vector <ustring> names_on_bus = method_call_wait_reply ("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListNames", false);
  // Check whether the BibleTime name is visible on the bus.
  for (unsigned int i = 0; i < names_on_bus.size(); i++) {
    if (names_on_bus[i].find ("BibleTime") != string::npos) {
      bibletime_bus_name = names_on_bus[i];
      gw_message ("BibleTime on DBus as service " + names_on_bus[i]);
      break;
    }
  }
  // If the BibleTime service was not found, inspect each name on the bus whether it represents BibleTime.
  if (bibletime_bus_name.empty()) {
    for (unsigned int i = 0; i < names_on_bus.size(); i++) {
      if (check_if_bibletime_bus_name (names_on_bus[i].c_str())) {
        bibletime_bus_name = names_on_bus[i];
        gw_message ("BibleTime on DBus represented by name " + names_on_bus[i]);
        break;
      }
    }
  }
  // If BibleTime is still not found, give a message.
  if (bibletime_bus_name.empty()) {
    gw_message ("BibleTime not on DBus");
  }
  // Look for the Xiphos name on the bus.
  for (unsigned int i = 0; i < names_on_bus.size(); i++) {
    if (names_on_bus[i].find ("xiphos") != string::npos) {
      xiphos_bus_name = names_on_bus[i];
      gw_message ("Xiphos on DBus as service " + names_on_bus[i]);
      break;
    }
  }
  // If Xiphos is not found, give a message.
  if (xiphos_bus_name.empty()) {
    gw_message ("Xiphos not on DBus");
  }
}


void DBus::send_to_xiphos (const gchar * object, const gchar * interface, const gchar * method, const ustring& value)
/*
Sends a message to BibleTime over the DBus

To let Xiphos scroll to a certain verse, do this:
dbus-send --print-reply --dest=org.xiphos.remote /org/xiphos/remote/ipc org.xiphos.remote.navigate "string:sword://Genesis 2:3"

*/
{
  // Bail out if Xiphos does not run.
  if (xiphos_bus_name.empty()) 
    return;
  // Send the message.
  send (xiphos_bus_name.c_str(), object, interface, method, value);
}



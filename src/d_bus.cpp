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


DBus::DBus(int dummy)
{
	// Obtain a connection to the Session Bus.
	GError *error = NULL;
	sigcon = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if(sigcon) {
  	con = dbus_g_connection_get_connection(sigcon);
	} else {
    log (error->message, true);
		g_error_free(error);
  }

	// Request the DBus daemon for our name.
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
  
  // Check the names currently available on the bus:
  // dbus-send --print-reply --dest=org.freedesktop.DBus /org/freedesktop/DBus org.freedesktop.DBus.ListNames
  vector <ustring> names_on_bus = method_call_wait_reply ("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListNames", false);
  for (unsigned int i = 0; i < names_on_bus.size(); i++) {
    if (check_if_bibletime_bus_name (names_on_bus[i].c_str())) {
      bibletime_bus_name = names_on_bus[i];
      gw_message ("BibleTime on DBus at name " + names_on_bus[i]);
      break;
    }
  }
  
  // Signals to connect to:
  // NameLost
  // NameAcquired
  // We connect to the NameList signal, and probably the NameAqcuired signal. This will track whether the relevant applications run.
  // We can then send the signals only if the applications runs. (But we might send regardless, that would be fine, even if the application does not run).

}


DBus::~DBus()
{
  // The connection obtained through dbus_g_connection_get_connection does not have its reference count incremented.
  // Therefore it should not be unreferenced.
  // dbus_connection_unref(con);
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


const gchar *DBus::dbusmethod(DBusMethodType dbmethod)
{
  switch (dbmethod) {
  case dbmtHello:
    return "Hello";
  case dbmtEnd:
    return NULL;
  }
  return NULL;
}


DBusMethodType DBus::dbusmethod(const char *dbmethod)
{
  for (int i = 0; i < dbmtEnd; i++) {
    if (strcmp(dbmethod, dbusmethod(DBusMethodType(i))) == 0) {
      return (DBusMethodType) i;
    }
  }
  return dbmtEnd;
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



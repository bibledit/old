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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/

#include "libraries.h"
#include "d_bus.h"
#include <glib.h>

DBus::DBus(DBusNameType name)
{
  // Initialize variables.
  listener_running = false;
  method_called_signal = NULL;

  // Connect to the session bus.
  DBusError error;
  dbus_error_init(&error);
  connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
  if (dbus_error_is_set(&error)) {
    log(error.message, true);
    dbus_error_free(&error);
  }
  // Bail out of we had no connection.
  if (!connection) {
    return;
  }
  // Request a name on the bus, if the name is not in use.
  // Typicall the name will be in use when bibledit is already running.
  // If then a script is called, it will be in use, so the script should not
  // request the name on the bus.
  if (dbusname(name)) {
    if (!name_in_use(name)) {
      int ret = dbus_bus_request_name(connection, dbusname(name), 0, &error);
      if (dbus_error_is_set(&error)) {
        dbus_error_free(&error);
      }
      if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        log("Requested dbus name in use", true);
      }
    }
  }
  // Start the thread with the listen and reply loop.
  listener_run = true;
  g_thread_create(GThreadFunc(listener_start), gpointer(this), false, NULL);
}

DBus::~DBus()
{
  // Indicate to the thread that we want to stop.
  listener_run = false;

  // Wait until the thread has exited.
  while (listener_running)
    g_usleep(10000);

  // Unreference the connection.
  if (connection)
    dbus_connection_unref(connection);

  // Destroy the signal button.
  if (method_called_signal)
    gtk_widget_destroy(method_called_signal);
}

gchar *DBus::dbusname(DBusNameType dbname)
{
  switch (dbname) {
  case dbntNone:
    return NULL;
  case dbntOrgBibleditBin:
    return "org.bibledit.bin";
  case dbntOrgBibleditBibletime:
    return "org.bibledit.bibletime";
  case dbntOrgBibleditGit:
    return "org.bibledit.git";
  }
  return NULL;
}

bool DBus::name_in_use(DBusNameType dbname)
/*
Returns true if the "name" is in use.
  
To query the names on the bus, do this in a terminal:
dbus-send --print-reply --dest=org.freedesktop.DBus /org/freedesktop/DBus org.freedesktop.DBus.ListNames
*/
{
  // In-use variable.
  bool in_use = false;

  // Assemble the "ListNames" method call.
  DBusMessage *message;
  message = dbus_message_new_method_call("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListNames");
  dbus_message_set_auto_start(message, TRUE);

  // Send message, handle the reply.  
  DBusError error;
  dbus_error_init(&error);
  int timeout = -1;             // Default timeout.
  DBusMessage *reply;
  reply = dbus_connection_send_with_reply_and_block(connection, message, timeout, &error);
  if (dbus_error_is_set(&error)) {
    ustring s(error.name);
    s.append(": ");
    s.append(error.message);
    log(s, true);
  }
  if (reply) {
    retrieve_message(reply);
    dbus_message_unref(reply);
    set < ustring > names(string_reply.begin(), string_reply.end());
    in_use = names.find(dbusname(dbname)) != names.end();
  }
  // Clear memory.
  dbus_message_unref(message);

  // Return whether the name is in use.
  return in_use;
}

gchar *DBus::dbuspath()
{
  return "/org/bibledit/dbus";
}

gchar *DBus::dbusinterface()
{
  return "org.bibledit.dbus";
}

gchar *DBus::dbusmethod(DBusMethodType dbmethod)
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

void DBus::listener_start(gpointer data)
{
  ((DBus *) data)->listener_main();
}

void DBus::listener_main()
/*
Listens for signals.
To send a signal to bibledit, do this:
dbus-send --dest=org.bibledit.bin /org/bibledit/settings org.bibledit.settings.project string:xxx

Listens for method calls and responds to them.
To call a method of bibledit, do this:
dbus-send --print-reply --dest=org.bibledit.bin /org/bibledit/settings org.bibledit.settings.project
*/
{
  // Indicate that the thread runs.
  listener_running = true;

  // Variables.
  DBusError err;
  DBusMessage *msg;

  // Add a rule for which messages we want to see.
  dbus_error_init(&err);
  dbus_bus_add_match(connection, "path='/org/bibledit',interface='org.bibledit'", &err);
  dbus_connection_flush(connection);
  if (dbus_error_is_set(&err)) {
    dbus_error_free(&err);
  }
  // While the flag is set loop listening for signals or calls being emmitted.
  while (listener_run) {

    // Non-blocking read of the next available message.
    dbus_connection_read_write(connection, 0);
    msg = dbus_connection_pop_message(connection);

    // No message: loop again.
    if (!msg) {
      g_usleep(1000);
      continue;
    }
    // Check if the message is a method call.
    if (dbus_message_get_type(msg) == DBUS_MESSAGE_TYPE_METHOD_CALL) {

      // Get the method (the member) that has been called.
      const char *member = dbus_message_get_member(msg);
      DBusMethodType method = dbusmethod(member);

      // Get the optional payload and store it.
      string_reply.clear();
      retrieve_message(msg);
      if (method != dbmtEnd) {
        methodcalls[method] = string_reply;
      }
      // See whether to emit a signal on this method.
      if (signalling_methods.find(method) != signalling_methods.end()) {
        gtk_button_clicked(GTK_BUTTON(method_called_signal));
      }
      //respond (msg, "Bibledit's project"); // Use bool send_response, so we can act on that.
    }
    // Free the message.
    dbus_message_unref(msg);
  }

  // Indicate that the thread quitted.
  listener_running = false;
}

void DBus::log(const ustring & message, bool critical)
{
  ustring msg = "DBus: " + message;
  if (critical) {
    g_critical("%s", message.c_str());
  } else {
    write(1, message.c_str(), strlen(message.c_str()));
    write(1, "\n", 1);
  }
}

void DBus::respond(DBusMessage * msg, const ustring & response)
// Responds to "message".
{
  // Read the parameters.
  DBusMessageIter args;
  if (dbus_message_iter_init(msg, &args)) {
    if (dbus_message_iter_get_arg_type(&args) == DBUS_TYPE_STRING) {
      char *parameter;
      dbus_message_iter_get_basic(&args, &parameter);
      // cout << "received method call, parameter: " << parameter << endl;
    }
  }
  // Create a reply from the message.
  DBusMessage *reply;
  reply = dbus_message_new_method_return(msg);

  // Add the arguments to the reply.
  //bool stat = true;
  //dbus_uint32_t level = 21614;
  dbus_message_iter_init_append(reply, &args);
//  dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &stat);
//  dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &level);
  const char *resp = response.c_str();
  dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &resp);

  // Send the reply and flush the connection.
  dbus_uint32_t serial = 0;
  dbus_connection_send(connection, reply, &serial);
  dbus_connection_flush(connection);

  // Free the reply.
  dbus_message_unref(reply);

}

void DBus::send(DBusNameType destination, DBusMethodType method, const vector < ustring > &payload)
{
  // Assemble the method call.
  DBusMessage *message;
  message = dbus_message_new_method_call(dbusname(destination), dbuspath(), dbusinterface(), dbusmethod(method));
  dbus_message_set_auto_start(message, TRUE);

  // Add the payload to the message.
  DBusMessageIter args;
  dbus_message_iter_init_append(message, &args);
  for (unsigned int i = 0; i < payload.size(); i++) {
    const char *pl = payload[i].c_str();
    dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &pl);
  }

  // Send the message and flush the connection.
  DBusError error;
  dbus_error_init(&error);
  dbus_uint32_t serial = 0;
  dbus_connection_send(connection, message, &serial);
  dbus_connection_flush(connection);

  // Clear memory.
  dbus_message_unref(message);
}

vector < ustring > DBus::get_payload(DBusMethodType method)
{
  vector < ustring > payload;
  payload = methodcalls[method];
  return payload;
}

void DBus::erase_payload(DBusMethodType method)
{
  methodcalls[method].clear();
}

void DBus::methodcall_add_signal(DBusMethodType method)
// Add a signal to the method call "method".
{
  // Create the signalling button on adding the first method.
  if (!method_called_signal) {
    method_called_signal = gtk_button_new();
  }
  // Store this method.
  signalling_methods.insert(method);
}

void DBus::methodcall_remove_all_signals()
// Remove all signals from the methodcalls.
{
  signalling_methods.clear();
}

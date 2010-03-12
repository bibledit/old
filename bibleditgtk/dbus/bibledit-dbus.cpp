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


#include "bibledit-dbus.h"
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glib.h>
#include <signal.h>


void start_xiphos_web_listener ()
{
	SoupMessage * listener_msg;
	listener_msg = soup_message_new (SOUP_METHOD_GET, "http://localhost/bibledit/ipc/getmessage.php?channel=xiphos");
  soup_session_queue_message (session, listener_msg, SoupSessionCallback (on_xiphos_web_listener_ready_callback), NULL);
}


void on_xiphos_web_listener_ready_callback (SoupSession *session, SoupMessage *msg, gpointer user_data)
{
	if (SOUP_STATUS_IS_SUCCESSFUL (msg->status_code)) {
		// Get the response body.
		string body (msg->response_body->data);
		body = trim (body);
		// Log it.
    printf ("%s\n", body.c_str());
    fflush (stdout);
    // Handle "quit" command.
		if (body.find ("quit") == 0) {
      g_main_loop_quit (loop);
		}
		// Handle "goto" command.
		if (body.find ("goto") == 0) {
			body.erase (0, 4);
			body = trim (body);
      send_to_xiphos (xiphos_dbus_object (), xiphos_dbus_interface (), "setCurrentReference", body);
		}
	} else {
		// If the message was cancelled, do not start it again, just quit.
		if (msg->status_code == 1) {
		  return;
		}
		printf ("Xiphos web listener failure, code: %d, reason: %s\n", msg->status_code, msg->reason_phrase);
    fflush (stdout);
		g_usleep (1000000);
	}
	g_usleep (100000);
	start_xiphos_web_listener ();
}


void start_bibletime_web_listener ()
{
	SoupMessage * listener_msg;
	listener_msg = soup_message_new (SOUP_METHOD_GET, "http://localhost/bibledit/ipc/getmessage.php?channel=bibletime");
  soup_session_queue_message (session, listener_msg, SoupSessionCallback (on_bibletime_web_listener_ready_callback), NULL);
}


void on_bibletime_web_listener_ready_callback (SoupSession *session, SoupMessage *msg, gpointer user_data)
{
	if (SOUP_STATUS_IS_SUCCESSFUL (msg->status_code)) {
		// Get the response body.
		string body (msg->response_body->data);
		body = trim (body);
		// Log it.
    printf ("%s\n", body.c_str());
    fflush (stdout);
    // Handle "quit" command.
		if (body.find ("quit") == 0) {
      g_main_loop_quit (loop);
		}
		// Handle "goto" command.
		if (body.find ("goto") == 0) {
			body.erase (0, 4);
			body = trim (body);
      send_to_bibletime (bibletime_dbus_object (), bibletime_dbus_interface (), "syncAllVerseBasedModules", body);
		}
		// Handle "reload" command.
		if (body.find ("reload") == 0) {
      send_to_bibletime (bibletime_dbus_object (), bibletime_dbus_interface (), "reloadModules", "");
		}
    // Handle "getref" command.
    if (body.find ("getref") == 0) {
      string message_identifier = get_extract_message_identifier (body);
      vector <string> reply = receive_from_bibletime (bibletime_dbus_object (), bibletime_dbus_interface (), "getCurrentReference");
      string message;
      for (unsigned int i = 0; i < reply.size(); i++) {
        printf ("%s\n", reply[i].c_str());
        fflush (stdout);
        if (i) message.append (" ");
        message.append (reply[i]);
      }
      send_response_to_bibledit ("bibletimeref", message_identifier, message);
    }
	} else {
		// If the message was cancelled, do not start it again, just quit.
		if (msg->status_code == 1) {
		  return;
		}
		printf ("BibleTime web listener failure, code: %d, reason: %s\n", msg->status_code, msg->reason_phrase);
    fflush (stdout);
		g_usleep (1000000);
	}
	g_usleep (100000);
	start_bibletime_web_listener ();
}


string trim(const string & s)
{
  if (s.length() == 0)
    return s;
  // Strip spaces, tabs, new lines, carriage returns.
  size_t beg = s.find_first_not_of(" \t\n\r");
  size_t end = s.find_last_not_of(" \t\n\r");
  // No non-spaces  
  if (beg == string::npos)
    return "";
  return string(s, beg, end - beg + 1);
}


void on_name_acquired (DBusGProxy *proxy, const char *name, gpointer user_data)
{
	//printf ("name acquired: %s\n", name);
  names_on_dbus_changed();
}


void on_name_owner_changed (DBusGProxy *proxy, const char *name, const char *prev, const char *nw, gpointer user_data)
{
	//printf ("name owner changed, name: %s, prev: %s, nw: %s\n", name, prev, nw);
  names_on_dbus_changed();
}


void on_name_lost (DBusGProxy *proxy, const char *name, gpointer user_data)
{
	//printf ("name lost: %s\n", name);
  names_on_dbus_changed();
}


void destroy_source(guint & event_id)
// This is a convenience wrapper.
{
  if (event_id) {
    GSource *source = g_main_context_find_source_by_id(NULL, event_id);
    if (source)
      g_source_destroy(source);
  }
  event_id = 0;
}


void retrieve_iter(DBusMessageIter * iter)
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
        break;
      }
    case DBUS_TYPE_OBJECT_PATH:
      {
        char *val;
        dbus_message_iter_get_basic(iter, &val);
        break;
      }
    case DBUS_TYPE_INT16:
      {
        dbus_int16_t val;
        dbus_message_iter_get_basic(iter, &val);
        break;
      }
    case DBUS_TYPE_UINT16:
      {
        dbus_uint16_t val;
        dbus_message_iter_get_basic(iter, &val);
        break;
      }
    case DBUS_TYPE_INT32:
      {
        dbus_int32_t val;
        dbus_message_iter_get_basic(iter, &val);
        break;
      }
    case DBUS_TYPE_UINT32:
      {
        dbus_uint32_t val;
        dbus_message_iter_get_basic(iter, &val);
        break;
      }
    case DBUS_TYPE_INT64:
      {
        dbus_int64_t val;
        dbus_message_iter_get_basic(iter, &val);
        break;
      }
    case DBUS_TYPE_UINT64:
      {
        dbus_uint64_t val;
        dbus_message_iter_get_basic(iter, &val);
        break;
      }
    case DBUS_TYPE_DOUBLE:
      {
        double val;
        dbus_message_iter_get_basic(iter, &val);
        break;
      }
    case DBUS_TYPE_BYTE:
      {
        unsigned char val;
        dbus_message_iter_get_basic(iter, &val);
        break;
      }
    case DBUS_TYPE_BOOLEAN:
      {
        dbus_bool_t val;
        dbus_message_iter_get_basic(iter, &val);
        break;
      }
    case DBUS_TYPE_VARIANT:
      {
        DBusMessageIter subiter;
        dbus_message_iter_recurse(iter, &subiter);
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
        retrieve_iter(&subiter);
        dbus_message_iter_next(&subiter);
        retrieve_iter(&subiter);
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
          if (dbus_message_iter_get_arg_type(&subiter) != DBUS_TYPE_INVALID) {
					}
        }
        break;
      }
    }
  } while (dbus_message_iter_next(iter));
}


void retrieve_message(DBusMessage * message)
// Retrieve the payload of the message.
{
  DBusMessageIter iter;
  dbus_message_iter_init(message, &iter);
  message_type = dbus_message_iter_get_arg_type(&iter);
  retrieve_iter(&iter);
}


bool on_rescan_bus_timeout(gpointer user_data)
{
  on_rescan_bus();
  return false;
}


void on_rescan_bus()
{
  // Clear event id.
  event_id_rescan_bus = 0;
  
  // Clear the relevant bus names.
  bibletime_bus_name.clear();
  xiphos_bus_name.clear();
  
  // Check the names currently available on the bus:
  // dbus-send --print-reply --dest=org.freedesktop.DBus /org/freedesktop/DBus org.freedesktop.DBus.ListNames
  vector <string> names_on_bus = method_call_wait_reply ("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListNames", false);
  // Check whether the BibleTime name is visible on the bus.
  for (unsigned int i = 0; i < names_on_bus.size(); i++) {
    if (names_on_bus[i].find ("BibleTime") != string::npos) {
      bibletime_bus_name = names_on_bus[i];
			printf ("BibleTime on DBus as service %s\n", names_on_bus[i].c_str());
      fflush (stdout);
      break;
    }
  }
  // If BibleTime is not found, give a message.
  if (bibletime_bus_name.empty()) {
    printf ("BibleTime not on DBus\n");
    fflush (stdout);
  }
  // Look for the Xiphos name on the bus.
  for (unsigned int i = 0; i < names_on_bus.size(); i++) {
    if (names_on_bus[i].find ("xiphos") != string::npos) {
      xiphos_bus_name = names_on_bus[i];
      printf ("Xiphos on DBus as service %s\n", names_on_bus[i].c_str());
      fflush (stdout);
      break;
    }
  }
  // If Xiphos is not found, give a message.
  if (xiphos_bus_name.empty()) {
    printf ("Xiphos not on DBus\n");
    fflush (stdout);
  }
}


vector <string> method_call_wait_reply (const gchar * bus_name, const gchar * object, const gchar * interface, const gchar * method, bool silent)
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
    string err(error.name);
    err.append(": ");
    err.append(error.message);
    printf ("%s\n", err.c_str());
    fflush (stdout);
  }
  if (dbus_reply) {
    retrieve_message(dbus_reply);
    dbus_message_unref(dbus_reply);
  }

  // Free the message.
  dbus_message_unref(dbus_message);

  // Return reply.
  vector <string> method_reply (string_reply);
  string_reply.clear();
  return method_reply;
}


void send(const gchar * bus_name, const gchar * object, const gchar * interface, const gchar * method, const string& payload)
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


void send_to_bibletime (const gchar * object, const gchar * interface, const gchar * method, const string& value)
/*
Sends a message to BibleTime over the DBus

dbus-send --print-reply --dest=info.bibletime.BibleTime /BibleTime org.freedesktop.DBus.Introspectable.Introspect
This gives BibleTime's methods.

To synchronize all verse based modules, do this:
dbus-send --print-reply --dest=info.bibletime.BibleTime /BibleTime info.bibletime.BibleTime.syncAllVerseBasedModules "string:Gen.2.2"

*/
{
  // Bail out if BibleTime's name on the bus is not known.
  if (bibletime_bus_name.empty()) 
    return;
  // Send the message.
  send (bibletime_bus_name.c_str(), object, interface, method, value);
}


vector <string> receive_from_bibletime (const gchar * object, const gchar * interface, const gchar * method)
// Calls a method in BibleTime and wait for the reply.
{
  vector <string> reply;
  if (!bibletime_bus_name.empty()) 
    reply = method_call_wait_reply (bibletime_bus_name.c_str(), object, interface, method, false);
  return reply;
}


void names_on_dbus_changed ()
{
  destroy_source (event_id_rescan_bus);
  event_id_rescan_bus = g_timeout_add_full(G_PRIORITY_DEFAULT, 200, GSourceFunc(on_rescan_bus_timeout), NULL, NULL);
}


const gchar * xiphos_dbus_object ()
{
  return "/org/xiphos/remote/ipc";
}


const gchar * xiphos_dbus_interface ()
{
  return "org.xiphos.remote";
}


const gchar * bibletime_dbus_object ()
{
  return "/BibleTime";
}


const gchar * bibletime_dbus_interface ()
{
  return "info.bibletime.BibleTime";
}


void send_to_xiphos (const gchar * object, const gchar * interface, const gchar * method, const string& value)
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


string get_extract_message_identifier (string& message)
// Gets and extracts the identifier from the message.
// The identifier has a fixed length. It looks like, e.g.:
//   message_identifier=1000000000
{
  string identifier;
  size_t pos = message.find ("message_identifier=");
  if (pos != string::npos) {
    identifier = message.substr (pos, 29);
    message.erase (pos, 29);
  }
  return identifier;
}


void send_response_to_bibledit (const string& subject, const string& identifier, const string& message)
{
  string url = "http://localhost/bibledit/ipc/storemessage.php?channel=bibledit&subject=";
  url.append (subject);
  url.append ("&message=");
  url.append (identifier);
  url.append (" ");
  url.append (message);
  SoupMessage * msg = soup_message_new (SOUP_METHOD_GET, url.c_str());
  soup_session_queue_message (session, msg, SoupSessionCallback (on_message_ready_callback), NULL);
}


void on_message_ready_callback (SoupSession *session, SoupMessage *msg, gpointer user_data)
{
}


void sigproc(int dummy)
{ 		 
	printf("\nCtrl-c trapped to quit\n");
  fflush (stdout);
  g_main_loop_quit (loop);
}


void sigquit(int dummy)
{ 		 
	printf("\nCtrl-\\ trapped to quit\n");
  fflush (stdout);
  g_main_loop_quit (loop);
}


int main (int argc, char **argv)
{
	// The necessary g_ initializers, in the proper order.
  g_thread_init(NULL);
	g_type_init ();

  // Initialize variables.
  event_id_rescan_bus = 0;

  // If a logfile was passed, handle it.
  // This implies that if the program is started by hand from the terminal, we can see its output.
  if (argc >= 2) {
    // Redirect stdout and stderr to file.
    // When a file is opened it is always allocated the lowest available file 
    // descriptor. Therefore the following commands cause stdout to be 
    // redirected to the logfile.
    close(1);
    creat (argv[1], 0666); 
    // The dup() routine makes a duplicate file descriptor for an already opened 
    // file using the first available file descriptor. Therefore the following 
    // commands cause stderr to be redirected to the file stdout writes to.
    close(2);
    if (dup(1));

  }

  // We use asynchronous transport, so that we can send several messages simultanously.
	session = soup_session_async_new_with_options (SOUP_SESSION_USER_AGENT, "bibledit-dbus/1.0", NULL);
  start_xiphos_web_listener ();
  start_bibletime_web_listener ();

	// Obtain a connection to the Session Bus.
	GError *error = NULL;
	sigcon = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if(sigcon) {
  	con = dbus_g_connection_get_connection(sigcon);
	} else {
    printf ("%s\n", error->message);
    fflush (stdout);
		g_error_free(error);
  	return 1;
  }

	// Request our name on the DBus.
  if (con) {
  	DBusError *dbuserror = NULL;
    int retval = dbus_bus_request_name(con, "org.bibledit.dbus", 0, dbuserror);
    if (retval != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
      if (dbus_error_is_set(dbuserror)) {
        printf ("%s\n", dbuserror->message);
        fflush (stdout);
        dbus_error_free(dbuserror);
      }
    }
  }

  // Connect to a couple of signals that indicate applications have started or have exited.
  // Then a rescan of the bus can show whether the programs that we're interested in run.  
  proxy = NULL;
  if (sigcon)
  	proxy = dbus_g_proxy_new_for_name(sigcon, "org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
  if (proxy) {
    dbus_g_proxy_add_signal(proxy, "NameAcquired", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(proxy, "NameAcquired", G_CALLBACK (on_name_acquired), NULL, NULL);
    dbus_g_proxy_add_signal(proxy, "NameOwnerChanged", G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(proxy, "NameOwnerChanged", G_CALLBACK (on_name_owner_changed), NULL, NULL);
    dbus_g_proxy_add_signal(proxy, "NameLost", G_TYPE_STRING, G_TYPE_INVALID);
    dbus_g_proxy_connect_signal(proxy, "NameLost", G_CALLBACK (on_name_lost), NULL, NULL);
  }

  // Signal trapping for doing a clean exit.
	signal(SIGINT, sigproc);
	signal(SIGQUIT, sigquit);

  // The main loop will block till something quits it.
	loop = g_main_loop_new (NULL, TRUE);
  g_main_loop_run (loop);
	g_main_loop_unref (loop);

  // Destroy any pending timeout.
  destroy_source(event_id_rescan_bus);

  // Abort the session including the listeners
  soup_session_abort (session);

  // The connection obtained through dbus_g_connection_get_connection does not have its reference count incremented.
  // Therefore it should not be unreferenced.
  // dbus_connection_unref(con);

  // Destroy the proxy.
  if (proxy) {
    g_object_unref(proxy);
  }

  // Well done, my boy.
	return 0;
}



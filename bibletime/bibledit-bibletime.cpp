/*
** Copyright (©) 2003-2008 Teus Benschop.
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


#include "bibledit-bibletime.h"
#include <config.h>
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


int main (int argc, char *argv[])
{
  
#ifndef WIN32
  // Do not run as root.
  if (getuid () == 0) {
    cout << "Bibledit-BibleTime has not been designed to run with root privileges." << endl;
    return 1;
  }
#endif

	// The necessary g* initializers, in the proper order.
  g_thread_init(NULL);
	g_type_init ();
  gtk_init (&argc, &argv);

  gchar * iconfile = g_build_filename (PACKAGE_DATA_DIR, "bibledit-bibletime.xpm", NULL);
  gtk_window_set_default_icon_from_file(iconfile, NULL);
  g_free (iconfile);

  // Initialize variables.
  event_id_rescan_bus = 0;
  last_message_id = "0";
  quit = false;
  
  // Read settings.
  GKeyFile *keyfile = g_key_file_new();
  gchar * key_file_name = registry_file_name ();
  g_key_file_load_from_file(keyfile, key_file_name, G_KEY_FILE_NONE, NULL);
  g_free (key_file_name);
  
  GtkBuilder *gtkbuilder;

  gtkbuilder = gtk_builder_new ();
  gchar * filename = g_build_filename (PACKAGE_DATA_DIR, "gtkbuilder.xml", NULL);
  gtk_builder_add_from_file (gtkbuilder, filename, NULL);
  g_free (filename);

  window = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "window"));
  g_signal_connect ((gpointer) window, "delete_event", G_CALLBACK (program_quit), NULL);
  gtk_widget_show_all (window);

  entry_url = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "entry_url"));
  gchar * url = g_key_file_get_string (keyfile, "settings", "bibledit-web-url", NULL);
  if (!url) {
    url = strdup ("http://localhost/bibledit");
  }
  gtk_entry_set_text (GTK_ENTRY (entry_url), url);
  g_free (url);
  g_signal_connect((gpointer) entry_url, "changed", G_CALLBACK(on_entry_url_changed), NULL);

  button_url = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "button_url"));
  g_signal_connect((gpointer) button_url, "clicked", G_CALLBACK(on_button_url_clicked), NULL);

  label_url = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_url"));

  entry_user = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "entry_user"));
  gchar * user = g_key_file_get_string (keyfile, "settings", "bibledit-web-user", NULL);
  if (!user) {
    user = strdup ("");
  }
  gtk_entry_set_text (GTK_ENTRY (entry_user), user);
  g_free (user);
  g_signal_connect((gpointer) entry_user, "changed", G_CALLBACK(on_entry_user_changed), NULL);

  label_bibletime_process = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_bibletime_process"));

  label_id = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_id"));
  label_command = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_command"));
  label_body = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_body"));

  label_bibletime = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "label_bibletime"));

  button_hide = GTK_WIDGET (gtk_builder_get_object (gtkbuilder, "button_hide"));
  g_signal_connect((gpointer) button_hide, "clicked", G_CALLBACK(on_button_hide_clicked), NULL);

  g_key_file_free (keyfile);

  // We use asynchronous transport, so that we can send several messages simultanously.
	session = soup_session_async_new_with_options (SOUP_SESSION_USER_AGENT, "bibledit-bibletime/1.0", NULL);
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
    int retval = dbus_bus_request_name(con, "org.bibledit.bibletime", 0, dbuserror);
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

  // Main loop.
  gtk_main();

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


TinySpawn::TinySpawn(const char *program)
{
  myprogram = program;
  myread = false;
  exitstatus = 0;
}


TinySpawn::~TinySpawn()
{
}


void TinySpawn::workingdirectory(string directory)
// The process' working directory.
{
  myworkingdirectory = directory;
}


void TinySpawn::arg(string value)
// Add one arguments to the arguments for running the program.
// This function can be repeated as many times as desired.
{
#ifdef WIN32
  // TODO(dennison): Find a portable way to quote the argument.
#else
  // Escape the '.
  // --- Why isn't this covered in shell_quote_space?
  size_t offposition = value.find("'");
  while (offposition != string::npos) {
    value.replace(offposition, 1, "\\'");
    offposition = value.find("'", offposition + 2);
  }
#endif
  // Save argument.
  arguments.push_back(value);
}


void TinySpawn::read()
// Make cout and cerr of the program available for later reading.
{
  myread = true;
}


void TinySpawn::run()
{
  // Working directory.
  const gchar *workingdirectory = NULL;
  if (!myworkingdirectory.empty())
    workingdirectory = myworkingdirectory.c_str();
  // Store arguments in argv.
  char *argv[arguments.size() + 2];
  argv[0] = (char *)myprogram;
  for (unsigned int i = 0; i < arguments.size(); i++) {
    argv[i + 1] = (char *)arguments[i].c_str();
  }
  // Terminate argv.
  argv[arguments.size() + 1] = NULL;
  // Spawn flags.
  int flags = G_SPAWN_SEARCH_PATH;
  // Possible pipes.
  gint standard_output_filedescriptor;
  gint standard_error_filedescriptor;
  gint *standard_output_filedescriptor_pointer = NULL;
  gint *standard_error_filedescriptor_pointer = NULL;
  gchar *standard_output = NULL;
  gchar *standard_error = NULL;
  gchar **standard_output_pointer = NULL;
  gchar **standard_error_pointer = NULL;
  if (myread) {
    standard_output_filedescriptor_pointer = &standard_output_filedescriptor;
    standard_error_filedescriptor_pointer = &standard_error_filedescriptor;
    standard_output_pointer = &standard_output;
    standard_error_pointer = &standard_error;
  }
  // Spawn process.
  result = g_spawn_sync(workingdirectory, argv, NULL, (GSpawnFlags) flags, NULL, NULL, standard_output_pointer, standard_error_pointer, &exitstatus, NULL);
  // Handle case we didn't spawn the process.
  if (!result) {
    exitstatus = -1;
    string message = myprogram;
    message.append(" didn't spawn");
    g_critical("%s", message.c_str());
    return;
  }
  // Handle reading the output.
  if (myread) {
    // In sync mode we have gchar * output.
    ParseLine parse_output (standard_output);
    standardout = parse_output.lines;
    ParseLine parse_error (standard_error);
    standarderr = parse_error.lines;
    // Free data.
    if (standard_output)
      g_free(standard_output);
    if (standard_error)
      g_free(standard_error);
  }
}


ParseLine::ParseLine(const string & text)
// Parses text in its separate lines.
{
  string processed_line;
  processed_line = trim(text);
  size_t newlineposition;
  newlineposition = processed_line.find("\n");
  while (newlineposition != string::npos) {
    string word = processed_line.substr(0, newlineposition);
    lines.push_back(trim(word));
    processed_line.erase(0, newlineposition + 1);
    processed_line = trim(processed_line);
    newlineposition = processed_line.find("\n");
  }
  if (!processed_line.empty())
    lines.push_back(trim(processed_line));
}


ParseLine::~ParseLine()
{
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


string convert_to_string(int i)
{
  ostringstream r;
  r << i;
  return r.str();
}


string convert_to_string(unsigned int i)
{
  ostringstream r;
  r << i;
  return r.str();
}


string convert_to_string(bool b)
{
  if (b)
    return "1";
  else
    return "0";
}


string convert_to_string(double d)
{
  ostringstream r;
  r << d;
  return r.str();
}


string convert_to_string(long unsigned int i)
{
  ostringstream r;
  r << i;
  return r.str();
}


void start_bibletime_web_listener ()
{
	SoupMessage * listener_msg;
  string url = gtk_entry_get_text (GTK_ENTRY (entry_url));
  string user = gtk_entry_get_text (GTK_ENTRY (entry_user));
  url.append ("/ipc/getmessage.php?user=" + user + "&channel=bibletime&id=" + last_message_id);
	listener_msg = soup_message_new (SOUP_METHOD_GET, url.c_str());
  soup_session_queue_message (session, listener_msg, SoupSessionCallback (on_bibletime_web_listener_ready_callback), NULL);
}


void on_bibletime_web_listener_ready_callback (SoupSession *session, SoupMessage *msg, gpointer user_data)
{
	if (SOUP_STATUS_IS_SUCCESSFUL (msg->status_code)) {
		// Get the response body.
		string body (msg->response_body->data);
		body = trim (body);
		// Log it.
    //printf ("%s\n", body.c_str());
    //fflush (stdout);
    ParseLine parseline (body);
    if (parseline.lines.size() >= 3) {
      // It retrieves the message ID and uses this to ask for a higher ID next poll.
      // Update GUI.
      last_message_id = parseline.lines[0];
      gtk_label_set_text (GTK_LABEL (label_id), last_message_id.c_str());
      string command = parseline.lines[1];
      gtk_label_set_text (GTK_LABEL (label_command), command.c_str());
      string body = parseline.lines[2];
      gtk_label_set_text (GTK_LABEL (label_body), body.c_str());
      // Handle "quit" command.
      if (command == "quit") {
        gtk_main_quit ();
      }
      // Handle "focus" command.
      if (command == "focus") {
        Parse parse (body, false, ".");
        if (parse.words.size() == 3) {
          string book;
          switch (convert_to_int (parse.words[0])) {
            case 1: book = "Gen"; break;
            case 2: book = "Exod"; break;
            case 3: book = "Lev"; break;
            case 4: book = "Num"; break;
            case 5: book = "Deut"; break;
            case 6: book = "Josh"; break;
            case 7: book = "Judg"; break;
            case 8: book = "Ruth"; break;
            case 9: book = "1Sam"; break;
            case 10: book = "2Sam"; break;
            case 11: book = "1Kgs"; break;
            case 12: book = "2Kgs"; break;
            case 13: book = "1Chr"; break;
            case 14: book = "2Chr"; break;
            case 15: book = "Ezra"; break;
            case 16: book = "Neh"; break;
            case 17: book = "Esth"; break;
            case 18: book = "Job"; break;
            case 19: book = "Ps"; break;
            case 20: book = "Prov"; break;
            case 21: book = "Eccl"; break;
            case 22: book = "Song"; break;
            case 23: book = "Isa"; break;
            case 24: book = "Jer"; break;
            case 25: book = "Lam"; break;
            case 26: book = "Ezek"; break;
            case 27: book = "Dan"; break;
            case 28: book = "Hos"; break;
            case 29: book = "Joel"; break;
            case 30: book = "Amos"; break;
            case 31: book = "Obad"; break;
            case 32: book = "Jonah"; break;
            case 33: book = "Mic"; break;
            case 34: book = "Nah"; break;
            case 35: book = "Hab"; break;
            case 36: book = "Zeph"; break;
            case 37: book = "Hag"; break;
            case 38: book = "Zech"; break;
            case 39: book = "Mal"; break;
            case 40: book = "Matt"; break;
            case 41: book = "Mark"; break;
            case 42: book = "Luke"; break;
            case 43: book = "John"; break;
            case 44: book = "Acts"; break;
            case 45: book = "Rom"; break;
            case 46: book = "1Cor"; break;
            case 47: book = "2Cor"; break;
            case 48: book = "Gal"; break;
            case 49: book = "Eph"; break;
            case 50: book = "Phil"; break;
            case 51: book = "Col"; break;
            case 52: book = "1Thess"; break;
            case 53: book = "2Thess"; break;
            case 54: book = "1Tim"; break;
            case 55: book = "2Tim"; break;
            case 56: book = "Titus"; break;
            case 57: book = "Phlm"; break;
            case 58: book = "Heb"; break;
            case 59: book = "Jas"; break;
            case 60: book = "1Pet"; break;
            case 61: book = "2Pet"; break;
            case 62: book = "1John"; break;
            case 63: book = "2John"; break;
            case 64: book = "3John"; break;
            case 65: book = "Jude"; break;
            case 66: book = "Rev"; break;
          }
          if (!book.empty()) {
            string reference = book + "." + parse.words[1] + "." + parse.words[2];
            send_to_bibletime (bibletime_dbus_object (), bibletime_dbus_interface (), "syncAllVerseBasedModules", reference);
            reference.insert (0, "syncAllVerseBasedModules ");
            gtk_label_set_text (GTK_LABEL (label_bibletime), reference.c_str());
          }
        }
      }
      /*
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
      */
    } 
	} else {
		// If the message was cancelled, do not start it again, just quit.
		if (msg->status_code == 1) {
		  return;
		}
    // Handle quitting.
    if (quit) return;
    // Handle error.
		printf ("BibleTime web listener failure, code: %d, reason: %s\n", msg->status_code, msg->reason_phrase);
    fflush (stdout);
    for (unsigned int i = 0; i < 10; i++) {
      while (gtk_events_pending()) gtk_main_iteration();
  		g_usleep (100000);
    }
	}
	g_usleep (100000);
	start_bibletime_web_listener ();
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
  
  // Flag whether BibleTime was available.
  bool bibletime_was_available = !bibletime_bus_name.empty();
  
  // Clear the relevant bus names.
  bibletime_bus_name.clear();
  
  // Check the names currently available on the bus:
  // dbus-send --print-reply --dest=org.freedesktop.DBus /org/freedesktop/DBus org.freedesktop.DBus.ListNames
  vector <string> names_on_bus = method_call_wait_reply ("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListNames", false);
  // Check whether the BibleTime name is visible on the bus.
  for (unsigned int i = 0; i < names_on_bus.size(); i++) {
    if (names_on_bus[i].find ("BibleTime") != string::npos) {
      if (names_on_bus[i].find ("bibledit") == string::npos) {
        bibletime_bus_name = names_on_bus[i];
        string message = "BibleTime is running on the DBus as service " + names_on_bus[i];
        gtk_label_set_text (GTK_LABEL (label_bibletime_process), message.c_str());
      }
      break;
    }
  }
  // If BibleTime is not found, give a message.
  if (bibletime_bus_name.empty()) {
    gtk_label_set_text (GTK_LABEL (label_bibletime_process), "BibleTime is not running");
    if (bibletime_was_available) {
      gtk_widget_show_all (window);
    }
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


const gchar * bibletime_dbus_object ()
{
  return "/BibleTime";
}


const gchar * bibletime_dbus_interface ()
{
  return "info.bibletime.BibleTime";
}


void send_response_to_bibledit (const string& subject, const string& identifier, const string& message)
{
  string url = "http://localhost/bibledit/ipc/storemessage.php?channel=bibledit&subject="; // URL no longer accurate.
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
  quit = true;
  gtk_main_quit ();
}


void sigquit(int dummy)
{ 		 
	printf("\nCtrl-\\ trapped to quit\n");
  fflush (stdout);
  quit = true;
  gtk_main_quit ();
}


void on_entry_url_changed(GtkEditable * editable, gpointer user_data)
{
  GKeyFile *keyfile = g_key_file_new();
  gchar * key_file_name = registry_file_name ();
  g_key_file_load_from_file(keyfile, key_file_name, G_KEY_FILE_NONE, NULL);
  g_key_file_set_string (keyfile, "settings", "bibledit-web-url", gtk_entry_get_text (GTK_ENTRY (entry_url)));
  gchar *key_file_data = g_key_file_to_data(keyfile, NULL, NULL);
  g_file_set_contents(key_file_name, key_file_data, -1, NULL);
  g_free(key_file_data);
  g_key_file_free(keyfile);
  g_free (key_file_name);
}


void on_entry_user_changed(GtkEditable * editable, gpointer user_data)
{
  GKeyFile *keyfile = g_key_file_new();
  gchar * key_file_name = registry_file_name ();
  g_key_file_load_from_file(keyfile, key_file_name, G_KEY_FILE_NONE, NULL);
  g_key_file_set_string (keyfile, "settings", "bibledit-web-user", gtk_entry_get_text (GTK_ENTRY (entry_user)));
  gchar *key_file_data = g_key_file_to_data(keyfile, NULL, NULL);
  g_file_set_contents(key_file_name, key_file_data, -1, NULL);
  g_free(key_file_data);
  g_key_file_free(keyfile);
  g_free (key_file_name);
}


gchar * registry_file_name ()
{
  gchar * directory = g_build_filename (g_get_home_dir(), ".bibledit-bibletime", NULL);
  g_mkdir_with_parents (directory, 0777);
  gchar * filename = g_build_filename (directory, "settings.ini", NULL);
  g_free (directory);
  return filename;
}


void on_button_url_clicked(GtkButton * button, gpointer user_data)
{
  TinySpawn spawn ("curl");
  spawn.arg ("-sS"); // Make curl silent, but show errors.
  string url = gtk_entry_get_text (GTK_ENTRY (entry_url));
  url.append ("/ipc/setmessage.php");
  spawn.arg (url);
  spawn.read ();
  spawn.run ();
  string message;
  for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
    message.append (spawn.standardout[i] + "\n");
  }
  for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
    message.append (spawn.standarderr[i] + "\n");
  }
  if (message.empty()) {
    if (spawn.exitstatus == 0) {
      message = "Access okay";
    }
  }
  gtk_label_set_text (GTK_LABEL (label_url), message.c_str());
}


Parse::Parse(const string & line, bool remove_punctuation, const string & separator)
// Parses a line of text in its separate words.
// remove_punctuation: remove some standard punctuation.
// separator: standard this is a space, but can be modified.
{
  string processed_line;
  processed_line = trim(line);
  processed_line.append(separator);
  size_t spaceposition;
  spaceposition = processed_line.find(separator);
  while (spaceposition != string::npos) {
    string word = processed_line.substr(0, spaceposition);
    if (remove_punctuation) {
      string::size_type location = word.find_last_of(".,;:");
      if (location != string::npos)
        word = word.substr(0, location);
    }
    words.push_back(word);
    processed_line.erase(0, spaceposition + separator.length());
    spaceposition = processed_line.find(separator);
  }
}


Parse::~Parse()
{
}


int convert_to_int(const string & str)
{
  unsigned int i = 0;
  istringstream r(str);
  r >> i;
  return i;
}


void on_button_hide_clicked(GtkButton * button, gpointer user_data)
{
  gtk_widget_hide_all (window);
}


void program_quit ()
{
  quit = true;
  gtk_main_quit ();
}

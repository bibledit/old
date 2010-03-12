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


#include <libsoup/soup.h>
#include <string>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <vector>


using namespace std;


static GMainLoop *loop;

static SoupSession *session;

static void start_xiphos_web_listener ();
static void on_xiphos_web_listener_ready_callback (SoupSession *session, SoupMessage *msg, gpointer user_data);

static void start_bibletime_web_listener ();
static void on_bibletime_web_listener_ready_callback (SoupSession *session, SoupMessage *msg, gpointer user_data);

static DBusConnection *con;
static DBusGConnection *sigcon;
static DBusGProxy *proxy;
static void on_name_acquired (DBusGProxy *proxy, const char *name, gpointer user_data);
static void on_name_owner_changed (DBusGProxy *proxy, const char *name, const char *prev, const char *nw, gpointer user_data);
static void on_name_lost (DBusGProxy *proxy, const char *name, gpointer user_data);

static guint event_id_rescan_bus;
static int message_type;
static vector <string> string_reply;
static string bibletime_bus_name;
static string xiphos_bus_name;
static const gchar * xiphos_dbus_object ();
static const gchar * xiphos_dbus_interface ();
static const gchar * bibletime_dbus_object ();
static const gchar * bibletime_dbus_interface ();
void send_to_bibletime (const gchar * object, const gchar * interface, const gchar * method, const string& value);
vector <string> receive_from_bibletime (const gchar * object, const gchar * interface, const gchar * method);
void send_to_xiphos (const gchar * object, const gchar * interface, const gchar * method, const string& value);
void send (const gchar * bus_name, const gchar * object, const gchar * interface, const gchar * method, const string& payload);
vector <string> method_call_wait_reply (const gchar * bus_name, const gchar * object, const gchar * interface, const gchar * method, bool silent);
void retrieve_message (DBusMessage *message);
void retrieve_iter (DBusMessageIter *iter);
void names_on_dbus_changed ();
static bool on_rescan_bus_timeout(gpointer user_data);
void on_rescan_bus();

string trim(const string & s);
static void destroy_source(guint & event_id);

string get_extract_message_identifier (string& message);
void send_response_to_bibledit (const string& subject, const string& identifier, const string& message);
static void on_message_ready_callback (SoupSession *session, SoupMessage *msg, gpointer user_data);

static void sigproc(int dummy);
static void sigquit(int dummy);
int main (int argc, char **argv);


// Aids for dbus: dbus-monitor, d-feet



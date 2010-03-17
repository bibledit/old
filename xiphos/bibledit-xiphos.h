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
** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#ifndef INCLUDED_BIBLEDIT_XIPHOS_H
#define INCLUDED_BIBLEDIT_XIPHOS_H


#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include <string>
#include <iostream>
#include <vector>
#include <sqlite3.h>
#include <fstream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <set>
#include <deque>
#include <libsoup/soup.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>


using namespace std;


GtkWidget * window;

GtkWidget * entry_url;
static void on_entry_url_changed(GtkEditable * editable, gpointer user_data);

GtkWidget * button_url;
static void on_button_url_clicked(GtkButton * button, gpointer user_data);

GtkWidget * label_url;

GtkWidget * label_xiphos_process;

GtkWidget * label_id;
string last_message_id;
GtkWidget * label_command;
GtkWidget * label_body;

GtkWidget * label_xiphos;

GtkWidget * button_hide;
static void on_button_hide_clicked(GtkButton * button, gpointer user_data);

gchar * registry_file_name ();

class TinySpawn
{
public:
  TinySpawn (const char * program);
  ~TinySpawn ();
  void workingdirectory (string directory);
  void arg (string value);
  void read ();
  void run ();
  bool result;
  gint exitstatus;
  vector <string> standardout;
  vector <string> standarderr;
private:
  const char * myprogram;
  string myworkingdirectory;
  bool myread;
  vector <string> arguments;
};


class ParseLine
{
public:
  ParseLine (const string & text);
  ~ParseLine ();
  vector <string> lines;
private:
};


class Parse
{
public:
  Parse (const string & line, bool remove_punctuation = true, const string& separator = " ");
  ~Parse ();
  vector <string> words;
private:
};


int main (int argc, char *argv[]);
void message (string message);
string trim(const string & s);
string convert_to_string(int i);
string convert_to_string(unsigned int i);
string convert_to_string(bool b);
string convert_to_string(double d);
string convert_to_string(long unsigned int i);


static SoupSession *session;

static void start_xiphos_web_listener ();
static void on_xiphos_web_listener_ready_callback (SoupSession *session, SoupMessage *msg, gpointer user_data);

static DBusConnection *con;
static DBusGConnection *sigcon;
static DBusGProxy *proxy;
static void on_name_acquired (DBusGProxy *proxy, const char *name, gpointer user_data);
static void on_name_owner_changed (DBusGProxy *proxy, const char *name, const char *prev, const char *nw, gpointer user_data);
static void on_name_lost (DBusGProxy *proxy, const char *name, gpointer user_data);

static guint event_id_rescan_bus;
static int message_type;
static vector <string> string_reply;
static string xiphos_bus_name;
static const gchar * xiphos_dbus_object ();
static const gchar * xiphos_dbus_interface ();
void send_to_xiphos (const gchar * object, const gchar * interface, const gchar * method, const string& value);
void send (const gchar * bus_name, const gchar * object, const gchar * interface, const gchar * method, const string& payload);
vector <string> method_call_wait_reply (const gchar * bus_name, const gchar * object, const gchar * interface, const gchar * method, bool silent);
void retrieve_message (DBusMessage *message);
void retrieve_iter (DBusMessageIter *iter);
void names_on_dbus_changed ();
static bool on_rescan_bus_timeout(gpointer user_data);
void on_rescan_bus();

static void destroy_source(guint & event_id);

string get_extract_message_identifier (string& message);
void send_response_to_bibledit (const string& subject, const string& identifier, const string& message);
static void on_message_ready_callback (SoupSession *session, SoupMessage *msg, gpointer user_data);

static void sigproc(int dummy);
static void sigquit(int dummy);

int convert_to_int(const string & str);

// Aids for dbus: dbus-monitor, d-feet

#endif

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
#include <vector>


using namespace std;


static GMainLoop *loop;

static SoupSession *session;

static void start_vcs_control_web_listener ();
static void on_vcs_control_web_listener_ready_callback (SoupSession *session, SoupMessage *msg, gpointer user_data);

string trim(const string & s);

string get_extract_message_identifier (string& message);
void send_response_to_bibledit (const string& subject, const string& identifier, const string& message);
static void on_message_ready_callback (SoupSession *session, SoupMessage *msg, gpointer user_data);

static void sigproc(int dummy);
static void sigquit(int dummy);
int main (int argc, char **argv);



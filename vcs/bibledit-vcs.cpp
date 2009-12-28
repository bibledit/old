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


#include "bibledit-vcs.h"
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


void start_vcs_control_web_listener ()
{
  SoupMessage * listener_msg;
  listener_msg = soup_message_new (SOUP_METHOD_GET, "http://localhost/bibledit/ipc/getmessage.php?channel=vcscontrol");
  soup_session_queue_message (session, listener_msg, SoupSessionCallback (on_vcs_control_web_listener_ready_callback), NULL);
}


void on_vcs_control_web_listener_ready_callback (SoupSession *session, SoupMessage *msg, gpointer user_data)
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
  } else {
    // If the message was cancelled, do not start it again, just quit.
    if (msg->status_code == 1) {
      return;
    }
    printf ("Shell web listener failure, code: %d, reason: %s\n", msg->status_code, msg->reason_phrase);
    fflush (stdout);
    g_usleep (1000000);
  }
  g_usleep (100000);
  start_vcs_control_web_listener ();
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
  session = soup_session_async_new_with_options (SOUP_SESSION_USER_AGENT, "bibledit-vcs/1.0", NULL);
  start_vcs_control_web_listener ();

  // Signal trapping for doing a clean exit.
  signal(SIGINT, sigproc);
  signal(SIGQUIT, sigquit);

  // The main loop will block till something quits it.
  loop = g_main_loop_new (NULL, TRUE);
  g_main_loop_run (loop);
  g_main_loop_unref (loop);

  // Abort the session including the listeners
  soup_session_abort (session);

  // Well done, my boy.
  return 0;
}



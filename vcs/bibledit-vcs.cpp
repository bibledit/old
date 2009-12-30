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
#include "spawn.h"


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
    // Handle the "pause" command.
    if (body.find ("pause") == 0) {
      run = false;
    }
    // Handle the "continue" command.
    if (body.find ("continue") == 0) {
      run = true;
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


void start_vcs_worker_web_listener ()
{
  SoupMessage * listener_msg;
  listener_msg = soup_message_new (SOUP_METHOD_GET, "http://localhost/bibledit/ipc/getmessage.php?channel=vcsworker");
  soup_session_queue_message (session, listener_msg, SoupSessionCallback (on_vcs_worker_web_listener_ready_callback), NULL);
}


void on_vcs_worker_web_listener_ready_callback (SoupSession *session, SoupMessage *msg, gpointer user_data)
{
  if (SOUP_STATUS_IS_SUCCESSFUL (msg->status_code)) {
    // Get the response body.
    string body (msg->response_body->data);
    body = trim (body);
    // Log it.
    printf ("%s\n", body.c_str());
    fflush (stdout);
    // Command is not to run?
    if (!run) {
      body.clear();
      printf ("Paused, skipping command\n");
      fflush (stdout);
    }
    // Assemble the command to run.
    vector <string> lines = parse_line (body);
    if (!lines.empty()) {
      string command = lines[0];
      string workingdirectory;
      if (lines.size() >= 2) {
        workingdirectory = lines[1];
      }
      // Only run when the working directory exists.
      string output;
      if (workingdirectory.empty () || g_file_test (workingdirectory.c_str(), G_FILE_TEST_IS_DIR)) {
        string shell_command = "cd '" + workingdirectory + "' ; " + command;
        printf ("Run %s\n", shell_command.c_str());
	fflush (stdout);
        // Glib's spawn routines choke after some time when it is called often, as happens here.
	// Therefore popen is used instead.
        FILE *stream = popen(shell_command.c_str(), "r");
	char buf[1024];
	while (fgets(buf, sizeof(buf), stream)) {
	  output.append(buf);
	}
	int exitcode = pclose(stream);
	printf ("Output:\n%s", output.c_str());
	printf ("Exit code %d\n", exitcode);
	fflush (stdout);
      } else {
	printf ("Directory %s does not exist\n", workingdirectory.c_str());
      }
      fflush (stdout);
      // Assemble the message to be returned to bibledit.
      string message;
      // Subject: vcs
      message.append ("vcs\n");
      // Shell command.
      message.append (command + "\n");
      // Working directory (if any).
      message.append (workingdirectory + "\n");
      // Output.
      message.append (output);
      // Store the message in a temporal file.
      gchar * name_used;
      g_file_open_tmp ("XXXXXX", &name_used, NULL);
      if (name_used) {
	g_file_set_contents (name_used, message.c_str(), -1, NULL);
	// Upload the message through curl.
	string command = "curl -F uploaded=@";
	command.append (name_used);
	command.append (" http://localhost/bibledit/ipc/uploadmessage.php");
	if (system (command.c_str()));
	// Remove the temporal file.
	unlink (name_used);
      }
    }
  } else {
    // If the message was cancelled, do not start the listener again.
    if (msg->status_code == 1) {
      return;
    }
    printf ("Shell web listener failure, code: %d, reason: %s\n", msg->status_code, msg->reason_phrase);
    fflush (stdout);
    g_usleep (1000000);
  }
  // Wait shortly, the restart the listener.
  g_usleep (100000);
  start_vcs_worker_web_listener ();
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
  
  run = true;

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
  start_vcs_worker_web_listener ();

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


vector <string> parse_line (string line)
// Parses the input into lines.
{
  vector <string> lines;
  line = trim(line);
  size_t pos = line.find("\n");
  while (pos != string::npos) {
    string word = line.substr(0, pos );
    lines.push_back(trim(word));
    line.erase(0, pos  + 1);
    line = trim(line);
    pos  = line.find("\n");
  }
  if (!line.empty())
    lines.push_back(trim(line));
  return lines;
}


vector <string> parse_word (string line)
// Parses the input into words.
{
  vector <string> words;
  line = trim(line);
  line.append(" ");
  size_t pos = line.find(" ");
  while (pos != string::npos) {
    string word = line.substr(0, pos);
    words.push_back(word);
    line.erase(0, pos + 1);
    pos = line.find(" ");
  }
  return words;
}


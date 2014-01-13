/*
** Copyright (©) 2003-2008 Teus Benschop.
** Copyright (©) 2011 David Gardner.
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


#include "bibledit-git.h"
#include <config.h>



int main (int argc, char *argv[])
{
  
#ifndef WIN32
  // Do not run as root.
  if (getuid () == 0) {
    cout << "Bibledit-Git has not been designed to run with root privileges." << endl;
    return 1;
  }
#endif

  // Initialize Gtk.
  gtk_init (&argc, &argv);

  // Display the window.      
  GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Bibledit-Git");
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
  gtk_window_set_type_hint (GTK_WINDOW (window), GDK_WINDOW_TYPE_HINT_NORMAL);
  g_signal_connect ((gpointer) window, "delete_event", G_CALLBACK (gtk_main_quit), NULL);

  GtkWidget * scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  GtkWidget * vbox = gtk_vbox_new(FALSE,1);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_container_add (GTK_CONTAINER (vbox), scrolledwindow);

  textview = gtk_text_view_new ();
  gtk_container_add (GTK_CONTAINER (scrolledwindow), textview);
  gtk_widget_set_size_request (textview, 640, 480);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview), GTK_WRAP_WORD_CHAR);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), false);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (textview), false);

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));

  GtkWidget * checkbutton = gtk_check_button_new_with_mnemonic ("_Repeat at end");
  g_signal_connect ((gpointer) checkbutton, "toggled", G_CALLBACK (on_checkbutton_loop_toggled), NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(checkbutton), true);
  gtk_container_add(GTK_CONTAINER (vbox), checkbutton);

  message ("Sending and receiving Bibles.");
  
  gtk_widget_show_all (window);

  // Read the folders to run git on.
  for (int i = 1; i < argc; i++) {
    folders.push_back (argv[i]);
  }
  folders.push_back ("");

  // Wait shortly, then process the data.
  g_timeout_add(2000, GSourceFunc(on_timeout), NULL);

  // Main loop.
  gtk_main();

  // Done.  
  return 0;
}


bool on_timeout (gpointer data)
{
  if (folders.empty()) {
    message ("");
    message ("Ready.");
    message ("You can close the window, or it will close itself in 5 minutes.");
    g_timeout_add (300000, GSourceFunc(exit_timeout), NULL);
  } else {
    string folder = folders[0];
    folders.pop_front ();

    // Keep going round and round in circles ...
    folders.push_back(folder);

    if (folder.empty() ) {
      if (we_loop) {
        message ("Will send and receive again after 5 minutes.");
        message ("Or close the window to not send and receive again.");
        g_timeout_add(300000, GSourceFunc(on_timeout), NULL);
      } else {
      	message ("Finished");
        message ("You can close the window, or it will close itself in 5 minutes.");
        g_timeout_add (300000, GSourceFunc(exit_timeout), NULL);
      }

    } else { 

      message (folder);

      // Tell git about the default method for pushing.
      {
        TinySpawn spawn ("git");
        spawn.arg ("config");
        spawn.arg ("push.default");
        spawn.arg ("matching");
        spawn.workingdirectory (folder);
        spawn.run ();
      }
    
      // Add everything because things could have been added or changed.
      {
        TinySpawn spawn ("git");
        spawn.arg ("add");
        spawn.arg (".");
        spawn.workingdirectory (folder);
        spawn.run ();
      }
    
      // Show status.
      {
        TinySpawn spawn ("git");
        spawn.arg ("status");
        spawn.workingdirectory (folder);
        spawn.read ();
        spawn.run ();
        for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
          message (spawn.standardout[i]);
        }
        for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
          message (spawn.standarderr[i]);
        }
      }
    
      // Commit changes locally.
      {
        TinySpawn spawn ("git");
        spawn.arg ("commit");
        spawn.arg ("-a");
        spawn.arg ("-m");
        spawn.arg ("Send and receive");
        spawn.workingdirectory (folder);
        spawn.read ();
        spawn.run ();
        for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
          message (spawn.standardout[i]);
        }
        for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
          message (spawn.standarderr[i]);
        }
      }
    
      // Pull changes from the remote repository.
      {
        TinySpawn spawn ("git");
        spawn.arg ("pull");
        spawn.workingdirectory (folder);
        spawn.read ();
        spawn.run ();
        // Normal operation: Exit status = 0.
        // Merge conflict: Exit status = 256.
        // Remote repository unavailable: Exit status = 256.
        // Conclusion: The exit status cannot be used conclusively for finding out about a merge conflict.
        bool merge_conflict = false;
        for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
          if (spawn.standardout[i].find ("CONFLICT") != string::npos) {
            merge_conflict = true;
          }
          message (spawn.standardout[i]);
        }
        for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
          message (spawn.standarderr[i]);
        }
        if (merge_conflict) {
          message ("Bibledit will resolve conflicts between its own data and the data on the server.");
          // Resolve merge conflict.
          TinySpawn mergetool ("php");
          string script;
          gchar *name;
          name = g_build_filename (PACKAGE_DATA_DIR, "conflictcli.php", NULL);
          script = name;
          g_free(name);
          mergetool.arg (script);
          mergetool.arg (folder);
          mergetool.read ();
          mergetool.run ();
          for (unsigned int i = 0; i < mergetool.standardout.size(); i++) {
            message (mergetool.standardout[i]);
          }
          for (unsigned int i = 0; i < mergetool.standarderr.size(); i++) {
            message (mergetool.standarderr[i]);
          }
        }
      }
    
      // Push changes to the remote repository.
      {
        TinySpawn spawn ("git");
        spawn.arg ("push");
        spawn.workingdirectory (folder);
        spawn.read ();
        spawn.run ();
        for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
          message (spawn.standardout[i]);
        }
        for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
          message (spawn.standarderr[i]);
        }
      }
  
      // Next folder.
      g_timeout_add(500, GSourceFunc(on_timeout), NULL);
    
    }

  }

  return false;
}


void message (string message)
{
  gtk_text_buffer_insert_at_cursor (buffer, message.c_str(), -1);
  gtk_text_buffer_insert_at_cursor (buffer, "\n", -1);
  GtkTextIter iter;
  gtk_text_buffer_get_end_iter (buffer, &iter);
  gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW (textview), &iter, 0.1, true, 0.5, 0.5);
  while (gtk_events_pending()) gtk_main_iteration();
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
  char *argv [arguments.size() + 2];
  argv[0] = (char *)myprogram;
  for (unsigned int i = 0; i < arguments.size(); i++) {
    argv[i + 1] = (char *)arguments[i].c_str();
  }
  // Terminate argv.
  argv[arguments.size() + 1] = NULL;
  // Spawn flags.
  int flags = G_SPAWN_SEARCH_PATH;
  // Possible pipes.
  gchar *standard_output = NULL;
  gchar *standard_error = NULL;
  gchar **standard_output_pointer = NULL;
  gchar **standard_error_pointer = NULL;
  if (myread) {
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


void on_checkbutton_loop_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  we_loop = gtk_toggle_button_get_active (togglebutton);
}


bool exit_timeout (gpointer data)
{
  gtk_main_quit ();
  return false;
}

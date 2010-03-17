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


#include "bibledit-git.h"




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
  gtk_container_add (GTK_CONTAINER (window), scrolledwindow);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  
  textview = gtk_text_view_new ();
  gtk_container_add (GTK_CONTAINER (scrolledwindow), textview);
  gtk_widget_set_size_request (textview, 640, 480);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview), GTK_WRAP_WORD_CHAR);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), false);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (textview), false);

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));

  message ("Sending and receiving Bibles.");
  
  gtk_widget_show_all (window);

  // Get folders to run git on.
  for (int i = 1; i < argc; i++) {
    folders.push_back (argv[i]);
  }

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
    message ("You can close the window.");
  } else {
    string folder = folders[0];
    folders.pop_front ();
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
      spawn.arg ("-a");
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
      spawn.arg ("commit");
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
      for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
        message (spawn.standardout[i]);
      }
      for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
        message (spawn.standarderr[i]);
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



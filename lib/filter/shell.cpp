/*
Copyright (©) 2003-2016 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include <filter/shell.h>
#include <filter/string.h>
#include <filter/url.h>
#include <database/logs.h>


string filter_shell_escape_argument (string argument)
{
  argument = filter_string_str_replace ("'", "\\'", argument);
  argument.insert (0, "'");
  argument.append ("'");
  return argument;
}


// Runs shell $command in folder $directory, with $parameters.
// If $output and $error are non-NULL, that is where the output of the shell command goes.
// If they are NULL, the output of the shell command goes to the Journal.
int filter_shell_run (string directory, string command, const vector <string> parameters,
                      string * output, string * error)
{
  command = filter_shell_escape_argument (command);
  if (!directory.empty ()) {
    directory = filter_shell_escape_argument (directory);
    command.insert (0, "cd " + directory + "; ");
  }
  for (string parameter : parameters) {
    parameter = filter_shell_escape_argument (parameter);
    command.append (" " + parameter);
  }
  string pipe = filter_url_tempfile ();
  string standardout = pipe + ".out";
  string standarderr = pipe + ".err";
  command.append (" > " + standardout);
  command.append (" 2> " + standarderr);
  int result = system (command.c_str());
  string contents = filter_url_file_get_contents (standardout);
  if (output) {
    output->assign (contents);
  } else {
    Database_Logs::log (contents);
  }
  contents = filter_url_file_get_contents (standarderr);
  if (error) {
    error->assign (contents);
  } else {
    Database_Logs::log (contents);
  }
  filter_url_unlink (standardout);
  filter_url_unlink (standarderr);
  return result;
}


// Runs $command with $parameters.
// It does not run $command through the shell, but executes it straight.
int filter_shell_run (string command, const char * parameter, string & output)
{
  // File descriptor for file to write child's stdout to.
  string path = filter_url_tempfile () + ".txt";
  int fd = open (path.c_str (), O_WRONLY|O_CREAT, 0666);
  
    // Create child process as a duplicate of this process.
  pid_t pid = fork ();
  
  if (pid == 0) {

    // This runs in the child.
    dup2(fd, 1);
    close(fd);
    execlp (command.c_str(), parameter, (char*)0);
    // The above only returns in case of an error.
    Database_Logs::log (strerror (errno));
    _exit (1);
    close (fd);
    return -1;
  }

  // Wait till child is ready.
  wait(0);
  close(fd);
  
  // Read the child's output.
  output = filter_url_file_get_contents (path);
  
  return 0;
}


// Runs $command as if it were typed on the command line.
// Does not escape anything in the $command.
// Returns the exit code of the process.
// The output of the process, both stdout and stderr, go into $out_err.
int filter_shell_run (string command, string & out_err)
{
  string pipe = filter_url_tempfile ();
  command.append (" > " + pipe + " 2>&1");
  int result = system (command.c_str());
  out_err = filter_url_file_get_contents (pipe);
  return result;
}


// Lists the running processes.
vector <string> filter_shell_active_processes ()
{
  vector <string> processes;

  string output, error;
  int result;
  
#ifdef HAVE_WINDOWS
  result = filter_shell_run ("tasklist.exe", NULL, output);
  // Note that the above works on a system which has Cygwin installed,
  // and fails to run on a system without Cygwin.
  // Perhaps Bibledit needs to install components from Cygwin into the $PATH for this to run.
  // Or use the other shell runner, and then copy bash.exe to /Windows/system32 or into the $PATH.
#else
  result = filter_shell_run ("", "ps", {"ax"}, &output, &error);
#endif

  if (result) {}
  
  if (!error.empty ()) {
    output.append ("\n");
    output.append (error);
  }
  processes = filter_string_explode (output, '\n');

  return processes;
}


// Returns true if $program is present on the system.
bool filter_shell_is_present (string program)
{
  string command = "which " + program + " > /dev/null 2>&1";
  int exitcode = system (command.c_str ());
  return (exitcode == 0);
}

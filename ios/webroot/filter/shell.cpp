/*
Copyright (©) 2003-2015 Teus Benschop.

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

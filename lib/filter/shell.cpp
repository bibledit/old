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


string filter_shell_escape_argument (string argument)
{
  argument = filter_string_str_replace ("'", "\\'", argument);
  return argument;
}


int filter_shell_run (string directory, string command, const vector <string> parameters, string & output, string & error)
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
  // The process changes directory, so the output pipes need to be in a well-known directory,
  // rather than in a relative directory name, like "./tmp/pipe.out".
  string pipe = filter_url_tempfile ("/tmp");
  string stdout = pipe + ".out";
  string stderr = pipe + ".err";
  command.append (" > " + stdout);
  command.append (" 2> " + stderr);
  int result = system (command.c_str());
  output = filter_url_file_get_contents (stdout);
  error = filter_url_file_get_contents (stderr);
  filter_url_unlink (stdout);
  filter_url_unlink (stderr);
  return result;
}

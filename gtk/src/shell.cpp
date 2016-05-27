/*
** Copyright (©) 2003-2013 Teus Benschop.
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


#include "libraries.h"
#include "shell.h"
#include "gwrappers.h"
#include "directories.h"
#include "utilities.h"
#include "gtkwrappers.h"
#include "progresswindow.h"
#include "unixwrappers.h"
#include "tiny_utilities.h"
#include <config.h>

bool program_is_running(const ustring & commandline)
// Returns true if the program given on "commandline" is running.
{
  return (programs_running_count(commandline) > 0);
}


int programs_running_count(const ustring & commandline)
// Returns how many times the program given on "commandline" is running.
{
  // Get the processes.
  vector <ustring> processes = list_processes ();
  // Usage count.
  int count = 0;
  for (unsigned int i = 0; i < processes.size(); i++) {
    if (processes[i].find(commandline) != string::npos) {
      count++;
	}
  }
  // Return the count.
  return count;
}


vector <ustring> list_processes ()
// Lists the current processes from the process status program.
{
  vector <ustring> processes;
#ifdef WIN32
  GwSpawn spawn("tasklist");
#else
  GwSpawn spawn("ps");
  spawn.arg("ax");
#endif
  spawn.read();
  spawn.run();
  processes = spawn.standardout;
  return processes;
}


ustring shell_quote_space(const ustring & filename)
// Puts quotes and spaces around a filename, making it fit for the shell.
// Example: /home/user/John Johnson/.bibledit/projects/test/Genesis
// becomes: '/home/user/John Johnson/.bibledit/projects/test/Genesis'
// with an extra space prefixed and suffixed.
// Unix does not allow spaces in the user names, at the time of writing,
// but Windows does.
// This function was introduced to address this situation, but has a wider
// use too.
// In Windows, do not quote the name if it contains no space.
// Quoting the /s of the rmdir command, for example, confused the command.
{
  ustring quotedname;
#ifdef WIN32
  if (filename.find(" ") != string::npos) {
    quotedname = " \"" + filename + "\" ";
  }
  else {
    quotedname = " " + filename + " ";
  }
#else
#if 0
  // Was quotedname = " '" + filename + "' ";
  // Then was:
  if (filename.find(" ") != string::npos) {
    quotedname = " '" + filename + "' ";
  }
  else { 
    quotedname = filename; // if no spaces, leave it alone
  }

  // Then was that we just escape any spaces in the argument
  quotedname = filename;
  replace_text(quotedname, " ", "\\ ");
  // The original way worked better. But it seemed to fail on "New Project" directory
  // name. So I tried the two above alternate methods, but it failed when creating
  // 1\ Samuel directory. I don't know...This is confusing.
#endif
  // Finally, we settle on this:
  if (filename.find(" ") != string::npos) {
    quotedname = " '" + filename + "' ";
    // Question: why the space before and after? Not sure. I don't think we need it.
  }
  else {
  quotedname = filename;
  }
#endif
  return quotedname;
}


ustring shell_clean_filename(const ustring & filename)
// Replace characters like ' and / occur in the filename with _.
{
  ustring cleanfile(filename);
  replace_text(cleanfile, "'", "_");
  replace_text(cleanfile, "/", "_");
  return cleanfile;
}


void shell_pipe_file_append(const ustring & inputfile, const ustring & outputfile)
// cat inputfile >> outputfile.
{
  ustring command;
#ifdef WIN32
  command.append("type");
#else
  command.append("cat");
#endif
  command.append(shell_quote_space(inputfile));
  command.append(">>");
  command.append(shell_quote_space(outputfile));
  if (system(command.c_str())) ; // This one does not work with GwSpawn because pipes used.
}

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


#include "spawn.h"


Spawn::Spawn(const string& program)
{
  myprogram = program;
  exitstatus = 0;
}


Spawn::~Spawn()
{
  if (standard_output)
    g_free(standard_output);
  if (standard_error)
    g_free(standard_error);
}


void Spawn::workingdirectory(string directory)
// The process' working directory.
{
  myworkingdirectory = directory;
}


void Spawn::arg(string value)
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


void Spawn::run()
{
  // Working directory.
  const gchar *workingdirectory = NULL;
  if (!myworkingdirectory.empty())
    workingdirectory = myworkingdirectory.c_str();
  // Store arguments in argv.
  char *argv[arguments.size() + 2];
  argv[0] = (char *)myprogram.c_str();
  for (unsigned int i = 0; i < arguments.size(); i++) {
    argv[i + 1] = (char *)arguments[i].c_str();
  }
  // Terminate argv.
  argv[arguments.size() + 1] = NULL;
  // Spawn flags.
  int flags = G_SPAWN_SEARCH_PATH;
  // Possible pipes.
  standard_output = NULL;
  standard_error = NULL;
  gchar **standard_output_pointer = &standard_output;
  gchar **standard_error_pointer = &standard_error;
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
}



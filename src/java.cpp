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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/


#include "libraries.h"
#include "utilities.h"
#include <glib.h>
#include "java.h"
#include "gwrappers.h"
#include "shell.h"


ustring java_home_environment_variable ()
// Returns the JAVA_HOME environment variable
{
  const gchar * variable;
  variable = g_getenv ("JAVA_HOME");
  ustring value;
  if (variable)
    value = variable;
  return value;
}


bool java_is_sun (const ustring& java_executable, ustring& output)
// Checks whether the Java machine is the Sun one, not GNU gij.
{
  output.clear();
  bool sun = false;
  if (!java_executable.empty()) {
    GwSpawn spawn ((char *)java_executable.c_str());
    spawn.arg ("-version");
    spawn.read ();
    spawn.run ();
    for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
      output.append (spawn.standardout[i] + "\n");
    }
    for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
      output.append (spawn.standarderr[i] + "\n");
    }
    if (output.find ("Java(TM)") != string::npos)
      sun = true;
  }
  return sun;
}


bool java_search (ustring& java_binary)
// This searches for a Sun Java interpreter.
// It gives preference to the current java binary passed.
{
  ustring dummy;
  ustring binary;
  // Try current binary, the one passed.
  if (java_is_sun (java_binary, dummy))
    return true;
  // Try "java", which will work on Darwin and Windows.
  // On Linux it will give GNU gij, but that one will not be accepted.
  binary = "java";
  if (java_is_sun (binary, dummy)) {
    java_binary = binary;
    return true;
  }
  // Try the one in $JAVA_HOME, if there.
  binary = java_home_environment_variable ();
  if (!binary.empty()) {
    binary.append ("/bin/java");
    if (java_is_sun (binary, dummy)) {
      java_binary = binary;
      return true;
    }
  }
  // If the user gave Java Home, instead of the executable, handle that case.
  binary = java_binary + "/bin/java";
  if (java_is_sun (binary, dummy)) {
    java_binary = binary;
    return true;
  }
  // If the user followed the installation instructions, we should "reward" him
  // and find the binary straightaway.
  ReadDirectories rd (g_get_home_dir(), "jre", "");
  for (unsigned int i = 0; i < rd.directories.size(); i++) {
    binary = gw_build_filename (g_get_home_dir(), rd.directories[i], "bin", "java");
    if (java_is_sun (binary, dummy)) {
      java_binary = binary;
      return true;
    }
  }
  // None found.
  return false;
}

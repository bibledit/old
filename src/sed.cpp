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


#include "sed.h"
#include "unixwrappers.h"
#include "directories.h"
#include "gwrappers.h"
#include "utilities.h"
#include "shell.h"


ustring sed_straight_through ()
{
  return "straight through";
}


ustring sed_script_prefix ()
{
  return "sed_script_";
}


vector <ustring> sed_available_scripts ()
// Gets a list of available sed scripts.
{
  ReadFiles rf (directories_get_configuration (), sed_script_prefix (), "");
  for (unsigned int i = 0; i < rf.files.size (); i++) {
    rf.files[i].erase (0, sed_script_prefix ().length ());
  }
  rf.files.push_back (sed_straight_through ());
  sort (rf.files.begin(), rf.files.end());
  return rf.files;
}


bool sed_script_available (const ustring& script)
// Returns whether "script" is available.
{
  vector <ustring> scripts = sed_available_scripts ();
  set <ustring> scripts_set (scripts.begin (), scripts.end ());
  return scripts_set.find (script) != scripts_set.end ();
}


ustring sed_filter (const ustring& rulesfile, bool straightthrough, const ustring& inputfile, const ustring& outputfile)
/*
Runs sed with the "rulesfile".
Input text in "inputfile", and the output text goes in "outputfile".
If everything is okay, it returns nothing.
If there were errors, it returns these.
*/
{
  // Remove any previous out.
  unlink (outputfile.c_str());
  unlink (sed_temporal_error_file ().c_str());
  
  // If the rules file does not exist, or the "straightthrough" flag is set, pass it straight through.
  if (!g_file_test (rulesfile.c_str(), G_FILE_TEST_IS_REGULAR) || straightthrough) {
    unix_cp (inputfile, outputfile);
    return "";
  }
  
  // Run sed with the filter.
  ustring command;
  command.append ("sed -f");
  command.append (shell_quote_space (rulesfile));
  command.append ("<");
  command.append (shell_quote_space (inputfile));  
  command.append (">");
  command.append (shell_quote_space (outputfile));  
  command.append ("2>");
  command.append (sed_temporal_error_file ());  
  int result = system (command.c_str ());

  // Handle OK.
  if (result == 0) return "";
  
  // Handle error.
  gchar *contents;
  g_file_get_contents (sed_temporal_error_file ().c_str(), &contents, NULL, NULL);
  ustring error;
  if (contents) {
    error = contents;
    g_free (contents);
  }
  return error;
}


ustring sed_script_path (const ustring& script)
{
  return gw_build_filename (directories_get_configuration (), sed_script_prefix () + script);
}


ustring sed_temporal_input_file ()
{
  return gw_build_filename (directories_get_temp (), "sed_filter_input");
}


ustring sed_temporal_output_file ()
{
  return gw_build_filename (directories_get_temp (), "sed_filter_output");
}


ustring sed_temporal_error_file ()
{
  return gw_build_filename (directories_get_temp (), "sed_filter_error");
}


/*

Todo diglot implementation


Make helpfile for diglot, but probably use another name.
Working were Todo occurs.



In the window, there is online help that only focuses on the sed-rules,
and another online help that focuses on regular expressions, and a link
to the homepage of sed for more information.





*/

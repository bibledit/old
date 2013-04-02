/*
** Copyright (©) 2003-2012 Teus Benschop.
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
#include "htmlbrowser.h"
#include <gtk/gtk.h>
#include "gtkwrappers.h"
#include "gwrappers.h"
#include "directories.h"
#include "utilities.h"
#include "unixwrappers.h"
#include "constants.h"
#include "uname.h"
#include "windowsoutpost.h"


#define NUMBER_OF_BROWSERS 10


const gchar *helpcommand(int index)
{
  // Returns a command to be executed to get the helpfile displayed.
  // Variable "index" selects which browser will be used.
  const gchar *command = NULL;
  switch (index) {
  case 0:
    {
      command = "htmlview";
      break;
    }
  case 1:
    {
      command = "google-chrome";
      break;
    }
  case 2:
    {
      command = "firefox";
      break;
    }
  case 3:
    {
      command = "firefox3";
      break;
    }
  case 4:
    {
      command = "mozilla";
      break;
    }
  case 5:
    {
      command = "galeon";
      break;
    }
  case 6:
    {
      command = "konqueror";
      break;
    }
  case 7:
    {
      command = "epiphany";
      break;
    }
  case 8:
    {
      command = "opera";
      break;
    }
  case 9:
    {
      command = "open";
      break;
    }
    // Note: when adding new browsers, ensure that
    // NUMBER_OF_BROWSERS is updated too.
  }
  return command;
}


void htmlbrowser (const ustring & filename, bool network, bool no_tamper)
// View the file through the default browser.
// no_tamper: No tampering with the "filename".
{
  // Get the prefix.
  ustring prefix;
  if (!no_tamper) {
    if (network)
      prefix = "http://";
    else
      prefix = "file://";
  }
  // Handle if Windows.
#ifdef WIN32
  ustring url(filename);
  url.insert(0, prefix);
  windowsoutpost_open_url(url);
  return;
#endif

  // Handle Unix.
  int browser_index = -1;
  ustring outputfile;
  for (unsigned int i = 0; i < NUMBER_OF_BROWSERS; i++) {
    // At first we used the exit code of "which" to determine whether it had
    // found the program, but this does not work on BSD likes. So now we read
    // the output to see what whether it found the program.
    if (gw_find_program_in_path(helpcommand(i))) {
      browser_index = i;
      break;
    }
  }
  if (browser_index >= 0) {
    GwSpawn spawn(helpcommand(browser_index));
    spawn.arg(prefix + filename);
    spawn.async();
    spawn.run();
  } else {
    // No suitable browser was found.
    // Inform the user about it, and what to do to solve it.
    ustring message;
    message = "The help cannot be displayed, because there is no suitable browser installed.\n";
    message.append("Install one of the following supported browsers:\n");
    for (unsigned int i = 0; i < NUMBER_OF_BROWSERS; i++) {
      message.append(helpcommand(i));
      message.append("\n");
    }
    gtkw_dialog_error(NULL, message);
  }
}


ustring html_server_url (const ustring& url)
{
  ustring server_url = gw_build_filename (directories_get_package_data (), url);
  return server_url;
}



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
#include "textviewer.h"
#include <gtk/gtk.h>
#include "gtkwrappers.h"
#include "unixwrappers.h"
#include "shell.h"
#include "uname.h"
#include "gwrappers.h"

void textviewer_run(const gchar * command, const gchar * argument, const ustring & filename)
{
  GwSpawn spawn(command);
  if (strlen(argument) > 0)
    spawn.arg(argument);
  spawn.arg(filename);
  spawn.async();
  spawn.run();
}

void textviewer_view(const ustring & filename)
{
  struct {
    const char *command;
    const char *argument;
  } text_viewers[] = {
    {
    "gedit", ""}, {
    "kedit", ""}, {
    "open", ""}
  };

  for (unsigned int i = 0; i < (sizeof(text_viewers) / sizeof(*text_viewers)); i++) {
    if (gw_find_program_in_path(text_viewers[i].command)) {
      textviewer_run(text_viewers[i].command, text_viewers[i].argument, filename);
      return;
    }
  }

  // If we're here, it shows that no suitable viewer was found.
  // Inform the user about it, and what to do to solve it.
  ustring message = "The file cannot be displayed, because there is no suitable text viewer installed.\n";
  message.append("Install a supported text viewer");
  gtkw_dialog_error(NULL, message);
}

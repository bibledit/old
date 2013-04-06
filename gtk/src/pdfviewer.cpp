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
#include "pdfviewer.h"
#include <gtk/gtk.h>
#include "gtkwrappers.h"
#include "unixwrappers.h"
#include "shell.h"
#include "uname.h"
#include "windowsoutpost.h"
#include "gwrappers.h"
#include "settings.h"

void pdfviewer_run(const gchar * command, const gchar * argument, const ustring & filename)
{
  GwSpawn spawn(command);
  if (strlen(argument) > 0)
    spawn.arg(argument);
  spawn.arg(filename);
  spawn.async();
  spawn.run();
}

void pdfviewer_view(const ustring & filename)
{
  // Handle Windows.
  if (uname_get() == untWindows) {
    windowsoutpost_open_url(filename);
    return;
  }
  // Handle possible manually set viewer.
  extern Settings *settings;
  if (!settings->genconfig.pdf_viewer_automatic_get()) {
    if (gw_find_program_in_path(settings->genconfig.pdf_viewer_path_get().c_str())) {
      pdfviewer_run((gchar *) settings->genconfig.pdf_viewer_path_get().c_str(), (gchar *) settings->genconfig.pdf_viewer_arguments_get().c_str(), filename);
      return;
    }
  }
  // Possible viewers on Linux and Macintosh.
  struct {
    const char *command;
    const char *argument;
  } pdf_viewers[] = {
    {
    "acroread", ""}, {
    "evince", ""}, {
    "xpdf", "-paper match"}, {
    "ghostview", ""}, {
    "gpdf", ""}, {
    "kpdf", ""}, {
    "kghostview", ""}, {
    "open", ""}
  };

  // Handle Unix.
  for (unsigned int i = 0; i < (sizeof(pdf_viewers) / sizeof(*pdf_viewers)); i++) {
    if (gw_find_program_in_path(pdf_viewers[i].command)) {
      pdfviewer_run(pdf_viewers[i].command, pdf_viewers[i].argument, filename);
      return;
    }
  }

  // If we're here, it shows that no suitable viewer was found.
  // Inform the user about it, and what to do to solve it.
  ustring message = "The file cannot be displayed, because there is no suitable pdf viewer installed.\n";
  message.append("Install a supported pdf viewer, or set up your own under Preferences");
  gtkw_dialog_error(NULL, message);
}

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
#include <glib/gi18n.h>
#include "directories.h"
#include <config.h>
#ifdef WIN32
#include <windows.h>
#endif

//void pdfviewer_run(const gchar * command, const gchar * argument, const ustring & filename)
void pdfviewer_run(const ustring &command, const ustring &argument, const ustring & filename)
{
  GwSpawn spawn(command);
  if (argument.size() > 0) {
    spawn.arg(argument);
  }
  spawn.arg(filename);
  spawn.async();
  spawn.run();
}

void pdfviewer_view(const ustring & filename)
{
#if 0	
  // Handle Windows.
  if (uname_get() == untWindows) {
    windowsoutpost_open_url(filename);
    return;
  }
#endif
  // Handle possible manually set viewer.
  extern Settings *settings;
  if (!settings->genconfig.pdf_viewer_automatic_get()) {
    if (gw_find_program_in_path(settings->genconfig.pdf_viewer_path_get().c_str())) {
      //pdfviewer_run((gchar *) settings->genconfig.pdf_viewer_path_get().c_str(), (gchar *) settings->genconfig.pdf_viewer_arguments_get().c_str(), filename);
	  pdfviewer_run(settings->genconfig.pdf_viewer_path_get(), 
		            settings->genconfig.pdf_viewer_arguments_get(),
					filename);
      return;
    }
  }
  // Use directories container that has figured out a PDF viewer in the path
  if (Directories->get_pdfviewer() != "") {
	pdfviewer_run(Directories->get_pdfviewer(), Directories->get_pdfviewer_args(), filename);
	return;
  }
#ifdef WIN32
  else {
	// If we're here, no suitable viewer was found on Windows.
	ShellExecute(0, 0, filename.c_str(), 0, 0 , SW_SHOW);
  }
#else
  else {
	// If we're here, no suitable viewer was found on Linux/Macintosh.
	// Inform the user about it, and what to do to solve it.
	ustring message = _("The file cannot be displayed, because there is no suitable pdf viewer installed.\n");
	message.append(_("Install a supported pdf viewer, or set up your own under Preferences | PDF Viewer"));
	gtkw_dialog_error(NULL, message);
  }
#endif
}

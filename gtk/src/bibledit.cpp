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


#include "libraries.h"
#include "mainwindow.h"
#include "directories.h"
#include "utilities.h"
#include "gwrappers.h"
#include "directories.h"
#include "dialogsystemlog.h"
#include "shell.h"
#include "maintenance.h"
#include "gtkwrappers.h"
#include "upgrade.h"
#include "unixwrappers.h"
#include "settings.h"
#include "constants.h"
#include "localizedbooks.h"
#include "versifications.h"
#include "mappings.h"
#include "ot-quotations.h"
#include "styles.h"
#include <libxml/xmlreader.h>
#include "startup.h"
#include <libxml/nanohttp.h>
#include "c_url.h"
#include "urltransport.h"
#include "runtime.h"
#include "vcs.h"


Settings *settings;
BookLocalizations *booklocalizations;
Versifications *versifications;
Mappings *mappings;
Styles *styles;
GtkAccelGroup *accelerator_group;
URLTransport * urltransport;
VCS *vcs;


int main(int argc, char *argv[])
{
  // Initialize g threads.
  g_thread_init(NULL);
  // Initialize g types.
  g_type_init();
  // Initialize GTK
  gtk_init(&argc, &argv);

  // Check whether it is fine to start the program.
  // If not, quit the program normally.
  if (!check_bibledit_startup_okay(argc, argv)) {
    return 0;
  }

  // Check on required directory structure.
  directories_check_structure();

  // Move logfile for shutdown program.
  move_log_file (lftShutdown);

  // Redirect stdout and stderr to file.
  {
    move_log_file (lftMain);
    // When a file is opened it is always allocated the lowest available file 
    // descriptor. Therefore the following commands cause stdout to be 
    // redirected to the logfile.
    close(1);
    creat (log_file_name(lftMain, false).c_str(), 0666); 
    // The dup() routine makes a duplicate file descriptor for an already opened 
    // file using the first available file descriptor. Therefore the following 
    // commands cause stderr to be redirected to the file stdout writes to.
    close(2);
    if (dup(1));
  }    

  // Check on runtime requirements.
  runtime_initialize ();
  // Initialize the xml library.
  xmlInitParser();
  // Initialize the http libraries.
  xmlNanoHTTPInit();
  // Maintenance system.
  maintenance_initialize ();
  // Settings object. 
  Settings mysettings(true);
  settings = &mysettings;
  // LocalizedBooks object.
  BookLocalizations mybooklocalizations(0);
  booklocalizations = &mybooklocalizations;
  // Versifications object.
  Versifications myversifications(0);
  versifications = &myversifications;
  // Verse mappings object.
  Mappings mymappings(0);
  mappings = &mymappings;
  // Styles object.
  Styles mystyles(0);
  styles = &mystyles;
  // Version control object.
  VCS myvcs (0);
  vcs = &myvcs;
  // URLTransport object.
  urltransport = new URLTransport (0);
  /*
  We used a trick to get Bibledit to operate as a true activity on OLPC. 
  The problem is that any regular X11 program that is started, 
  eg by os.system in an activity.py, appears as an extra icon in the "wheel" on the home screen, 
  and sugar doesn't see it as being connected in any way with the original activity that started it. 
  It will also have a generic icon.
  So I decided to use GTK's cross-process embedding (GtkSocket/GtkPlug) 
  to make the Bibledit window be a pane inside the Python activity's window. 
  This uses the XEmbed protocol behind the scenes, and requires a window ID to be passed to Bibledit. 
  You can then quit Bibledit from the File menu, and the activity goes away, 
  or you can Stop the activity and Bibledit goes away.
  I changed Bibledit to accept a --xembed argument with a window ID. 
  It that argument is present, it then creates a plug for its main window instead of a normal top-level window.   
  */
  // Accelerators.
  accelerator_group = gtk_accel_group_new();
  // Window.
  unsigned long xembed = 0;
  if (argc > 2 && strcmp(argv[1], "--xembed") == 0) {
    xembed = strtoul(argv[2], &argv[2], 0);
    if (*argv[2] != 0) {
      fprintf(stderr, "Bad xembed value: %s\n", argv[2]);
      return 0;
    }
  }
  // Upgrade data.
  upgrade();
  // Window icon fallback.
  gtk_window_set_default_icon_from_file(gw_build_filename(directories_get_package_data(), "bibledit.xpm").c_str(), NULL);
  // Start the gui.
  MainWindow mainwindow(xembed, accelerator_group);
  gtk_main();
  // Destroy the accelerator group.
  g_object_unref(G_OBJECT(accelerator_group));
  // Clean up XML library.
  xmlCleanupParser();
  // Clean up http libraries.
  xmlNanoHTTPCleanup();
  // Destroy URL transporter.
  delete urltransport;
  // Quit.
  return 0;
}



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
#include "mainwindow.h"
#include "directories.h"
#include "utilities.h"
#include "gwrappers.h"
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
#include "urltransport.h"
#include "runtime.h"
#include "vcs.h"
#include "books.h"
#include <new>
#include <libintl.h>
#include <glib.h>
#include <glib/gi18n.h>

directories *Directories;
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
  // Internationalization: initialize gettext
  bindtextdomain(GETTEXT_PACKAGE, BIBLEDIT_LOCALEDIR);
  bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
  textdomain(GETTEXT_PACKAGE);

  // Initialize g threads.
  // g_thread_init has been deprecated since version 2.32 and should not be used in newly-written code. 
  // This function is no longer necessary. 
  // The GLib threading system is automatically initialized at the start of your program.
  // Initialize g types.
  // g_type_init has been deprecated since version 2.36.
  // The type system is now initialised automatically.
  // g_type_init();
  // Initialize GTK
  gtk_init(&argc, &argv);

  // Create a new directories 'factory' and initialize it with argv[0]
  Directories = new directories(argv[0]);

  // Check whether it is fine to start the program.
  // If not, quit the program normally.
  if (!check_bibledit_startup_okay(argc, argv)) {
    return 0;
  }

  books_init(); // TEMP - MAP

  Directories->check_structure(); // Check on required directory structure.
  Directories->find_utilities();  // Find core utilities like copy, rm, etc.

  // Move logfile for shutdown program.
  move_log_file (lftShutdown);
  move_log_file (lftSettings);

  // Redirect stdout and stderr to file.
  {
    move_log_file (lftMain);
    // When a file is opened it is always allocated the lowest available file 
    // descriptor. Therefore the following commands cause stdout to be 
    // redirected to the logfile.
    close(1);
    creat (log_file_name (lftMain, false).c_str(), 0666); 
    // The dup() routine makes a duplicate file descriptor for an already opened 
    // file using the first available file descriptor. Therefore the following 
    // commands cause stderr to be redirected to the file stdout writes to.
    close(2);
    if (dup(1));
  }

#ifdef WIN32
  gw_message("WIN32 WAS defined in this build");
#else
  gw_message("WIN32 was NOT defined in this build");
#endif
    
  Directories->print(); // called after the stdout/stderr redirects above
  
  // Check on runtime requirements.
  runtime_initialize ();
  // Initialize the xml library.
  xmlInitParser();
  // Initialize the http libraries.
  xmlNanoHTTPInit();
  // Maintenance system.
  maintenance_initialize ();
  // Settings object. 
  settings = new Settings(true);
  // LocalizedBooks object.
  booklocalizations = new BookLocalizations(0);
  gw_message("Finished booklocalizations");
  // Versifications object.
  versifications = new Versifications(0);
  // Verse mappings object.
  mappings = new Mappings(0);
  // Styles object.
  styles = new Styles(0);
  // Version control object.
  vcs = new VCS(0);
  // URLTransport object.
  urltransport = new URLTransport(0);
  gw_message("Finished URLTransport");
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
  If that argument is present, it then creates a plug for its main window instead of a normal top-level window.   
  */
  // Accelerators.
  accelerator_group = gtk_accel_group_new();
  // Window.
  unsigned long xembed = 0;
  if (argc > 2 && strcmp(argv[1], "--xembed") == 0) {
    xembed = strtoul(argv[2], &argv[2], 0);
    if (*argv[2] != 0) {
      /// Translators: please leave the '%s' as is.
      fprintf(stderr, _("Bad xembed value: %s\n"), argv[2]);
      return 0;
    }
  }
  // Upgrade data.
  upgrade();
  // Window icon fallback.
  gtk_window_set_default_icon_from_file(gw_build_filename(Directories->get_package_data(), "bibledit.xpm").c_str(), NULL);
  // Start the gui.
  MainWindow *mainwindow = new MainWindow(xembed, accelerator_group, settings, urltransport, vcs);
  gw_message("Finished initialization");
  gtk_main();
  delete mainwindow;

  // Destroy the accelerator group.
  g_object_unref(G_OBJECT(accelerator_group));
  // Clean up XML library.
  xmlCleanupParser();
  // Clean up http libraries.
  xmlNanoHTTPCleanup();
  //-------------------------------------------------------
  // Destroy global objects. For now, this order is
  // important. It shouldn't be, but it is. MAP 1/9/2015.
  // TO DO: Have ProjectConfiguration and Settings write out
  // their status using a method we call, NOT relying on
  // a destructor to do the work of saving important state.
  //-------------------------------------------------------
  delete urltransport;
  delete vcs;
  delete styles;
  delete mappings;
  delete versifications;
  delete booklocalizations;
  delete settings;
  delete Directories; // must be last, because above rely on it
  // Quit.
  return 0;
}


/*

Gtk3
http://developer.gnome.org/gtk3/stable/gtk-migrating-2-to-3.html
http://developer.gnome.org/gtk/2.24/

Do not include individual headers: Done.
Do not use deprecated symbols: Done.
Use accessor functions instead of direct access: Done.
Replace GDK_<keyname> with GDK_KEY_<keyname>: Done.
Use GIO for launching applications: Done.
Use cairo for drawing: Done.

Gtk3 is available on Windows.

Switching to Gtk3 needs the webkitgtk for gtk3 also.

pkg-config gtk+-3.0 --modversion

*/

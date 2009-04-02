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
#include "exception.h"
#include "mainwindow.h"
#include "directories.h"
#include "utilities.h"
#include "gwrappers.h"
#include "directories.h"
#include "dialogshowscript.h"
#include "shell.h"
#include "user.h"
#include "shutdown.h"
#include "gtkwrappers.h"
#include "upgrade.h"
#include "unixwrappers.h"
#include "settings.h"
#include "constants.h"
#include "ipc.h"
#include "localizedbooks.h"
#include "versifications.h"
#include "mappings.h"
#include "ot-quotations.h"
#include "styles.h"
#include <curl/curl.h>
#include "htmlcache.h"
#include <libxml/xmlreader.h>

Settings *settings;
InterprocessCommunication *ipc;
BookLocalizations *booklocalizations;
Versifications *versifications;
Mappings *mappings;
Styles *styles;
CURL *curl;
HtmlCache *htmlcache;
GtkAccelGroup *accelerator_group;

int main(int argc, char *argv[])
{
  // Unhandled exception handler.
  set_terminate(terminator);
  // Handler for unexpected exceptions.
  set_unexpected(my_unexpected);
  // Do not allow to run as root.
  if (runs_as_root())
    return 1;
  // Initialize the xml library.
  xmlInitParser();
  // Check on default data structure.
  directories_check_structure();
  // Settings object. 
  // Only save the settings on exit if there is no scripting.
  Settings mysettings(argc == 1);
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
  // Curl initialization.
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  // Html cache.
  htmlcache = new HtmlCache(0);
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
  g_type_init();
  accelerator_group = gtk_accel_group_new();
  // Window.
  unsigned long xembed = 0;
  if (argc > 2 && strcmp(argv[1], "--xembed") == 0) {
    xembed = strtoul(argv[2], &argv[2], 0);
    if (*argv[2] != 0) {
      fprintf(stderr, "Bad xembed value: %s\n", argv[2]);
      return 1;
    }
  }
  // We need thread support.
  g_thread_init(NULL);
  // Interprocess Communication object.
  InterprocessCommunication myipc(ipcstBibleditBin);
  ipc = &myipc;
  // Initialize GTK
  gtk_init(&argc, &argv);
  // Upgrade data.
  upgrade(true);
  // Window icon fallback.
  gtk_window_set_default_icon_from_file(gw_build_filename(directories_get_package_data(), "bibledit.xpm").c_str(), NULL);
  // Start the gui.
  MainWindow mainwindow(xembed, accelerator_group);
  gtk_main();
  // Cleanup http fetching. Note: the html cache must be deleted before curl is cleaned up. 
  // If this is done in the wrong order, and a fetch is going on at this moment, then bibledit crashes on shutdown.
  delete htmlcache;
  curl_easy_cleanup(curl);
  // Destroy the accelerator group.
  g_object_unref(G_OBJECT(accelerator_group));
  // Clean xml library up.
  xmlCleanupParser();
  // Quit.
  return 0;
}

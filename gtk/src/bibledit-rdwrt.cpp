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
#include "gwrappers.h"
#include "settings.h"
#include "localizedbooks.h"
#include "versifications.h"
#include "mappings.h"
#include "styles.h"
#include "urltransport.h"
#include "vcs.h"
#include "readwrite.h"


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
  // Initialize g types.
  g_type_init();

  // Settings object. 
  Settings mysettings(true);
  settings = &mysettings;

  // Bibledit can read from or write to Bible data.
  // Syntax: bibledit-rdwrt -r|-w ...
  bool readdata = false;
  bool writedata = false;
  if (argc > 2) {
    readdata = (strcmp(argv[1], "-r") == 0);
    writedata = (strcmp(argv[1], "-w") == 0);
  }
  if (readdata ^ writedata) {
    // Do the reading or the writing.
    read_write_data (argc, argv, readdata, writedata);
  } else {
    // Nothing to do.
    gw_message ("Nothing was done");
  }

  // Quit.
  return 0;
}



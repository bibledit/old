/*
Copyright (©) 2003-2016 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include <library/bibledit.h>
#include <config/libraries.h>
#include <config/globals.h>
#include <filter/url.h>
#include <filter/string.h>
#include <config/logic.h>
#include <setup/logic.h>
#include <demo/logic.h>


int main (int argc, char **argv)
{
  cout << "Bibledit Data Generator " << config_logic_version () << endl;
  
  if (argc < 2) {
    cerr << "Please pass the document root folder as the first argument" << endl;
    return EXIT_FAILURE;
  }
  config_globals_document_root = argv [1];
  cout << "Document root folder: " << config_globals_document_root << endl;
  if (!file_exists (config_globals_document_root)) {
    cerr << "Please pass an existing document root folder" << endl;
    return EXIT_FAILURE;
  }
  
  if (argc < 3) {
    cerr << "Please pass a command as the second argument" << endl;
    return EXIT_FAILURE;
  }
  string command = argv [2];
  
  if (command == "locale") {
  
    cout << "Generating locale databases from the *.po files in folder locale" << endl;
    setup_generate_locale_databases (true);
    
  } else if (command == "samplebible") {

    cout << "Generating the sample Bible" << endl;
    demo_prepare_sample_bible ();

  } else {
    
    cerr << "This command is unknown" << endl;
    cerr << "The following commands are supported:" << endl;
    cerr << "locale: Generate locale databases from the *.po files in folder locale" << endl;
    cerr << "samplebible: Generate the sample Bible" << endl;
    return EXIT_FAILURE;
    
  }
  
  return EXIT_SUCCESS;
}

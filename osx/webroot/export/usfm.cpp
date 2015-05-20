/*
 Copyright (©) 2003-2015 Teus Benschop.
 
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


#include <export/usfm.h>
#include <export/logic.h>
#include <tasks/logic.h>
#include <database/bibles.h>
#include <database/books.h>
#include <database/logs.h>
#include <database/config/bible.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/roles.h>
#include <filter/text.h>
#include <filter/archive.h>
#include <filter/shell.h>
#include <locale/translate.h>
#include <styles/sheets.h>


void export_usfm (string bible)
{
  Database_Bibles database_bibles;
  
  
  // Root USFM directory, plus info file.
  string usfmDirectory = Export_Logic::USFMdirectory (bible, 2);
  if (!file_exists (usfmDirectory)) filter_url_mkdir (usfmDirectory);
  string infopath = filter_url_create_root_path ("export", "usfm.html");
  string infocontents = filter_url_file_get_contents (infopath);
  infopath = filter_url_create_path (usfmDirectory, "readme.html");
  filter_url_file_put_contents (infopath, infocontents);
  
  
  // USFM directories
  string usfmDirectoryFull = Export_Logic::USFMdirectory (bible, 0);
  if (!file_exists (usfmDirectoryFull)) filter_url_mkdir (usfmDirectoryFull);
  
  
  // Remove possible secured zip file.
  filter_url_unlink (filter_url_create_path (usfmDirectoryFull, "bible.zip"));
  
  
  // Take the USFM from the Bible database.
  // Generate one USFM file per book.
  
  
  vector <int> books = database_bibles.getBooks (bible);
  for (auto book : books) {
    
    
    // The USFM data of the current book.
    string bookUsfmDataFull;
    
    
    // Collect the USFM for all chapters in this book.
    vector <int> chapters = database_bibles.getChapters (bible, book);
    for (auto chapter : chapters) {
      // Get the USFM code for the current chapter.
      string chapter_data = database_bibles.getChapter (bible, book, chapter);
      chapter_data = filter_string_trim (chapter_data);
      // Add the chapter USFM code to the book's USFM code.
      bookUsfmDataFull += chapter_data;
      bookUsfmDataFull += "\n";
    }
    
    
    // The filename for the USFM for this book.
    string filename = Export_Logic::baseBookFileName (book);
    string path = filter_url_create_path (usfmDirectoryFull, filename + ".usfm");
    
    
    // Save.
    filter_url_file_put_contents (path, bookUsfmDataFull);
    
    
  }
  
  
  // Compress USFM files into one zip file.
  string zipfile = filter_url_create_path (usfmDirectoryFull, Export_Logic::baseBookFileName (0) + ".zip");
  filter_url_unlink (zipfile);
  string archive = filter_archive_zip_folder (usfmDirectoryFull);
  filter_url_rename (archive, zipfile);
  
  
  if (Database_Config_Bible::getSecureUsfmExport (bible)) {
    // Securing the full USFM export means that there will be one zip file secured with a password.
    // This zip file contains all exported USFM data.
    // All other files will be removed.
    // It uses the external zip binary.
    vector <string> files = filter_url_scandir (usfmDirectoryFull);
    string basefile = filter_url_basename (zipfile);
    for (auto file : files) {
      if (file != basefile) filter_url_unlink (filter_url_create_path (usfmDirectoryFull, file));
    }
    string password = Database_Config_Bible::getExportPassword (bible);
    string output, error;
    filter_shell_run (usfmDirectoryFull, "zip", {"-P", password, "bible.zip", basefile}, &output, &error);
    filter_url_unlink (zipfile);
  }
  
  
  Database_Logs::log (translate("Exported to USFM") + ": " + bible + " " + translate("All books"), Filter_Roles::translator ());
}

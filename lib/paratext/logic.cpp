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


#include <paratext/logic.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/roles.h>
#include <filter/usfm.h>
#include <pwd.h>
#include <database/books.h>
#include <database/logs.h>
#include <database/config/bible.h>
#include <database/config/general.h>
#include <database/bibles.h>
#include <tasks/logic.h>
#include <locale/translate.h>


string Paratext_Logic::searchProjectsFolder ()
{
  const char *homedir;

  // Try Linux.
  if ((homedir = getenv("HOME")) == NULL) {
    homedir = getpwuid(getuid())->pw_dir;
  }
  if (homedir) {
    vector <string> files = filter_url_scandir (homedir);
    for (auto file : files) {
      if (file.find ("Paratext") != string::npos) {
        return filter_url_create_path (homedir, file);
      }
    }
  }
  
  // Try Windows.
  homedir = "C:\\";
  vector <string> files = filter_url_scandir (homedir);
  for (auto file : files) {
    if (file.find ("Paratext") != string::npos) {
      string path = filter_url_create_path (homedir, file);
      path = filter_string_str_replace ("\\/", "\\", path);
      return path;
    }
  }
  
  // No Paratext projects folder found.
  return "";
}


vector <string> Paratext_Logic::searchProjects (string projects_folder)
{
  vector <string> projects;
  vector <string> folders = filter_url_scandir (projects_folder);
  for (auto folder : folders) {
    string path = filter_url_create_path (projects_folder, folder);
    if (filter_url_is_dir (path)) {
      map <int, string> books = searchBooks (path);
      if (!books.empty ()) projects.push_back (folder);
    }
  }
  return projects;
}


map <int, string> Paratext_Logic::searchBooks (string project_path)
{
  map <int, string> books;
  vector <string> files = filter_url_scandir (project_path);
  for (auto file : files) {
    if (file.find (".BAK") != string::npos) continue;
    if (file.find ("~") != string::npos) continue;
    string path = filter_url_create_path (project_path, file);
    int id = getBook (path);
    if (id) books [id] = file;
  }
  return books;
}


int Paratext_Logic::getBook (string filename)
{
  // A USFM file should not be larger than 4 Mb and not be smaller than 7 bytes.
  int filesize = filter_url_filesize (filename);
  if (filesize < 7) return 0;
  if (filesize > 4000000) return 0;
  
  // Read a small portion of the file for higher speed.
  ifstream fin (filename);
  fin.seekg (0);
  char buffer [128];
  fin.read (buffer, 7);
  buffer [7] = 0;
  string fragment (buffer);

  // Check for "\id "
  if (fragment.find ("\\id ") == string::npos) return 0;
  fragment.erase (0, 4);
  
  // Get book from the USFM id.
  int id = Database_Books::getIdFromUsfm (fragment);
  return id;
}


void Paratext_Logic::setup (string bible, string master)
{
  if (bible.empty ()) {
    Database_Logs::log ("No Bible given for Paratext link setup.");
    return;
  }
  if (master == "bibledit") {
    copyBibledit2Paratext (bible);
    Database_Config_Bible::setParatextCollaborationEnabled (bible, true);
  } else if (master == "paratext") {
    copyParatext2Bibledit (bible);
    Database_Config_Bible::setParatextCollaborationEnabled (bible, true);
  } else {
    Database_Logs::log ("Unknown master copy for Paratext link setup.");
  }
}


void Paratext_Logic::copyBibledit2Paratext (string bible) // Todo
{
  Database_Bibles database_bibles;
  
  Database_Logs::log ("Copying Bible from Bibledit to a Paratext project.");

  string project_folder = projectFolder (bible);

  Database_Logs::log ("Bibledit Bible: " + bible);
  Database_Logs::log ("Paratext project: " + project_folder);

  map <int, string> paratext_books = searchBooks (project_folder);
  
  vector <int> bibledit_books = database_bibles.getBooks (bible);
  for (int book : bibledit_books) {

    string bookname = Database_Books::getEnglishFromId (book);

    string paratext_book = paratext_books [book];

    string usfm;
    vector <int> chapters = database_bibles.getChapters (bible, book);
    for (int chapter : chapters) {
      if (!usfm.empty ()) usfm.append ("\n");
      usfm.append (database_bibles.getChapter (bible, book, chapter));
    }
    
    if (!paratext_book.empty ()) {

      string path = filter_url_create_path (projectFolder (bible), paratext_book);
      Database_Logs::log (bookname + ": " "Storing to:" " " + path);
      filter_url_file_put_contents (path, usfm);
      
      paratext_books [book].clear ();
    
    } else {

      Database_Logs::log (bookname + ": " "It could not be stored because the Paratext project does not have this book." " " "Create it, then retry.");
    
    }

    // Ancestor data needed for future merge.
    ancestor (bible, book, usfm);
  }
  
  for (auto element : paratext_books) {
    string paratext_book = element.second;
    if (paratext_book.empty ()) continue;
    Database_Logs::log (paratext_book + ": " "This Paratext project file was not affected.");
  }
}


void Paratext_Logic::copyParatext2Bibledit (string bible) // Todo
{
  Database_Bibles database_bibles;

  Database_Logs::log ("Copying Paratext project to a Bible in Bibledit.");
  
  string project_folder = projectFolder (bible);
  
  Database_Logs::log ("Paratext project: " + project_folder);
  Database_Logs::log ("Bibledit Bible: " + bible);

  vector <int> bibledit_books = database_bibles.getBooks (bible);

  map <int, string> paratext_books = searchBooks (project_folder);
  for (auto element : paratext_books) {

    int book = element.first;
    string bookname = Database_Books::getEnglishFromId (book);

    string paratext_book = element.second;
    string path = filter_url_create_path (projectFolder (bible), paratext_book);

    Database_Logs::log (bookname + ": " "Scheduling import from:" " " + path);

    // It is easiest to schedule an import task.
    // The task will take care of everything, including recording what to send to the Cloud.
    tasks_logic_queue (IMPORTUSFM, { path, bible });

    // Ancestor data needed for future merge.
    string usfm = filter_url_file_get_contents (path);
    ancestor (bible, book, usfm);
  }
}


string Paratext_Logic::projectFolder (string bible)
{
  return filter_url_create_path (Database_Config_General::getParatextProjectsFolder (), Database_Config_Bible::getParatextProject (bible));
}


void Paratext_Logic::ancestor (string bible, int book, string usfm)
{
  string path = ancestorPath (bible, book);
  filter_url_file_put_contents (path, usfm);
}


string Paratext_Logic::ancestor (string bible, int book)
{
  string path = ancestorPath (bible, book);
  return filter_url_file_get_contents (path);
}


string Paratext_Logic::ancestorPath (string bible, int book)
{
  string path = filter_url_create_root_path ("paratext", "ancestors", bible);
  if (!file_exists (path)) filter_url_mkdir (path);
  if (book) path = filter_url_create_path (path, convert_to_string (book));
  return path;
}


vector <string> Paratext_Logic::enabledBibles () // Todo
{
  vector <string> enabled;
  Database_Bibles database_bibles;
  vector <string> bibles = database_bibles.getBibles ();
  for (auto bible : bibles) {
    if (Database_Config_Bible::getParatextCollaborationEnabled (bible)) {
      enabled.push_back (bible);
    }
  }
  return enabled;
}


void Paratext_Logic::synchronize () // Todo
{
  Database_Bibles database_bibles;

  
  Database_Logs::log (synchronizeStartText (), Filter_Roles::translator ());
  
  
  // The Bibles for which Paratext synchronization has been enabled.
  vector <string> bibles = enabledBibles ();
  for (auto bible : bibles) {
  
    
    // The Paratext project folder for the current Bible.
    string project_folder = projectFolder (bible);
    if (!file_exists (project_folder)) {
      Database_Logs::log ("Cannot find Paratext project folder:" " " + project_folder, Filter_Roles::translator ());
      continue;
    }

    
    string stylesheet = Database_Config_Bible::getExportStylesheet (bible);
    
    
    vector <int> bibledit_books = database_bibles.getBooks (bible);
    map <int, string> paratext_books = searchBooks (project_folder);

    
    for (auto book : bibledit_books) {

      
      // Check whether the book exists in the Paratext project, if not, skip it.
      string paratext_book = paratext_books [book];
      if (paratext_book.empty ()) {
        Database_Logs::log (journalTag (bible, book, -1) + "The Paratext project does not have this book", Filter_Roles::translator ());
        continue;
      }
      

      // Ancestor USFM per chapter.
      map <int, string> ancestor_usfm;
      {
        string usfm = ancestor (bible, book);
        vector <BookChapterData> import = usfm_import (usfm, stylesheet);
        for (auto data : import) {
          ancestor_usfm [data.chapter] = data.data;
        }
      }


      // Paratext USFM per chapter.
      map <int, string> paratext_usfm;
      {
        string path = filter_url_create_path (projectFolder (bible), paratext_book);
        string usfm = filter_url_file_get_contents (path);
        vector <BookChapterData> import = usfm_import (usfm, stylesheet);
        for (auto data : import) {
          paratext_usfm [data.chapter] = data.data;
        }
      }

      
      // Bible_Logic::storeChapter (const string& bible, int book, int chapter, const string& usfm);

      // Store ancestor data for a future merge.
      //string usfm = filter_url_file_get_contents (path);
      //ancestor (bible, book, usfm);

      
    }
    
    
  }
  
  
  Database_Logs::log (synchronizeReadyText (), Filter_Roles::translator ());
}


string Paratext_Logic::synchronizeStartText ()
{
  return translate ("Paratext: Send/receive");
}


string Paratext_Logic::synchronizeReadyText ()
{
  return translate ("Paratext: Up to date");
}


// Create tag for the journal.
// If chapter is negative, it is left out from the tag.
string Paratext_Logic::journalTag (string bible, int book, int chapter)
{
  string bookname = Database_Books::getEnglishFromId (book);
  string project = Database_Config_Bible::getParatextProject (bible);
  string fragment = bible + " <> " + project + " " + bookname;
  if (chapter >= 0) fragment.append (" " + convert_to_string (chapter));
  fragment.append (": ");
  return fragment;
}

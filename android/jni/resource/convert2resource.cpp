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


#include <resource/convert2resource.h>
#include <filter/string.h>
#include <filter/roles.h>
#include <resource/logic.h>
#include <database/bibles.h>
#include <database/usfmresources.h>
#include <database/books.h>
#include <database/logs.h>
#include <locale/translate.h>


void convert_bible_to_resource (string bible)
{
  Database_Bibles database_bibles = Database_Bibles ();
  Database_UsfmResources database_usfmresources = Database_UsfmResources ();
  
  
  Database_Logs::log (translate("Converting Bible to USFM Resource") + ": " + bible, Filter_Roles::manager ());
  
  
  vector <int> books = database_bibles.getBooks (bible);
  for (auto & book : books) {
    string bookname = Database_Books::getEnglishFromId (book);
    Database_Logs::log (bookname, Filter_Roles::manager ());
    vector <int> chapters = database_bibles.getChapters (bible, book);
    for (auto & chapter : chapters) {
      string usfm = database_bibles.getChapter (bible, book, chapter);
      database_usfmresources.storeChapter (bible, book, chapter, usfm);
      database_bibles.deleteChapter (bible, book, chapter);
    }
  }
  database_bibles.deleteBible (bible);
  // Any information about user access to this Bible is left untouched,
  // because this information is useful when the USFM Resource is converted
  // back to a Bible.
  
  
  Database_Logs::log (translate("Completed"), Filter_Roles::manager ());
}

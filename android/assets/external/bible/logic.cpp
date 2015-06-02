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


#include <bible/logic.h>
#include <filter/string.h>
#include <filter/url.h>
#include <database/bibles.h>
#include <database/modifications.h>
#include <database/bibleactions.h>
#include <config/logic.h>
#include <client/logic.h>


void Bible_Logic::storeChapter (const string& bible, int book, int chapter, const string& usfm)
{
  Database_Bibles database_bibles = Database_Bibles ();

  // Record data of the chapter to be stored prior to storing the new version.
  if (client_logic_client_enabled ()) {

    // Client stores Bible action.
    string oldusfm = database_bibles.getChapter (bible, book, chapter);
    Database_BibleActions database_bibleactions = Database_BibleActions ();
    database_bibleactions.record (bible, book, chapter, oldusfm);

  } else {

    // Server stores diff data.
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.storeTeamDiff (bible, book, chapter);

  }

  // Store the chapter in the database.
  database_bibles.storeChapter (bible, book, chapter, usfm);
}


void Bible_Logic::deleteChapter (const string& bible, int book, int chapter)
{
  Database_Bibles database_bibles = Database_Bibles ();

  // Record data of the chapter to be deleted prior to deletion.
  if (client_logic_client_enabled ()) {

    // Client stores Bible action.
    string usfm = database_bibles.getChapter (bible, book, chapter);
    Database_BibleActions database_bibleactions = Database_BibleActions ();
    database_bibleactions.record (bible, book, chapter, usfm);

  } else {

    // Server stores diff data.
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.storeTeamDiff (bible, book, chapter);

  }

  // Delete the chapter from the database.
  database_bibles.deleteChapter (bible, book, chapter);
}


void Bible_Logic::deleteBook (const string& bible, int book)
{
  Database_Bibles database_bibles = Database_Bibles ();

  // Record data of the book to be deleted prior to deletion.
  if (client_logic_client_enabled ()) {

    // Client stores Bible actions.
    Database_BibleActions database_bibleactions = Database_BibleActions ();
    vector <int> chapters = database_bibles.getChapters (bible, book);
    for (auto & chapter : chapters) {
      string usfm = database_bibles.getChapter (bible, book, chapter);
      database_bibleactions.record (bible, book, chapter, usfm);
    }

  } else {

    // Server stores diff data.
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.storeTeamDiffBook (bible, book);

  }

  // Delete the book from the database.
  database_bibles.deleteBook (bible, book);
}


void Bible_Logic::deleteBible (const string& bible)
{
  Database_Bibles database_bibles = Database_Bibles ();

  // Record data of the Bible to be deleted prior to deletion.
  if (client_logic_client_enabled ()) {

    // Client stores Bible actions.
    Database_BibleActions database_bibleactions = Database_BibleActions ();
    vector <int> books = database_bibles.getBooks (bible);
    for (auto book : books) {
      vector <int> chapters = database_bibles.getChapters (bible, book);
      for (auto chapter : chapters) {
        string usfm = database_bibles.getChapter (bible, book, chapter);
        database_bibleactions.record (bible, book, chapter, usfm);
      }
    }

  } else {

    // Server stores diff data.
    Database_Modifications database_modifications = Database_Modifications ();
    database_modifications.storeTeamDiffBible (bible);

  }

  // Delete the Bible from the database.
  database_bibles.deleteBible (bible);
}

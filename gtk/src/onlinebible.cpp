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
#include "onlinebible.h"
#include "utilities.h"
#include "clean.h"
#include "usfmtools.h"
#include "bible.h"
#include "books.h"
#include "gwrappers.h"
#include "directories.h"
#include "unixwrappers.h"
#include "tiny_utilities.h"
#include "dialogradiobutton.h"
#include "localizedbooks.h"
#include "categorize.h"
#include "projectutils.h"
#include "progresswindow.h"
#include "shell.h"
#include "versification.h"


bool online_bible_is_running ()
// Returns true if the Online Bible runs on Wine.
{
  vector <ustring> processes = list_processes ();
  for (unsigned int i = 0; i < processes.size(); i++) {
    ustring process = lowerCase (processes[i]);
    if (g_str_has_suffix (process.c_str(), "olb.exe"))
      return true;
  }
  return false;
}


bool online_bible_ok_reply_validate (ustring& reply)
// Normally a reply obtained from the Online Bible through the Bibledit Windows
// Outpost starts with OK Reply:. This function validates it,
// and removes the bit.
{
  if (reply.find ("OK Reply: ") == 0) {
    reply.erase (0, 10);
    return true;
  }
  return false;
}


void import_online_bible (WindowsOutpost * windows_outpost, const ustring& online_bible, const ustring& bibledit_bible, vector <ustring>& summary_messages)
{
  ustring response;

  ProgressWindow progresswindow ("Import", true);
  
  // Get the books in this Online Bible.
  vector <ustring> books;
  response = windows_outpost->OnlineBibleDirectCommandResponseGet("GetBookAbbreviationsForVersion " + online_bible);
  if (online_bible_ok_reply_validate (response)) {
    ParseLine parseline (response);
    books = parseline.lines;
  }
  
  // Bail out if there were no books.
  if (books.empty()) {
    summary_messages.push_back ("Could not get the names of the books");
    return;
  }

  // Go through the books.
  for (unsigned int bk = 0; bk < books.size(); bk++) {
    
    // Get the Online Bible book, and the Bibledit equivalent.
    ustring book_olb = books[bk];
    unsigned int book_id = books_online_bible_to_id (book_olb);

    // Progress.
    progresswindow.set_text (book_olb);
        
    // If it could not find a matching Bibledit equivalent, skip this book.
    if (book_id == 0) {
      summary_messages.push_back ("Could not find a matching book for " + book_olb);
      continue;
    }
    /*
     * If in the future it becomes a problem to determine which book name
     * or abbreviation of the OLB corresponds with which book in Bibledit, 
     * we may use the trick of counting the available books. 
     * If there's the OT number, then they are taken as OT books in the
     * order given. And if there are 66 books, then it is the whole Bible. 
     * And so on.
     */

    // Store chapter 0.
    {
      vector <ustring> lines;
      versification_create_chapter_template("", book_id, 0, lines);
      CategorizeChapterVerse ccv(lines);
      project_store_chapter(bibledit_bible, book_id, ccv);
    }

    // Get the chapter count in this book.
    unsigned int chapter_count = 0;
    response = windows_outpost->OnlineBibleDirectCommandResponseGet("GetChapterCountForBook " + online_bible + " " + book_olb);
    if (online_bible_ok_reply_validate (response)) {
      chapter_count = convert_to_int (response);
    }

    // Go through the chapters.
    progresswindow.set_iterate (0, 1, chapter_count);
    for (unsigned int ch = 1; ch <= chapter_count; ch++) {

      // Progress update.
      progresswindow.iterate ();
      progresswindow.set_text (book_olb + " " + convert_to_string (ch));

      // If the chapter has been imported already, skip it.
      {
        vector <ustring> lines = project_retrieve_chapter (bibledit_bible, book_id, ch);
        if (!lines.empty()) {
          continue;
        }
      }
      
      // Storage for chapter contents, plus first bits.
      vector <ustring> chapter_contents;
      chapter_contents.push_back ("\\c " + convert_to_string (ch));
      chapter_contents.push_back ("\\p");
      
      // Get the verse count in the chapter.
      unsigned int verse_count = 0;
      response = windows_outpost->OnlineBibleDirectCommandResponseGet("GetVerseCountForChapter " + online_bible + " " + book_olb + " " + convert_to_string (ch));
      if (online_bible_ok_reply_validate (response)) {
        verse_count = convert_to_int (response);
      }

      // Go through the verses.
      for (unsigned int vs = 1; vs <= verse_count; vs++) {

        // Progress update.
        progresswindow.set_text (book_olb + " " + convert_to_string (ch) + ":" + convert_to_string (vs));

        // Did the user cancel the import? (Could be, because it takes a huge amount of time).
        if (progresswindow.cancel) {
          summary_messages.push_back ("The rest of the import was cancelled");
          summary_messages.push_back ("What has been imported already remains");
          return;
        }

        // Get and store the verse text.
        response = windows_outpost->OnlineBibleDirectCommandResponseGet("GetVerseText " + online_bible + " \"" + book_olb + " " + convert_to_string (ch) + ":" + convert_to_string (vs) + "\"");
        if (!online_bible_ok_reply_validate (response)) {
          response = "Verse text could not be imported";
        }
        chapter_contents.push_back ("\\v " + convert_to_string (vs) + " " + response);
      }
 
      // Store the whole chapter.
      CategorizeChapterVerse ccv(chapter_contents);
      project_store_chapter(bibledit_bible, book_id, ccv);
    }
  }
}



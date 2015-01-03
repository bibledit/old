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


#include <navigation/passage.h>
#include <filter/string.h>
#include <filter/usfm.h>
#include <database/versifications.h>
#include <database/navigation.h>
#include <database/books.h>
#include <webserver/request.h>
#include <locale/translate.h>
#include <ipc/focus.h>


string Navigation_Passage::getContainer ()
{
  return "<div id=\"versepickerwrapper\"></div>";
}


string Navigation_Passage::getNavigator (void * webserver_request, string bible) // Todo
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  Database_Navigation database_navigation = Database_Navigation ();
  
  string user = request->session_logic()->currentUser ();
  
  vector <string> fragments;
  
  // Links to go back and forward are grayed out or active depending on available passages to go to.
  fragments.push_back ("<div id=\"backforward\">");
  if (database_navigation.previousExists (user)) {
    fragments.push_back ("<a id=\"navigateback\" href=\"navigateback\" title=\"" + gettext("Back") + "\">↶</a>");
  } else {
    fragments.push_back ("<span class=\"grayedout\">↶</span>");
  }
  fragments.push_back ("");
  if (database_navigation.nextExists (user)) {
    fragments.push_back ("<a id=\"navigateforward\" href=\"navigateforward\" title=\"" + gettext("Forward") + "\">↷</a>");
  } else {
    fragments.push_back ("<span class=\"grayedout\">↷</span>");
  }
  fragments.push_back ("</div>");
  
  int book = Ipc_Focus::getBook (request);
  
  // The book should exist in the Bible.
  if (bible != "") {
    vector <int> books = request->database_bibles()->getBooks (bible);
    if (find (books.begin(), books.end(), book) == books.end()) {
      if (!books.empty ()) book = books [0];
      else book = 0;
    }
  }
  
  string bookName = Database_Books::getEnglishFromId (book);
  
  int chapter = Ipc_Focus::getChapter (request);
  
  // The chapter should exist in the book.
  if (bible != "") {
    vector <int> chapters = request->database_bibles()->getChapters (bible, book);
    if (find (chapters.begin(), chapters.end(), chapter) == chapters.end()) {
      if (!chapters.empty()) chapter = chapters [0];
      else chapter = 1;
    }
  }
  
  int verse = Ipc_Focus::getVerse (request);
  
  fragments.push_back ("<div id=\"versepicker\">");
  
  fragments.push_back ("<input name=\"selectpassage\" id=\"selectpassage\" type=\"text\" value=\"" + bookName + " " + convert_to_string (chapter) + ":" + convert_to_string (verse) + "\" size=\"14\" />");
  fragments.push_back ("<input name=\"submitpassage\" id=\"submitpassage\" type=\"submit\" value=\"" + gettext("Go") + "\" />");
  
  fragments.push_back ("<div id=\"handpicker\">");
  
  fragments.push_back ("<select id=\"booklist\" size=15>");
  fragments.push_back (getBooksFragment (request, bible));
  fragments.push_back ("</select>");
  
  fragments.push_back ("<select id=\"chapterlist\" size=15>");
  fragments.push_back (getChaptersFragment (request, bible, book, chapter));
  fragments.push_back ("</select>");
  
  fragments.push_back ("<select id=\"verselist\" size=15 multiple>");
  fragments.push_back (getVersesFragment (request, bible, book, chapter, verse));
  
  fragments.push_back ("</select>");
  
  fragments.push_back ("</div>");
  
  fragments.push_back ("</div>");
  
  // The result.
  return filter_string_implode (fragments, "\n");
}


string Navigation_Passage::getBooksFragment (void * webserver_request, string bible)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  int activeBook = Ipc_Focus::getBook (request);
  // Take standard books in case of no Bible.
  vector <int> books;
  if (bible == "") {
    books = Database_Books::getIDs ();
  } else {
    books = filter_passage_get_ordered_books (bible);
  }
  string html = "";
  for (auto book : books) {
    string bookName = Database_Books::getEnglishFromId (book);
    string selected;
    if (book == activeBook) selected = " selected";
    html += "<option" + selected + ">" + bookName + "</option>";
  }
  return html;
}


string Navigation_Passage::getChaptersFragment (void * webserver_request, string bible, int book, int chapter)
{
  vector <int> chapters;
  if (bible == "") {
    Database_Versifications database_versifications = Database_Versifications ();
    chapters = database_versifications.getChapters ("English", book, true);
  } else {
    Webserver_Request * request = (Webserver_Request *) webserver_request;
    chapters = request->database_bibles()->getChapters (bible, book);
  }
  string html = "";
  for (auto ch : chapters) {
    string selected = "";
    if (ch == chapter) selected = " selected";
    html += "<option" + selected + ">" + convert_to_string (ch) + "</option>";
  }
  return html;
}


string Navigation_Passage::getVersesFragment (void * webserver_request, string bible, int book, int chapter, int verse)
{
  vector <int> verses;
  if (bible == "") {
    Database_Versifications database_versifications = Database_Versifications ();
    verses = database_versifications.getVerses ("English", book, chapter);
  } else {
    Webserver_Request * request = (Webserver_Request *) webserver_request;
    verses = usfm_get_verse_numbers (request->database_bibles()->getChapter (bible, book, chapter));
  }
  string html = "";
  for (auto vs : verses) {
    string selected = "";
    if (verse == vs) selected = " selected";
    html += "<option" + selected + ">" + convert_to_string (vs) +  "</option>";
  }
  return html;
}


string Navigation_Passage::code (string bible, bool header)
{
  string code = "";
  if (header) {
    code += "<script type=\"text/javascript\" src=\"/navigation/passage.js\"></script>";
    code += "\n";
  }
  code += "<script type=\"text/javascript\">";
  code += "\n";
  code += "navigationBible = \"" + bible + "\";";
  code += "\n";
  code += "</script>";
  code += "\n";
  return code;
}


void Navigation_Passage::setBookChapterVerse (void * webserver_request, int book, int chapter, int verse)
{
  Ipc_Focus::set (webserver_request, book, chapter, verse);
  recordHistory (webserver_request, book, chapter, verse);
}


void Navigation_Passage::setPassage (void * webserver_request, string bible, string passage)
{
  int currentBook = Ipc_Focus::getBook (webserver_request);
  int currentChapter = Ipc_Focus::getChapter (webserver_request);
  int currentVerse = Ipc_Focus::getVerse (webserver_request);
  passage = filter_string_trim (passage);
  Passage passage_to_set;
  if ((passage == "") || (passage == "+")) {
    passage_to_set = Navigation_Passage::getNextVerse (webserver_request, bible, currentBook, currentChapter, currentVerse);
  } else if (passage == "-") {
    passage_to_set = Navigation_Passage::getPreviousVerse (webserver_request, bible, currentBook, currentChapter, currentVerse);
  } else {
    Passage inputpassage = Passage ("", currentBook, currentChapter, convert_to_string (currentVerse));
    passage_to_set = filter_passage_interpret_passage (inputpassage, passage);
  }
  if (passage_to_set.book != 0) {
    Ipc_Focus::set (webserver_request, passage_to_set.book, passage_to_set.chapter, convert_to_int (passage_to_set.verse));
    Navigation_Passage::recordHistory (webserver_request, passage_to_set.book, passage_to_set.chapter, convert_to_int (passage_to_set.verse));
  }
}


Passage Navigation_Passage::getNextVerse (void * webserver_request, string bible, int book, int chapter, int verse)
{
  verse++;
  if (bible != "") {
    Webserver_Request * request = (Webserver_Request *) webserver_request;
    vector <int> verses = usfm_get_verse_numbers (request->database_bibles()->getChapter (bible, book, chapter));
    if (find (verses.begin(), verses.end(), verse) == verses.end()) {
      if (!verses.empty()) verse = verses.back ();
    }
  }
  Passage passage = Passage ("", book, chapter, convert_to_string (verse));
  return passage;
}


Passage Navigation_Passage::getPreviousVerse (void * webserver_request, string bible, int book, int chapter, int verse)
{
  verse--;
  if (bible != "") {
    Webserver_Request * request = (Webserver_Request *) webserver_request;
    vector <int> verses = usfm_get_verse_numbers (request->database_bibles()->getChapter (bible, book, chapter));
    if (find (verses.begin(), verses.end(), verse) == verses.end()) {
      if (!verses.empty ()) verse = verses [0];
    }
  }
  Passage passage = Passage ("", book, chapter, convert_to_string (verse));
  return passage;
}


void Navigation_Passage::gotoNextVerse (void * webserver_request, string bible)
{
  int currentBook = Ipc_Focus::getBook (webserver_request);
  int currentChapter = Ipc_Focus::getChapter (webserver_request);
  int currentVerse = Ipc_Focus::getVerse (webserver_request);
  Passage passage = Navigation_Passage::getNextVerse (webserver_request, bible, currentBook, currentChapter, currentVerse);
  if (passage.book != 0) {
    Ipc_Focus::set (webserver_request, passage.book, passage.chapter, convert_to_int (passage.verse));
    Navigation_Passage::recordHistory (webserver_request, passage.book, passage.chapter, convert_to_int (passage.verse));
  }
}


void Navigation_Passage::gotoPreviousVerse (void * webserver_request, string bible)
{
  int currentBook = Ipc_Focus::getBook (webserver_request);
  int currentChapter = Ipc_Focus::getChapter (webserver_request);
  int currentVerse = Ipc_Focus::getVerse (webserver_request);
  Passage passage = Navigation_Passage::getPreviousVerse (webserver_request, bible, currentBook, currentChapter, currentVerse);
  if (passage.book != 0) {
    Ipc_Focus::set (webserver_request, passage.book, passage.chapter, convert_to_int (passage.verse));
    Navigation_Passage::recordHistory (webserver_request, passage.book, passage.chapter, convert_to_int (passage.verse));
  }
}


void Navigation_Passage::recordHistory (void * webserver_request, int book, int chapter, int verse)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  string user = request->session_logic()->currentUser ();
  Database_Navigation database_navigation = Database_Navigation ();
  database_navigation.record (filter_string_date_seconds_since_epoch (), user, book, chapter, verse);
}


void Navigation_Passage::goBack (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  Database_Navigation database_navigation = Database_Navigation ();
  string user = request->session_logic()->currentUser ();
  Passage passage = database_navigation.getPrevious (user);
  if (passage.book) {
    Ipc_Focus::set (webserver_request, passage.book, passage.chapter, convert_to_int (passage.verse));
  }
}


void Navigation_Passage::goForward (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  Database_Navigation database_navigation = Database_Navigation ();
  string user = request->session_logic()->currentUser ();
  Passage passage = database_navigation.getNext (user);
  if (passage.book) {
    Ipc_Focus::set (webserver_request, passage.book, passage.chapter, convert_to_int (passage.verse));
  }
}

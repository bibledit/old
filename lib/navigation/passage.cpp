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
#include <webserver/request.h>


string Navigation_Passage::getContainer ()
{
  return "<div id=\"versepickerwrapper\"></div>";
}


string Navigation_Passage::getNavigator (string bible)
{
  /*
  $ipc_focus = Ipc_Focus::getInstance(); // Todo port as dependency.
  $database_books = Database_Books::getInstance ();
  $database_bibles = Database_Bibles::getInstance ();
  $database_navigation = Database_Navigation::getInstance ();
  $session_logic = Session_Logic::getInstance ();
  
  $user = $session_logic->currentUser ();
  
  $fragments = array ();
  
  // Links to go back and forward are grayed out or active depending on available passages to go to.
  $fragments [] = '<div id="backforward">';
  if ($database_navigation->previousExists ($user)) {
    $fragments [] = '<a id="navigateback" href="navigateback" title="' . gettext("Back") . '">↶</a>';
  } else {
    $fragments [] = '<span class="grayedout">↶</span>';
  }
  $fragments [] = "";
  if ($database_navigation->nextExists ($user)) {
    $fragments [] = '<a id="navigateforward" href="navigateforward" title="' . gettext("Forward") . '">↷</a>';
  } else {
    $fragments [] = '<span class="grayedout">↷</span>';
  }
  $fragments [] = '</div>';
  
  $book = $ipc_focus->getBook ();
  
  // The book should exist in the Bible.
  if ($bible != "") {
    $books = $database_bibles->getBooks ($bible);
    if (!in_array ($book, $books)) {
      if (count ($books) > 0) $book = $books [0];
      else $book = 0;
    }
  }
  
  $bookName = $database_books->getEnglishFromId ($book);
  
  $chapter = $ipc_focus->getChapter ();
  
  // The chapter should exist in the book.
  if ($bible != "") {
    $chapters = $database_bibles->getChapters ($bible, $book);
    if (!in_array ($chapter, $chapters)) {
      if (count ($chapters) > 0) $chapter = $chapters [0];
      else $chapter = 1;
    }
  }
  
  $verse = $ipc_focus->getVerse ();
  
  $fragments [] = '<div id="versepicker">';
  
  $fragments [] = '<input name="selectpassage" id="selectpassage" type="text" value="' . "$bookName $chapter:$verse" . '" size="14" />';
  $fragments [] = '<input name="submitpassage" id="submitpassage" type="submit" value="' . gettext("Go") . '" />';
  
  $fragments [] = '<div id="handpicker">';
  
  $fragments [] = '<select id="booklist" size=15>';
  $fragments [] = self::getBooksFragment ($bible);
  $fragments [] = '</select>';
  
  $fragments [] = '<select id="chapterlist" size=15>';
  $fragments [] = self::getChaptersFragment ($bible, $book, $chapter);
  $fragments [] = '</select>';
  
  $fragments [] = '<select id="verselist" size=15 multiple>';
  $fragments [] = self::getVersesFragment ($bible, $book, $chapter, $verse);
  
  $fragments [] = '</select>';
  
  $fragments [] = '</div>';
  
  $fragments [] = '</div>';
  
  // The result.
  return implode ("\n", $fragments);
  */
  return "";
}


string Navigation_Passage::getBooksFragment (string bible)
{
  /* C++Port
  $database_bibles = Database_Bibles::getInstance ();
  $database_books = Database_Books::getInstance ();
  $ipc_focus = Ipc_Focus::getInstance(); // Todo port it first.
  $activeBook = $ipc_focus->getBook ();
  // Take standard books in case of no Bible.
  if ($bible == "") {
    $books = $database_books->getIDs ();
  } else {
    $books = filter_passage_get_ordered_books ($bible);
  }
  $html = "";
  for ($books as $book) {
    $bookName = $database_books->getEnglishFromId ($book);
    $selected = "";
    if ($book == $activeBook) $selected = " selected";
    $html += "<option$selected>" . $bookName . "</option>";
  }
  return $html;
  */
  return "";
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


void Navigation_Passage::setBookChapterVerse (int book, int chapter, int verse)
{
  /* Todo port dependency.
  $ipc_focus = Ipc_Focus::getInstance();
  $ipc_focus->set ($book, $chapter, $verse);
  recordHistory (book, chapter, verse);
   */
}


void Navigation_Passage::setPassage (string bible, Passage & passage)
{
  /* Todo port dependencies.
  $ipc_focus = Ipc_Focus::getInstance();
  $currentBook = $ipc_focus->getBook ();
  $currentChapter = $ipc_focus->getChapter ();
  $currentVerse = $ipc_focus->getVerse ();
  $passage = filter_string_trim ($passage);
  if (($passage == "") || ($passage == "+")) {
    $passage = Navigation_Passage::getNextVerse ($bible, $currentBook, $currentChapter, $currentVerse);
  } else if ($passage == "-") {
    $passage = Navigation_Passage::getPreviousVerse ($bible, $currentBook, $currentChapter, $currentVerse);
  } else {
    $passage = filter_passage_interpret_passage (array ($currentBook, $currentChapter, $currentVerse), $passage);
  }
  if ($passage[0] != 0) {
    $ipc_focus->set ($passage [0], $passage [1], $passage [2]);
    Navigation_Passage::recordHistory ($passage [0], $passage [1], $passage [2]);
    
  }
  */
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


void Navigation_Passage::gotoNextVerse (string bible)
{
  /* Todo port dependency.
  $ipc_focus = Ipc_Focus::getInstance();
  $currentBook = $ipc_focus->getBook ();
  $currentChapter = $ipc_focus->getChapter ();
  $currentVerse = $ipc_focus->getVerse ();
  $passage = Navigation_Passage::getNextVerse ($bible, $currentBook, $currentChapter, $currentVerse);
  if ($passage[0] != 0) {
    $ipc_focus->set ($passage [0], $passage [1], $passage [2]);
    Navigation_Passage::recordHistory ($passage [0], $passage [1], $passage [2]);
  }
  */
}


void Navigation_Passage::gotoPreviousVerse (string bible)
{
  /* Todo
  $ipc_focus = Ipc_Focus::getInstance();
  $currentBook = $ipc_focus->getBook ();
  $currentChapter = $ipc_focus->getChapter ();
  $currentVerse = $ipc_focus->getVerse ();
  $passage = Navigation_Passage::getPreviousVerse ($bible, $currentBook, $currentChapter, $currentVerse);
  if ($passage[0] != 0) {
    $ipc_focus->set ($passage [0], $passage [1], $passage [2]);
    Navigation_Passage::recordHistory ($passage [0], $passage [1], $passage [2]);
  }
  */
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
    // Todo port $ipc_focus = Ipc_Focus::getInstance();
    // $ipc_focus->set ($passage [0], $passage [1], $passage [2]);
  }
}


void Navigation_Passage::goForward (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  Database_Navigation database_navigation = Database_Navigation ();
  string user = request->session_logic()->currentUser ();
  Passage passage = database_navigation.getNext (user);
  if (passage.book) {
    // Todo port dependency $ipc_focus = Ipc_Focus::getInstance();
    // $ipc_focus->set ($passage [0], $passage [1], $passage [2]);
  }
}

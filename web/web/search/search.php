<?php

/*

Copyright (©) 2012-2012 Teus Benschop.

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

require_once ("../bootstrap/bootstrap.php");
page_access_level (GUEST_LEVEL);

// The query: The word or string to search for.
$queryString = isset($_GET['q'])?$_GET['q']:'';

// Put the query string into the search box.
Assets_Page::header (gettext ("Search"), $queryString);

$view = new Assets_View (__FILE__);

// Clean the query string up.
$queryString = trim ($queryString);

// Generate search words for emphasizing the search hits.
$queryWords = explode (" ", $queryString);
$markedWords = array ();
foreach ($queryWords as $word) {
  $markedWords [] = '<mark>' . $word . '</mark>';
}

$database_config_general = Database_Config_General::getInstance ();
$database_notes = Database_Notes::getInstance ();
$database_bibles = Database_Bibles::getInstance ();

$siteUrl = $database_config_general->getSiteURL ();

// Search the notes.
$identifiers = $database_notes->searchNotes ($queryString);

$noteCount = count ($identifiers);
$view->view->noteCount = $noteCount;

// Assemble the search results.
$noteTitles = array ();
$noteUrls = array ();
$noteExcerpts = array ();

foreach ($identifiers as $identifier) {

  // The title.
  $summary = $database_notes->getSummary ($identifier);
  $verses = Filter_Books::passagesDisplayInline ($database_notes->getPassages ($identifier));
  $title = "$summary | $verses";
  $title = Filter_Html::sanitize ($title);
  $noteTitles [] = $title;

  // The url.
  $url = "$siteUrl/notes/note.php?id=$identifier";
  $noteUrls [] = $url;

  // The excerpt.
  $text = $database_notes->getSearchField ($identifier);
  $text = explode ("\n", $text);
  $excerpt = "";
  // Go through each line of text separately.
  foreach ($text as $line) {
    $count = 0;
    $line = str_ireplace ($queryWords, $markedWords, $line, $count);
    if ($count == 0) continue;
    // Store this bit of the excerpt.
    $excerpt .= "<p style=\"margin-top: 0em\">$line</p>\n";
  }
  $noteExcerpts [] = $excerpt;
}

// Display the search results for the notes.
$view->view->noteUrls = $noteUrls;
$view->view->noteTitles = $noteTitles;
$view->view->noteExcerpts = $noteExcerpts;

// Search the Bible text.
$ids = $database_bibles->searchText ($queryString);

$textCount = count ($ids);
$view->view->textCount = $textCount;

// Assemble the search results.
$textTitles = array ();
$textUrls = array ();
$textExcerpts = array ();

foreach ($ids as $id) {
  
  // Get the details of this search hit.
  $details = $database_bibles->getBibleBookChapter ($id);
  if ($details == NULL) continue;
  $bible = $database_bibles->getName ($details["bible"]);
  $book = $details ["book"];
  $chapter = $details ["chapter"];

  // The title.
  $title = "$bible" . " | " . Filter_Books::passageDisplay ($book, $chapter, "");
  $title = substr ($title, 0, -1);
  $textTitles [] = $title;
  
  // The URL.
  $url = "$siteUrl/editusfm/index.php?switchbook=$book&switchchapter=$chapter";
  $textUrls [] = $url;

  // The excerpt.
  $text = $database_bibles->getSearchField ($id);
  $text = explode ("\n", $text);
  $excerpt = "";
  // Go through each line of text separately.
  foreach ($text as $line) {
    $count = 0;
    $line = str_ireplace ($queryWords, $markedWords, $line, $count);
    if ($count == 0) continue;
    // Store this bit of the excerpt.
    $excerpt .= "<p style=\"margin-top: 0em\">$line</p>\n";
  }
  $textExcerpts [] = $excerpt;
  
}

// Display the search results for the Bible text.
$view->view->textUrls = $textUrls;
$view->view->textTitles = $textTitles;
$view->view->textExcerpts = $textExcerpts;

$view->render ("search.php");

Assets_Page::footer ();

?>

<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

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
page_access_level (Filter_Roles::GUEST_LEVEL);


// The query: The word or string to search for.
$queryString = isset($_GET['q']) ? $_GET['q'] : '';


// Put the query string into the search box.
Assets_Page::header (Locale_Translate::_("Search"), $queryString);


$view = new Assets_View (__FILE__);


// Clean the query string up.
$queryString = trim ($queryString);


// Generate search words for emphasizing the search hits.
$queryWords = explode (" ", $queryString);


$database_config_general = Database_Config_General::getInstance ();
$database_notes = Database_Notes::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_search = Database_Search::getInstance ();


$siteUrl = $database_config_general->getSiteURL ();


// Search the notes.
$bibles = Access_Bible::bibles ();
$identifiers = $database_notes->searchNotes ($queryString, $bibles);


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
    $markedLine =  Filter_Markup::words ($queryWords, $line);
    // If the line is marked up, add it to the excerpts.
    if ($markedLine != $line) {
      $excerpt .= "<p style=\"margin-top: 0em\">$markedLine</p>\n";
    }
  }
  $noteExcerpts [] = $excerpt;
}


// Display the search results for the notes.
$view->view->noteUrls = $noteUrls;
$view->view->noteTitles = $noteTitles;
$view->view->noteExcerpts = $noteExcerpts;


// Search the Bible text.
$hits = $database_search->searchText ($queryString, $bibles);


$textCount = count ($hits);
$view->view->textCount = $textCount;


// Assemble the search results.
$textLinks = array ();
$textExcerpts = array ();


foreach ($hits as $hit) {


  // Get the details of this search hit.
  $details = $database_search->getBiblePassage ($hit);
  $bible = $details ["bible"];
  $book = $details ["book"];
  $chapter = $details ["chapter"];
  $verse = $details ["verse"];


  // The title plus link.
  $link = "$bible" . " | " . Filter_Books::linkForOpeningEditorAt ($book, $chapter, $verse);
  $textLinks [] = $link;

  // The excerpt.
  $text = $database_search->getBibleVerseText ($bible, $book, $chapter, $verse);
  $text = explode ("\n", $text);
  $excerpt = "";
  // Go through each line of text separately.
  foreach ($text as $line) {
    $markedLine =  Filter_Markup::words ($queryWords, $line);
    if ($markedLine != $line) {
      // Store this bit of the excerpt.
      $excerpt .= "<p style=\"margin-top: 0em\">$markedLine</p>\n";
    }
  }
  $textExcerpts [] = $excerpt;


}


// Display the search results for the Bible text.
$view->view->textLinks = $textLinks;
$view->view->textExcerpts = $textExcerpts;


$view->render ("search.php");


Assets_Page::footer ();


?>

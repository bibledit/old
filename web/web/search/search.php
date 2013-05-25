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

$smarty = new Smarty_Bibledit (__FILE__);

// Put the query string into the search box.
Assets_Page::header (gettext ("Search"), $queryString);

// Clean the query string up.
$queryString = trim ($queryString);

$database_config_general = Database_Config_General::getInstance ();
$database_notes = Database_Notes::getInstance ();
$database_bibles = Database_Bibles::getInstance ();

$siteUrl = $database_config_general->getSiteURL ();

// Search the notes.
$identifiers = $database_notes->searchNotes ($queryString);

$noteCount = count ($identifiers);
$smarty->assign ("noteCount", $noteCount);

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
  $url = "$siteUrl/consultations/notes.php?consultationnote=$identifier";
  $noteUrls [] = $url;

  // The excerpt.
  $excerpt = "";
  $noteExcerpts [] = $excerpt;

/*
  // $text = $database_notes->getContents ($noteIdentifier); Todo see if this can be used somewhere. Else remove it.
  // $text = Filter_Html::html2text ($text);

  // The excerpt with the hits in bold.
  $text = $docinfo['attrs']['text'];
  $options = array
  (
    "before_match" => "<b>",
    "after_match" => "</b>",
    "chunk_separator" => " ... ",
    "limit" => 60,
    "around" => 3,
    "exact_phrase" => 0
  );
  // Go through each line of text separately.
  $text = explode ("\n", $text);
  $excerpt = "";
  foreach ($text as $textLine) {
    $partialExcerpt = $sphinxClient->BuildExcerpts (array ($textLine), "sphinxsearch", $queryString, $options);
    $partialExcerpt = $partialExcerpt [0];
    // If this line of text has no hit, skip it.
    if (strpos ($partialExcerpt, "<b>") === false) continue;
    // Store this bit of the excerpt.
    $excerpt .= "<p style=\"margin-top: 0em\">$partialExcerpt</p>\n";
  }
  */
}

// Display the search results for the notes.
$smarty->assign ("noteUrls", $noteUrls);
$smarty->assign ("noteTitles", $noteTitles);
$smarty->assign ("noteExcerpts", $noteExcerpts);

// Search the Bible text.
$ids = $database_bibles->searchText ($queryString);

$textCount = count ($ids);
$smarty->assign ("textCount", $textCount);

// Assemble the search results.
$textTitles = array ();
$textUrls = array ();
$textExcerpts = array ();

foreach ($ids as $id) {
  $details = $database_bibles->getBibleBookChapter ($id);
  if ($details == NULL) continue;
  $bible = $database_bibles->getName ($details["bible"]);
  $book = $details ["book"];
  $chapter = $details ["chapter"];
  $title = "$bible" . " | " . Filter_Books::passageDisplay ($book, $chapter, "");
  $title = substr ($title, 0, -1);
  $textTitles [] = $title;
  $url = "$siteUrl/desktop/index.php?desktop=edittext&switchbook=$book&switchchapter=$chapter";
  $textUrls [] = $url;
  $excerpt = "";
  $textExcerpts [] = $excerpt;
}

// Display the search results for the Bible text.
$smarty->assign ("textUrls", $textUrls);
$smarty->assign ("textTitles", $textTitles);
$smarty->assign ("textExcerpts", $textExcerpts);

$smarty->display ("search.tpl");

Assets_Page::footer ();

?>

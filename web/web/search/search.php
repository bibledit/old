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
//while (strpos ($queryString, "  ") !== false) {
//  $queryString = str_replace ("  ", " ", $queryString);
//}

// Add wildcards to the words in the query string.
//$queryString = explode (" ", $queryString);
//foreach ($queryString as &$line) {
//  if ($line == "") continue;
//  $line = "*" . $line . "*";
//}
//$queryString = implode (" ", $queryString);

$database_config_general = Database_Config_General::getInstance ();
$database_notes = Database_Notes::getInstance ();

$siteUrl = $database_config_general->getSiteURL ();

$identifiers = $database_notes->searchNotes ($queryString);

$notesCount = count ($identifiers);
$smarty->assign ("notesCount", $notesCount);

// Assemble the search results.
$titles = array ();
$urls = array ();
$excerpts = array ();

foreach ($identifiers as $identifier) {

  // The title.
  $summary = $database_notes->getSummary ($identifier);
  $verses = Filter_Books::passagesDisplayInline ($database_notes->getPassages ($identifier));
  $title = "$summary | $verses";
  $title = Filter_Html::sanitize ($title);
  $titles [] = $title;

  // The url.
  $url = "$siteUrl/consultations/notes.php?consultationnote=$identifier";
  $urls [] = $url;

  // The excerpt.
  $excerpt = "";
  $excerpts [] = $excerpt;


/*

  // $text = $database_notes->getContents ($noteIdentifier);
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


// Display the search results.
$smarty->assign ("urls", $urls);
$smarty->assign ("titles", $titles);
$smarty->assign ("excerpts", $excerpts);


$smarty->display ("search.tpl");


Assets_Page::footer ();


?>

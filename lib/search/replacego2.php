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
page_access_level (Filter_Roles::TRANSLATOR_LEVEL);


$database_config_general = Database_Config_General::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_search = Database_Search::getInstance ();
$database_bibles = Database_Bibles::getInstance ();


$siteUrl = $database_config_general->getSiteURL ();


// Get the action variables from the query.
@$id = $_GET ['id'];
@$searchfor = $_GET ['q'];
@$replacewith = $_GET ['r'];
@$casesensitive = ($_GET ['c'] == "true");
@$searchplain = ($_GET ['p'] == "true");


// Get Bible and passage for this identifier.
$details = explode ("_", $id);
$bibleID = $details [0];
$bible = $database_bibles->getName ($bibleID);
$book = $details [1];
$chapter = $details [2];
$verse = $details [3];


$stylesheet = $database_config_bible->getExportStylesheet ($bible);


// As a standard to compare against, get the plain text from the search database,
// do the replacements, count the replacements, and then get the desired new plain text.
// This only applies when searching/replacing in the plain text, not when doing it in the USFM.
if ($searchplain) {
  $standardReplacementCount = 0;
  $standardPlainText = $database_search->getBibleVerseText ($bible, $book, $chapter, $verse);
  if ($casesensitive) {
    $standardPlainText = str_replace ($searchfor, $replacewith, $standardPlainText, $standardReplacementCount);
  } else {
    $needles = Filter_Search::needles ($searchfor, $standardPlainText);
    $standardPlainText = str_replace ($needles, $replacewith, $standardPlainText, $standardReplacementCount);
  }
}


// Get the old USFM into an array of verse => USFM fragment.
$usfmArray = array ();
$usfmString = $database_bibles->getChapter ($bible, $book, $chapter);
$verses = Filter_Usfm::getVerseNumbers ($usfmString);
$verses = array_unique ($verses);
sort ($verses, SORT_NUMERIC);
foreach ($verses as $vs) {
  $usfmArray [$vs] = Filter_Usfm::getVerseText ($usfmString, $vs);
}


// Do the replacing in the correct verse of the raw USFM, and count how many replacements were made.
$usfm = $usfmArray [$verse];
$usfmReplacementCount = 0;
if ($casesensitive) {
  $usfm = str_replace ($searchfor, $replacewith, $usfm, $usfmReplacementCount);
} else {
  $needles = Filter_Search::needles ($searchfor, $usfm);
  $usfm = str_replace ($needles, $replacewith, $usfm, $usfmReplacementCount);
}
$usfmArray [$verse] = $usfm;


// Create the updated chapter USFM as a string.
$updatedUsfm = implode ("\n", $usfmArray);


// Text filter for getting the new plain text from the new USFM.
// This is for search/replace in plain text, not in USFM.
if ($searchplain) {
  $filter_text = new Filter_Text ($bible);
  $filter_text->text_text = new Text_Text ();
  $filter_text->initializeHeadingsAndTextPerVerse ();
  $filter_text->addUsfmCode ($updatedUsfm);
  $filter_text->run ($stylesheet);
}


// Get the updated plain text of the correct verse of the updated USFM.
// This is for search/replace in plain text, not in USFM.
if ($searchplain) {
  $updatedPlainText = "";
  $texts = $filter_text->getVersesText ();
  foreach ($texts as $vs => $text) {
    if ($vs == $verse) $updatedPlainText .= "$text\n";
  }
  $headings = $filter_text->verses_headings;
  foreach ($headings as $vs => $heading) {
    if ($vs == $verse) $updatedPlainText .= "$heading\n";
  }
}


// Check that the standard and real number of replacements, and the standard and new texts, are the same.
// This is for search/replace in plain text, not in USFM.
$replacementOkay = true;
if ($searchplain) {
  if ($usfmReplacementCount != $standardReplacementCount) $replacementOkay = false;
  if ($updatedPlainText != $standardPlainText) $replacementOkay = false;
}


// Generate success or failure icon.
if ($replacementOkay) {
  $icon = '<span class="success">✔</span>';
} else {
  $icon = '<span class="error">✗</span>';
}


// Store the new chapter in the database on success.
if ($replacementOkay) {
  Bible_Logic::storeChapter ($bible, $book, $chapter, $updatedUsfm);
}


// Mark the new plain text.
if ($replacewith != "") {
  if ($searchplain) {
    $updatedPlainText = Filter_Markup::words (array ($replacewith), $updatedPlainText);
  }
}


// Clickable passage.
$link = Filter_Books::linkForOpeningEditorAt ($book, $chapter, $verse);


// Success or failure message.
if ($replacementOkay) {
  if ($searchplain) {
    $msg = $updatedPlainText;
  } else {
    $msg = $updatedUsfm;
  }
} else {
  $msg = '<span class="error">' . Locale_Translate::_("This text could not be automatically replaced. Click the passage to do it manually.") . '</span>';
}


// Create output.
$output = <<< EOD
<p>$icon $link $msg</p>
EOD;


// Output to browser.
echo $output;


?>

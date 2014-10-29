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
page_access_level (Filter_Roles::CONSULTANT_LEVEL);


$database_config_user = Database_Config_User::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_search = Database_Search::getInstance ();
$database_volatile = Database_Volatile::getInstance ();
$ipc_focus = Ipc_Focus::getInstance ();


$myIdentifier = Filter_User::myIdentifier ();


@$bible = $_GET ['b'];
if (!isset ($bible)) $bible = $database_config_user->getBible ();


@$load = $_GET ['load'];
if (isset ($load)) {

  $book = $ipc_focus->getBook ();
  $chapter = $ipc_focus->getChapter ();
  $verse = $ipc_focus->getVerse ();
  
  // Text of the focused verse in the active Bible.
  // Remove all punctuation from it.
  $versetext = $database_search->getBibleVerseText ($bible, $book, $chapter, $verse);
  $punctuation = $database_config_bible->getSentenceStructureEndPunctuation ($bible);
  $punctuation = explode (" ", $punctuation);
  $versetext = str_replace ($punctuation, "", $versetext);
  $punctuation = $database_config_bible->getSentenceStructureMiddlePunctuation ($bible);
  $punctuation = explode (" ", $punctuation);
  $versetext = str_replace ($punctuation, "", $versetext);
  $versetext = trim ($versetext);

  $database_volatile->setValue ($myIdentifier, "searchsimilar", $versetext);
  
  echo $versetext;
  die;
}


@$words = $_GET ['words'];
if (isset ($words)) {

  $words = trim ($words);
  $database_volatile->setValue ($myIdentifier, "searchsimilar", $words);
  $words = explode (" " , $words);
  
  // Include items if there are no more search hits than 30% of the total number of verses in the Bible.
  $maxcount = intval (0.3 * $database_search->getVerseCount ($bible));

  // Store how often a verse occurs in an array.
  // The keys are the identifiers of the search results.
  // The values are how often the identifiers occur in the entire focused verse.
  $identifiers = array ();

  foreach ($words as $word) {
    
    // Find out how often this word occurs in the Bible. Skip if too often.
    $ids = $database_search->searchBibleText ($bible, $word);
    $count = count ($ids);
    if ($count > $maxcount) continue;
    
    // Store the identifiers and their count.
    foreach ($ids as $id) {
      if (isset ($identifiers [$id])) $identifiers [$id]++;
      else $identifiers [$id] = 1;
    }
    
  }

  // Sort on occurence from high to low.
  arsort ($identifiers, SORT_NUMERIC);
  
  // Output the passage identifiers to the browser.
  // Skip identifiers that only occur once.
  foreach ($identifiers as $key => $value) {
    if ($value <= 1) continue;
    echo "$key\n";
  }

  die;

}


@$id = $_GET ['id'];
if (isset ($id)) {
  
  // Get the Bible and passage for this identifier.
  $details = $database_search->getBiblePassage ($id);
  $bible = $details ['bible'];
  $book = $details ['book'];
  $chapter = $details ['chapter'];
  $verse = $details ['verse'];
  
  // Get the plain text.
  $text = $database_search->getBibleVerseText ($bible, $book, $chapter, $verse);

  // Get search words.
  $words = $database_volatile->getValue ($myIdentifier, "searchsimilar");
  $words = explode (" " , $words);
  
  // Format it.
  $link = Filter_Books::linkForOpeningEditorAt ($book, $chapter, $verse);
  $text =  Filter_Markup::words ($words, $text);
  $output = "<div>$link $text</div>";
  
  // Output to browser.
  echo $output;

  // Done.  
  die;
}


$header = new Assets_Header (Locale_Translate::_("Search"));
$header->run ();


$view = new Assets_View (__FILE__);


$view->view->bible = $bible;


$script = <<<EOD
var searchBible = "$bible";
EOD;
$view->view->script = $script;


$view->render ("similar.php");


Assets_Page::footer ();


?>

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
$database_kjv = Database_Kjv::getInstance ();
$ipc_focus = Ipc_Focus::getInstance ();


$myIdentifier = Filter_User::myIdentifier ();


@$bible = $_GET ['b'];
if (!isset ($bible)) $bible = $database_config_user->getBible ();


@$load = $_GET ['load'];
if (isset ($load)) {

  $book = $ipc_focus->getBook ();
  $chapter = $ipc_focus->getChapter ();
  $verse = $ipc_focus->getVerse ();
  
  // Get Strong's numbers, plus English snippets.
  $searchtext = "";
  $details = $database_kjv->getVerse ($book, $chapter, $verse);
  foreach ($details as $offset => $detail) {
    if ($offset) $searchtext .= " ";
    $searchtext .= $detail ['strong'];
    $searchtext .= " (";
    $searchtext .= $detail ['english'];
    $searchtext .= ")";
  }
  
  $searchtext = trim ($searchtext);
  
  echo $searchtext;

  die;
}


@$words = $_GET ['words'];
if (isset ($words)) {

  $words = trim ($words);
  $words = explode (" " , $words);
  
  // Include items if there are no more search hits than 30% of the total number of verses in the KJV.
  $maxcount = intval (0.3 * 31102);

  // Store how often a verse occurs in an array.
  // The keys are the passages of the search results.
  // The values are how often the passages occur in the search results.
  $passages = array ();

  foreach ($words as $strong) {
    
    // Skip short words.
    if (strlen ($strong) < 2) continue;
    
    // Find out how often this word occurs in the Bible. Skip if too often.
    $details = $database_kjv->searchStrong ($strong);
    $count = count ($details);
    if ($count < 1) continue;
    if ($count > $maxcount) continue;
    
    // Store the identifiers and their count.
    foreach ($details as $detail) {
      $book = $detail ['book'];
      $chapter = $detail ['chapter'];
      $verse = $detail ['verse'];
      $passage = array ($book, $chapter, $verse);
      $passage = Filter_Books::passage2integer ($passage);
      if (isset ($passages [$passage])) $passages [$passage]++;
      else $passages [$passage] = 1;
    }
    
  }

  // Sort on occurence from high to low.
  arsort ($passages, SORT_NUMERIC);
  
  // Output the passage identifiers to the browser.
  // Skip identifiers that only occur once.
  foreach ($passages as $key => $value) {
    if ($value <= 1) continue;
    echo "$key\n";
  }

  die;

}


@$id = $_GET ['id'];
if (isset ($id)) {
  
  // Get the and passage for this identifier.
  $passage = Filter_Books::integer2passage ($id);
  $book = $passage [0];
  $chapter = $passage [1];
  $verse = $passage [2];
  
  // Get the plain text.
  $text = $database_search->getBibleVerseText ($bible, $book, $chapter, $verse);

  // Format it.
  $link = Filter_Books::linkForOpeningEditorAt ($book, $chapter, $verse);
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


$view->render ("strongs.php");


Assets_Page::footer ();


?>

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

$database_config_general = Database_Config_General::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_search = Database_Search::getInstance ();
$database_volatile = Database_Volatile::getInstance ();


$siteUrl = $database_config_general->getSiteURL ();


@$bible = $_GET ['bible'];
if (!isset ($bible)) $bible = $database_config_user->getBible ();


@$identifier = $_GET ['i'];
@$query = $_GET ['q'];
@$hit = $_GET ['h'];


// Get one search hit.
if (isset ($hit)) {


  // Retrieve the search parameters from the volatile database.
  $query = $database_volatile->getValue ($identifier, "query");
  $casesensitive = $database_volatile->getValue ($identifier, "casesensitive");
  $plaintext = $database_volatile->getValue ($identifier, "plaintext");


  // Get the Bible and passage for this identifier.
  $details = $database_search->getBiblePassage ($hit);
  $bible = $details ['bible'];
  $book = $details ['book'];
  $chapter = $details ['chapter'];
  $verse = $details ['verse'];

  
  // Get the plain text or USFM.
  if ($plaintext) {
    $text = $database_search->getBibleVerseText ($bible, $book, $chapter, $verse);
  } else {
    $text = $database_search->getBibleVerseUsfm ($bible, $book, $chapter, $verse);
  }

  
  // Format it.
  $link = Filter_Books::linkForOpeningEditorAt ($book, $chapter, $verse);
  $text =  Filter_Markup::words (array ($query), $text);
  $output = "<div>$link $text</div>";

  
  // Output to browser.
  echo $output;


  // Done.  
  die;
}


// Perform the initial search.
if (isset ($query)) {
  

  // Get extra search parameters and store them all in the volatile database.
  @$casesensitive = ($_GET ['c'] == "true");
  @$plaintext = ($_GET ['p'] == "true");
  @$currentbook = ($_GET ['b'] == "true");
  @$sharing = $_GET ['s'];
  $database_volatile->setValue ($identifier, "query", $query);
  $database_volatile->setValue ($identifier, "casesensitive", $casesensitive);
  $database_volatile->setValue ($identifier, "plaintext", $plaintext);


  // Deal with case sensitivity. 
  // Deal with whether to search in the main text without notes, or in the raw USFM.
  // Fetch the initial set of hits.
  $hits = array ();
  if ($plaintext) {
    if ($casesensitive) {
      $hits = $database_search->searchBibleTextCaseSensitive ($bible, $query);
    } else {
      $hits = $database_search->searchBibleText ($bible, $query);
    }
  } else {
    if ($casesensitive) {
      $hits = $database_search->searchBibleUsfmCaseSensitive ($bible, $query);
    } else {
      $hits = $database_search->searchBibleUsfm ($bible, $query);
    }
  }
  
  
  // Deal with possible searching in the current book only.
  if ($currentbook) {
    $ipc_focus = Ipc_Focus::getInstance();
    $book = $ipc_focus->getBook ();
    $bookhits = array ();
    foreach ($hits as $hit) {
      $details = $database_search->getBiblePassage ($hit);
      if ($book == $details ['book']) {
        $bookhits [] = $hit;
      }
    }
    $hits = $bookhits;
  }
  
  
  // Deal with how to share the results.
  if ($sharing != "load") {
    $loadedHits = $database_volatile->getValue ($identifier, "hits");
    $loadedHits = Filter_String::string2array ($loadedHits);
    if ($sharing == "add") {
      $hits = array_merge ($loadedHits, $hits);
    }
    if ($sharing == "remove") {
      $hits = array_diff ($loadedHits, $hits);
    }
    if ($sharing == "intersect") {
      $hits = array_intersect ($loadedHits, $hits);
    }
    $hits = array_unique ($hits);
  }

  
  // Generate one string from the hits.
  $output = "";
  foreach ($hits as $hit) {
    $output .= "$hit\n";
  }
  
  // Store search hits in the volatile database.
  $database_volatile->setValue ($identifier, "hits", $output);


  // Output results.
  echo $output;


  // Done.
  die;
}


// Build the advanced search page.
$header = new Assets_Header (Locale_Translate::_("Search"));
$header->setNavigator ();
$header->run ();


$view = new Assets_View (__FILE__);
$view->view->bible = $bible;
$script = <<< EOD
var searchBible = "$bible";
EOD;
$view->view->script = $script;
$view->render ("search2.php");
Assets_Page::footer ();


?>

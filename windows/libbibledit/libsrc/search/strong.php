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
  $html = "<table>\n";
  $details = $database_kjv->getVerse ($book, $chapter, $verse);
  foreach ($details as $detail) {
    $strong = $detail ['strong'];
    $english = $detail ['english'];
    $html .= "<tr><td><a href=\"$strong\">$strong</a></td><td>$english</td></tr>\n";
  }
  $html .= "</table>\n";
  
  echo $html;
  die;
}


@$strong = $_GET ['strong'];
if (isset ($strong)) {

  $strong = trim ($strong);
 
  $passages = array ();

  $details = $database_kjv->searchStrong ($strong);
    
  foreach ($details as $detail) {
    $book = $detail ['book'];
    $chapter = $detail ['chapter'];
    $verse = $detail ['verse'];
    $passage = array ($book, $chapter, $verse);
    $passage = Filter_Books::passage2integer ($passage);
    $passages [] = $passage;
  }
  
  $passages = array_unique ($passages, SORT_NUMERIC);
    
  foreach ($passages as $passage) {
    echo "$passage\n";
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


$view->render ("strong.php");


Assets_Page::footer ();


?>

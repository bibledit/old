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
$database_search = Database_Search::getInstance ();
$database_bibles = Database_Bibles::getInstance ();


$siteUrl = $database_config_general->getSiteURL ();


// Get search variables from the query.
@$searchfor = $_GET ['q'];
@$replacewith = $_GET ['r'];
@$casesensitive = ($_GET ['c'] == "true");
@$id = $_GET ['id'];


// Get the Bible and passage for this identifier.
$details = $database_search->getBiblePassage ($id);
$bible = $details ['bible'];
$book = $details ['book'];
$chapter = $details ['chapter'];
$verse = $details ['verse'];


// Get the plain text.
$text = $database_search->getBibleVerseText ($bible, $book, $chapter, $verse);


// Clickable passage.
$link = Filter_Books::linkForOpeningEditorAt ($book, $chapter, $verse);


$oldtext =  Filter_Markup::words (array ($searchfor), $text);


if ($casesensitive) {
  $newtext = str_replace ($searchfor, $replacewith, $text);
} else {
  $needles = Filter_Search::needles ($searchfor, $text);
  $newtext = str_replace ($needles, $replacewith, $text);
}
if ($replacewith != "") $newtext =  Filter_Markup::words (array ($replacewith), $newtext);


// The id sent to the browser contains bible identifier, book, chapter, and verse.
$bibleID = $database_bibles->getID ($bible);
$id = implode ("_", array ($bibleID, $book, $chapter, $verse));


// Create output.
$output = <<< EOD
<div id="$id">
<p><a href="replace"> ✔ </a> <a href="delete"> ✗ </a> $link</p>
<p>$oldtext</p>
<p>$newtext</p>
</div>
EOD;


// Output to browser.
echo $output;


?>

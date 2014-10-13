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


// Security: The script runs from the cli SAPI only.
Filter_Cli::assert ();


ignore_user_abort (true);
set_time_limit (0);


$bible = Filter_Cli::argument (@$argv, 1);


// Create folders for the web export.
$directory = Export_Logic::webDirectory ($bible);
if (!file_exists ($directory)) mkdir ($directory, 0777, true);


// Filenames for the web file and stylesheet.
$indexFile = "$directory/index.html";
$index00 = "$directory/00_index.html";
$filecss = "$directory/stylesheet.css";


$database_logs = Database_Logs::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_books = Database_Books::getInstance ();


$stylesheet = $database_config_bible->getExportStylesheet ($bible);


// Create stylesheet.
$styles_sheets = new Styles_Sheets ();
$styles_sheets->create ($stylesheet, $filecss, false, $bible);


$backLinkPath = Export_Logic::webBackLinkDirectory ($bible);


// Main index file.
$html_text_rich_bible_index = new Html_Text ($bible);
// On top are the breadcrumbs, starting with a clickable Bible name.
$htmlHeader = new Html_Header ($html_text_rich_bible_index);
$htmlHeader->searchBackLink ($backLinkPath . Filter_Paths::htmlFileNameBible (), Locale_Translate::_("Go back to Bible"));
$htmlHeader->create (array (array ($bible, Filter_Paths::htmlFileNameBible ())));


// Prepare for the list of books in de html index file.
$html_text_rich_bible_index->newParagraph ("navigationbar");
$html_text_rich_bible_index->addText (" |");


// Go through the Bible books.
$books = $database_bibles->getBooks ($bible);
foreach ($books as $book) {
  // Add this book to the main web index.
  $html_text_rich_bible_index->addLink ($html_text_rich_bible_index->currentPDomElement,  Filter_Paths::htmlFileNameBible ("", $book), "", $database_books->getEnglishFromId ($book), "", " " . $database_books->getEnglishFromId ($book) . " ");
  $html_text_rich_bible_index->addText ("|");
}


// Save index file for the interlinked web export.
$database_logs->log ("exports: Create index file for interlinked Web");
$html_text_rich_bible_index->save ($indexFile);
$html_text_rich_bible_index->save ($index00);


// Lens image supporting search.
copy ("../webbible/lens.png", "$directory/lens.png");


$database_logs->log (Locale_Translate::_("Web export index") . " $bible", Filter_Roles::TRANSLATOR_LEVEL);


?>

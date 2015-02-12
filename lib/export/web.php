<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

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


require_once ("../bootstrap/bootstrap");


// Security: The script runs from the cli SAPI only.
Filter_Cli::assert ();


ignore_user_abort (true);
set_time_limit (0);


$bible = Filter_Cli::argument (@$argv, 1);
$book = Filter_Cli::argument (@$argv, 2);


$directory = Export_Logic::webDirectory ($bible);
if (!file_exists ($directory)) mkdir ($directory, 0777, true);


$database_logs = Database_Logs::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_books = Database_Books::getInstance ();


$stylesheet = Database_Config_Bible::getExportStylesheet ($bible);


// Copy font to the output directory.
$font = Database_Config_Bible::getTextFont ($bible);
if ($font) {
  if (Fonts_Logic::fontExists ($font)) {
    $fontpath = Fonts_Logic::getFontPath ($font);
    copy ($fontpath, "$directory/$font");
  }
}


$backLinkPath = Export_Logic::webBackLinkDirectory ($bible);


$bibleBookText = $bible . " " . Database_Books::getEnglishFromId ($book);


// Web index file for the book.
$html_text_rich_book_index = new Html_Text ($bibleBookText);
$htmlHeader = new Html_Header ($html_text_rich_book_index);
$htmlHeader->searchBackLink ($backLinkPath . Filter_Paths::htmlFileNameBible ("", $book), translate("Go back to") . " " . $bibleBookText);
$htmlHeader->create (array (array ($bible, Filter_Paths::htmlFileNameBible ()), array (Database_Books::getEnglishFromId ($book), Filter_Paths::htmlFileNameBible ()) ));
$html_text_rich_book_index->newParagraph ("navigationbar");
$html_text_rich_book_index->addText ("|");


// Go through the chapters of this book.
$chapters = request->database_bibles()->getChapters (bible, book);
for ($chapters as $chapter) {

  // The text filter for this chapter.
  $filter_text_chapter = new Filter_Text ($bible);

  $usfm = request->database_bibles()->getChapter (bible, book, chapter);
  $usfm = filter_string_trim ($usfm);
  // Use small chunks of USFM at a time for much better performance.
  $filter_text_chapter->addUsfmCode ($usfm);

  // Interlinked web data for one chapter.
  $filter_text_chapter->html_text_linked = new Html_Text (translate("Bible"));
  $filter_text_chapter->html_text_linked->customClass = Filter_CustomCSS::getClass ($bible);

  // Create breadcrumbs for the chapter.
  $htmlHeader = new Html_Header ($filter_text_chapter->html_text_linked);
  $htmlHeader->searchBackLink ($backLinkPath . Filter_Paths::htmlFileNameBible ("", $book, $chapter), translate("Go back to") . " " . $bibleBookText . " " . $chapter);
  $htmlHeader->create (array (array ($bible, Filter_Paths::htmlFileNameBible ()),
                              array (Database_Books::getEnglishFromId ($book), Filter_Paths::htmlFileNameBible ()),
                              array ($chapter, Filter_Paths::htmlFileNameBible ("", $book))
                             ));

  // Create interlinked html for the chapter.
  $filter_text_chapter->run ($stylesheet);
  $filter_text_chapter->html_text_linked->save (Filter_Paths::htmlFileNameBible ($directory, $book, $chapter));

  $html_text_rich_book_index->addLink ($html_text_rich_book_index->currentPDomElement, Filter_Paths::htmlFileNameBible ("", $book, $chapter), "", $chapter, "", " " . $chapter . " ");
  $html_text_rich_book_index->addText ("|");

}


// Save the book index.
$html_text_rich_book_index->save (Filter_Paths::htmlFileNameBible ($directory, $book));



Database_Logs::log (translate("Exported to web") . " $bible " . Export_Logic::baseBookFileName ($book), Filter_Roles::translator ());


?>

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


// The input parameters.
$bible = Filter_Cli::argument (@$argv, 1);
$book = Filter_Cli::argument (@$argv, 2);


// Create folders for the html export.
$directory = Export_Logic::bibleDirectory ($bible) . "/html";
if (!file_exists ($directory)) mkdir ($directory, 0777, true);


// Filename for the html file.
$basename = Export_Logic::baseBookFileName ($book);
$filename = "$directory/$basename.html";
$filecss = "$directory/stylesheet.css";


$database_logs = Database_Logs::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_books = Database_Books::getInstance ();


$stylesheet = $database_config_bible->getExportStylesheet ($bible);


// Create stylesheet.
$styles_sheets = new Styles_Sheets ();
$styles_sheets->create ($stylesheet, $filecss, false, $bible);


// Copy font to the output directory.
$font = $database_config_bible->getTextFont ($bible);
if ($font) {
  if (Fonts_Logic::fontExists ($font)) {
    $fontpath = Fonts_Logic::getFontPath ($font);
    copy ($fontpath, "$directory/$font");
  }
}


$filter_text = new Filter_Text ($bible);
$filter_text->html_text_standard = new Html_Text (Locale_Translate::_("Bible"));
$filter_text->html_text_standard->customClass = Filter_CustomCSS::getClass ($bible);


// Load one book.
$chapters = $database_bibles->getChapters ($bible, $book);
foreach ($chapters as $chapter) {
  $usfm = $database_bibles->getChapter ($bible, $book, $chapter);
  $usfm = trim ($usfm);
  // Use small chunks of USFM at a time for much better performance.
  $filter_text->addUsfmCode ($usfm);
}


// Convert the USFM.
$filter_text->run ($stylesheet);


// Save file.
$filter_text->html_text_standard->save ($filename);


$database_logs->log (Locale_Translate::_("Exported to html") . " $bible " . Export_Logic::baseBookFileName ($book), Filter_Roles::TRANSLATOR_LEVEL);


?>

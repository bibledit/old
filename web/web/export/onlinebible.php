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


$directory = Export_Logic::bibleDirectory ($bible) . "/onlinebible";
if (!file_exists ($directory)) mkdir ($directory, 0777, true);


$filename = "$directory/bible.exp";


$database_logs = Database_Logs::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_books = Database_Books::getInstance ();


$stylesheet = $database_config_bible->getExportStylesheet ($bible);


$filter_text_bible = new Filter_Text ($bible);
$filter_text_bible->onlinebible_text = new Onlinebible_Text ();
$books = $database_bibles->getBooks ($bible);
foreach ($books as $book) {
  $chapters = $database_bibles->getChapters ($bible, $book);
  foreach ($chapters as $chapter) {
    $chapter_data = $database_bibles->getChapter ($bible, $book, $chapter);
    $chapter_data = trim ($chapter_data);
    $filter_text_bible->addUsfmCode ($chapter_data);
  }
}
$filter_text_bible->run ($stylesheet);
$filter_text_bible->onlinebible_text->save ($filename);


$database_logs->log ($bible . ": " . Locale_Translate::_("Exported to Online Bible"), Filter_Roles::TRANSLATOR_LEVEL);


?>

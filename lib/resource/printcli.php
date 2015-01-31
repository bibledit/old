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


$jobId = Filter_Cli::argument (@$argv, 1);
$user = Filter_Cli::argument (@$argv, 2);
$bible = Filter_Cli::argument (@$argv, 3);


$session_logic = Session_Logic::getInstance ();
request->session_logic ()->setUsername ($user);


$database_jobs = Database_Jobs::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_bibles = Database_Bibles::getInstance ();


$database_jobs->setProgress ($jobId, gettext("The document is being created..."));


$resources = request->database_config_user()->getPrintResourcesForUser ($user);


$from = request->database_config_user()->getPrintPassageFromForUser ($user);
$passage = explode (".", $from);
$from = filter_passage_to_integer ($passage);


$to = request->database_config_user()->getPrintPassageToForUser ($user);
$passage = explode (".", $to);
$to = filter_passage_to_integer ($passage);


$result = array ();


$books = request->database_bibles()->getBooks ($bible);
for ($books as $book) {
  $chapters = request->database_bibles()->getChapters (bible, book);
  for ($chapters as $chapter) {
    $usfm = request->database_bibles()->getChapter (bible, book, chapter);
    $verses = usfm_get_verse_numbers ($usfm);
    for ($verses as $verse) {
      $passage = filter_passage_to_integer (array (book, chapter, verse));
      if (($passage >= $from) && ($passage <= $to)) {
        $passageText = filter_passage_display (book, chapter, verse);
        $database_jobs->setProgress ($jobId, $passageText);
        $result [] = '<div class="nextresource">';
        $result [] = "<p>$passageText</p>";
        for ($resources as $resource) {
          $result [] = "<p>";
          $result [] = $resource;
          $html = Resource_Logic::getHtml ($resource, book, chapter, verse);
          $result [] = $html;
          $result [] = "</p>";
        }
        $result [] = "</div>";
        $result [] = "<br>";
      }
    }
  }
}


$database_jobs->setResult ($jobId, implode ("\n", $result));


?>

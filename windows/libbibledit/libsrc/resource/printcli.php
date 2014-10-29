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


$jobId = Filter_Cli::argument (@$argv, 1);
$user = Filter_Cli::argument (@$argv, 2);
$bible = Filter_Cli::argument (@$argv, 3);


$session_logic = Session_Logic::getInstance ();
$session_logic->setUsername ($user);


$database_jobs = Database_Jobs::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_bibles = Database_Bibles::getInstance ();


$database_jobs->setProgress ($jobId, Locale_Translate::_("The document is being created..."));


$resources = $database_config_user->getPrintResourcesForUser ($user);


$from = $database_config_user->getPrintPassageFromForUser ($user);
$passage = explode (".", $from);
$from = Filter_Books::passage2integer ($passage);


$to = $database_config_user->getPrintPassageToForUser ($user);
$passage = explode (".", $to);
$to = Filter_Books::passage2integer ($passage);


$result = array ();


$books = $database_bibles->getBooks ($bible);
foreach ($books as $book) {
  $chapters = $database_bibles->getChapters ($bible, $book);
  foreach ($chapters as $chapter) {
    $usfm = $database_bibles->getChapter ($bible, $book, $chapter);
    $verses = Filter_Usfm::getVerseNumbers ($usfm);
    foreach ($verses as $verse) {
      $passage = Filter_Books::passage2integer (array ($book, $chapter, $verse));
      if (($passage >= $from) && ($passage <= $to)) {
        $passageText = Filter_Books::passageDisplay ($book, $chapter, $verse);
        $database_jobs->setProgress ($jobId, $passageText);
        $result [] = '<div class="nextresource">';
        $result [] = "<p>$passageText</p>";
        foreach ($resources as $resource) {
          $result [] = "<p>";
          $result [] = $resource;
          $html = Resource_Logic::getHtml ($resource, $book, $chapter, $verse);
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

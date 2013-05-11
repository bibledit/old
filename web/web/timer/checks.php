<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2013 Teus Benschop.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


require_once ("../bootstrap/bootstrap.php");
$database_logs = Database_Logs::getInstance ();
$database_logs->log (gettext ("checks: Running checks on the Bibles"), true);


// Security: The script runs from the cli SAPI only.
if (php_sapi_name () != "cli") {
  $database_logs->log ("checks: Fatal: This only runs through the cli Server API", true);
  die;
}


$database_config_general = Database_Config_General::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_check = Database_Check::getInstance ();
$database_users = Database_Users::getInstance ();
$database_mail = Database_Mail::getInstance ();
$database_books = Database_Books::getInstance ();


$database_check->truncateOutput ();
$check_double_spaces_usfm = $database_config_general->getCheckDoubleSpacesUsfm ();


// Go through the Bibles.
$checkedBibles = $database_config_general->getCheckedBibles ();
$bibles = $database_bibles->getBibles ();
foreach ($bibles as $bible) {
  // Skip Bibles that should not be checked.
  if (!in_array ($bible, $checkedBibles)) continue;
  // Go through the books and chapters.
  $books = $database_bibles->getBooks ($bible);
  foreach ($books as $book) {
    $bookname = $database_books->getEnglishFromId ($book);
    $chapters = $database_bibles->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      $chapterUsfm = $database_bibles->getChapter ($bible, $book, $chapter);
      $verses = Filter_Usfm::getVerseNumbers ($chapterUsfm);
      foreach ($verses as $verse) {
        $verseUsfm = Filter_Usfm::getVerseText ($chapterUsfm, $verse);
        if ($check_double_spaces_usfm) {
          Checks_Doublespace::usfm ($bible, $book, $chapter, $verse, $verseUsfm);
        }
      }
    }
  }
}


// Create an email with details of the checks.
$emailBody = array ();
$hits = $database_check->getHits ();
foreach ($hits as $hit) {
  $identifiers [] = $hit['id'];
  $bible = Filter_Html::sanitize ($database_bibles->getName ($hit['bible']));
  $passage = Filter_Books::passagesDisplayInline (array (array ($hit['book'], $hit['chapter'], $hit['verse'])));
  $data = Filter_Html::sanitize ($hit['data']);
  $result = "<p>$bible $passage $data</p>";
  $emailBody [] = $result;
}


// Send email to the users who subscribed to it.
if (count ($emailBody) > 0) {
  $subject = gettext ("Bible Checks");
  $emailBody = implode ("\n", $emailBody);
  $users = $database_users->getUsers ();
  foreach ($users as $user) {
    if ($database_config_user->getUserBibleChecksNotification ($user)) {
      $database_mail->send ($user, $subject, $emailBody);
    }
  }
}


$database_logs->log (gettext ("checks: Completed"), true);


?>

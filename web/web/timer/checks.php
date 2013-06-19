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
$stylesheet = $database_config_general->getExportStylesheet ();
$check_double_spaces_usfm = $database_config_general->getCheckDoubleSpacesUsfm ();
$check_full_stop_in_headings = $database_config_general->getCheckFullStopInHeadings ();
$check_space_before_punctuation = $database_config_general->getCheckSpaceBeforePunctuation ();
$check_sentence_structure = $database_config_general->getCheckSentenceStructure ();
$check_paragraph_structure = $database_config_general->getCheckParagraphStructure ();
$checks_sentences = new Checks_Sentences ();
$checks_sentences->enterCapitals ($database_config_general->getSentenceStructureCapitals ());
$checks_sentences->enterSmallLetters ($database_config_general->getSentenceStructureSmallLetters ());
$end_marks = $database_config_general->getSentenceStructureEndPunctuation ();
$checks_sentences->enterEndMarks ($end_marks);
$center_marks = $database_config_general->getSentenceStructureMiddlePunctuation ();
$checks_sentences->enterCenterMarks ($center_marks);
$checks_sentences->enterDisregards ($database_config_general->getSentenceStructureDisregards ());
$checks_sentences->enterNames ($database_config_general->getSentenceStructureNames ());
$check_versification = $database_config_general->getCheckChaptesVersesVersification ();
$check_well_formed_usfm = $database_config_general->getCheckWellFormedUsfm ();
$checks_usfm = new Checks_Usfm ();


// Go through the Bibles.
$checkedBibles = $database_config_general->getCheckedBibles ();
$bibles = $database_bibles->getBibles ();
foreach ($bibles as $bible) {
  // Skip Bibles that should not be checked.
  if (!in_array ($bible, $checkedBibles)) continue;
  // Get the books.
  $books = $database_bibles->getBooks ($bible);
  if ($check_versification) Checks_Versification::books ($bible, $books);
  // Go through the books.
  foreach ($books as $book) {
    // Get the chapters.
    $chapters = $database_bibles->getChapters ($bible, $book);
    if ($check_versification) Checks_Versification::chapters ($bible, $book, $chapters);
    // Go through the chapters.
    foreach ($chapters as $chapter) {
      $chapterUsfm = $database_bibles->getChapter ($bible, $book, $chapter);
      // Get the verses.
      $verses = Filter_Usfm::getVerseNumbers ($chapterUsfm);
      if ($check_versification) Checks_Versification::verses ($bible, $book, $chapter, $verses);
      // Go through the verses.
      foreach ($verses as $verse) {
        $verseUsfm = Filter_Usfm::getVerseText ($chapterUsfm, $verse);
        if ($check_double_spaces_usfm) {
          Checks_Space::doubleSpaceUsfm ($bible, $book, $chapter, $verse, $verseUsfm);
        }
      }
      $filter_text = new Filter_Text ("");
      $filter_text->initializeHeadingsAndTextPerVerse ();
      $filter_text->addUsfmCode ($chapterUsfm);
      $filter_text->run ($stylesheet);
      $verses_headings = $filter_text->verses_headings;
      $verses_text = $filter_text->getVersesText ();
      if ($check_full_stop_in_headings) {
        Checks_Headers::noPunctuationAtEnd ($bible, $book, $chapter, $verses_headings, $center_marks, $end_marks);
      }
      if ($check_space_before_punctuation) {
        Checks_Space::spaceBeforePunctuation ($bible, $book, $chapter, $verses_text);
      }
      if ($check_sentence_structure || $check_paragraph_structure) {
        $checks_sentences->initialize ();
        if ($check_sentence_structure) $checks_sentences->check ($verses_text);
        if ($check_paragraph_structure) $checks_sentences->paragraphs ($verses_text, $filter_text->paragraph_start_positions);
        $results = $checks_sentences->getResults ();
        foreach ($results as $result) {
          $verse = array_keys ($result);
          $verse = $verse [0];
          $database_check->recordOutput ($bible, $book, $chapter, $verse, $result[$verse]);
        }
      }
      if ($check_well_formed_usfm) {
        $checks_usfm->initialize ();
        $checks_usfm->check ($chapterUsfm);
        $checks_usfm->finalize ();
        $results = $checks_usfm->getResults ();
        foreach ($results as $result) {
          $verse = array_keys ($result);
          $verse = $verse [0];
          $database_check->recordOutput ($bible, $book, $chapter, $verse, $result[$verse]);
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

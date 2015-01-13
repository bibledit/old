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


require_once ("../bootstrap/bootstrap.php");


// Security: The script runs from the cli SAPI only.
Filter_Cli::assert ();


$database_logs = Database_Logs::getInstance ();
$database_config_general = Database_Config_General::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_check = Database_Check::getInstance ();
$database_users = Database_Users::getInstance ();
$database_mail = Database_Mail::getInstance ();
$database_books = Database_Books::getInstance ();


@$bible = Filter_Cli::argument (@$argv, 1);
if ($bible == "") {
  echo "No Bible given\n";
}


Database_Logs::log ("Check $bible: Start", Filter_Roles::translator ());


$database_check->truncateOutput ($bible);


$stylesheet = Database_Config_Bible::getExportStylesheet ($bible);


$check_double_spaces_usfm = Database_Config_Bible::getCheckDoubleSpacesUsfm ($bible);
$check_full_stop_in_headings = Database_Config_Bible::getCheckFullStopInHeadings ($bible);
$check_space_before_punctuation = Database_Config_Bible::getCheckSpaceBeforePunctuation ($bible);
$check_sentence_structure = Database_Config_Bible::getCheckSentenceStructure ($bible);
$check_paragraph_structure = Database_Config_Bible::getCheckParagraphStructure ($bible);
$checks_sentences = new Checks_Sentences ();
$checks_sentences->enterCapitals (Database_Config_Bible::getSentenceStructureCapitals ($bible));
$checks_sentences->enterSmallLetters (Database_Config_Bible::getSentenceStructureSmallLetters ($bible));
$end_marks = Database_Config_Bible::getSentenceStructureEndPunctuation ($bible);
$checks_sentences->enterEndMarks ($end_marks);
$center_marks = Database_Config_Bible::getSentenceStructureMiddlePunctuation ($bible);
$checks_sentences->enterCenterMarks ($center_marks);
$checks_sentences->enterDisregards (Database_Config_Bible::getSentenceStructureDisregards ($bible));
$checks_sentences->enterNames (Database_Config_Bible::getSentenceStructureNames ($bible));
$check_versification = Database_Config_Bible::getCheckChaptesVersesVersification ($bible);
$check_well_formed_usfm = Database_Config_Bible::getCheckWellFormedUsfm ($bible);
$checks_usfm = new Checks_Usfm ($bible);
$check_missing_punctuation_end_verse = Database_Config_Bible::getCheckMissingPunctuationEndVerse ($bible);
$check_patterns = Database_Config_Bible::getCheckPatterns ($bible);
$checking_patterns = Database_Config_Bible::getCheckingPatterns ($bible);
$checking_patterns = Filter_String::string2array ($checking_patterns);
$checking_patterns = array_filter ($checking_patterns, 'strlen');


$books = request->database_bibles()->getBooks ($bible);
if ($check_versification) Checks_Versification::books ($bible, $books);


for ($books as $book) {


  $chapters = request->database_bibles()->getChapters ($bible, $book);
  if ($check_versification) Checks_Versification::chapters (bible, book, chapters);


  for ($chapters as $chapter) {
    $chapterUsfm = request->database_bibles()->getChapter (bible, book, chapter);


    $verses = usfm_get_verse_numbers ($chapterUsfm);
    if ($check_versification) Checks_Versification::verses (bible, book, chapter, $verses);


    for ($verses as $verse) {
      $verseUsfm = usfm_get_verse_text ($chapterUsfm, $verse);
      if ($check_double_spaces_usfm) {
        Checks_Space::doubleSpaceUsfm (bible, book, chapter, $verse, $verseUsfm);
      }
    }


    $filter_text = new Filter_Text ($bible);
    $filter_text->initializeHeadingsAndTextPerVerse ();
    $filter_text->addUsfmCode ($chapterUsfm);
    $filter_text->run ($stylesheet);
    $verses_headings = $filter_text->verses_headings;
    $verses_text = $filter_text->getVersesText ();
    if ($check_full_stop_in_headings) {
      Checks_Headers::noPunctuationAtEnd (bible, book, chapter, $verses_headings, $center_marks, $end_marks);
    }
    if ($check_space_before_punctuation) {
      Checks_Space::spaceBeforePunctuation (bible, book, chapter, $verses_text);
    }


    if ($check_sentence_structure || $check_paragraph_structure) {
      $checks_sentences->initialize ();
      if ($check_sentence_structure) $checks_sentences->check ($verses_text);
      if ($check_paragraph_structure) $checks_sentences->paragraphs ($verses_text, $filter_text->paragraph_start_positions);
      $results = $checks_sentences->getResults ();
      for ($results as $result) {
        $verse = array_keys ($result);
        $verse = $verse [0];
        $database_check->recordOutput (bible, book, chapter, $verse, $result[$verse]);
      }
    }


    if ($check_well_formed_usfm) {
      $checks_usfm->initialize ();
      $checks_usfm->check ($chapterUsfm);
      $checks_usfm->finalize ();
      $results = $checks_usfm->getResults ();
      for ($results as $result) {
        $verse = array_keys ($result);
        $verse = $verse [0];
        $database_check->recordOutput (bible, book, chapter, $verse, $result[$verse]);
      }
    }


    if ($check_missing_punctuation_end_verse) {
      Checks_Verses::missingPunctuationAtEnd (bible, book, chapter, $verses_text, $center_marks, $end_marks);
    }


    if ($check_patterns) {
      Checks_Verses::patterns (bible, book, chapter, $verses_text, $checking_patterns);
    }

  }
}


// Identifier for this $bible.
$bibleID = request->database_bibles()->getID ($bible);


// Create an email with the checking results for this $bible.
$emailBody = array ();
$hits = $database_check->getHits ();
for ($hits as $hit) {
  if ($hit['bible'] == $bibleID) {
    $passage = filter_passage_display_inline (array (array ($hit['book'], $hit['chapter'], $hit['verse'])));
    $data = filter_string_sanitize_html ($hit['data']);
    $result = "<p>$passage $data</p>";
    $emailBody [] = $result;
  }
}


// Send email to users with write access to the Bible and a subscription to the notification.
if (count ($emailBody) > 0) {
  $subject = gettext("Bible Checks") . " " . $bible;
  $emailBody = implode ("\n", $emailBody);
  $users = $database_users->getUsers ();
  for ($users as $user) {
    if (request->database_config_user()->getUserBibleChecksNotification ($user)) {
      if (access_bible_write ($bible, $user)) {
        if (!config_logic_client_enabled ()) $database_mail->send ($user, $subject, $emailBody);
      }
    }
  }
}


Database_Logs::log ("Check $bible: Complete", Filter_Roles::translator ());


?>

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


$database_logs = Database_Logs::getInstance ();


$bible = Filter_Cli::argument (@$argv, 1);
$compare = Filter_Cli::argument (@$argv, 2);
$jobId = Filter_Cli::argument (@$argv, 3);


$database_logs->log (Locale_Translate::_("Comparing Bibles") . " $bible " . Locale_Translate::_ ("and") . " $compare", Filter_Roles::CONSULTANT_LEVEL);


$database_config_bible = Database_Config_Bible::getInstance ();
$database_jobs = Database_Jobs::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_usfmresources = Database_UsfmResources::getInstance ();
$database_books = Database_Books::getInstance ();


$stylesheet = $database_config_bible->getExportStylesheet ($bible);


$database_jobs->setProgress ($jobId, Locale_Translate::_("The Bibles are being compared..."));


// The results of the comparison. Will be displayed to the user.
$result = array ();
$result [] = Locale_Translate::_("Bible") . " " . $bible . " " . Locale_Translate::_ ("has been compared with") . " " . $compare . ".";
$result [] = Locale_Translate::_("Additions are in bold.") . " " . Locale_Translate::_ ("Removed words are in strikethrough.");
$result [] = "";


// Get the combined books in both Bibles/Resources.
$bibleBooks = $database_bibles->getBooks ($bible);
$compareBooks = $database_bibles->getBooks ($compare);
$resourceBooks = $database_usfmresources->getBooks ($compare);
$books = array_merge ($bibleBooks, $compareBooks, $resourceBooks);
$books = array_unique ($books);
$books = array_values ($books);


// Results of comparison of raw USFM.
$raw = array ();


// Absent books / chapters.
$absent = array ();


// The new verses as in the $bible.
$new = array ();


foreach ($books as $book) {


  $bookName = $database_books->getEnglishFromId ($book);
  $database_jobs->setProgress ($jobId, $bookName);


  if (!in_array ($book, $bibleBooks)) {
    $absent [] = Locale_Translate::_("Bible") . " " . $bible . " " . Locale_Translate::_ ("does not contain") . " $bookName.";
    continue;
  }
  
  if (!in_array ($book, $compareBooks) && !in_array ($book, $resourceBooks)) {
    $absent [] = Locale_Translate::_("Bible/Resource") . " " . $compare . " " . Locale_Translate::_ ("does not contain") . " $bookName.";
    continue;
  }


  $bibleChapters = $database_bibles->getChapters ($bible, $book);
  $compareChapters = $database_bibles->getChapters ($compare, $book);
  $resourceChapters = $database_usfmresources->getChapters ($compare, $book);
  $chapters = array_merge ($bibleChapters, $compareChapters, $resourceChapters);
  $chapters = array_unique ($chapters);
  $chapters = array_values ($chapters);

 
  foreach ($chapters as $chapter) {


    if (!in_array ($chapter, $bibleChapters)) {
      $absent [] = Locale_Translate::_("Bible") . " " . $bible . " " . Locale_Translate::_ ("does not contain") . " $bookName $chapter.";
      continue;
    }
    
    if (!in_array ($chapter, $compareChapters) && !in_array ($chapter, $resourceChapters)) {
      $absent [] = Locale_Translate::_("Bible/Resource") . " " . $compare . " " . Locale_Translate::_ ("does not contain") . " $bookName $chapter.";
      continue;
    }
    
    
    $bible_chapter_usfm = $database_bibles->getChapter ($bible, $book, $chapter);
    $compare_chapter_usfm = $database_bibles->getChapter ($compare, $book, $chapter);
    if ($compare_chapter_usfm == "") {
      $compare_chapter_usfm = $database_usfmresources->getUsfm ($compare, $book, $chapter);
    }


    $bible_verse_numbers = Filter_Usfm::getVerseNumbers ($bible_chapter_usfm);
    $compare_verse_numbers = Filter_Usfm::getVerseNumbers ($compare_chapter_usfm);
    $verses = array_merge ($bible_verse_numbers, $compare_verse_numbers);
    $verses = array_unique ($verses);
    sort ($verses, SORT_NUMERIC);


    foreach ($verses as $verse) {
      $bible_verse_usfm = Filter_Usfm::getVerseText ($bible_chapter_usfm, $verse);
      $compare_verse_usfm = Filter_Usfm::getVerseText ($compare_chapter_usfm, $verse);
      if ($bible_verse_usfm != $compare_verse_usfm) {
        $filter_text_bible = new Filter_Text ($bible);
        $filter_text_compare = new Filter_Text ($compare);
        $filter_text_bible->html_text_standard = new Html_Text ("");
        $filter_text_compare->html_text_standard = new Html_Text ("");
        $filter_text_bible->text_text = new Text_Text ();
        $filter_text_compare->text_text = new Text_Text ();
        $filter_text_bible->addUsfmCode ($bible_verse_usfm);
        $filter_text_compare->addUsfmCode ($compare_verse_usfm);
        $filter_text_bible->run ($stylesheet);
        $filter_text_compare->run ($stylesheet);
        $bible_html = $filter_text_bible->html_text_standard->getInnerHtml ();
        $compare_html = $filter_text_compare->html_text_standard->getInnerHtml ();
        $bible_text = $filter_text_bible->text_text->get ();
        $compare_text = $filter_text_compare->text_text->get ();
        if ($bible_text != $compare_text) {
          $modification = Filter_Diff::diff ($compare_text, $bible_text);
          $result [] = Filter_Books::passageDisplay ($book, $chapter, $verse) . " " . $modification;
          $new [] = Filter_Books::passageDisplay ($book, $chapter, $verse) . " " . $bible_text;
        }
        $modification = Filter_Diff::diff ($compare_verse_usfm, $bible_verse_usfm);
        $raw [] = Filter_Books::passageDisplay ($book, $chapter, $verse) . " " . $modification;
      }
    }


  }


}


// Add the absent books / chapters to the comparison.
if (count ($absent)) {
  $result [] = "";
  $result = array_merge ($result, $absent);
}

// Add any differences in the raw USFM to the comparison.
if (count ($raw)) {
  $result [] = "";
  $result = array_merge ($result, $raw);
}

// Add the text of the new verses, as they are in the $bible.
if (count ($new)) {
  $result [] = "";
  $result [] = Locale_Translate::_("The texts as they are in the Bible") . " " . $bible;
  $result [] = "";
  $result = array_merge ($result, $new);
}

// Format and store the result of the comparison.
foreach ($result as &$line) {
  if ($line == "") {
    $line = "<br>";
  } else {
    $line = "<p>$line</p>";
  }
}
$result = implode ("\n", $result);
$database_jobs->setResult ($jobId, $result);


$database_logs->log (Locale_Translate::_("Comparison is ready"), Filter_Roles::CONSULTANT_LEVEL);


?>

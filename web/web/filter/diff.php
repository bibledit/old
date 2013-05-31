<?php


class Filter_Diff
{


  /**
  * This filter produces files in USFM, html and text format.
  * The text files are to be used for showing the differences between them.
  * The files contain all verses that differ.
  * $bibleIdentifier: The Bible identifier to go through.
  * $directory: The existing directory where to put the files.
  * Two files are created: verses_old.usfm and verses_new.usfm.
  * The book chapter.verse precede each verse.
  */
  public static function produceVerseLevel ($bibleIdentifier, $directory)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_books = Database_Books::getInstance ();
    $database_config_general = Database_Config_General::getInstance ();
    $stylesheet = $database_config_general->getExportStylesheet ();

    $old_vs_usfm = array ();
    $new_vs_usfm = array ();
    
    $filter_text_old = new Filter_Text ("");
    $filter_text_old->html_text_standard = new Html_Text (gettext ("Bible"));
    $filter_text_old->text_text = new Text_Text ();
    $filter_text_new = new Filter_Text ("");
    $filter_text_new->html_text_standard = new Html_Text (gettext ("Bible"));
    $filter_text_new->text_text = new Text_Text ();
    
    $books = $database_bibles->getDiffBooks ($bibleIdentifier); // Todo transfer this to changes.php
    foreach ($books as $book) {
      $bookname = $database_books->getEnglishFromId ($book);
      $chapters = $database_bibles->getDiffChapters ($bibleIdentifier, $book);
      foreach ($chapters as $chapter) {
        // Go through the combined verse numbers in the old and new chapter.
        $old_chapter_usfm = $database_bibles->getDiff ($bibleIdentifier, $book, $chapter);
        $new_chapter_usfm = $database_bibles->getChapter ($bibleIdentifier, $book, $chapter);
        $old_verse_numbers = Filter_Usfm::getVerseNumbers ($old_chapter_usfm);
        $new_verse_numbers = Filter_Usfm::getVerseNumbers ($new_chapter_usfm);
        $verses = array_merge ($old_verse_numbers, $new_verse_numbers);
        $verses = array_unique ($verses);
        sort ($verses, SORT_NUMERIC);
        foreach ($verses as $verse) {
          $old_verse_text = Filter_Usfm::getVerseText ($old_chapter_usfm, $verse);
          $new_verse_text = Filter_Usfm::getVerseText ($new_chapter_usfm, $verse);
          if ($old_verse_text != $new_verse_text) {
            $usfmCode = "\\p $bookname $chapter.$verse $old_verse_text";
            $old_vs_usfm [] = $usfmCode;
            $filter_text_old->addUsfmCode ($usfmCode);
            $usfmCode = "\\p $bookname $chapter.$verse $new_verse_text";
            $new_vs_usfm [] = $usfmCode;
            $filter_text_new->addUsfmCode ($usfmCode);
          }
        }
      }
    }
    
    file_put_contents ("$directory/verses_old.usfm", implode ("\n", $old_vs_usfm));
    file_put_contents ("$directory/verses_new.usfm", implode ("\n", $new_vs_usfm));
    $filter_text_old->run ($stylesheet);
    $filter_text_new->run ($stylesheet);
    $filter_text_old->html_text_standard->save ("$directory/verses_old.html");
    $filter_text_new->html_text_standard->save ("$directory/verses_new.html");
    $filter_text_old->text_text->save ("$directory/verses_old.txt");
    $filter_text_new->text_text->save ("$directory/verses_new.txt");
  }


  /**
  * This filter runs wdiff.
  * $oldfile: The name of the old file for input.
  * $newfile: The name of the new file for input.
  * $outputfile: The name of the output file
  * Notes:
  * wdiff does not handle UTF8 properly.
  * Possible fix: 
  * Let PHP split the text in words. Then run ordinary diff. PHP assembles the words again into lines.
  */
  public static function runWDiffFile ($oldfile, $newfile, $outputfile)
  {
    $database_logs = Database_Logs::getInstance ();
    
    $oldfile = escapeshellarg ($oldfile);
    $newfile = escapeshellarg ($newfile);
    $escapedoutputfile = escapeshellarg ($outputfile);
    $command = "wdiff --start-insert='<span style=\"font-weight: bold;\">' --end-insert='</span>' --start-delete='<span style=\"text-decoration: line-through;\">' --end-delete='</span>' $oldfile $newfile > $escapedoutputfile 2>&1";

    $database_logs->log ("changes: $command", true);
    exec ($command, $output, $return_var);
    foreach ($output as $line) {
      $database_logs->log ("changes: $line", true);
    }
    
    $contents = file_get_contents ($outputfile);
    $contents = explode ("\n", $contents);
    foreach ($contents as &$line) {
      $line = "<p>" . $line . "</p>";
    }
    $contents = implode ("\n", $contents);
    file_put_contents ($outputfile, $contents);
  }


  /**
  * This filter returns the diff of two input strngs.
  * $oldstring: The old string for input.
  * $newstring: The new string for input.
  * The function returns the differences marked.
  */
  public static function diff ($oldstring, $newstring) // Todo
  {
    include_once ("finediff.php");
    // FineDiff::$paragraphGranularity
    // FineDiff::$sentenceGranularity
    // FineDiff::$wordGranularity
    // FineDiff::$characterGranularity
    $diff = new FineDiff ($oldstring, $newstring, FineDiff::$characterGranularity);
    $rendering = $diff->renderDiffToHTML ();
    $search = array ("<ins>", "</ins>", "<del>", "</del>");
    $replace = array ("<span style=\"font-weight: bold;\">", 
                      "</span>",
                      "<span style=\"text-decoration: line-through;\">",
                      "</span>");
    $html = str_replace ($search, $replace, $rendering);
    return $html;
  }


}

?>

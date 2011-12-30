<?php

class Filter_Diff
{

  /**
  * This filter produces two USFM files to be used for showing the differences between them.
  * The files contain the chapters that differ.
  * $bibleIdentifier: The Bible identifier to go through.
  * $directory: The existing directory where to put the files.
  * Two files are created: chapters_old.usfm and chapters_new.usfm.
  * The name of the Bible is stated at the top of the files.
  * The name of the book and the chapter number precede each chapter.
  */
  public function produceUsfmChapterLevel ($bibleIdentifier, $directory)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_books = Database_Books::getInstance ();
    $database_config_general = Database_Config_General::getInstance ();
    $stylesheet = $database_config_general->getExportStylesheet ();
    
    $old_ch_usfm = array ();
    $new_ch_usfm = array ();

    $filter_text_old = new Filter_Text;
    $filter_text_new = new Filter_Text;
    
    $books = $database_bibles->getDiffBooks ($bibleIdentifier);
    foreach ($books as $book) {
      $bookname = $database_books->getEnglishFromId ($book);
      $chapters = $database_bibles->getDiffChapters ($bibleIdentifier, $book);
      foreach ($chapters as $chapter) {
        $usfmCode = "\\mt2 $bookname $chapter";
        $old_ch_usfm [] = $usfmCode;
        $new_ch_usfm [] = $usfmCode;
        $filter_text_old->addUsfmCode ($usfmCode);
        $filter_text_new->addUsfmCode ($usfmCode);
        $old_chapter_text = explode ("\n", $database_bibles->getDiff ($bibleIdentifier, $book, $chapter));
        foreach ($old_chapter_text as $line) {
          $old_ch_usfm [] = $line;
          $filter_text_old->addUsfmCode ($line);
        }
        $new_chapter_text = explode ("\n", $database_bibles->getChapter ($bibleIdentifier, $book, $chapter));
        foreach ($new_chapter_text as $line) {
          $new_ch_usfm [] = $line;
          $filter_text_new->addUsfmCode ($line);
        }
      }
    }
    
    file_put_contents ("$directory/chapters_old.usfm", implode ("\n", $old_ch_usfm));
    file_put_contents ("$directory/chapters_new.usfm", implode ("\n", $new_ch_usfm));
    $filter_text_old->run ($stylesheet);
    $filter_text_new->run ($stylesheet);
    $filter_text_old->html_text_standard->save ("$directory/chapters_old.html");
    $filter_text_new->html_text_standard->save ("$directory/chapters_new.html");
  }
  

  /**
  * This filter produces USFM and html files to be used for showing the differences between them.
  * The files contain all verses that differ.
  * $bibleIdentifier: The Bible identifier to go through.
  * $directory: The existing directory where to put the files.
  * Two files are created: verses_old.usfm and verses_new.usfm.
  * The book chapter.verse precede each verse.
  */
  public function produceVerseLevel ($bibleIdentifier, $directory)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_books = Database_Books::getInstance ();
    $database_config_general = Database_Config_General::getInstance ();
    $stylesheet = $database_config_general->getExportStylesheet ();

    $old_vs_usfm = array ();
    $new_vs_usfm = array ();
    
    $filter_text_old = new Filter_Text;
    $filter_text_new = new Filter_Text;
    
    $books = $database_bibles->getDiffBooks ($bibleIdentifier);
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
  }


  /**
  * This filter runs wdiff.
  * $oldfile: The name of the old file for input.
  * $newfile: The name of the new file for input.
  * $outputfile: The name of the output file
  */
  public function runWDiff ($oldfile, $newfile, $outputfile)
  {
    $database_logs = Database_Logs::getInstance ();
    $oldfile = escapeshellarg ($oldfile);
    $newfile = escapeshellarg ($newfile);
    $outputfile = escapeshellarg ($outputfile);
    $command = "wdiff --start-insert='<span style=\"font-weight: bold;\">' --end-insert='</span>' --start-delete='<span style=\"text-decoration: line-through;\">' --end-delete='</span>' $oldfile $newfile > $outputfile 2>&1";
    $database_logs->log ($command, true);
    exec ($command, $output, &$return_var);
    foreach ($output as $line) {
      $database_logs->log ($line, true);
    }
  }


  /**
  * This filter inserts links at the top of the html file.
  * $htmlfile: The html to operate on.
  * $links: array of array of links and texts.
  */
  public function insertLinks ($htmlfile, $links)
  {
    $contents = file_get_contents ($htmlfile);
    if ($contents !== false) {
      $htmlcode = "<p>";
      foreach ($links as $link) {
        $url = $link[0];
        $txt = $link[1];
        if ($url != "") $htmlcode .= " <a href=\"$url\">$txt</a> ";
        else $htmlcode .= " $txt ";
      }
      $htmlcode .= "</p>\n";
      $htmlcode .= $contents;
      file_put_contents ($htmlfile, $htmlcode);
    }
  }


}

?>

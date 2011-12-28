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
  * This filter copies the Daisy Diff libraries to $directory.
  * These libraries consist of the stylesheet, and the DoJo libraries, and the Javascript libraries.
  */
  public function copyDaisyDiffLibraries ($directory) // Todo goes out.
  {
    $daisydiffdirectory = dirname (dirname (__FILE__)) . "/daisydiff";
    $daisydiffdirectory = escapeshellarg ($daisydiffdirectory);
    $directory = escapeshellarg ($directory);
    $command = "cd $daisydiffdirectory && cp -r css images js $directory 2>&1";
    exec ($command, $output, &$return_var);
  }


  /**
  * This filter runs Daisy Diff.
  * $oldfile: The old file for input.
  * $newfile: The new file for input.
  * $outputfile: The output file
  */
  public function runDaisyDiff ($oldfile, $newfile, $outputfile) // Todo probably to be renamed.
  {
    $database_logs = Database_Logs::getInstance ();
    $workingdirectory = dirname (dirname (__FILE__)) . "/daisydiff";
    $workingdirectory = escapeshellarg ($workingdirectory);
    $oldfile = escapeshellarg ($oldfile);
    $newfile = escapeshellarg ($newfile);
    $outputfile = escapeshellarg ($outputfile);
    $command = "wdiff --start-insert='<span style=\"font-weight: bold;\">' --end-insert='</span>' --start-delete='<span style=\"text-decoration: line-through;\">' --end-delete='</span>' $newfile $oldfile > $outputfile 2>&1";
    $database_logs->log ($command, true);
    exec ($command, $output, &$return_var);
    foreach ($output as $line) {
      $database_logs->log ($line, true);
    }
  }


  /**
  * This filter sets the two main titles for a html file generated by the Daisy Diff software.
  * $htmlfile: The html to operate on.
  * $title1, $title2: The two titles.
  */
  public function setDaisyDiffTitle ($htmlfile, $title1, $title2) // Todo look how to implement the title in the new situation.
  {
    $contents = file_get_contents ($htmlfile);
    if ($contents !== false) {
      $contents = str_replace ("Daisy Diff", $title1, $contents);
      $contents = str_replace ("http://code.google.com/p/daisydiff/", "", $contents);
      $contents = str_replace ("compare report.", $title2, $contents);
      file_put_contents ($htmlfile, $contents);
    }
  }


  /**
  * This filter sets the subtitle for the html file generated by the Daisy Diff software.
  * $htmlfile: The html to operate on.
  * $subtitle: The subtitle.
  */
  public function setDaisyDiffSubtitle ($htmlfile, $subtitle) // Todo See how to get the subtitle done.
  {
    $contents = file_get_contents ($htmlfile);
    if ($contents !== false) {
      $contents = str_replace ("Click on the changed parts for a detailed description. Use the left and right arrow keys to walk through the modifications.", $subtitle, $contents);
      file_put_contents ($htmlfile, $contents);
    }
  }


  /**
  * This filter integrates the external Daisy Diff stylesheet into the html file.
  * $htmlfile: The html to operate on.
  * $addedBold: When true, the style for added text is made bold.
  * (Bold is useful when printing the changes, since colour is not visible on monochrome printers.)
  */
  public function integrateDaisyDiffStylesheet ($htmlfile, $addedBold) // Todo this probably is no longer needed - check it out.
  {
    $contents = file_get_contents ($htmlfile);
    if ($contents !== false) {
      $styledata = file_get_contents (dirname (dirname (__FILE__)) . "/daisydiff/css/diff.css");
      if ($styledata !== false) {
        if ($addedBold) {
          $styledata = str_replace ("span.diff-html-added {", "span.diff-html-added {\n  font-weight: bolder;", $styledata);
        }
        $contents = str_replace ("<link href=\"css/diff.css\" type=\"text/css\" rel=\"stylesheet\">", "<style type=\"text/css\">\n$styledata\n</style>", $contents);
        file_put_contents ($htmlfile, $contents);
      }
    }
  }


  /**
  * This removes javascript from the html file generated by the Daisy Diff software.
  * Javascript may cause email to be flagged a spam.
  * $htmlfile: The html file to operate on.
  */
  public function removeDaisyDiffJavascript ($htmlfile) // Todo this probably goes out altogether.
  {
    $contents = file_get_contents ($htmlfile);
    if ($contents !== false) {
      $contents = preg_replace ('/(<script.+?)+(<\/script>)/i', '', $contents);
      $contents = str_replace ('<script>', "", $contents);
      $contents = str_replace ('htmlDiffInit();', "", $contents);
      $contents = str_replace ('</script>', "", $contents);
      file_put_contents ($htmlfile, $contents);
    }
  }


  /**
  * This removes the left and the right arrows and their associated data 
  * from the html file generated by the Daisy Diff software.
  * $htmlfile: The html file to operate on.
  */
  public function removeDaisyDiffArrows ($htmlfile) // Todo this probably goes out altogether.
  {
    $contents = file_get_contents ($htmlfile);
    if ($contents !== false) {
      $contents = preg_replace ('/(<script.+?)+(<\/script>)/i', '', $contents);
      $contents = str_replace ('<img class="diff-icon" src="images/diff-first.gif" title="Go to first change.">', "", $contents);
      $contents = str_replace ('<img class="diff-icon" src="images/diff-last.gif" title="Go to last change.">', "", $contents);
      $contents = str_replace ('&nbsp;first', "", $contents);
      $contents = str_replace ('last&nbsp;', "", $contents);
      file_put_contents ($htmlfile, $contents);
    }
  }


  /**
  * This filter inserts links at the top of the html file.
  * $htmlfile: The html to operate on.
  * $links: array of array of links and texts.
  */
  public function insertLinks ($htmlfile, $links) // Todo see to this one.
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

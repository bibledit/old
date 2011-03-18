<?php

class Filter_Diff
{

  /**
  * This filter produces two USFM files to be used for showing the differences between them.
  * The files contain all chapters that differ.
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
    
    $old_ch_usfm = array ();
    $new_ch_usfm = array ();

    $books = $database_bibles->getDiffBooks ($bibleIdentifier);
    foreach ($books as $book) {
      $bookname = $database_books->getEnglishFromId ($book);
      $chapters = $database_bibles->getDiffChapters ($bibleIdentifier, $book);
      foreach ($chapters as $chapter) {
        $old_ch_usfm [] = "<h2>$bookname $chapter</h2>";
        $old_chapter_text = explode ("\n", $database_bibles->getDiff ($bibleIdentifier, $book, $chapter));
        foreach ($old_chapter_text as $line) {
          $old_ch_usfm [] = "<p>$line</p>";
        }
        $new_ch_usfm [] = "<h2>$bookname $chapter</h2>";
        $new_chapter_text = explode ("\n", $database_bibles->getChapter ($bibleIdentifier, $book, $chapter));
        foreach ($new_chapter_text as $line) {
          $new_ch_usfm [] = "<p>$line</p>";
        }
      }
    }
    
    file_put_contents ("$directory/chapters_old.usfm", implode ("\n", $old_ch_usfm));
    file_put_contents ("$directory/chapters_new.usfm", implode ("\n", $new_ch_usfm));
  }
  

  /**
  * This filter produces two USFM files to be used for showing the differences between them.
  * The files contain all verses that differ.
  * $bibleIdentifier: The Bible identifier to go through.
  * $directory: The existing directory where to put the files.
  * Two files are created: verses_old.usfm and verses_new.usfm.
  * The name of the Bible is stated at the top of the files.
  * The name of the book and the chapter number and verse number precede each verse.
  */
  public function produceUsfmVerseLevel ($bibleIdentifier, $directory)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_books = Database_Books::getInstance ();
    
    $old_vs_usfm = array ();
    $new_vs_usfm = array ();
    
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
            $old_vs_usfm [] = "<p>$bookname $chapter.$verse $old_verse_text</p>";
            $new_vs_usfm [] = "<p>$bookname $chapter.$verse $new_verse_text</p>";
          }
        }
      }
    }
    
    file_put_contents ("$directory/verses_old.usfm", implode ("\n", $old_vs_usfm));
    file_put_contents ("$directory/verses_new.usfm", implode ("\n", $new_vs_usfm));
  }


  /**
  * This filter copies the Daisy Diff libraries to $directory.
  * These libraries consist of the stylesheet, and the DoJo libraries, and the Javascript libraries.
  */
  public function copyDaisyDiffLibraries ($directory)
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
  public function runDaisyDiff ($oldfile, $newfile, $outputfile)
  {
    $database_logs = Database_Logs::getInstance ();
    $workingdirectory = dirname (dirname (__FILE__)) . "/daisydiff";
    $workingdirectory = escapeshellarg ($workingdirectory);
    $oldfile = escapeshellarg ($oldfile);
    $newfile = escapeshellarg ($newfile);
    $outputfile = escapeshellarg ($outputfile);
    $command = "cd $workingdirectory && java -jar daisydiff.jar $oldfile $newfile --file=$outputfile 2>&1";
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
  public function setDaisyDiffTitle ($htmlfile, $title1, $title2)
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
  public function setDaisyDiffSubtitle ($htmlfile, $subtitle)
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
  public function integrateDaisyDiffStylesheet ($htmlfile, $addedBold)
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
  public function removeDaisyDiffJavascript ($htmlfile)
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
  public function removeDaisyDiffArrows ($htmlfile)
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
  public function insertLinks ($htmlfile, $links) // Todo
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
      $contents = "$htmlcode</p>\n$contents";
      file_put_contents ($htmlfile, $contents);
    }
  }


}

?>

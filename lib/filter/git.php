<?php
class Filter_Git
{


  // This filter takes the Bible data as it is stored in Bibledit's database,
  // and puts this information into the layout in books and chapters
  // such as is used in Bibledit-Gtk into the $git folder.
  // The $git is a git repository, and may contain other data as well.
  // The filter focuses on reading the data in the git repository, and only writes to it if necessary,
  // This speeds up the filter.
  public static function syncBible2Git ($bible, $git, $progress = false)
  {
    $success = true;

    $database_bibles = Database_Bibles::getInstance ();
    $database_books = Database_Books::getInstance ();

    // First stage.
    // Read the chapters in the git repository,
    // and check if they occur in the database.
    // If a chapter is not in the database, remove it from the repository.
    $books = $database_bibles->getBooks ($bible);
    for (new DirectoryIterator ($git) as $fileInfo) {
      if ($fileInfo->isDot ()) continue;
      if ($fileInfo->isDir ()) {
        $bookname = $fileInfo->getFilename ();
        $book = Database_Books::getIdFromEnglish ($bookname);
        if ($book) {
          if (in_array ($book, $books)) {
            // Book exists in the database: Check the chapters.
            $chapters = $database_bibles->getChapters ($bible, $book);
            for (new DirectoryIterator ("$git/$bookname") as $fileInfo2) {
              if ($fileInfo2->isDot ()) continue;
              if ($fileInfo2->isDir ()) {
                $chapter = $fileInfo2->getFilename ();
                if (is_numeric ($chapter)) {
                  $filename = "$git/$bookname/$chapter/data";
                  if (file_exists ($filename)) {
                    if (!in_array ($chapter, $chapters)) {
                      // Chapter does not exist in the database.
                      filter_url_rmdir ("$git/$bookname/$chapter");
                    }
                  }
                }
              }
            }
          } else {
            // Book does not exist in the database: Remove it from $git.
            filter_url_rmdir ("$git/$bookname");
          }
        }
      }
    }

    // Second stage.
    // Read the books / chapters from the database,
    // and check if they occur in the repository, and the data matches.
    // If necessary, save the chapter to the repository.
    $books = $database_bibles->getBooks ($bible);
    for ($books as $book) {
      $bookname = Database_Books::getEnglishFromId ($book);
      if ($progress) echo "$bookname ";
      $bookdir = "$git/$bookname";
      if (!file_exists ($bookdir)) mkdir ($bookdir);
      $chapters = $database_bibles->getChapters ($bible, $book);
      for ($chapters as $chapter) {
        $chapterdir = "$bookdir/$chapter";
        if (!file_exists ($chapterdir)) mkdir ($chapterdir);
        $datafile = "$chapterdir/data";
        @$contents = filter_url_file_get_contents ($datafile);
        $usfm = $database_bibles->getChapter ($bible, $book, $chapter);
        if ($contents != $usfm)filter_url_file_put_contents ($datafile, $usfm);
      }
    }
    if ($progress) echo "\n";
    return $success;
  }


  // This filter takes a $line of the output of the git pull command.
  // It tries to interpret it to find a passage that would have updated.
  // If a valid book and chapter are found, it returns them.
  // Else it returns NULL.
  public static function getPullPassage ($line)
  {
    // Sample lines:
    // "From https://github.com/joe/test"
    // "   443579b..90dcb57  master     -> origin/master"
    // "Updating 443579b..90dcb57"
    // "Fast-forward"
    // " Genesis/1/data | 2 +-"
    // " 1 file changed, 1 insertion(+), 1 deletion(-)"
    // " delete mode 100644 Leviticus/1/data"
    // " create mode 100644 Leviticus/2/data"
    $bits = explode ("/", $line);
    if (count ($bits) != 3) return NULL;
    $book = filter_string_trim ($bits [0]);
    $database_books = Database_Books::getInstance();
    $book = Database_Books::getIdFromEnglish ($book);
    if (!$book) return NULL;
    $chapter = $bits [1];
    if (!is_numeric ($chapter)) return NULL;
    $data = $bits [2];
    if (strpos ($data, "data") === false) return NULL;
    return array ('book' => $book, 'chapter' => $chapter);
  }
  

  // This filter takes the Bible data as it is stored in the $git folder,
  // and puts this information into Bibledit's database.
  // The $git is a git repository, and may contain other data as well.
  // The filter focuses on reading the data in the git repository and the database,
  // and only writes to the database if necessary,
  // This speeds up the filter.
  public static function syncGit2Bible ($git, $bible)
  {
    $success = true;

    $database_bibles = Database_Bibles::getInstance ();
    $database_books = Database_Books::getInstance ();
    $database_logs = Database_Logs::getInstance ();

    // Stage one:
    // Read the chapters in the git repository,
    // and check that they occur in the database.
    // If any does not occur, add the chapter to the database.
    // This stage does not check the contents of the chapters.
    $books = $database_bibles->getBooks ($bible);
    for (new DirectoryIterator ($git) as $fileInfo) {
      if ($fileInfo->isDot ()) continue;
      if ($fileInfo->isDir ()) {
        $bookname = $fileInfo->getFilename ();
        $book = Database_Books::getIdFromEnglish ($bookname);
        if ($book) {
          // Check the chapters.
          $chapters = $database_bibles->getChapters ($bible, $book);
          for (new DirectoryIterator ("$git/$bookname") as $fileInfo2) {
            if ($fileInfo2->isDot ()) continue;
            if ($fileInfo2->isDir ()) {
              $chapter = $fileInfo2->getFilename ();
              if (is_numeric ($chapter)) {
                $filename = "$git/$bookname/$chapter/data";
                if (file_exists ($filename)) {
                  if (!in_array ($chapter, $chapters)) {
                    // Chapter does not exist in the database: Add it.
                    $usfm = filter_url_file_get_contents ($filename);
                    Bible_Logic::storeChapter ($bible, $book, $chapter, $usfm);
                    Database_Logs::log (gettext("A translator added chapter") . " $bible $bookname $chapter");
                  }
                }
              }
            }
          }
        }
      }
    }


    // Stage two:
    // Read through the chapters in the database,
    // and check that they occur in the git folder.
    // If necessary, remove a chapter from the database.
    // If a chapter matches, check that the contents of the data in the git
    // folder and the contents in the database match.
    // If necessary, update the data in the database.
    $books = $database_bibles->getBooks ($bible);
    for ($books as $book) {
      $bookname = Database_Books::getEnglishFromId ($book);
      $bookdir = "$git/$bookname";
      if (file_exists ($bookdir)) {
        $chapters = $database_bibles->getChapters ($bible, $book);
        for ($chapters as $chapter) {
          $chapterdir = "$bookdir/$chapter";
          if (file_exists ($chapterdir)) {
            $datafile = "$chapterdir/data";
            $contents = filter_url_file_get_contents ($datafile);
            $usfm = $database_bibles->getChapter ($bible, $book, $chapter);
            if ($contents != $usfm) {
              Bible_Logic::storeChapter ($bible, $book, $chapter, $contents);
              Database_Logs::log (gettext("A translator updated chapter") . " $bible $bookname $chapter");
           }
          } else {
            Bible_Logic::deleteChapter ($bible, $book, $chapter);
            Database_Logs::log (gettext("A translator deleted chapter") . " $bible $bookname $chapter");
         }
        }
      } else {
        Bible_Logic::deleteBook ($bible, $book);
        Database_Logs::log (gettext("A translator deleted book") . " $bible $bookname");
      }
    }
  }


  // This filter takes one chapter of the Bible data as it is stored in the $git folder,
  // and puts this information into Bibledit's database.
  // The $git is a git repository, and may contain other data as well.
  public static function syncGitChapter2Bible ($git, $bible, $book, $chapter)
  {
    // The databases.
    $database_bibles = Database_Bibles::getInstance ();
    $database_books = Database_Books::getInstance ();
    $database_logs = Database_Logs::getInstance ();
    
    // Filename for the chapter.
    $bookname = Database_Books::getEnglishFromId ($book);
    $filename = "$git/$bookname/$chapter/data";
    
    if (file_exists ($filename)) {

      // Store chapter in database.
      $usfm = filter_url_file_get_contents ($filename);
      Bible_Logic::storeChapter ($bible, $book, $chapter, $usfm);
      Database_Logs::log (gettext("A collaborator updated") . " $bible $bookname $chapter");

    } else {

      // Delete chapter from database.
      Bible_Logic::deleteChapter ($bible, $book, $chapter);
      Database_Logs::log (gettext("A collaborator deleted chapter") . " $bible $bookname $chapter");

    }
  }


  // This function returns an array with all commits in the git repository in $directory.
  public static function commits ($directory)
  {
    $directory = escapeshellarg ($directory);
    $commits = array ();
    $command = "cd $directory; git log --all --pretty=format:%H 2>&1";
    unset ($result);
    exec ($command, $result, $exit_code);
    for ($result as $line) {
      $commit = explode (" ", $line);
      $commit = $commit [0];
      $commits [] = $commit;
    }
    return $commits;
  }


  // This function returns an array with files changed by commit $sha1 in git repository in $directory.
  public static function files ($directory, $sha1)
  {
    $directory = escapeshellarg ($directory);
    $files = array ();
    $command = "cd $directory; git diff-tree --no-commit-id --name-only -r $sha1 2>&1";
    unset ($result);
    exec ($command, $result, $exit_code);
    for ($result as $line) {
      $files [] = $line;
    }
    return $files;
  }


  // This function takes a $path as it occurs in the git repository as used in Bibledit,
  // and explodes it into book and chapter.
  // It returns an array of them.
  // If anything is invalid, it returns NULL.
  public static function explodePath ($path)
  {
    $data = basename ($path);
    if ($data == "data") {
      $path = dirname ($path);
      $chapter = basename ($path);
      if (is_numeric ($chapter)) {
        $book = dirname ($path);
        $database_books = Database_Books::getInstance ();
        $book = Database_Books::getIdFromEnglish ($book);
        if ($book != 0) {
          return array ('book' => $book, 'chapter' => $chapter);
        }
      }
    }
    return NULL;
  }


  // Returns the timestamp of commit $sha1 in git repository $directory.
  public static function timestamp ($directory, $sha1)
  {
    $directory = escapeshellarg ($directory);
    $command = "cd $directory; git show --pretty=format:'%at' $sha1 2>&1";
    unset ($result);
    exec ($command, $result, $exit_code);
    for ($result as $line) {
      return $line;
    }
    return time ();
  }


  // Returns the committer of a given $sha1 commit in the git repository in $directory.
  public static function committer ($directory, $sha1)
  {
    $directory = escapeshellarg ($directory);
    $command = "cd $directory; git show --pretty=format:'%cn %ce' $sha1 2>&1";
    unset ($result);
    exec ($command, $result, $exit_code);
    for ($result as $line) {
      return $line;
    }
    return "Unknown";
  }


  // Returns the old and new text of a commit $sha1 and file $path in git repo $directory.
  public static function changes ($directory, $sha1, $path)
  {
    $directory = escapeshellarg ($directory);
    $shapath = escapeshellarg ("$sha1:$path");
    $path = escapeshellarg ($path);

    // Get the new text.
    $newfile = tempnam (sys_get_temp_dir (), "");
    $command = "cd $directory; git show --all --pretty=format:%b $shapath > $newfile 2>&1";
    unset ($result);
    exec ($command, $result, $exit_code);
    $newtext = filter_url_file_get_contents ($newfile);
    $newtext = filter_string_trim ($newtext);

    // Get the patch.
    $patchfile = tempnam (sys_get_temp_dir (), "");
    $command = "cd $directory; git diff-tree --patch $sha1 $path > $patchfile 2>&1";
    unset ($result);
    exec ($command, $result, $exit_code);

    // Get the old text by reverse patching the new text.
    $command = "patch -R $newfile $patchfile 2>&1";
    unset ($result);
    exec ($command, $result, $exit_code);
    $oldtext = filter_url_file_get_contents ($newfile);
    $oldtext = filter_string_trim ($oldtext);

    // Clean up.
    unlink ($newfile);
    @unlink ("$newfile.orig");
    unlink ($patchfile);

    // Result.
    return array ('old' => $oldtext, 'new' => $newtext);
  }


}


?>

<?php
class Filter_Git
{




  // This filter takes a $line of the output of the git pull command.
  // It tries to interpret it to find a passage that would have updated.
  // If a valid book and chapter are found, it returns them.
  // Else it returns NULL.
  public static function getPullPassage ($line) C++Port may not be needed for libgit2.
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

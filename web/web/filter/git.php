<?php


class Filter_Git
{

  /**
  * This filter takes the Bible data as it is stored in a git repository, 
  * using the layout in books and chapters such as is used in Bibledit-Gtk,
  * and transfers this information into Bibledit-Web's Bible database.
  * The $directory is where the file data resides.
  * It overwrites whatever data was in the $bible in the database.
  */
  public function filedata2database ($directory, $bible)
  {
    // Maintain a list of chapters that were stored.
    $stored_chapters = array ();
    // Go through the Bible book names in $directory.
    $book_names = scandir($directory);
    $book_names = array_diff ($book_names, array ('.', '..'));
    $book_names = array_values ($book_names);
    $database_books = Database_Books::getInstance ();
    $database_bibles = Database_Bibles::getInstance ();
    $database_snapshots = Database_Snapshots::getInstance ();
    foreach ($book_names as $book_name) {
      $book_id = $database_books->getIdFromEnglish ($book_name);
      if ($book_id > 0) {
        $book_directory = "$directory/$book_name";
        // Go through the chapters in the book.
        $chapter_numbers = scandir($book_directory);
        $chapter_numbers = array_diff ($chapter_numbers, array ('.', '..'));
        $chapter_numbers = array_values ($chapter_numbers);
        foreach ($chapter_numbers as $chapter_number) {
          // Go ahead if $chapter_number is numerical.
          if (is_numeric ($chapter_number)) {
            // Store data into database, but only when the new data differs from existing data.
            // This reduces the number of snapshots created, and the time taken to complete the operation,
            // assuming that in most cases the old does not differ from the new, which is a reasonable assumption.
            $data = file_get_contents ("$directory/$book_name/$chapter_number/data");
            $old_data = $database_bibles->getChapter ($bible, $book_id, $chapter_number);
            if ($data != $old_data) {
              $database_bibles->storeChapter ($bible, $book_id, $chapter_number, $data);
              $database_snapshots->snapChapter ($bible, $book_id, $chapter_number, $data, false);
            }
            // List the chapter number.
            $stored_chapters [] = array ($book_id, $chapter_number);
          }
        }
      }
    }
    // Delete any chapters not stored above.
    $books = $database_bibles->getBooks ($bible);
    foreach ($books as $book) {
      $chapters = $database_bibles->getChapters ($bible, $book);
      foreach ($chapters as $chapter) {
        $item = array ($book, $chapter);
        if (!in_array ($item, $stored_chapters)) {
          $database_bibles->deleteChapter ($bible, $book, $chapter);
        }
      }
    }
  }


  /**
  * This filter takes the Bible data as it is stored in Bibledit-Web's database, 
  * and transfers this information into the layout in books and chapters
  * such as is used in Bibledit-Gtk into $directory.
  * The $directory is supposed to be completely empty, 
  * apart from a .git directory which may be there.
  */
  public function database2filedata ($bible, $directory)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_books = Database_Books::getInstance ();
    $books = $database_bibles->getBooks ($bible);
    foreach ($books as $book) {
      $book_name = $database_books->getEnglishFromId ($book);
      mkdir ("$directory/$book_name");
      $chapters = $database_bibles->getChapters ($bible, $book);
      foreach ($chapters as $chapter) {
        mkdir ("$directory/$book_name/$chapter");
        $data = $database_bibles->getChapter ($bible, $book, $chapter);
        file_put_contents ("$directory/$book_name/$chapter/data", $data);
      }
    }
  }


  /**
  * This filter takes a git repository as it is stored in .git
  * in $directory, and transfers this information into Bibledit-Web's
  * Repository database under name $bible.
  */
  public function repository2database ($directory, $bible)
  {
    // Put the .git directory in an uncompressed tar ball (-czf would compress it).
    $filename = "git.tar";
    system ("cd $directory; tar -cf $filename .git");
    $data = fread(fopen("$directory/$filename", "r"), filesize("$directory/$filename"));
    $database_repositories = Database_Repositories::getInstance();
    $database_repositories->storeRepository ($bible, $data);    
  }
  

  /**
  * This filter takes the git repository for $bible
  * as it is stored in Bibledit-Web's database, 
  * and puts it in a .git directory in $directory.
  * There should not be an existing .git directory in $directory,
  * but other relevant data may be there.
  */
  public function database2repository ($bible, $directory)
  {
    $database_repositories = Database_Repositories::getInstance();
    $data = $database_repositories->getRepository ($bible);
    $filename = "git.tar";
    file_put_contents ("$directory/$filename", $data);
    system ("cd $directory; tar -xf $filename");
    unlink ("$directory/$filename");
  }


  /**
  * Bibledit-Web accesses secure git repositories through ssh.
  * This function sets up both git and ssh to use a configuration file
  * and a private key.
  * The $url is the URL of the secure git repository.
  */
  public function git_config ($url)
  {
    // Temporal directory where to put the config files.
    $directory = tempnam (sys_get_temp_dir(), '');
    unlink ($directory);
    mkdir ($directory);

    // Extract the host name from the $url.
    $host = parse_url ($url, PHP_URL_HOST);

$config_data = <<<EOD
Host $host
     IdentityFile $directory/key
     BatchMode yes
     StrictHostKeyChecking no
EOD;
    file_put_contents ("$directory/config", $config_data);
    
    $database_config_general = Database_Config_General::getInstance();
    file_put_contents ("$directory/key", $database_config_general->getPrivateSshKey ());
    chmod ("$directory/key", 0600);

$git_ssh_data = <<<EOD
#!/bin/sh
exec ssh -F $directory/config $*
EOD;
    file_put_contents ("$directory/git_ssh", $git_ssh_data);
    chmod ("$directory/git_ssh", 0777);
    putenv ("GIT_SSH=$directory/git_ssh");

    return $directory;    
  }


  /**
  * Bibledit-Web accesses secure git repositories through ssh.
  * This function remove the private key set up by git_config
  */
  public function git_un_config ($directory)
  {
    unlink ("$directory/key");
  }
      
      

}


?>

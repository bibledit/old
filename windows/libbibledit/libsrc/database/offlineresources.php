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


// This database is not very resilient.
// It contains external data that can be replaced if it gets corrupted.
// It stores one book in a database file.
// This provides some measure of robustness.
// When downloading an external resource, there is one script per book, 
// so that only one script writes to the database at any time.


class Database_OfflineResources
{


  private static $instance;
  private function __construct() {
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_OfflineResources ();
    }
    return self::$instance;
  }


  private function mainFolder ()
  {
    $folder = realpath (__DIR__ . "/../databases/offlineresources");
    return $folder;
  }


  private function resourceFolder ($name)
  {
    $folder = $this->mainFolder () . "/$name";
    return $folder;
  }


  private function databaseFile ($name, $book)
  {
    $file = $this->resourceFolder ($name) . "/$book.sqlite";
    return $file;
  }


  public function delete ($name)
  {
    $path = $this->resourceFolder ($name);
    // If a folder: Delete it.
    if (is_dir ($path)) {
      Filter_Rmdir::rmdir ($path);
    }
    // If a file: Delete it.
    if (file_exists ($path)) {
      unlink ($path);
    }
  }
  
  
  private function connect ($file)
  {
    $db = new PDO ("sqlite:$file");
    return $db;
  }
  

  public function store ($name, $book, $chapter, $verse, $html)
  {
    $folder = $this->resourceFolder ($name);
    if (!file_exists ($folder)) mkdir ($folder, 0777, true);

    $db = null;
    $file = $this->databaseFile ($name, $book);
    
    $this->healthy ($file);

    if (!file_exists ($file)) {
      $db = $this->connect ($file);
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS offlineresources (
  chapter integer,
  verse integer,
  html text
);
EOD;
      Database_SQLite::exec ($db, $sql);
    }

    if ($db == null) $db = $this->connect ($file);

    $chapter = Database_SQLiteInjection::no ($chapter);
    $verse = Database_SQLiteInjection::no ($verse);
    $html = Database_SQLiteInjection::no ($html);
    
    $query = "DELETE FROM offlineresources WHERE chapter = $chapter AND verse = $verse;";
    Database_SQLite::exec ($db, $query);

    $query = "INSERT INTO offlineresources VALUES ($chapter, $verse, '$html');";
    Database_SQLite::exec ($db, $query);

    unset ($db);
  }


  public function exists ($name, $book, $chapter, $verse)
  {
    $file = $this->databaseFile ($name, $book);
    $exists = false;
    if (file_exists ($file)) {
      $db = $this->connect ($file);
      $chapter = Database_SQLiteInjection::no ($chapter);
      $verse = Database_SQLiteInjection::no ($verse);
      $query = "SELECT html FROM offlineresources WHERE chapter = $chapter AND verse = $verse;";
      $result = Database_SQLite::query ($db, $query);
      foreach ($result as $row) {
        $exists = true;
      }
      unset ($db);
    }
    return $exists;
  }


  // Counts the number of verses in the offline resource $name.
  public function count ($name)
  {
    $count = 0;
    $resourceFolder = $this->resourceFolder ($name);
    @$databases = scandir ($resourceFolder);
    if ($databases) {
      $databases = Filter_Folders::cleanup ($databases);
      foreach ($databases as $database) {
        if (strpos ($database, "sqlite") !== false) {
          $file = "$resourceFolder/$database";
          $db = $this->connect ($file);
          $query = "SELECT count(*) FROM offlineresources;";
          $result = Database_SQLite::query ($db, $query);
          foreach ($result as $row) {
            $count += $row [0];
          }
          unset ($db);
        }
      }
    }
    return $count;
  }


  public function get ($name, $book, $chapter, $verse)
  {
    $html = "";
    $file = $this->databaseFile ($name, $book);
    if (file_exists ($file)) {
      $db = $this->connect ($file);
      $chapter = Database_SQLiteInjection::no ($chapter);
      $verse = Database_SQLiteInjection::no ($verse);
      $query = "SELECT html FROM offlineresources WHERE chapter = $chapter AND verse = $verse;";
      $result = Database_SQLite::query ($db, $query);
      foreach ($result as $row) {
        $html = $row [0];
      }
      unset ($db);
    }
    return $html;
  }
  
  
  public function healthy ($file)
  {
    if (file_exists ($file)) {
      $healthy = false;
      $db = $this->connect ($file);
      $query = "PRAGMA integrity_check;";
      $result = Database_SQLite::query ($db, $query);
      foreach ($result as $row) {
        if ($row[0] == "ok") $healthy = true;
      }
      unset ($db);
      if (!$healthy) {
        unlink ($file);
      }
    }
  }
  
  
  // Return an array with the names of the available offline resources.
  public function names ()
  {
    $names = scandir ($this->mainFolder ());
    $names = Filter_Folders::cleanup ($names);
    return $names;
  }


  // Returns an array with the files in offline resource $name.
  public function files ($name)
  {
    $files = array ();
    $folder = $this->resourceFolder ($name);
    if (is_dir ($folder)) $files = scandir ($folder);
    $files = Filter_Folders::cleanup ($files);
    return $files;
  }
  
  
  // Returns the size of the file that contains resource $name and $file.
  public function size ($name, $file)
  {
    $file = $this->resourceFolder ($name) . "/$file";
    $size = 0;
    if (file_exists ($file)) $size = filesize ($file);
    return $size;
  }
  
  
  public function unlink ($name, $file)
  {
    $path = $this->resourceFolder ($name) . "/$file";
    // If a folder: Delete it.
    if (is_dir ($path)) {
      Filter_Rmdir::rmdir ($path);
    }
    // If a file: Delete it.
    if (file_exists ($path)) {
      unlink ($path);
    }
  }


  // Saves an entire database in resource $name as file $file.
  public function save ($name, $file, $contents)
  {
    $path = $this->resourceFolder ($name);
    if (!is_dir ($path)) mkdir ($path, 0777, true);
    $file = "$path/$file";
    file_put_contents ($file, $contents);
  }
  
  
  // Returns the raw contents of an entire database in resource $name file $file.
  public function load ($name, $file)
  {
    $path = $this->resourceFolder ($name) . "/$file";
    $contents = file_get_contents ($path);
    return $contents;
  }


}


?>

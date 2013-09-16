<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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


class Database_ChangesUser // Todo
{

  private static $instance;
  private function __construct () {
  } 
  public static function getInstance () 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_ChangesUser ();
    }
    return self::$instance;
  }


  public function optimize ()
  {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("REPAIR TABLE changes_user;");
    $database_instance->runQuery ("OPTIMIZE TABLE changes_user;");
  }


  public function recordSave ($username, $bible, $book, $chapter, $oldID, $oldText, $newID, $newText)
  {
    $username = Database_SQLInjection::no ($username);
    $bible = Database_SQLInjection::no ($bible);
    $book = Database_SQLInjection::no ($book);
    $chapter = Database_SQLInjection::no ($chapter);
    $oldID = Database_SQLInjection::no ($oldID);
    $oldText = Database_SQLInjection::no ($oldText);
    $newID = Database_SQLInjection::no ($newID);
    $newText = Database_SQLInjection::no ($newText);
    $database_instance = Database_Instance::getInstance();
    $query = "INSERT INTO changes_user VALUES (NULL, '$username', '$bible', $book, $chapter, $oldID, '$oldText', '$newID', '$newText');";
    $result = $database_instance->runQuery ($query);
  }


  public function clearUser ($username)
  {
    $username = Database_SQLInjection::no ($username);
    $database_instance = Database_Instance::getInstance();
    $query = "DELETE FROM changes_user WHERE username = '$username';";
    $result = $database_instance->runQuery ($query);
  }


  public function getUsernames ()
  {
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT DISTINCT username FROM changes_user;";
    $result = $database_instance->runQuery ($query);
    $usernames = array ();
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row ();
      $usernames [] = $row [0];
    }
    return $usernames;
  }


  public function getBibles ($username)
  {
    $username = Database_SQLInjection::no ($username);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT DISTINCT bible FROM changes_user WHERE username = '$username';";
    $result = $database_instance->runQuery ($query);
    $bibles = array ();
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row ();
      $bibles [] = $row [0];
    }
    return $bibles;
  }


  public function getBooks ($username, $bible)
  {
    $username = Database_SQLInjection::no ($username);
    $bible = Database_SQLInjection::no ($bible);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT DISTINCT book FROM changes_user WHERE username = '$username' AND bible = '$bible' ORDER BY book ASC;";
    $result = $database_instance->runQuery ($query);
    $books = array ();
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row ();
      $books [] = $row [0];
    }
    return $books;
  }


  public function getChapters ($username, $bible, $book)
  {
    $username = Database_SQLInjection::no ($username);
    $bible = Database_SQLInjection::no ($bible);
    $book = Database_SQLInjection::no ($book);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT DISTINCT chapter FROM changes_user WHERE username = '$username' AND bible = '$bible' AND book = $book ORDER BY chapter ASC;";
    $result = $database_instance->runQuery ($query);
    $chapters = array ();
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_row ();
      $chapters [] = $row [0];
    }
    return $chapters;
  }


  public function getIdentifiers ($username, $bible, $book, $chapter)
  {
    $username = Database_SQLInjection::no ($username);
    $bible = Database_SQLInjection::no ($bible);
    $book = Database_SQLInjection::no ($book);
    $chapter = Database_SQLInjection::no ($chapter);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT oldid, newid FROM changes_user WHERE username = '$username' AND bible = '$bible' AND book = $book AND chapter = $chapter ORDER BY oldid ASC;";
    $result = $database_instance->runQuery ($query);
    $ids = array ();
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_assoc ();
      $ids [] = $row;
    }
    return $ids;
  }


  public function getChapter ($username, $bible, $book, $chapter, $oldID)
  {
    $username = Database_SQLInjection::no ($username);
    $bible = Database_SQLInjection::no ($bible);
    $book = Database_SQLInjection::no ($book);
    $chapter = Database_SQLInjection::no ($chapter);
    $oldID = Database_SQLInjection::no ($oldID);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT oldtext, newtext FROM changes_user WHERE username = '$username' AND bible = '$bible' AND book = $book AND chapter = $chapter AND oldid = $oldID;";
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $row = $result->fetch_assoc ();
      return $row;
    }
    return array ();
  }


}



?>

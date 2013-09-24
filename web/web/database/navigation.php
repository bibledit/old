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


class Database_Navigation
{

  private static $instance;
  private function __construct () {
  } 
  public static function getInstance () 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Navigation ();
    }
    return self::$instance;
  }


  public function trim ()
  {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("TRUNCATE TABLE navigation;");
  }


  public function optimize ()
  {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("REPAIR TABLE navigation;");
    $database_instance->runQuery ("OPTIMIZE TABLE navigation;");
  }
  
  
  public function record ($time, $user, $book, $chapter, $verse)
  {
    // Make input safe.
    $time = Database_SQLInjection::no ($time);
    $user = Database_SQLInjection::no ($user);
    $book = Database_SQLInjection::no ($book);
    $chapter = Database_SQLInjection::no ($chapter);
    $verse = Database_SQLInjection::no ($verse);

    $database_instance = Database_Instance::getInstance();
    
    // Clear any 'active' flags.
    $query = "UPDATE navigation SET active = false WHERE username = '$user';";
    $database_instance->runQuery ($query);

    // Remove too recent entries.
    $recent = $time - 5;
    $query = "DELETE FROM navigation WHERE timestamp >= $recent AND username = '$user';";
    $database_instance->runQuery ($query);

    // Record entry.
    $query = "INSERT INTO navigation VALUES (NULL, $time, '$user', $book, $chapter, $verse, true);";
    $database_instance->runQuery ($query);
  }


  public function previousExists ($user)
  {
    return ($this->getPreviousId ($user) != 0);
  }


  public function nextExists ($user)
  {
    return ($this->getNextId ($user) != 0);
  }


  public function getPrevious ($user)
  {
    $id = $this->getPreviousId ($user);
    if ($id == NULL) return NULL;

    $database_instance = Database_Instance::getInstance();

    // Update the 'active' flag.
    $query = "UPDATE navigation SET active = false WHERE username = '$user';";
    $database_instance->runQuery ($query);
    $query = "UPDATE navigation SET active = true WHERE id = $id;";
    $database_instance->runQuery ($query);

    $query = "SELECT book, chapter, verse FROM navigation WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    $passage = $result->fetch_row();
    return $passage;
  }
  

  public function getNext ($user)
  {
    $id = $this->getNextId ($user);
    if ($id == NULL) return NULL;

    $database_instance = Database_Instance::getInstance();

    // Update the 'active' flag.
    $query = "UPDATE navigation SET active = false WHERE username = '$user';";
    $database_instance->runQuery ($query);
    $query = "UPDATE navigation SET active = true WHERE id = $id;";
    $database_instance->runQuery ($query);

    $query = "SELECT book, chapter, verse FROM navigation WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    $passage = $result->fetch_row();
    return $passage;
  }
  

  private function getPreviousId ($user)
  {
    $user = Database_SQLInjection::no ($user);
    $database_instance = Database_Instance::getInstance();

    // Get the ID of the active entry for the user.
    $id = 0;
    $query = "SELECT id FROM navigation WHERE username = '$user' AND active = true;";
    $result = $database_instance->runQuery ($query);
    if ($result->num_rows > 0) {
      $row = $result->fetch_row();
      $id = $row[0];
    }
    
    // If no active ID was found, return NULL.
    if ($id == 0) return NULL;

    // Get the ID of the entry just before the active entry.
    $query = "SELECT id FROM navigation WHERE id < $id AND username = '$user' ORDER BY id DESC LIMIT 1;";
    $result = $database_instance->runQuery ($query);
    if ($result->num_rows > 0) {
      $row = $result->fetch_row();
      $id = $row[0];
      return $id;
    }
    
    // Nothing found.
    return NULL;
  }
  
  
  private function getNextId ($user)
  {
    $user = Database_SQLInjection::no ($user);
    $database_instance = Database_Instance::getInstance();

    // Get the ID of the active entry for the user.
    $id = 0;
    $query = "SELECT id FROM navigation WHERE username = '$user' AND active = true;";
    $result = $database_instance->runQuery ($query);
    if ($result->num_rows > 0) {
      $row = $result->fetch_row();
      $id = $row[0];
    }
    
    // If no active ID was found, return NULL.
    if ($id == 0) return NULL;

    // Get the ID of the entry just after the active entry.
    $query = "SELECT id FROM navigation WHERE id > $id AND username = '$user' ORDER BY id ASC LIMIT 1;";
    $result = $database_instance->runQuery ($query);
    if ($result->num_rows > 0) {
      $row = $result->fetch_row();
      $id = $row[0];
      return $id;
    }
    
    // Nothing found.
    return NULL;
  }
  
  
}


?>

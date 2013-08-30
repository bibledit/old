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


// This database is for the mailer.
class Database_Mailer
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Mailer();
    }
    return self::$instance;
  }

  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("REPAIR TABLE mailer;");
    $database_instance->runQuery ("OPTIMIZE TABLE mailer;");
  }


  public function trim () {
  }


  // Returns true if mail $id has been postponed.
  public function isPostponed ($id) {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT id FROM mailer WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    return ($result->num_rows > 0);
  }


  // Postpone mail $id.
  public function postpone ($id) {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $delay = $this->getDelay ($id);
    if ($delay == 0) {
      // First failure: delay 60 seconds.
      $delay = 60;
    } else {
      // Subsequent failures: double the delay,
      $delay = 2 * $delay;
    }
    $retry = time () + $delay;
    $query = "DELETE FROM mailer WHERE id = $id;";
    $database_instance->runQuery ($query);
    $query = "INSERT INTO mailer VALUES ($id, $retry, $delay);";
    $database_instance->runQuery ($query);
  }
  
  
  private function getDelay ($id) {
    $id = Database_SQLInjection::no ($id);
    $database_instance = Database_Instance::getInstance();
    $query = "SELECT delay FROM mailer WHERE id = $id;";
    $result = $database_instance->runQuery ($query);
    $result_array = $result->fetch_row();
    return $result_array[0];
  }


  public function getRetryMails ()
  {
    $ids = array ();
    $database_instance = Database_Instance::getInstance();
    $time = time ();
    $query = "SELECT id FROM mailer WHERE $time >= retry;";    
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $result_array = $result->fetch_row();
      $ids[] = $result_array [0];
    }
    return $ids;
  }


  public function getOverdueMails ()
  {
    $ids = array ();
    $database_instance = Database_Instance::getInstance();
    $retry = time () + 345600;
    $query = "SELECT id FROM mailer WHERE retry > $retry;";    
    $result = $database_instance->runQuery ($query);
    for ($i = 0; $i < $result->num_rows; $i++) {
      $result_array = $result->fetch_row();
      $ids[] = $result_array [0];
    }
    return $ids;
  }


  public function delete ($id) 
  {
    $database_instance = Database_Instance::getInstance();
    $query = "DELETE FROM mailer WHERE id = $id;";
    $database_instance->runQuery ($query);
  }

    
}



?>

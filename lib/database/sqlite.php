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


class Database_SQLite {


  public static function connect ($database)
  {
    $file = dirname (__DIR__) . "/databases/$database.sqlite";
    try {
      $db = new PDO ("sqlite:$file");
    } catch (PDOException $e) {
      $message = $e->getMessage () . " " . $file;
      if (class_exists ("Database_Logs", true)) {
        $database_logs = Database_Logs::getInstance ();
        $database_logs->log ($message, Filter_Roles::ADMIN_LEVEL);
      } else {
        echo "<p>$message</p>";
      }
      if (php_sapi_name () == "cli") {
        echo "$message\n";
      }
    }
    return $db;
  }


  public static function exec ($db, $sql)
  {
    $success = $db->exec ($sql);
    if ($success === false) {
      $message = $db->errorInfo ();
      $message = "Database error " . $message [0] . " - " . $message [2] . " - Query was: " . $sql;
      if (class_exists ("Database_Logs", true)) {
        $database_logs = Database_Logs::getInstance ();
        $database_logs->log ($message, Filter_Roles::ADMIN_LEVEL);
      } else {
        echo "<p>$message</p>";
      }
      if (php_sapi_name () == "cli") {
        echo "$message\n";
      }
    }
  }


  public static function query ($db, $sql)
  {
    // Run the SQL on the database handle.
    $success = $result = $db->query ($sql);
    // Check / log errors.
    if ($success === false) {
      $message = $db->errorInfo ();
      $message = "Database error: " . $message [0] . " - " . $message [2] . " - Query was: " . $sql;
      if (class_exists ("Database_Logs", true)) {
        $database_logs = Database_Logs::getInstance ();
        $database_logs->log ($message, Filter_Roles::ADMIN_LEVEL);
      } else {
        echo "<p>$message</p>";
      }
      if (php_sapi_name () == "cli") {
        echo "$message\n";
      }
    }
    // In case of errors, the $result won't be an array.
    // Make it an ampty array so it won't produce further errors down the code.
    if (!$result) {
      $result = array ();
    }
    return $result;
  }


}


?>

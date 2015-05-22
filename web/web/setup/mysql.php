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


/*
It has been seen on a shared hosting platform that the MySQL server did not have
sufficient available concurrent connections during the night.
Other processes were using many connections, so that none remained for Bibledit-Web.
That is a reason against using MySQL on shared hosting.
*/


class Setup_MySQL {

  // Indicates whether the database instance is connected.
  public $is_connected;
  // Database object.
  private $mysqli;
  // Singleton object instance.
  private static $instance;
  // Whether to echo errors to the output.
  public $display_errors = false;

  private function __construct ($display_errors) {

    // File with credentials to access the database server.
    // Or use defaults in case the configuration file is not there.
    // This is for the unit tests.
    if (file_exists ("credentials.php")) {
      require_once ("credentials.php");
    } else {
      $database_host = "localhost";
      $database_name = "bibledit-web";
      $database_user = "root";
      $database_pass = "root";
    }
    //$display_errors = true;

    $this->is_connected = true;

    // Connect to the database server.
    // This checks the MySQL server host, the user and password to connect to the database.
    @$this->mysqli = new mysqli ($database_host, $database_user, $database_pass);
    if ($this->mysqli->connect_errno) {
      $this->is_connected = false;
      if ($display_errors) {
        $this->warning ($this->mysqli->connect_error);
      }
    }

    // Connect to the database.
    if ($this->is_connected) {
      $this->is_connected = $this->mysqli->select_db ($database_name);
      if (!$this->is_connected) {
        if ($display_errors) {
          $this->warning ("Could not select database $database_name. Are the credentials correct?");
        }
      }
    }

    // If there was no connection, give a short diagnostic message.
    if (!$this->is_connected) {
      $message = Locale_Translate::_("Cannot access the database");
      echo "<h1>$message</h1>";
    }
  }

  public static function getInstance($display_errors = false) {
    if (empty (self::$instance)) {
      self::$instance = new Setup_MySQL($display_errors);
    }
    return self::$instance;
  }


  public function runQuery ($query)
  {
    $result = $this->mysqli->query ($query);
    if ($result !== true) {
      $error = $this->mysqli->error;
      if ($error != "") {
        if ($this->display_errors) {
          $this->warning ($error);
        }
      }
    }
    return $result;
  }


  public function flush ()
  {
    $flag = $this->display_errors;
    $this->display_errors = false;
    $this->runQuery ("FLUSH QUERY CACHE");
    $this->runQuery ("FLUSH TABLES");
    $this->display_errors = $flag;
  }


  private function warning ($message)
  {
    echo Locale_Translate::_("Warning"), ": $message<br>\n";
  }


}


?>

<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2009 Teus Benschop.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


/**
* This is a database in memory for use by the Poor Man's Crontab.
* Memory is chosen since the information in it is volatile, which is what is needed.
*/
class Database_Cron
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Cron();
    }
    return self::$instance;
  }

  public function verify () {
    $database_instance = Database_Instance::getInstance();
    $query = "CREATE TABLE IF NOT EXISTS cron (id varchar(10), value int) ENGINE = MEMORY;";
    $database_instance->runQuery ($query);
    $this->setShutdown();
  }

  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $query = "OPTIMIZE TABLE cron;";
    $database_instance->runQuery ($query);
    include_once ("messages/messages.php");
    message_information ("Stopping timer");
    $this->setShutdown();
  }

  public function clearFlags () {
    $server = Database_Instance::getInstance ();
    $server->runQuery ("DELETE FROM cron;");
  }
  
  public function setShutdown () {
    $server = Database_Instance::getInstance ();
    $server->runQuery ("INSERT INTO cron VALUES ('shutdown', 1);");
  }
  
  public function getShutdown () {
    $server = Database_Instance::getInstance ();
    $result = $server->runQuery ("SELECT value FROM cron WHERE id = 'shutdown';");
    if ($result->num_rows == 0) {
      return 0;
    }
    $result_array = $result->fetch_array();
    return $result_array['value'];
  }

  public function setWatchdog () {
    $second = time ();
    $server = Database_Instance::getInstance ();
    $server->runQuery ("DELETE FROM cron WHERE id = 'watchdog';");
    $server->runQuery ("INSERT INTO cron VALUES ('watchdog', $second);");
  }
  
  public function getWatchdog () {
    $server = Database_Instance::getInstance ();
    $result = $server->runQuery ("SELECT value FROM cron WHERE id = 'watchdog';");
    if ($result->num_rows == 0) {
      return 0;
    }
    $result_array = $result->fetch_row();
    return $result_array[0];
  }

}



?>

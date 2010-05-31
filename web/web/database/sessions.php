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


class Database_Sessions
{
  private static $instance;
  private function __construct() {
  } 
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Database_Sessions();
    }
    return self::$instance;
  }

  public function verify () {
    $database_instance = Database_Instance::getInstance();
$query = <<<EOD
CREATE TABLE IF NOT EXISTS sessions (
id int,
timestamp int,
identifier varchar(256),
value varchar(256)
);
EOD;
    $database_instance->runQuery ($query);
  }

  public function optimize () {
    $database_instance = Database_Instance::getInstance();
    $database_instance->runQuery ("OPTIMIZE TABLE sessions;");
  }

  private function getNewUniqueSessionId ()
  {
    $server = Database_Instance::getInstance ();
    do {
      $id = rand (100000000, 999999999);
    } while ($this->SessionIdExists ($id));
    return $id;
  }


  private function SessionIdExists ($id)
  {
    $server = Database_Instance::getInstance ();
    $id = Database_SQLInjection::no ($id);
    $query = "SELECT id FROM sessions WHERE id = $id;";
    $result = $server->runQuery ($query);
    return ($result->num_rows > 0);
  }
  
  
  public function trim ()
  {
    $server = Database_Instance::getInstance ();
    $time = time() - 86400;
    $query = "DELETE FROM sessions WHERE timestamp < $time;";
    $server->runQuery ($query);
  }
  
  
  public function getCurrentSessionId ()
  {
    @$id = $_GET ['session'];
    if (!is_numeric ($id)) {
      $id = $this->getNewUniqueSessionId ();
      $_GET ['session'] = $id;
    }
    return $id;
  }
  
  
  private function setValue ($key, $value)
  {
    $server = Database_Instance::getInstance ();
    $value = Database_SQLInjection::no ($value);
    $id = $this->getCurrentSessionId ();
    $query = "DELETE FROM sessions WHERE id = $id AND identifier = '$key';";
    $server->runQuery ($query);
    $time = time();
    $query = "INSERT INTO sessions VALUES ($id, $time, '$key', '$value')";
    $server->runQuery ($query);
  }
  

  private function getValue ($key)
  {
    $server = Database_Instance::getInstance ();
    $id = $this->getCurrentSessionId ();
    $query = "SELECT value FROM sessions WHERE id = $id AND identifier = '$key';";
    $result = $server->runQuery ($query);
    if ($result->num_rows == 0)
      return "";
    $row = $result->fetch_row();
    return $row[0];
  }
  
  
  public function setDesktop ($value)
  {
    $this->setValue ("desktop", $value);
  }
  public function getDesktop ()
  {
    return $this->getValue ("desktop");
  }


  public function setBible ($value)
  {
    $this->setValue ("bible", $value);
  }
  public function getBible ()
  {
    return $this->getValue ("bible");
  }
  

  public function setConsultationNote ($value)
  {
    $this->setValue ("consultationnote", $value);
  }
  public function getConsultationNote ()
  {
    return $this->getValue ("consultationnote");
  }


  public function setDisplayConsultationNoteActions ($value)
  {
    $this->setValue ("displayconsultationnoteactions", $value);
  }
  public function getDisplayConsultationNoteActions ()
  {
    return $this->getValue ("displayconsultationnoteactions");
  }


  public function setEditConsultationNoteView ($value)
  {
    $this->setValue ("editconsultationnoteview", $value);
  }
  public function getEditConsultationNoteView ()
  {
    return $this->getValue ("editconsultationnoteview");
  }


  public function setBulkUpdateConsultationNotes ($value)
  {
    $this->setValue ("bulkupdateconsultationnotes", $value);
  }
  public function getBulkUpdateConsultationNotes ()
  {
    return $this->getValue ("bulkupdateconsultationnotes");
  }


  public function setConsultationNoteStartingLimit ($value)
  {
    $this->setValue ("consultationnotestartinglimit", $value);
  }
  public function getConsultationNoteStartingLimit ()
  {
    return $this->getValue ("consultationnotestartinglimit");
  }


  
  

}



?>

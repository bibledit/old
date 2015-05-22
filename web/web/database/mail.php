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

/**
* Database_Mail
* Handles mail sent from the translation environment to the users.
* This is a translation environment, not a message board.
* If users wish to communicate with one another about the translation, they can do so through the issues tracker.
* The application does not provide for private messages between users. They can use their email for that.
*/


// Database resilience: It stores the emails to be sent shortly after.
// The database is not very often written to.
// The risk of corruption is low and acceptable.


class Database_Mail
{
  private static $instance;
  private $db;
  private function __construct() {
    $this->db = Database_SQLite::connect ("mail");
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Mail();
    }
    return self::$instance;
  }


  public function create ()
  {
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS mail (
  username text,
  timestamp integer,
  subject text,
  body text,
  retry integer
);
EOD;
    Database_SQLite::exec ($this->db, $sql);
  }


  public function optimize ()
  {
    Database_SQLite::exec ($this->db, "VACUUM mail;");
  }


  public function trim ()
  {
    $time = time () - 2592000; // Remove entries after 30 days.
    $query = "DELETE FROM mail WHERE timestamp < $time;";
    Database_SQLite::exec ($this->db, $query);
    $query = "DELETE FROM mail WHERE retry > 10;";
    Database_SQLite::exec ($this->db, $query);
  }


  // Send email.
  // $to: The email address.
  // $subject: The subject.
  // $body: The body.
  // $time: Normally not given, but if given, it indicates the $time stamp for sending this email.
  public function send ($to, $subject, $body, $time = false)
  {
    $to = Database_SQLiteInjection::no ($to);
    $subject = Database_SQLiteInjection::no ($subject);
    $body = Database_SQLiteInjection::no ($body);
    if ($time == false) $time = time();
    $time = Database_SQLiteInjection::no ($time);
    $query = "INSERT INTO mail VALUES ('$to',   $time, '$subject', '$body', 0);";
    Database_SQLite::exec ($this->db, $query);
  }


  // Get number of mails for the current user.
  public function getMailCount ()
  {
    $session = Session_Logic::getInstance ();
    $user = $session->currentUser();
    $user = Database_SQLiteInjection::no ($user);
    $query = "SELECT count(*) FROM mail WHERE username = '$user';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }
    return 0;
  }


  // Get the mails of the current user.
  public function getMails ()
  {
    $mails = array ();
    $session = Session_Logic::getInstance ();
    $user = $session->currentUser();
    $query = "SELECT rowid, timestamp, subject FROM mail WHERE username = '$user' ORDER BY timestamp DESC;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      unset ($row[0]);
      unset ($row[1]);
      unset ($row[2]);
      $mails [] = $row;
    }
    return $mails;
  }


  // Delete a mail.
  public function delete ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "DELETE FROM mail WHERE rowid = $id;";
    Database_SQLite::exec ($this->db, $query);
  }


  // Get a mail.
  public function get ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT username, subject, body FROM mail WHERE rowid = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      unset ($row[0]);
      unset ($row[1]);
      unset ($row[2]);
      return $row;
    }
    return NULL;
  }


  // Get ids of all mails reading for sending.
  public function getMailsToSend ()
  {
    $ids = array ();
    $timestamp = time ();
    $query = "SELECT rowid FROM mail WHERE timestamp <= $timestamp;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $ids[] = $row [0];
    }
    return $ids;
  }
  

  // Postpones a mail till later.
  // Used for retrying after failure to send the mail.  
  public function postpone ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "UPDATE mail SET retry = retry + 1 WHERE rowid = $id;";
    Database_SQLite::exec ($this->db, $query);
    $query = "UPDATE mail SET timestamp = timestamp + retry * 900 WHERE rowid = $id;";
    Database_SQLite::exec ($this->db, $query);
  }



}


?>

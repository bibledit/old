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


// This database is resilient.
// Part of the data is stored in a SQLite database.
// This part is read often, and infrequently written to.
// Due to the infrequent write operations, there is a low and acceptable change of corruption.
// Another part of the data is stored in the bare filesystem.
// This part is written to often. It contains non-important information.
// In the unlikely case of corruption, it will get fixed next time it is written to.


class Database_Users
{
  private static $instance;
  public $db;
  private function __construct() {
    $this->db = Database_SQLite::connect ("users");
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Users();
    }
    return self::$instance;
  }


  public function create ()
  {
$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS users (
  username text,
  password text,
  level integer,
  email text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);

$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS teams (
  username text,
  bible text,
  readonly boolean
);
EOD;
    Database_SQLite::exec ($this->db, $sql);

$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS logins (
  username text,
  address text,
  agent text,
  fingerprint text,
  cookie text
);
EOD;
    Database_SQLite::exec ($this->db, $sql);
  }


  public function upgrade ()
  {
    // Upgrade table "users".
    // Column 'timestamp' is available in older databases. It is not in use.
    // It cannot be dropped easily in SQLite. Leave it for just now.
    $columns = array ();
    $query = "PRAGMA table_info (users);";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $columns [] = $row ['name'];
    }
  }


  private function mainFolder ()
  {
    $folder = realpath (__DIR__ . "/../databases/users");
    return $folder;
  }
  
  
  private function timestampFile ($user)
  {
    $file = $this->mainFolder () . "/timestamp" . $user;
    return $file;
  }
  

  public function trim ()
  {
    // Remove persistent logins after a day of inactivity.
    $dayAgo = time () - 86400;
    $users = $this->getUsers ();
    foreach ($users as $username) {
      $timestamp = $this->getTimestamp ($username);
      if ($timestamp < $dayAgo) {
        $username = Database_SQLiteInjection::no ($username);
        $query = "UPDATE logins SET fingerprint = '' WHERE username = '$username';";
        Database_SQLite::exec ($this->db, $query);
      }
    }
  }
    

  public function optimize ()
  {
    Database_SQLite::exec ($this->db, "VACUUM users;");
  }


  // Returns true if the username and password match.
  public function matchUsernamePassword ($username, $password)
  {
    $username = Database_SQLiteInjection::no ($username);
    $password = md5 ($password);
    $query = "SELECT * FROM users WHERE username = '$username' and password = '$password'";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return true;
    }
    return false;
  }


  // Returns true if the email and password match.
  public function matchEmailPassword ($email, $password)
  {
    $email = Database_SQLiteInjection::no ($email);
    $password = md5 ($password);
    $query = "SELECT * FROM users WHERE email = '$email' and password = '$password'";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return true;
    }
    return false;
  }


  // Add the user details to the database.
  public function addNewUser ($username, $password, $level, $email) 
  {
    $username = Database_SQLiteInjection::no ($username);
    $level = Database_SQLiteInjection::no ($level);
    $email = Database_SQLiteInjection::no ($email);
    $query = "INSERT INTO users (username, level, email) VALUES ('$username', $level, '$email');";
    Database_SQLite::exec ($this->db, $query);
    $this->updateUserPassword ($username, $password);
  }


  // Returns the query to execute to add a new user.
  public function addNewUserQuery ($username, $password, $level, $email) 
  {
    $username = Database_SQLiteInjection::no ($username);
    $password = md5 ($password);
    $level = Database_SQLiteInjection::no ($level);
    $email = Database_SQLiteInjection::no ($email);
    $query = "INSERT INTO users (username, password, level, email) VALUES ('$username', '$password', $level, '$email')";
    return $query;
  }


  // Returns the username that belongs to the $email.
  public function getEmailToUser ($email) 
  {
    $email = Database_SQLiteInjection::no ($email);
    $query = "SELECT username FROM users WHERE email = '$email';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row [0];
    }
    return "";
  }


  // Returns the email address that belongs to $user.
  public function getUserToEmail ($user) 
  {
    $user = Database_SQLiteInjection::no ($user);
    $query = "SELECT email FROM users WHERE username = '$user';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row [0];
    }
    return "";
  }


  // Returns true if the username exists in the database.
  public function usernameExists ($user)
  {
    $user = Database_SQLiteInjection::no ($user);
    $query = "SELECT * FROM users WHERE username = '$user'";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return true;
    }
    return false;
  }


  // Returns true if the email address exists in the database.
  public function emailExists ($email)
  {
    $email  = Database_SQLiteInjection::no ($email);
    $query  = "SELECT * FROM users WHERE email = '$email'";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return true;
    }
    return false;
  }


  // Returns the level that belongs to the user.
  public function getUserLevel ($user)
  {
    $user = Database_SQLiteInjection::no ($user);
    $query = "SELECT level FROM users WHERE username = '$user';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row [0];
    }
    return Filter_Roles::GUEST_LEVEL;
  }


  // Updates the level of a given user.
  public function updateUserLevel ($user, $level) 
  {
    $user = Database_SQLiteInjection::no ($user);
    $level = Database_SQLiteInjection::no ($level);
    $query = "UPDATE users SET level = $level WHERE username = '$user';";
    Database_SQLite::exec ($this->db, $query);
  }


  // Remove a user from the database.
  public function removeUser ($user)
  {
    $user = Database_SQLiteInjection::no ($user);
    $query = "DELETE FROM users WHERE username = '$user'";
    Database_SQLite::exec ($this->db, $query);
    $query = "DELETE FROM teams WHERE username = '$user'";
    Database_SQLite::exec ($this->db, $query);
  }


  // Returns an array with the usernames of the site administrators.
  public function getAdministrators ()
  {
    $administrators = array ();
    $query = "SELECT username FROM users WHERE level = " . Filter_Roles::ADMIN_LEVEL . ";";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $administrators [] = $row [0];
    }
    return $administrators;
  }


  // Updates the $password for $user.
  public function updateUserPassword ($user, $password) 
  {
    $user = Database_SQLiteInjection::no ($user);
    $password = md5 ($password);
    $query = "UPDATE users SET password = '$password' WHERE username = '$user';";
    Database_SQLite::exec ($this->db, $query);
  }


  // Returns the query to update a user's email address.
  public function updateEmailQuery ($username, $email)
  {
    $query = "UPDATE users SET email = '$email' WHERE username = '$username';";
    return $query;
  }


  // Updates the $email for $user.
  public function updateUserEmail ($user, $email) 
  {
    $user = Database_SQLiteInjection::no ($user);
    $email = Database_SQLiteInjection::no ($email);
    $query = $this->updateEmailQuery ($user, $email);
    Database_SQLite::exec ($this->db, $query);
  }


  // Return an array with the available users.
  public function getUsers ()
  {
    $users = array ();
    $query = "SELECT username FROM users;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $users [] = $row[0];
    }
    return $users;
  }
  
  
  // Returns the md5 hash for the $user's password.
  public function getmd5 ($user)
  {
    $user = Database_SQLiteInjection::no ($user);
    $query = "SELECT password FROM users WHERE username = '$user';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row [0];
    }
    return false;
  }
  

  // Sets the login security tokens for a user.
  public function setTokens ($username, $address, $agent, $fingerprint)
  {
    if ($username == $this->getUsername ($address, $agent, $fingerprint)) return;
    $username = Database_SQLiteInjection::no ($username);
    $address = md5 ($address);
    $agent = md5 ($agent);
    $fingerprint = md5 ($fingerprint);
    $query = "INSERT INTO logins (username, address, agent, fingerprint) VALUES ('$username', '$address', '$agent', '$fingerprint')";
    Database_SQLite::exec ($this->db, $query);
  }
  
  
  // Remove the login securityh tokens for a user.
  public function removeTokens ($username)
  {
    $username = Database_SQLiteInjection::no ($username);
    $query = "DELETE FROM logins WHERE username = '$username';";
    Database_SQLite::exec ($this->db, $query);
  }


  // Returns the username that matches the remote IP $address and the browser's user $agent,
  // and the remaining fingerprint from the user.
  // Returns false if no match was found.
  public function getUsername ($address, $agent, $fingerprint)
  {
    $address = md5 ($address);
    $agent = md5 ($agent);
    $fingerprint = md5 ($fingerprint);
    $query = "SELECT username FROM logins WHERE address = '$address' AND agent = '$agent' AND fingerprint = '$fingerprint';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row [0];
    }
    return false;
  }
  

  // Ping the access timestamp for $username.
  public function pingTimestamp ($username)
  {
    $existingTimestamp = $this->getTimestamp ($username);
    $timestamp = time ();
    if ($timestamp != $existingTimestamp) {
      $file = $this->timestampFile ($username);
      file_put_contents ($file, $timestamp);
    }
  }


  // Gets the access timestamp for $username.
  public function getTimestamp ($username)
  {
    $file = $this->timestampFile ($username);
    @$timestamp = file_get_contents ($file);
    if ($timestamp) return $timestamp;
    return 0;
  }
  
  
  // Get array with the teams.
  public function getTeams ()
  {
    $teams = array ();
    $query = "SELECT DISTINCT bible FROM teams;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $teams [] = $row [0];
    }
    return $teams;
  }
  
  
  // Give a $user access to a $bible.
  public function grantAccess2Bible ($user, $bible)
  {
    $this->revokeAccess2Bible ($user, $bible);
    $user = Database_SQLiteInjection::no ($user);
    $bible = Database_SQLiteInjection::no ($bible);
    $query = "INSERT INTO teams (username, bible) VALUES ('$user', '$bible');";
    Database_SQLite::exec ($this->db, $query);
  }
  
   
  // Revoke a $user's access to a $bible.
  public function revokeAccess2Bible ($user, $bible)
  {
    $user = Database_SQLiteInjection::no ($user);
    $bible = Database_SQLiteInjection::no ($bible);
    $query = "DELETE FROM teams WHERE username = '$user' AND bible = '$bible';";
    Database_SQLite::exec ($this->db, $query);
  }
  
  
  // Get an array of Bibles the $user has access to.
  public function getBibles4User ($user)
  {
    $bibles = array ();
    $user = Database_SQLiteInjection::no ($user);
    $query = "SELECT DISTINCT bible FROM teams WHERE username = '$user';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $bibles [] = $row [0];
    }
    return $bibles;
  }


  // Get an array of users who have access to a $bible.
  public function getUsers4Bible ($bible)
  {
    $users = array ();
    $bible = Database_SQLiteInjection::no ($bible);
    $query = "SELECT DISTINCT username FROM teams WHERE bible = '$bible';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $users [] = $row [0];
    }
    return $users;
  }
  
  
  // Returns whether a $user has access to a $bible: true or false.
  // If no more than one team has been formed, all users have access to all Bibles.
  // Administrators have access to any Bible.
  public function hasAccess2Bible ($user, $bible)
  {
    $teams = $this->getTeams ();
    if (count ($teams) <= 1) return true;
    $level = $this->getUserLevel ($user);
    if ($level >= Filter_Roles::ADMIN_LEVEL) return true;
    $bibles = $this->getBibles4User ($user);
    return in_array ($bible, $bibles);
  }


  // Set $readonly access for $user to $bible to true or false.
  public function setReadOnlyAccess2Bible ($user, $bible, $readonly)
  {
    $user = Database_SQLiteInjection::no ($user);
    $bible = Database_SQLiteInjection::no ($bible);
    $readonly = Filter_Bool::int ($readonly);
    $query = "UPDATE teams SET readonly = $readonly WHERE username = '$user' AND bible = '$bible';";
    Database_SQLite::exec ($this->db, $query);
  }
  

  // Returns true or false depending on whether $user has read-only access to $bible.
  public function hasReadOnlyAccess2Bible ($user, $bible)
  {
    $user = Database_SQLiteInjection::no ($user);
    $bible = Database_SQLiteInjection::no ($bible);
    $query = "SELECT readonly FROM teams WHERE username = '$user' AND bible = '$bible';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return (boolean) $row [0];
    }
    // Entry not found for user/bible: Default is not read-only.
    return false;
  }
    
  
  public function debug ()
  {
    $query = "SELECT * FROM teams;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      unset ($row[0]);
      unset ($row[1]);
      var_dump ($row);
    }
  }


}


?>

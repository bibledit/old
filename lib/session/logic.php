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


require_once ("bootstrap/bootstrap.php");


/*

The PHP persistent storage of sessions works well, normally.
On shared hosts, the hosting provider may influence how PHP sessions work.
In such a case, the session mechanism does not always work as desired.
This may result in frequently being locked out.
A possible solution would be to work around this behaviour.
Bibledit-Web has another solution:
It has its own persistent session storage mechanism.
This mechanism is independent of any hosting provider or any PHP session mechanism.

The current mechanism makes the following situation possible, although not likely:
* One user logs in.
* Another user, with exactly the same signature, is automatically logged in too.
The above is undesired.
The behaviour can be prevented when one of the users sets another user agent.

To improve the persistent login mechanism, the following could be done:
1. On sending the login screen, Bibledit-Web generates a unique cookie with unique name and the correct path.
2. When the user logs in, the browser will send this new cookie.
3. Bibledit-Web stores the cookie details as it receives the cookie details through the header during login.
4. Next time when the user requests any page page, the browser will again send this cookie.
   Bibledit-Web checks the details from the submitted header with its database.
   If the details match, the user is logged in.
Optionally it sets the cookie to expire after a week, or a month, or to never expire.
Upon regenerating the cookie, it will keep storing the old cookie signature, and will also store the new cookie signature.
Both are now acceptable for the time being.
This requires two columns in the table, where the stuff is shifted from new to old, to swapped away.

*/

class Session_Logic
{

  private $level = 0;           // The level of the user.
  private $check_ip_blocks = 3; // How many numbers from IP use in fingerprint?
  private static $instance;     // Current singleton instance.
  private $logged_in;           // Whether user is logged in.
  private $username;            // The username.


  // The class constructor is private, so no outsider can call it.
  private function __construct () 
  {
    $this->Open ();
  }


  // Gets or creates the singleton object instance.
  public static function getInstance ()
  {
    if (empty (self::$instance)) {
      self::$instance = new Session_Logic ();
    }
    return self::$instance;
  }


  // Call this when logging in.
  public function Open ()
  {
    if ($this->openAccess ()) return;

    if ($this->clientAccess ()) return;

    $address = $this->remoteAddress ();
    @$agent = $_SERVER['HTTP_USER_AGENT'];
    $database_users = Database_Users::getInstance ();
    $username = $database_users->getUsername ($address, $agent, $this->fingerprint ());
    if ($username) {
      $this->setUsername ($username);
      $this->logged_in = true;
      $database_users->pingTimestamp ($username);
    } else {
      $this->setUsername ("");
      $this->logged_in = false;
    }
  }
  
  
  public function setUsername ($username)
  {
    $this->username = $username;
  }


  // Returns IP blocks of remote address.
  private function remoteAddress ()
  {
     $address = "";

     $num_blocks = abs (intval ($this->check_ip_blocks));
     if ($num_blocks > 4) {
       $num_blocks = 4;
     }
     @$blocks = explode ('.', $_SERVER['REMOTE_ADDR']);
     for ($i = 0; $i < $num_blocks; $i++) {
       @$address .= $blocks[$i] . '.';
     }

     return $address;
  }
  
  
  // Returns a fingerprint from the user's browser.
  private function fingerprint ()
  {
    $fingerprint = "";
    //@$fingerprint = $_SERVER ['HTTP_CONNECTION']; Unstable fingerprint. No use for persistent login.
    //@$fingerprint .= $_SERVER ['HTTP_ACCEPT_ENCODING']; Unstable fingerprint. No use for persistent login.
    @$fingerprint .= $_SERVER ['HTTP_ACCEPT_LANGUAGE'];
    return $fingerprint;
  }


  // Attempts to log into the system.
  public function attemptLogin ($user_or_email, $password) {
    $database = Database_Users::getInstance();
    $login_okay = false;
    // Match username and email.
    if ($database->matchUsernamePassword ($user_or_email, $password)) {
      $login_okay = true;
    }
    // Match password and email.
    if ($database->matchEmailPassword ($user_or_email, $password)) {
      $login_okay = true;
      // Fetch username that belongs to the email address that was used to login.
      $user_or_email = $database->getEmailToUser ($user_or_email);
    }

    if ($login_okay) {
      $this->Open ();
      $this->setUsername ($user_or_email);
      $this->logged_in = true;
      $security1 = $this->remoteAddress ();
      $security2 = $_SERVER['HTTP_USER_AGENT'];
      $security3 = $this->fingerprint ();
      $database->setTokens ($user_or_email, $security1, $security2, $security3);
      $this->currentLevel (true);
      return true;
    } else {
      return false;
    }
  }


  // Returns true if the user is logged in.
  public function loggedIn () 
  {
    // The logged-in status is stored in the singleton object, so that if it is requested twice,
    // the session system is queries only once. It has been seen on some sites that if the php session
    // system was queried more than once, it did not behave consistently.
    // Buffering the status in the object resolved this.
    // After the session system was dropped, the above comment is no longer relevant.
    // The information this comment contains remains relevant for the future.
    if ($this->openAccess ()) return true;
    return $this->logged_in;
  }


  public function currentUser () 
  {
    $user = $this->username;
    return $user;
  }


  // Returns the current level of the session as an integer.
  public function currentLevel ($force = false) {
    if ($this->openAccess ()) return  $this->level;
    if (($this->level == 0) || $force) {
      if ($this->loggedIn()) {
        $database = Database_Users::getInstance();
        $this->level = $database->getUserLevel ($this->currentUser());
      } else {
        $this->level = Filter_Roles::GUEST_LEVEL;
      }
    }
    return $this->level;
  }


  function logout ()
  {
    $username = $this->currentUser ();
    $database_users = Database_Users::getInstance ();
    $database_users->removeTokens ($username);
    $this->setUsername ("");
    $this->level = Filter_Roles::GUEST_LEVEL;
  }


  function openAccess () 
  {
    // Open access if it is flagged as such in the configuration.
    include ("config/open.php");
    if ($open_installation) {
      $this->setUsername ("admin");
      $this->level = Filter_Roles::ADMIN_LEVEL;
      $this->logged_in = true;
      return true;
    }
    return false;
  }


  function clientAccess () 
  {
    // If client mode is prepared, 
    // log in as the first username in the users database,
    // of as the admin in case no user have been set up yet.
    if (Filter_Client::prepared ()) {
      $database_users = Database_Users::getInstance ();
      $users = $database_users->getUsers ();
      if (empty ($users)) {
        $user = "admin";
        $level = Filter_Roles::ADMIN_LEVEL;
      } else {
        $user = $users [0];
        $level = $database_users->getUserLevel ($user);
      }
      $this->setUsername ($user);
      $this->level = $level;
      $this->logged_in = true;
      return true;
    }
    return false;
  }


}


?>

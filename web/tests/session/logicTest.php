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

 
class logicTest extends PHPUnit_Framework_TestCase
{

  public function testIdentifier()
  {
    $session_logic = Session_Logic::getInstance ();
    $database_users = Database_Users::getInstance ();
    // It is assumed that default account adminusername exists, with default password.
    // If not, bail out so we don't damage the database contents.
    $adminusername = "adminusername";
    $adminpassword = "adminpassword";
    $adminusername_exists = $database_users->usernameExists ($adminusername);
    $this->assertEquals (true, $adminusername_exists);
    if (!$adminusername_exists) return;
    // Safekeep adminusername's email address.
    $adminusername_email = $database_users->getUserToEmail ($adminusername);
    $this->assertNotEquals ("", $adminusername_email);
    // Try login with this username and password.
    $_SERVER['HTTP_USER_AGENT'] = "PHPUnit";
    $_SERVER['REMOTE_ADDR'] = "127.0.0.1";
    $logged_in = $session_logic->attemptLogin ($adminusername, $adminpassword);
    $this->assertEquals (true, $logged_in);
    // Set adminuser's level lower, and attempt to login, and get the level.
    $database_users->updateUserLevel ($adminusername, 0);
    $logged_in = $session_logic->attemptLogin ($adminusername, $adminpassword);
    $this->assertEquals (true, $logged_in);
    // Remove adminuser and attempt to login.
    $database_users->removeUser ($adminusername);
    $logged_in = $session_logic->attemptLogin ($adminusername, $adminpassword);
    $this->assertEquals (true, $logged_in);
    // Restore email address.
    $query = $database_users->updateEmailQuery ($adminusername, $adminusername_email);
    $server = Database_Instance::getInstance ();
    $server->runQuery ($query);
    $this->assertEquals ($adminusername_email, $database_users->getUserToEmail ($adminusername));
  }
  
}
?>

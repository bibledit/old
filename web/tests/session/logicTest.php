<?php

 
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

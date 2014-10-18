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

 
class databaseUsersTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }


  public function tearDown ()
  {
    $database_users = Database_Users::getInstance ();
    $database_users->removeUser ("phpunit");
    $database_users->removeUser ("phpunit2");
    $database_users->removeUser ("adminusername");
  }


  public function testOptimize ()
  {
    $database_users = Database_Users::getInstance ();
    $database_users->optimize ();
  }
  
  
  public function testTrim ()
  {
    $database_users = Database_Users::getInstance ();
    $database_users->trim ();
  }
  
  
  public function testAddCheckUser ()
  {
    $database_users = Database_Users::getInstance ();
    $exists = $database_users->usernameExists ("phpunit");
    $this->assertFalse ($exists);
    $database_users->addNewUser ("phpunit", "", 0, "");
    $exists = $database_users->usernameExists ("phpunit");
    $this->assertTrue ($exists);
    $database_users->removeUser ("phpunit");
    $exists = $database_users->usernameExists ("phpunit");
    $this->assertFalse ($exists);
  }


  public function testLevels ()
  {
    $database_users = Database_Users::getInstance ();
    $database_users->addNewUser ("phpunit", "", 0, "");
    $admins = $database_users->getAdministrators ();
    $this->assertEquals (0, count ($admins));
    $level = $database_users->getUserLevel ("phpunit");
    $this->assertEquals (0, $level);
    $level = $database_users->getUserLevel ("phpunit2");
    $this->assertEquals (Filter_Roles::GUEST_LEVEL, $level);
    $database_users->updateUserLevel ("phpunit", Filter_Roles::ADMIN_LEVEL);
    $admins = $database_users->getAdministrators ();
    $this->assertEquals (1, count ($admins));
    $database_users->updateUserLevel ("phpunit", Filter_Roles::ADMIN_LEVEL - 1);
    $admins = $database_users->getAdministrators ();
    $this->assertEquals (0, count ($admins));
  }


  public function testMatch ()
  {
    $database_users = Database_Users::getInstance ();
    $match = $database_users->matchUsernamePassword ("phpunit", "password");
    $this->assertFalse ($match);
    $database_users->addNewUser ("phpunit", "password", 0, "user@host.com");
    $match = $database_users->matchUsernamePassword ("phpunit", "password");
    $this->assertTrue ($match);
    $match = $database_users->matchUsernamePassword ("phpunit", "password2");
    $this->assertFalse ($match);
    $database_users->updateUserPassword ("phpunit", "password3");
    $match = $database_users->matchUsernamePassword ("phpunit", "password2");
    $this->assertFalse ($match);
    $match = $database_users->matchUsernamePassword ("phpunit", "password3");
    $this->assertTrue ($match);
    $match = $database_users->matchEmailPassword ("user@host.com", "password3");
    $this->assertTrue ($match);
  }


  public function testEmailQueries ()
  {
    $database_users = Database_Users::getInstance ();
    $query = $database_users->addNewUserQuery ("phpunit", "@#'^__$", 2, "mail@host.com");
    $standard = "INSERT INTO users (username, password, level, email) VALUES ('phpunit', '73becb339d5996bb5d118e9fa04c166e', 2, 'mail@host.com')";
    $this->assertEquals ($standard, $query);
    $query = $database_users->updateEmailQuery ("phpunit", "email@host.com");
    $standard = "UPDATE users SET email = 'email@host.com' WHERE username = 'phpunit';";
    $this->assertEquals ($standard, $query);
  }
  
  
  public function testUserEmail ()
  {
    $database_users = Database_Users::getInstance ();
    $user = $database_users->getEmailToUser ("email@host");
    $this->assertEquals ("", $user);
    $database_users->addNewUser ("phpunit", "password", 0, "user@host.com");
    $user = $database_users->getEmailToUser ("user@host.com");
    $this->assertEquals ("phpunit", $user);
    $email = $database_users->getUserToEmail ("phpunit");
    $this->assertEquals ("user@host.com", $email);
    $email = $database_users->getUserToEmail ("phpunit2");
    $this->assertEquals ("", $email);
    $exists = $database_users->emailExists ("email");
    $this->assertFalse ($exists);
    $exists = $database_users->emailExists ("user@host.com");
    $this->assertTrue ($exists);
    $database_users->updateUserEmail ("phpunit", "securemail");
    $email = $database_users->getUserToEmail ("phpunit");
    $this->assertEquals ("securemail", $email);
  }
  
  
  public function testAdministrator ()
  {
    $database_users = Database_Users::getInstance ();
    $admins = $database_users->getAdministrators ();
    $this->assertEquals (array (), $admins);
    $database_users->addNewUser ("phpunit", "", Filter_Roles::ADMIN_LEVEL, "");
    $admins = $database_users->getAdministrators ();
    $this->assertEquals (array ("phpunit"), $admins);
    $database_users->addNewUser ("phpunit2", "", Filter_Roles::ADMIN_LEVEL, "");
    $admins = $database_users->getAdministrators ();
    $this->assertEquals (2, count ($admins));
  }
  
  
  public function testUsers ()
  {
    $database_users = Database_Users::getInstance ();
    $users = $database_users->getUsers ();
    $this->assertGreaterThan (0, count ($users));
    $this->assertNotContains ("phpunit", $users);
    $database_users->addNewUser ("phpunit", "", 0, "");
    $users = $database_users->getUsers ();
    $this->assertContains ("phpunit", $users);
    $database_users->addNewUser ("phpunit2", "", 0, "");
    $users = $database_users->getUsers ();
    $this->assertContains ("phpunit", $users);
    $this->assertContains ("phpunit2", $users);
  }
  
  
  public function testGetmd5 ()
  {
    $database_users = Database_Users::getInstance ();
    $database_users->addNewUser ("phpunit", "phpunit", 0, "");
    $md5 = $database_users->getmd5 ("phpunit");
    $this->assertEquals ("85af727fd022d3a13e7972fd6a418582", $md5);
  }


  public function testSecurity ()
  {
    $database_users = Database_Users::getInstance ();
    $database_users->addNewUser ("phpunit", "", 0, "");
    $username = $database_users->getUsername ("127.0.0.1", "MyAgent", "fingerprint");
    $this->assertFalse ($username);
    $database_users->setTokens ("phpunit", "127.0.0.1", "MyAgent", "fingerprint");
    $username = $database_users->getUsername ("127.0.0.1", "MyAgent", "fingerprint");
    $this->assertEquals ("phpunit", $username);
    $username = $database_users->getUsername ("127.0.0.1", "MyAgentt", "fingerprint");
    $this->assertFalse ($username);
    $username = $database_users->getUsername ("127.0.0.2", "MyAgent", "fingerprint");
    $this->assertFalse ($username);
    $username = $database_users->getUsername ("127.0.0.1", "MyAgent", "fingerprintt");
    $this->assertFalse ($username);
  }


  public function testTimestamp ()
  {
    $database_users = Database_Users::getInstance ();
    $time = time ();
    $database_users->pingTimestamp ("phpunit");
    $timestamp = $database_users->getTimestamp ("phpunit");
    $this->assertGreaterThanOrEqual ($time, $timestamp);
    $this->assertLessThanOrEqual ($time + 1, $timestamp);
  }
  
  
  public function testTeams ()
  {
    $database_users = Database_Users::getInstance ();
    
    // Start off without any teams.
    $teams = $database_users->getTeams ();
    $this->assertEquals (array (), $teams);

    // Give user access to Bible, and check that a team has been formed.
    $database_users->grantAccess2Bible ("phpunit", "bible");
    $teams = $database_users->getTeams ();
    $this->assertEquals (array ("bible"), $teams);
    
    // Remove user, and check that no teams have been defined.
    $database_users->removeUser ("phpunit");
    $teams = $database_users->getTeams ();
    $this->assertEquals (array (), $teams);
    
    // Give user access to two Bibles, and check that there are two teams.
    $database_users->grantAccess2Bible ("phpunit", "bible1");
    $database_users->grantAccess2Bible ("phpunit2", "bible2");
    $teams = $database_users->getTeams ();
    $this->assertEquals (array ("bible1", "bible2"), $teams);
  }
  
  
  public function testUserAccess ()
  {
    $database_users = Database_Users::getInstance ();
    
    // Since no teams have been defined, any user has access to any Bible.
    $access = $database_users->hasAccess2Bible ("x", "y");
    $this->assertTrue ($access);
    
    // Grant one user access to one bible, and check free access.
    $database_users->grantAccess2Bible ("phpunit", "bible");
    $access = $database_users->hasAccess2Bible ("phpunit", "bible");
    $this->assertTrue ($access);
    $access = $database_users->hasAccess2Bible ("phpunit2", "bible2");
    $this->assertTrue ($access);
    
    // Grant a user access to a second different bible, and check restricted access.
    $database_users->grantAccess2Bible ("phpunit2", "bible2");
    $access = $database_users->hasAccess2Bible ("phpunit", "bible");
    $this->assertTrue ($access);
    $access = $database_users->hasAccess2Bible ("phpunit2", "bible2");
    $this->assertTrue ($access);
    $access = $database_users->hasAccess2Bible ("phpunit", "bible2");
    $this->assertFalse ($access);
    $access = $database_users->hasAccess2Bible ("x", "y");
    $this->assertFalse ($access);
    
    // Remove that second user/Bible again, and check free access.
    $database_users->removeUser ("phpunit2");
    $access = $database_users->hasAccess2Bible ("phpunit", "bible");
    $this->assertTrue ($access);
    $access = $database_users->hasAccess2Bible ("phpunit", "bible2");
    $this->assertTrue ($access);
    $access = $database_users->hasAccess2Bible ("x", "y");
    $this->assertTrue ($access);

    // Grant and revoke access, and check.
    $database_users->grantAccess2Bible ("phpunit", "bible1");
    $database_users->grantAccess2Bible ("phpunit", "bible2");
    $access = $database_users->hasAccess2Bible ("phpunit", "bible1");
    $this->assertTrue ($access);
    $database_users->revokeAccess2Bible ("phpunit", "bible1");
    $access = $database_users->hasAccess2Bible ("phpunit", "bible1");
    $this->assertFalse ($access);
  }


  public function testAdminAccess ()
  {
    $database_users = Database_Users::getInstance ();
    // Admins have access to any Bible.
    $database_users->addNewUser ("phpunit", "", Filter_Roles::ADMIN_LEVEL, "");

    $access = $database_users->hasAccess2Bible ("phpunit", "bible");
    $this->assertTrue ($access);

    $database_users->grantAccess2Bible ("phpunit2", "bible2");
    $database_users->grantAccess2Bible ("phpunit2", "bible3");
    $database_users->grantAccess2Bible ("phpunit2", "bible4");

    $access = $database_users->hasAccess2Bible ("phpunit", "bible2");
    $this->assertTrue ($access);

    $access = $database_users->hasAccess2Bible ("phpunit", "x");
    $this->assertTrue ($access);
  }  

  
  public function testBiblesUsers ()
  {
    $database_users = Database_Users::getInstance ();
    
    // Give certain users access to certain Bibles, and check their relations.
    $database_users->grantAccess2Bible ("phpunit", "bible1");
    $database_users->grantAccess2Bible ("phpunit", "bible2");
    $database_users->grantAccess2Bible ("phpunit", "bible3");
    $database_users->grantAccess2Bible ("phpunit2", "bible3");
    $database_users->grantAccess2Bible ("phpunit2", "bible4");
    $users = $database_users->getBibles4User ("phpunit");
    $this->assertEquals (array ("bible1", "bible2", "bible3"), $users);
    $users = $database_users->getBibles4User ("phpunit3");
    $this->assertEquals (array (), $users);
    $users = $database_users->getUsers4Bible ("bible3");
    $this->assertEquals (array ("phpunit", "phpunit2"), $users);
    $users = $database_users->getUsers4Bible ("bible4");
    $this->assertEquals (array ("phpunit2"), $users);
  }
  
  
  public function testReadOnlyAccess ()
  {
    $database_users = Database_Users::getInstance ();

    // Without any entry, read-only access should be false.
    $readonly = $database_users->hasReadOnlyAccess2Bible ("phpunit", "bible");
    $this->assertFalse ($readonly);
    
    // After granting access to a Bible, read-only should be false.
    $database_users->grantAccess2Bible ("phpunit", "bible");
    $readonly = $database_users->hasReadOnlyAccess2Bible ("phpunit", "bible");
    $this->assertFalse ($readonly);
    
    // Set read-only access to true, and check.
    $database_users->setReadOnlyAccess2Bible ("phpunit", "bible", true);
    $readonly = $database_users->hasReadOnlyAccess2Bible ("phpunit", "bible");
    $this->assertTrue ($readonly);

    // Check on another user.
    $readonly = $database_users->hasReadOnlyAccess2Bible ("phpunit2", "bible");
    $this->assertFalse ($readonly);

    // Set read-only access to false, and check.
    $database_users->setReadOnlyAccess2Bible ("phpunit", "bible", false);
    $readonly = $database_users->hasReadOnlyAccess2Bible ("phpunit", "bible");
    $this->assertFalse ($readonly);
  }
  
  
}


?>

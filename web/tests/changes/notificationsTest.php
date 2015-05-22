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

 
class modificationsNotificationsTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $database_modifications->create ();
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $ids = $database_modifications->getNotificationIdentifiers ();
    foreach ($ids as $id) {
      $database_modifications->deleteNotification ($id);
    }
  }
  
  
  public function testTrim ()
  {
    $database_modifications = Database_Modifications::getInstance ();

    // Record two entries.
    $database_modifications->recordNotification (array ("phpunit1", "phpunit2"), "A", 1, 1, 2, 3, "old1", "mod1", "new1");
    $database_modifications->indexTrimAllNotifications ();
    $ids = $database_modifications->getNotificationIdentifiers ();
    $this->assertEquals (array (1, 2), $ids);

    // After trimming the two entries should still be there.
    $database_modifications->indexTrimAllNotifications ();
    $ids = $database_modifications->getNotificationIdentifiers ();
    $this->assertEquals (array (1, 2), $ids);

    // Set the time back, re-index, trim, and check one entry's gone.
    $file = $database_modifications->notificationTimeFile (1);
    $database_modifications->indexTrimAllNotifications ();
    file_put_contents ($file, time () - 7776001);
    $database_modifications->indexTrimAllNotifications ();
    $ids = $database_modifications->getNotificationIdentifiers ();
    $this->assertEquals (array (2), $ids);
  }


  public function testNextIdentifier ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $identifier = $database_modifications->getNextAvailableNotificationIdentifier ();
    $this->assertEquals (1, $identifier);
    $database_modifications->recordNotification (array ("phpunit1"), "A", 1, 1, 2, 3, "old1", "mod1", "new1");
    $identifier = $database_modifications->getNextAvailableNotificationIdentifier ();
    $this->assertEquals (2, $identifier);
    $database_modifications->deleteNotification (1);
    $identifier = $database_modifications->getNextAvailableNotificationIdentifier ();
    $this->assertEquals (1, $identifier);
  }
  

  public function testRecordDetailsRetrieval ()
  {
    $database_modifications = Database_Modifications::getInstance ();

    // Start with no identifiers.
    $ids = $database_modifications->getNotificationIdentifiers ();
    $this->assertEmpty ($ids);

    // Record three notifications and reindex.
    $database_modifications->recordNotification (array ("phpunit1", "phpunit2"), "A", 1, 1, 2, 3, "old1", "mod1", "new1");
    $database_modifications->recordNotification (array ("phpunit2", "phpunit3"), "A", 1, 4, 5, 6, "old2", "mod2", "new2");
    $database_modifications->recordNotification (array ("phpunit3", "phpunit4"), "B", 1, 7, 8, 9, "old3", "mod3", "new3");
    $database_modifications->indexTrimAllNotifications ();

    // There should be six notifications now: Two users per recordNotification call.
    $ids = $database_modifications->getNotificationIdentifiers ();
    $this->assertEquals (array (1, 2, 3, 4, 5, 6), $ids);
    
    // Test notifications per user.
    $ids = $database_modifications->getNotificationIdentifiers ("phpunit1");
    $this->assertEquals (array (1), $ids);
    $ids = $database_modifications->getNotificationIdentifiers ("phpunit3");
    $this->assertEquals (array (4, 5), $ids);
  }
  
  
  public function testTimestamps ()
  {
    $database_modifications = Database_Modifications::getInstance ();

    $timestamp = $database_modifications->getNotificationTimeStamp (0);
    $this->assertGreaterThan (time() - 2, $timestamp);
    $this->assertLessThan (time() + 2, $timestamp);

    $time = time () - 21600;
    $database_modifications->recordNotification (array ("phpunit"), "A", 1, 1, 2, 3, "old1", "mod1", "new1");
    $database_modifications->indexTrimAllNotifications ();
    $timestamp = $database_modifications->getNotificationTimeStamp (1);
    $this->assertGreaterThan ($time - 2, $timestamp);
    $this->assertLessThan ($time + 2, $timestamp);
  }
  
  
  public function testCategory ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $database_modifications->recordNotification (array ("phpunit"), "A", 1, 1, 2, 3, "old1", "mod1", "new1");
    $database_modifications->indexTrimAllNotifications ();
    $category = $database_modifications->getNotificationCategory (1);
    $this->assertEquals ("A", $category);
    $category = $database_modifications->getNotificationCategory (2);
    $this->assertNull ($category);
  }


  public function testBible ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $database_modifications->recordNotification (array ("phpunit"), "A", 1, 1, 2, 3, "old1", "mod1", "new1");
    $database_modifications->indexTrimAllNotifications ();
    $bible = $database_modifications->getNotificationBible (1);
    $this->assertEquals (1, $bible);
    $bible = $database_modifications->getNotificationBible (3);
    $this->assertEquals (0, $bible);
  }


  public function testPassage ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $database_modifications->recordNotification (array ("phpunit"), "A", 1, 1, 2, 3, "old1", "mod1", "new1");
    $database_modifications->indexTrimAllNotifications ();
    $passage = $database_modifications->getNotificationPassage (1);
    $this->assertEquals (array ('book' => 1, 'chapter' => 2, 'verse' => 3), $passage);
    $passage = $database_modifications->getNotificationPassage (3);
    $this->assertEmpty ($passage);
  }


  public function testOldText ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $database_modifications->recordNotification (array ("phpunit"), "A", 1, 1, 2, 3, "old1", "mod1", "new1");
    $database_modifications->indexTrimAllNotifications ();
    $old = $database_modifications->getNotificationOldText (1);
    $this->assertEquals ("old1", $old);
    $old = $database_modifications->getNotificationOldText (3);
    $this->assertEquals ("", $old);
  }


  public function testModification ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $database_modifications->recordNotification (array ("phpunit"), "A", 1, 1, 2, 3, "old1", "mod1", "new1");
    $database_modifications->indexTrimAllNotifications ();
    $modification = $database_modifications->getNotificationModification (1);
    $this->assertEquals ("mod1", $modification);
    $modification = $database_modifications->getNotificationOldText (3);
    $this->assertEquals ("", $modification);
  }


  public function testNewText ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $database_modifications->recordNotification (array ("phpunit"), "A", 1, 1, 2, 3, "old1", "mod1", "new1");
    $database_modifications->indexTrimAllNotifications ();
    $new = $database_modifications->getNotificationNewText (1);
    $this->assertEquals ("new1", $new);
    $new = $database_modifications->getNotificationNewText (3);
    $this->assertEquals ("", $new);
  }
  
  
  public function testClearUser ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $database_modifications->recordNotification (array ("phpunit1", "phpunit2", "phpunit3"), "A", 1, 1, 2, 3, "old1", "mod1", "new1");
    $database_modifications->indexTrimAllNotifications ();
    $ids = $database_modifications->getNotificationIdentifiers ();
    $this->assertEquals (3, count ($ids));

    $database_modifications->clearNotificationsUser ("phpunit2");

    $ids = $database_modifications->getNotificationIdentifiers ();
    $this->assertEquals (2, count ($ids));

    $ids = $database_modifications->getNotificationIdentifiers ("phpunit2");
    $this->assertEquals (0, count ($ids));
  }


  public function testClearMatchesOne ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $database_modifications->recordNotification (array ("phpunit"), "P", 1, 2, 3, 4, "old1", "mod1", "new1");
    $database_modifications->recordNotification (array ("phpunit"), "T", 1, 2, 3, 4, "old1", "mod1", "new1");
    $database_modifications->indexTrimAllNotifications ();
    $ids = $database_modifications->getNotificationIdentifiers ();
    $this->assertEquals (2, count ($ids));
    $database_modifications->clearNotificationMatches ("phpunit", "P", "T");
    $database_modifications->indexTrimAllNotifications ();
    $ids = $database_modifications->getNotificationIdentifiers ();
    $this->assertEquals (0, count ($ids));
  }


  public function testNotificationPersonalIdentifiers ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $database_modifications->recordNotification (array ("phpunit1", "phpunit2"), "A", 1, 1, 2, 3, "old1", "mod1", "new1");
    $database_modifications->recordNotification (array ("phpunit2", "phpunit1"), "B", 1, 1, 2, 3, "old2", "mod2", "new2");
    $database_modifications->recordNotification (array ("phpunit3", "phpunit4"), "B", 1, 7, 8, 9, "old3", "mod3", "new3");
    $database_modifications->indexTrimAllNotifications ();
    $ids = $database_modifications->getNotificationPersonalIdentifiers ("phpunit1", "A");
    $this->assertEquals (array (1, 4), $ids);
  }
  

  public function testNotificationTeamIdentifiers ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $database_modifications->recordNotification (array ("phpunit1", "phpunit2"), "A", 1, 1, 2, 3, "old1", "mod1", "new1");
    $database_modifications->recordNotification (array ("phpunit2", "phpunit1"), "B", 1, 1, 2, 3, "old2", "mod2", "new2");
    $database_modifications->recordNotification (array ("phpunit3", "phpunit4"), "B", 1, 7, 8, 9, "old3", "mod3", "new3");
    $database_modifications->indexTrimAllNotifications ();
    $ids = $database_modifications->getNotificationTeamIdentifiers ("phpunit1", "A");
    $this->assertEquals (array (1), $ids);
    $ids = $database_modifications->getNotificationTeamIdentifiers ("phpunit1", "B");
    $this->assertEquals (array (4), $ids);
  }


}


?>

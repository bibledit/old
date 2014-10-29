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

 
class databaseNotesTest extends PHPUnit_Framework_TestCase
{


  private $identifiers;


  public function setUp ()
  {
    $this->identifiers = array ();
  }

  
  public function tearDown ()
  {
    $database_notes = Database_Notes::getInstance();
    foreach ($this->identifiers as $identifier) {
      $database_notes->delete ($identifier);
    }
    $database_notes->set_availability (true);
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("");
  }


  public function testDatabasePath ()
  {
    $database_notes = Database_Notes::getInstance ();

    $path = $database_notes->database_path ();
    $this->assertEquals (dirname (dirname (__DIR__)) . "/web/databases/notes.sqlite", $path);

    $path = $database_notes->checksums_database_path ();
    $this->assertEquals (dirname (dirname (__DIR__)) . "/web/databases/notes_checksums.sqlite", $path);
  }
  

  public function testTrimOptimize ()
  {
    $database_notes = Database_Notes::getInstance();
    $database_notes->optimize ();
    $identifier = $database_notes->storeNewNote ("", 0, 0, 0, "", "", false);
    $database_notes->delete ($identifier);
    $database_notes->trim ();
    $database_notes->trim_server ();
  }


  public function testIdentifier()
  {
    $database_notes = Database_Notes::getInstance();

    $identifier = Notes_Logic::lowNoteIdentifier;
    $this->assertEquals (100000000, $identifier);

    $identifier = Notes_Logic::highNoteIdentifier;
    $this->assertEquals (999999999, $identifier);

    $identifier = $database_notes->getNewUniqueIdentifier ();
    $this->assertGreaterThanOrEqual (100000000, $identifier);
    $this->assertLessThanOrEqual (999999999, $identifier);
    $this->assertFalse ($database_notes->identifierExists ($identifier));

    $identifier = $database_notes->storeNewNote ("", 0, 0, 0, "", "", false);
    $this->assertTrue ($database_notes->identifierExists ($identifier));
    $database_notes->delete ($identifier);
    $this->assertFalse ($database_notes->identifierExists ($identifier));
  }


  public function testSummaryContents ()
  {
    $database_notes = Database_Notes::getInstance();
    // Test inserting data for both summary and contents.
    $summary = "Summary";
    $contents = "Contents";
    $identifier = $database_notes->storeNewNote ("", 0, 0, 0, $summary, $contents, false);
    $this->identifiers [] = $identifier;
    $value = $database_notes->getSummary ($identifier);
    $this->assertEquals ($summary, $value);
    $value = $database_notes->getContents ($identifier);
    $value = explode ("\n", $value);
    $value = $value[1];
    $this->assertEquals ("<p>$contents</p>", $value);
    // Test that if the summary is not given, it is going to be the first line of the contents.
    $contents = "This is a note.\nLine two.";
    $identifier = $database_notes->storeNewNote ("", 0, 0, 0, "", $contents, false);
    $this->identifiers [] = $identifier;
    $value = $database_notes->getSummary ($identifier);
    $this->assertEquals ("This is a note.", $value);
    $value = $database_notes->getContents ($identifier);
    $value = explode ("\n", $value);
    $value = $value[2];
    $this->assertEquals ("<p>Line two.</p>", $value);
    // Test setSummary function.
    $database_notes->setSummary ($identifier, "summary1");
    $value = $database_notes->getSummary ($identifier);
    $this->assertEquals ("summary1", $value);
    // Test setContents function.
    $database_notes->setContents ($identifier, "contents1");
    $value = $database_notes->getContents ($identifier);
    $this->assertEquals ("contents1", $value);
    // Test adding comment.
    $value = $database_notes->getContents ($identifier);
    $length = strlen ($value);
    $database_notes->addComment ($identifier, "comment1");
    $value = $database_notes->getContents ($identifier);
    $this->assertGreaterThan ($length + 30, strlen ($value));
    $this->assertContains ("comment1", $value);
  }


  public function testSubscriptions ()
  {
    $database_notes = Database_Notes::getInstance();
    $session_logic = Session_Logic::getInstance ();

    // Normally creating a new note would subscribe the current user to the note.
    // But since this PHPUnit test runs without sessions, it would have subscribed an empty user.
    $session_logic->setUsername ("");
    $identifier = $database_notes->storeNewNote ("", 0, 0, 0, "Summary", "Contents", false);
    $this->identifiers [] = $identifier;
    $subscribers = $database_notes->getSubscribers ($identifier);
    $this->assertEquals (array (), $subscribers);

    // Create a note again, but this time set the session variable to a certain user.
    $session_logic->setUsername ("phpunit");
    $database_config_user = Database_Config_User::getInstance ();
    $database_config_user->setSubscribeToConsultationNotesEditedByMe (true);
    $identifier = $database_notes->storeNewNote ("", 1, 1, 1, "Summary", "Contents", false);
    $this->identifiers [] = $identifier;
    $notes_logic = Notes_Logic::getInstance();
    $notes_logic->handlerNewNote ($identifier);
    $subscribers = $database_notes->getSubscribers ($identifier);
    $this->assertEquals (array ("phpunit"), $subscribers);
    $this->assertTrue ($database_notes->isSubscribed ($identifier, "phpunit"));
    $database_config_user->setSubscribeToConsultationNotesEditedByMe (false);
    
    // Test various other subscription related functions.
    $this->assertFalse ($database_notes->isSubscribed ($identifier, "phpunit_phpunit"));
    $database_notes->unsubscribe ($identifier);
    $this->assertFalse ($database_notes->isSubscribed ($identifier, "phpunit"));
    $database_notes->subscribeUser ($identifier, "phpunit_phpunit_phpunit");
    $this->assertTrue ($database_notes->isSubscribed ($identifier, "phpunit_phpunit_phpunit"));
    $database_notes->unsubscribeUser ($identifier, "phpunit_phpunit_phpunit");
    $this->assertFalse ($database_notes->isSubscribed ($identifier, "phpunit_phpunit_phpunit"));
  }


  public function testAssignments ()
  {
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("PHPUnit2");
    $database_notes = Database_Notes::getInstance();

    // Create a note and check that it was not assigned to anybody.
    $identifier = $database_notes->storeNewNote ("", 0, 0, 0, "Summary", "Contents", false);
    $this->identifiers [] = $identifier;
    $assignees = $database_notes->getAssignees ($identifier);
    $this->assertEquals (array (), $assignees);

    // Assign the note to a user, and check that this reflects in the list of assignees.
    $database_notes->assignUser ($identifier, "PHPUnit");
    $assignees = $database_notes->getAssignees ($identifier);
    $this->assertEquals (array ("PHPUnit"), $assignees);

    // Test the setAssignees function.
    $database_notes->setAssignees ($identifier, array ("PHPUnit"));
    $assignees = $database_notes->getAssignees ($identifier);
    $this->assertEquals (array ("PHPUnit"), $assignees);
        
    // Assign note to second user, and check it reflects.
    $database_notes->assignUser ($identifier, "PHPUnit2");
    $assignees = $database_notes->getAssignees ($identifier);
    $this->assertEquals (array ("PHPUnit", "PHPUnit2"), $assignees);

    // Based on the above, check the isAssigned function.
    $this->assertTrue ($database_notes->isAssigned ($identifier, "PHPUnit"));
    $this->assertTrue ($database_notes->isAssigned ($identifier, "PHPUnit2"));
    $this->assertFalse ($database_notes->isAssigned ($identifier, "PHPUnit3"));
    
    // Based on the above, test getAllAssignees().
    $assignees = $database_notes->getAllAssignees (array (""));
    $this->assertTrue (in_array ("PHPUnit", $assignees));
    $this->assertTrue (in_array ("PHPUnit2", $assignees));
    $this->assertFalse (in_array ("PHPUnit3", $assignees));
    
    // Based on the above, test the unassignUser function.
    $database_notes->unassignUser ($identifier, "PHPUnit");
    $assignees = $database_notes->getAssignees ($identifier);
    $this->assertEquals (array ("PHPUnit2"), $assignees);
    $database_notes->unassignUser ($identifier, "PHPUnit2");
    $assignees = $database_notes->getAssignees ($identifier);
    $this->assertEquals (array (), $assignees);
  }


  public function testBible ()
  {
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("PHPUnit");
    $database_notes = Database_Notes::getInstance();
    @$identifier = $database_notes->storeNewNote ("PHPUnit", 0, 0, 0, "Summary", "Contents", false);
    $this->identifiers [] = $identifier;
    $bible = $database_notes->getBible ($identifier);
    $this->assertEquals ("PHPUnit", $bible);
    $database_notes->setBible ($identifier, "PHPUnit2");
    $bible = $database_notes->getBible ($identifier);
    $this->assertEquals ("PHPUnit2", $bible);
    $database_notes->setBible ($identifier, "");
    $bible = $database_notes->getBible ($identifier);
    $this->assertEquals ("", $bible);
  }


  public function testPassage ()
  {
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("PHPUnit");

    // Create note for a certain passage.
    $database_notes = Database_Notes::getInstance();
    @$identifier = $database_notes->storeNewNote ("", 10, 9, 8, "Summary", "Contents", false);
    $this->identifiers [] = $identifier;
    
    // Test the getPassages method.
    $passages = $database_notes->getPassages ($identifier);
    $this->assertEquals (array (array (10, 9, 8)), $passages);

    // Test the setPassage method.
    $database_notes->setPassages ($identifier, array (array (5, 6, 7)));
    $passages = $database_notes->getPassages ($identifier);
    $this->assertEquals (array (array (5, 6, 7)), $passages);
  }


  public function testStatus ()
  {
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("PHPUnit");

    // Create note.
    $database_notes = Database_Notes::getInstance();
    @$identifier = $database_notes->storeNewNote ("", 0, 0, 0, "Summary", "Contents", false);
    $this->identifiers [] = $identifier;

    // Test default status = New.
    $status = $database_notes->getStatus ($identifier);
    $this->assertEquals ("New", $status);
    
    // Test setStatus function.
    $database_notes->setStatus ($identifier, "xxxxx");
    $status = $database_notes->getStatus ($identifier);
    $this->assertEquals ("xxxxx", $status);
    
    // Test the getStatuses function.
    $statuses = $database_notes->getPossibleStatuses ();
    foreach ($statuses as $status) {
      $statuses2[] = $status[0];
    }
    $this->assertTrue (in_array ("xxxxx", $statuses2));
    // The standard statuses should be there too.
    $this->assertTrue (in_array ("New", $statuses2));
    $this->assertTrue (in_array ("Pending", $statuses2));
    $this->assertTrue (in_array ("In progress", $statuses2));
    $this->assertTrue (in_array ("Done", $statuses2));
    // Test that the array has unique values only.
    $unique_statuses = array_unique ($statuses2);
    $this->assertEquals (count ($unique_statuses), count ($statuses2));
  }
      
      
  public function testSeverity ()
  {
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("PHPUnit");

    // Create note.
    $database_notes = Database_Notes::getInstance();
    @$identifier = $database_notes->storeNewNote ("", 0, 0, 0, "Summary", "Contents", false);
    $this->identifiers [] = $identifier;

    // Test default severity = Normal.
    $severity = $database_notes->getSeverity ($identifier);
    $this->assertEquals ("Normal", $severity);

    // Test setSeverity.
    $database_notes->setRawSeverity ($identifier, 0);
    $severity = $database_notes->getSeverity ($identifier);
    $this->assertEquals ("Wish", $severity);
    $database_notes->setRawSeverity ($identifier, 4);
    $severity = $database_notes->getSeverity ($identifier);
    $this->assertEquals ("Major", $severity);
    
    // Test getSeverities.
    $severities = $database_notes->getPossibleSeverities ();
    $this->assertEquals (array (array (0, "Wish"), array (1, "Minor"), 
                                array (2, "Normal"), array (3, "Important"),
                                array (4, "Major"), array (5, "Critical")), $severities);
  }


  public function testModified ()
  {
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("PHPUnit");

    // Create note.
    $database_notes = Database_Notes::getInstance();
    $identifier = $database_notes->storeNewNote ("", 0, 0, 0, "Summary", "Contents", false);
    $this->identifiers [] = $identifier;
    
    // Test getModified.
    $time = time ();
    $value = $database_notes->getModified ($identifier);
    $this->assertGreaterThanOrEqual ($time, $value);
    $this->assertLessThanOrEqual ($time + 1, $value);
    
    // Test setModified.
    $database_notes->setModified ($identifier, 123456789);
    $value = $database_notes->getModified ($identifier);
    $this->assertEquals (123456789, $value);
  }
  
  
  public function testGetIdentifiers ()
  {
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("phpunit");
    $database_notes = Database_Notes::getInstance();
    
    // Create a few notes.
    for ($i = 0; $i < 3; $i++) {
      $identifier = $database_notes->storeNewNote ("", 0, 0, 0, "summary", "contents", false);
      $this->identifiers [] = $identifier;
    }
    
    // Get the identifiers.
    $identifiers = $database_notes->getIdentifiers ();
    sort ($this->identifiers);
    sort ($identifiers);
    $this->assertEquals ($this->identifiers, $identifiers);
  }
  
  
  public function testSetIdentifier ()
  {
    // Create note.
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("phpunit");
    $database_notes = Database_Notes::getInstance ();
    $identifier = $database_notes->storeNewNote ("", 0, 0, 0, "summary", "contents", false);
    $this->identifiers [] = $identifier;
    
    // Contents of the note.
    $originalContents = $database_notes->getContents ($identifier);
    $this->assertGreaterThan (20, strlen ($originalContents));
    
    // Checksum of the note.
    $originalChecksum = $database_notes->getChecksum ($identifier);
    $this->assertEquals (32, strlen ($originalChecksum));
    
    // Change the identifier.
    $newId = 1234567;
    $database_notes->setIdentifier ($identifier, $newId);
    $this->identifiers [] = $newId;
    
    // Check old and new identifier.
    $contents = $database_notes->getContents ($identifier);
    $this->assertEmpty ($contents);
    $contents = $database_notes->getContents ($newId);
    $this->assertEquals ($originalContents, $contents);

    $checksum = $database_notes->getChecksum ($identifier);
    $this->assertEmpty ($checksum);
    $checksum = $database_notes->getChecksum ($newId);
    $this->assertEquals ($originalChecksum, $checksum);
  }


  public function testExpireOneNote ()
  {
    // It tests whether a note marked for deletion,
    // after touching it 7 or 8 times, is returned as due for deletion,
    // and whether it is not yet due for deletion before that.
    $database_notes = Database_Notes::getInstance();
    $identifier = $database_notes->storeNewNote ("", 0, 0, 0, "summary", "contents", false);
    $this->identifiers [] = $identifier;
    $database_notes->markForDeletion ($identifier);
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array (), $identifiers);
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array (), $identifiers);
    $database_notes->touchMarkedForDeletion ();
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array ($identifier), $identifiers);
    $database_notes->touchMarkedForDeletion ();
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array ($identifier), $identifiers);
  }


  public function testExpireUnmarkNote ()
  {
    // It tests whethe a note marked for deletion,
    // touched 6 times, then unmarked, touched again,
    // will not be due for deletion.
    $database_notes = Database_Notes::getInstance();
    $identifier = $database_notes->storeNewNote ("", 0, 0, 0, "summary", "contents", false);
    $this->identifiers [] = $identifier;
    $database_notes->markForDeletion ($identifier);
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $database_notes->unmarkForDeletion ($identifier);
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array (), $identifiers);
    $database_notes->touchMarkedForDeletion ();
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array (), $identifiers);
  }


  public function testExpireMoreNotes ()
  {
    // It tests whether three notes, marked for deletion on different days,
    // are properly touched so they keep their own number of days.
    $database_notes = Database_Notes::getInstance ();
    $identifier1 = $database_notes->storeNewNote ("", 0, 0, 0, "summary", "contents", false);
    $this->identifiers [] = $identifier1;
    $identifier2 = $database_notes->storeNewNote ("", 0, 0, 0, "summary", "contents", false);
    $this->identifiers [] = $identifier2;
    $identifier3 = $database_notes->storeNewNote ("", 0, 0, 0, "summary", "contents", false);
    $this->identifiers [] = $identifier3;
    $database_notes->markForDeletion ($identifier1);
    $database_notes->touchMarkedForDeletion ();
    $database_notes->markForDeletion ($identifier2);
    $database_notes->touchMarkedForDeletion ();
    $database_notes->markForDeletion ($identifier3);
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $database_notes->touchMarkedForDeletion ();
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array (), $identifiers);
    $database_notes->touchMarkedForDeletion ();
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array ($identifier1), $identifiers);
    $database_notes->unmarkForDeletion ($identifier1);
    $database_notes->touchMarkedForDeletion ();
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array ($identifier2), $identifiers);
    $database_notes->unmarkForDeletion ($identifier2);
    $database_notes->touchMarkedForDeletion ();
    $identifiers = $database_notes->getDueForDeletion ();
    $this->assertEquals (array ($identifier3), $identifiers);
  }  


  public function testExpireIsMarked ()
  {
    $database_notes = Database_Notes::getInstance ();
    $identifier1 = $database_notes->storeNewNote ("", 0, 0, 0, "summary", "contents", false);
    $this->identifiers [] = $identifier1;
    $identifier2 = $database_notes->storeNewNote ("", 0, 0, 0, "summary", "contents", false);
    $this->identifiers [] = $identifier2;
    $identifier3 = $database_notes->storeNewNote ("", 0, 0, 0, "summary", "contents", false);
    $this->identifiers [] = $identifier3;
    $database_notes->markForDeletion ($identifier1);
    $this->assertTrue ($database_notes->isMarkedForDeletion ($identifier1));
    $this->assertFalse ($database_notes->isMarkedForDeletion ($identifier2));
    $database_notes->unmarkForDeletion ($identifier2);
    $this->assertTrue ($database_notes->isMarkedForDeletion ($identifier1));
    $this->assertFalse ($database_notes->isMarkedForDeletion ($identifier2));
    $database_notes->unmarkForDeletion ($identifier1);
    $this->assertFalse ($database_notes->isMarkedForDeletion ($identifier1));
    $this->assertFalse ($database_notes->isMarkedForDeletion ($identifier2));
    $database_notes->markForDeletion ($identifier2);
    $this->assertTrue ($database_notes->isMarkedForDeletion ($identifier2));
  }


  public function testChecksumOne ()
  {
    $database_notes = Database_Notes::getInstance ();

    // Create note to work with.
    $identifier = $database_notes->storeNewNote ("bible", 1, 2, 3, "summary", "contents", false);
    $this->identifiers [] = $identifier;

    // Checksum of new note should be calculated.
    $checksum1 = $database_notes->getChecksum ($identifier);
    $this->assertNotEquals ("", $checksum1);

    // Clear checksum, and recalculate it.
    $database_notes->setChecksum ($identifier, "");
    $checksum = $database_notes->getChecksum ($identifier);
    $this->assertEquals ("", $checksum);
    $database_notes->sync ();
    $checksum2 = $database_notes->getChecksum ($identifier);
    $this->assertEquals ($checksum1, $checksum2);

    // Test that saving a note updates the checksum in most cases.
    $database_notes->setChecksum ($identifier, "");
    $checksum = $database_notes->getChecksum ($identifier);
    $this->assertEquals ("", $checksum);
    $database_notes->setModified ($identifier, 1234567);
    $checksum = $database_notes->getChecksum ($identifier);
    $this->assertNotEquals ("", $checksum);

    $database_notes->deleteChecksum ($identifier);
    $checksum = $database_notes->getChecksum ($identifier);
    $this->assertEquals ("", $checksum);
    $database_notes->setSubscribers ($identifier, array ("subscribers"));
    $checksum = $database_notes->getChecksum ($identifier);
    $this->assertNotEquals ("", $checksum);

    $database_notes->setChecksum ($identifier, "");
    $checksum = $database_notes->getChecksum ($identifier);
    $this->assertEquals ("", $checksum);
    $database_notes->setBible ($identifier, "phpunit");
    $checksum = $database_notes->getChecksum ($identifier);
    $this->assertNotEquals ("", $checksum);

    $database_notes->deleteChecksum ($identifier, "");
    $checksum = $database_notes->getChecksum ($identifier);
    $this->assertEquals ("", $checksum);
    $database_notes->setPassages ($identifier, array ());
    $checksum = $database_notes->getChecksum ($identifier);
    $this->assertNotEquals ("", $checksum);

    $database_notes->setChecksum ($identifier, "");
    $checksum = $database_notes->getChecksum ($identifier);
    $this->assertEquals ("", $checksum);
    $database_notes->setStatus ($identifier, "Status");
    $checksum = $database_notes->getChecksum ($identifier);
    $this->assertNotEquals ("", $checksum);

    $database_notes->deleteChecksum ($identifier, "");
    $checksum = $database_notes->getChecksum ($identifier);
    $this->assertEquals ("", $checksum);
    $database_notes->setRawSeverity ($identifier, 123);
    $checksum = $database_notes->getChecksum ($identifier);
    $this->assertNotEquals ("", $checksum);

    $database_notes->setChecksum ($identifier, "");
    $checksum = $database_notes->getChecksum ($identifier);
    $this->assertEquals ("", $checksum);
    $database_notes->setSummary ($identifier, "new");
    $checksum = $database_notes->getChecksum ($identifier);
    $this->assertNotEquals ("", $checksum);

    $database_notes->deleteChecksum ($identifier, "");
    $checksum = $database_notes->getChecksum ($identifier);
    $this->assertEquals ("", $checksum);
    $database_notes->setContents ($identifier, "new");
    $checksum = $database_notes->getChecksum ($identifier);
    $this->assertNotEquals ("", $checksum);
  }


  public function testChecksumTwo ()
  {
    $database_notes = Database_Notes::getInstance ();

    // Create notes to work with.
    $this->identifiers [] = $database_notes->storeNewNote ("bible1", 1, 2, 3, "summary1", "contents1", false);
    $this->identifiers [] = $database_notes->storeNewNote ("bible2", 2, 3, 4, "summary2", "contents2", false);
    $this->identifiers [] = $database_notes->storeNewNote ("bible3", 3, 4, 5, "summary3", "contents3", false);

    // Checksum calculation: slow and fast methods should be the same.
    $checksum1 = Sync_Logic::checksum ($this->identifiers);
    $this->assertEquals (32, strlen ($checksum1));
    $checksum2 = $database_notes->getMultipleChecksum ($this->identifiers);
    $this->assertEquals ($checksum1, $checksum2);
  }
  
  
  public function testGetNotesInRangeForBibles ()
  {
    $database_notes = Database_Notes::getInstance ();

    // Create a couple of notes to work with.
    $identifier = $database_notes->storeNewNote ("bible1", 1, 2, 3, "summary", "contents", false);
    $identifier1 = 100000000;
    $this->identifiers [] = $identifier1;
    $database_notes->setIdentifier ($identifier, $identifier1);

    $identifier = $database_notes->storeNewNote ("bible2", 1, 2, 3, "summary", "contents", false);
    $identifier2 = 500000000;
    $this->identifiers [] = $identifier2;
    $database_notes->setIdentifier ($identifier, $identifier2);

    $identifier = $database_notes->storeNewNote ("", 1, 2, 3, "summary", "contents", false);
    $identifier3 = 999999999;
    $this->identifiers [] = $identifier3;
    $database_notes->setIdentifier ($identifier, $identifier3);

    // Test selection mechanism for certain Bibles.
    $identifiers = $database_notes->getNotesInRangeForBibles (100000000, 999999999, array ("bible1", "bible2"));
    $this->assertEquals (array (100000000, 500000000, 999999999), $identifiers);

    $identifiers = $database_notes->getNotesInRangeForBibles (100000000, 999999999, array ("bible1", "bible3"));
    $this->assertEquals (array (100000000, 999999999), $identifiers);

    $identifiers = $database_notes->getNotesInRangeForBibles (100000000, 999999999, array ());
    $this->assertEquals (array (999999999), $identifiers);

    // Test selection mechanism for any Bible.
    $identifiers = $database_notes->getNotesInRangeForBibles (100000000, 999999999, NULL);
    $this->assertEquals (array (100000000, 500000000, 999999999), $identifiers);
  }


  public function testCreateRange ()
  {
    $ranges = Sync_Logic::create_range (100000000, 999999999);
    $standard = array (
      array (100000000, 189999998),
      array (189999999, 279999997),
      array (279999998, 369999996),
      array (369999997, 459999995),
      array (459999996, 549999994),
      array (549999995, 639999993),
      array (639999994, 729999992),
      array (729999993, 819999991),
      array (819999992, 909999990),
      array (909999991, 999999999)
    );
    $this->assertEquals ($standard, $ranges);

    $ranges = Sync_Logic::create_range (100000000, 100000100);
    $standard = array (
      array (100000000, 100000009),
      array (100000010, 100000019),
      array (100000020, 100000029),
      array (100000030, 100000039),
      array (100000040, 100000049),
      array (100000050, 100000059),
      array (100000060, 100000069),
      array (100000070, 100000079),
      array (100000080, 100000089),
      array (100000090, 100000100),
    );
    $this->assertEquals ($standard, $ranges);
  }
  
  
  public function testSelectBible ()
  {
    $database_notes = Database_Notes::getInstance ();

    // Create a couple of notes to work with.
    $identifier1 = $database_notes->storeNewNote ("bible1", 1, 2, 3, "summary1", "contents1", false);
    $this->identifiers [] = $identifier1;
    $identifier2 = $database_notes->storeNewNote ("bible2", 1, 2, 3, "summary2", "contents2", false);
    $this->identifiers [] = $identifier2;
    $identifier3 = $database_notes->storeNewNote ("bible3", 1, 2, 3, "summary3", "contents3", false);
    $this->identifiers [] = $identifier3;

    // Select notes while varying Bible selection.
    $identifiers = $database_notes->selectNotes (array ("bible1"), 0, 0, 0, 3, 0, 0, "", "bible1", "", "", -1, 0, "", NULL);
    $this->assertEquals (array ($identifier1), $identifiers);

    $identifiers = $database_notes->selectNotes (array ("bible1", "bible2"), 0, 0, 0, 3, 0, 0, "", "bible2", "", "", -1, 0, "", NULL);
    $this->assertEquals (array ($identifier2), $identifiers);

    $identifiers = $database_notes->selectNotes (array ("bible1", "bible2"), 0, 0, 0, 3, 0, 0, "", "", "", "", -1, 0, "", NULL);
    $this->assertEquals (array ($identifier1, $identifier2), $identifiers);

    $identifiers = $database_notes->selectNotes (array ("bible1", "bible2", "bible4"), 0, 0, 0, 3, 0, 0, "", "bible", "", "", -1, 0, "", NULL);
    $this->assertEquals (array (), $identifiers);

    $identifiers = $database_notes->selectNotes (array (), 0, 0, 0, 3, 0, 0, "", "", "", "", -1, 0, "", NULL);
    $this->assertEquals (array (), $identifiers);

    $identifiers = $database_notes->selectNotes (array ("bible1", "bible2", "bible3"), 0, 0, 0, 3, 0, 0, "", "bible3", "", "", -1, 0, "", NULL);
    $this->assertEquals (array ($identifier3), $identifiers);

    $identifiers = $database_notes->selectNotes (array (), 0, 0, 0, 3, 0, 0, "", "bible3", "", "", -1, 0, "", NULL);
    $this->assertEquals (array ($identifier3), $identifiers);

    $identifiers = $database_notes->selectNotes (NULL, 0, 0, 0, 3, 0, 0, "", "bible3", "", "", -1, 0, "", NULL);
    $this->assertEquals (array ($identifier1, $identifier2, $identifier3), $identifiers);
  }
  

  public function testResilienceNotes ()
  {
    $database_notes = Database_Notes::getInstance ();
    $corrupted_database = "/home/teus/documents/dev/dependencies/notes.sqlite.damaged";

    $path = $database_notes->database_path ();
    
    $healthy = $database_notes->healthy ();
    $this->assertTrue ($healthy);
    
    if (file_exists ($corrupted_database)) {
      copy ($corrupted_database, $path);
      $healthy = $database_notes->healthy ();
      $this->assertFalse ($healthy);
    }
    
    $database_notes->checkup ();
    $healthy = $database_notes->healthy ();
    $this->assertTrue ($healthy);
  }


  public function testResilienceChecksumsNotes ()
  {
    $database_notes = Database_Notes::getInstance ();
    $corrupted_database = "/home/teus/documents/dev/dependencies/notes.sqlite.damaged";

    $path = $database_notes->checksums_database_path ();
    
    $healthy = $database_notes->checksums_healthy ();
    $this->assertTrue ($healthy);
    
    if (file_exists ($corrupted_database)) {
      copy ($corrupted_database, $path);
      $healthy = $database_notes->checksums_healthy ();
      $this->assertFalse ($healthy);
    }
    
    $database_notes->checkup_checksums ();
    $healthy = $database_notes->checksums_healthy ();
    $this->assertTrue ($healthy);
  }
  
  
  public function testAvailability ()
  {
    $database_notes = Database_Notes::getInstance ();
    $this->assertTrue ($database_notes->available ());
    $database_notes->set_availability (false);
    $this->assertFalse ($database_notes->available ());
    $database_notes->set_availability (true);
    $this->assertTrue ($database_notes->available ());
  }


}


?>

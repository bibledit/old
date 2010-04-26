<?php
require_once 'PHPUnit/Framework.php';
 
class notesTest extends PHPUnit_Framework_TestCase
{

  public function testIdentifier()
  {
    $database_notes = Database_Notes::getInstance();
    $identifier = $database_notes->getNewUniqueIdentifier ();
    $this->assertGreaterThanOrEqual (100000000, $identifier);
    $this->assertLessThanOrEqual (999999999, $identifier);
    $this->assertFalse ($database_notes->identifierExists ($identifier));
  }

  public function testSummaryContents ()
  {
    $database_notes = Database_Notes::getInstance();
    // Test inserting data for both summary and contents.
    $summary = "Summary";
    $contents = "Contents";
    @$identifier = $database_notes->storeNewNote ("", 0, 0, 0, $summary, $contents);
    $value = $database_notes->getSummary ($identifier);
    $this->assertEquals ($summary, $value);
    $value = $database_notes->getContents ($identifier);
    $value = explode ("\n", $value);
    $value = $value[1];
    $this->assertEquals ("<p>$contents</p>", $value);
    $database_notes->delete ($identifier);
    // Test that if the summary is not given, it is going to be the first line of the contents.
    unset ($summary);
    $contents = "This is a note.\nLine two.";
    @$identifier = $database_notes->storeNewNote ("", 0, 0, 0, $summary, $contents);
    $value = $database_notes->getSummary ($identifier);
    $this->assertEquals ("This is a note.", $value);
    $value = $database_notes->getContents ($identifier);
    $value = explode ("\n", $value);
    $value = $value[2];
    $this->assertEquals ("<p>Line two.</p>", $value);
    $database_notes->delete ($identifier);
  }

  public function testSubscriptions ()
  {
    $database_notes = Database_Notes::getInstance();

    // Normally creating a new note would subscribe the current user to the note.
    // But since this PHPUnit test runs without sessions, it would have subscribed an empty user.
    unset ($_SESSION['user']);
    @$identifier = $database_notes->storeNewNote ("", 0, 0, 0, "Summary", "Contents");
    $subscribers = $database_notes->getSubscribers ($identifier);
    $this->assertEquals (array (), $subscribers);
    $database_notes->delete ($identifier);

    // Create a note again, but this time set the session variable to a certain user.
    $_SESSION['user'] = 'phpunit';
    $database_config_user = Database_Config_User::getInstance ();
    $database_config_user->setSubscribeToConsultationNotesEditedByMe (true);
    @$identifier = $database_notes->storeNewNote ("", 0, 0, 0, "Summary", "Contents");
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
    $database_notes->delete ($identifier);
    unset ($_SESSION['user']);
  }


  public function testAssignments ()
  {
    $database_notes = Database_Notes::getInstance();

    // Create a note and check that it was not assigned to anybody.
    @$identifier = $database_notes->storeNewNote ("", 0, 0, 0, "Summary", "Contents");
    $assignees = $database_notes->getSubscribers ($identifier);
    $this->assertEquals (array (), $assignees);

    // Assign the note to a user, and check that this reflects in the list of assignees.
    $database_notes->assignUser ($identifier, "PHPUnit");
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
    
    // Based on the above, test the unassign(User) function.
    $database_notes->unassignUser ($identifier, "PHPUnit");
    $assignees = $database_notes->getAssignees ($identifier);
    $this->assertEquals (array ("PHPUnit2"), $assignees);
    $_SESSION['user'] = 'PHPUnit2';
    $database_notes->unassign ($identifier);
    $assignees = $database_notes->getAssignees ($identifier);
    $this->assertEquals (array (), $assignees);
    unset ($_SESSION['user']);

    // Tear down.
    $database_notes->delete ($identifier);
  }


      
}
?>



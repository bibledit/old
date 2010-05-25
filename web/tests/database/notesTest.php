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
    @$identifier = $database_notes->storeNewNote ("", 0, 0, 0, $summary, $contents, false);
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
    @$identifier = $database_notes->storeNewNote ("", 0, 0, 0, $summary, $contents, false);
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
    $_SESSION['user'] = "";
    @$identifier = $database_notes->storeNewNote ("", 0, 0, 0, "Summary", "Contents", false);
    $subscribers = $database_notes->getSubscribers ($identifier);
    $this->assertEquals (array (), $subscribers);
    $database_notes->delete ($identifier);

    // Create a note again, but this time set the session variable to a certain user.
    $_SESSION['user'] = 'phpunit';
    $database_config_user = Database_Config_User::getInstance ();
    $database_config_user->setSubscribeToConsultationNotesEditedByMe (true);
    @$identifier = $database_notes->storeNewNote ("", 0, 0, 0, "Summary", "Contents", false);
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
    $_SESSION = array ();
    $_SESSION['user'] = 'PHPUnit2';
    $database_notes = Database_Notes::getInstance();

    // Create a note and check that it was not assigned to anybody.
    @$identifier = $database_notes->storeNewNote ("", 0, 0, 0, "Summary", "Contents", false);
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
    $assignees = $database_notes->getAllAssignees ();
    $this->assertTrue (in_array ("PHPUnit", $assignees));
    $this->assertTrue (in_array ("PHPUnit2", $assignees));
    $this->assertFalse (in_array ("PHPUnit3", $assignees));
    
    // Based on the above, test the unassign(User) function.
    $database_notes->unassignUser ($identifier, "PHPUnit");
    $assignees = $database_notes->getAssignees ($identifier);
    $this->assertEquals (array ("PHPUnit2"), $assignees);
    $database_notes->unassign ($identifier);
    $assignees = $database_notes->getAssignees ($identifier);
    $this->assertEquals (array (), $assignees);

    // Tear down.
    $database_notes->delete ($identifier);
    unset ($_SESSION);
  }


  public function testBible ()
  {
    $_SESSION = array ();
    $_SESSION['user'] = "PHPUnit";
    $database_notes = Database_Notes::getInstance();
    @$identifier = $database_notes->storeNewNote ("PHPUnit", 0, 0, 0, "Summary", "Contents", false);
    $bible = $database_notes->getBible ($identifier);
    $this->assertEquals ("PHPUnit", $bible);
    $database_notes->setBible ($identifier, "PHPUnit2");
    $bible = $database_notes->getBible ($identifier);
    $this->assertEquals ("PHPUnit2", $bible);
    $database_notes->setBible ($identifier, "");
    $bible = $database_notes->getBible ($identifier);
    $this->assertEquals ("", $bible);
    $database_notes->delete ($identifier);
    unset ($_SESSION);
  }


  public function testPassage ()
  {
    $_SESSION = array ();
    $_SESSION['user'] = "PHPUnit";

    // Create note for a certain passage.
    $database_notes = Database_Notes::getInstance();
    @$identifier = $database_notes->storeNewNote ("", 10, 9, 8, "Summary", "Contents", false);
    
    // Test the getPassages method.
    $passages = $database_notes->getPassages ($identifier);
    $this->assertEquals (array (array (10, 9, 8)), $passages);

    // Test the setPassage method.
    $database_notes->setPassages ($identifier, array (array (5, 6, 7)));
    $passages = $database_notes->getPassages ($identifier);
    $this->assertEquals (array (array (5, 6, 7)), $passages);

    // Test the addPassages method.
    $database_notes->setPassages ($identifier, array (array (5, 6, 7)));
    $database_notes->addPassage ($identifier, 10, 11, 12);
    $database_notes->addPassage ($identifier, 10, 11, 12);
    $passages = $database_notes->getPassages ($identifier);
    $this->assertEquals (array (array (5, 6, 7), array (10, 11, 12)), $passages);

    // Test the removePassage method.
    $database_notes->setPassages ($identifier, array (array (1, 2, 3)));
    $database_notes->removePassage ($identifier, 1, 2, 3);
    $passages = $database_notes->getPassages ($identifier);
    $this->assertEquals (array (), $passages);
    $database_notes->addPassage ($identifier, 10, 11, 12);
    $database_notes->addPassage ($identifier, 13, 14, 15);
    $database_notes->removePassage ($identifier, 10, 11, 12);
    $passages = $database_notes->getPassages ($identifier);
    $this->assertEquals (array (array (13, 14, 15)), $passages);

    // Test the passageContained method.
    $database_notes->setPassages ($identifier, array (array (5, 4, 3)));
    $this->assertTrue ($database_notes->passageContained ($identifier, 5, 4, 3));
    $this->assertFalse ($database_notes->passageContained ($identifier, 5, 44, 3));

    // Tear down.
    $database_notes->delete ($identifier);
    unset ($_SESSION);
  }


  public function testStatus ()
  {
    $_SESSION = array ();
    $_SESSION['user'] = "PHPUnit";

    // Create note.
    $database_notes = Database_Notes::getInstance();
    @$identifier = $database_notes->storeNewNote ("", 0, 0, 0, "Summary", "Contents", false);

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

    // Tear down.
    $database_notes->delete ($identifier);
    unset ($_SESSION);
  }
      
      
  public function testSeverity ()
  {
    $_SESSION = array ();
    $_SESSION['user'] = "PHPUnit";

    // Create note.
    $database_notes = Database_Notes::getInstance();
    @$identifier = $database_notes->storeNewNote ("", 0, 0, 0, "Summary", "Contents", false);

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

    // Tear down.
    $database_notes->delete ($identifier);
    unset ($_SESSION);
  }


  public function testPrivacy ()
  {
    $_SESSION = array ();
    $_SESSION['user'] = "PHPUnit";
    
    // Create note.
    $database_notes = Database_Notes::getInstance();
    @$identifier = $database_notes->storeNewNote ("", 0, 0, 0, "Summary", "Contents", false);

    // Test default privacy = 0.
    $privacy = $database_notes->getPrivacy ($identifier);
    $this->assertEquals (0, $privacy);

    // Test setPrivacy.
    $database_notes->setPrivacy ($identifier, 111);
    $privacy = $database_notes->getPrivacy ($identifier);
    $this->assertEquals (111, $privacy);

    // Test getPossiblePrivacies.
    $privacies = $database_notes->getPossiblePrivacies ();
    $this->assertEquals (array (1, 2, 3, 4, 5, 6), $privacies);

    // Tear down.
    $database_notes->delete ($identifier);
    unset ($_SESSION);
  }

      
}
?>



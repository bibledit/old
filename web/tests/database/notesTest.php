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
  
}
?>



<?php


class filterNotesTest extends PHPUnit_Framework_TestCase
{


  protected $note_identifiers = array ();


  public function testPrivacy2text()
  {
    $this->assertEquals("Manager | Administrator", Filter_Notes::privacy2text (5));
    $this->assertEquals("Guest | Member | Consultant | Translator | Manager | Administrator", Filter_Notes::privacy2text (0));
    $this->assertEquals("", Filter_Notes::privacy2text (1000));
  }


  public function testFilterNotesImportFromBibleditGtkFile()
  {
    $_SERVER['HTTP_USER_AGENT'] = "PHPUnit";
    $_SERVER['REMOTE_ADDR'] = "127.0.0.1";
    $_SESSION['user'] = "PHPUnit";
    $database_notes = Database_Notes::getInstance();
    $filename = tempnam (sys_get_temp_dir(), '');
$data = <<<EOD
731157
Note Author
Acts.3.13
No issue
Ndebele
731157
umdumisile: Text was changed from "has given glory" to "has glorified".
Logbook:
EOD;
    file_put_contents ($filename, $data);
    $identifier = Filter_Notes::importFromBibleditGtkFile ($filename);
    $this->note_identifiers [] = $identifier;
    $bible = $database_notes->getBible ($identifier);
    $this->assertEquals ("Ndebele", $bible);
    $passages = $database_notes->getPassages ($identifier);
    $this->assertEquals (array (array (44, 3, 13)), $passages);
    $summary = $database_notes->getSummary ($identifier);
    $this->assertEquals ("umdumisile: Text was changed from \"has given glory\" to \"has glorified\".", $summary);
    $status = $database_notes->getStatus ($identifier);
    $this->assertEquals ("No issue", $status);
    
$data = <<<EOD
731157
Note Author
Lev.26.16 Deut.28.22
No issue
Ndebele
731157
umdumisile: Text was changed from "has given glory" to "has glorified".
Logbook:
EOD;
    file_put_contents ($filename, $data);
    $identifier = Filter_Notes::importFromBibleditGtkFile ($filename);
    $this->note_identifiers [] = $identifier;
    $passages = $database_notes->getPassages ($identifier);
    $this->assertEquals (array (array (3, 26, 16), array (5, 28, 22)), $passages);
    $contents = $database_notes->getContents ($identifier);
    $this->assertEquals ("umdumisile: Text was changed from \"has given glory\" to \"has glorified\".\n\nLogbook:\n", $contents);
    
    // Tear down.
    unlink ($filename);
  }
  

  protected function tearDown()
  {
    $database_notes = Database_Notes::getInstance();
    foreach ($this->note_identifiers as $identifier) {
      $database_notes->delete ($identifier);
    }
  } 
}
?>
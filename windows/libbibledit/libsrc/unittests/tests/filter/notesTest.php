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


class filterNotesTest extends PHPUnit_Framework_TestCase
{


  protected $note_identifiers = array ();


  public function testFilterNotesImportFromBibleditGtkFile()
  {
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("PHPUnit");
    $_SERVER['HTTP_USER_AGENT'] = "PHPUnit";
    $_SERVER['REMOTE_ADDR'] = "127.0.0.1";
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

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

 
class ipcNotesTest extends PHPUnit_Framework_TestCase
{

  protected function setUp () 
  {
    $this->tearDown ();
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("phpunit");
  }


  protected function tearDown ()
  {
    $database_ipc = Database_Ipc::getInstance ();
    $database_ipc->trim ();
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("");
  }


  public function testNone ()
  {
    $ipc_notes = Ipc_Notes::getInstance ();
    $data = $ipc_notes->get ();
    $this->assertNull ($data);
  }

  
  public function testOpen ()
  {
    $ipc_notes = Ipc_Notes::getInstance ();
    $ipc_notes->open (123456789);
    $identifier = $ipc_notes->get ();
    $this->assertEquals (123456789, $identifier);
  }
  

  public function testDeleteOne ()
  {
    $ipc_notes = Ipc_Notes::getInstance ();
    $ipc_notes->open (123456789);
    $ipc_notes->delete ();
    $identifier = $ipc_notes->get ();
    $this->assertNull ($identifier);
  }

  
  public function testDeleteTwo ()
  {
    $ipc_notes = Ipc_Notes::getInstance ();
    $ipc_notes->open (123456789);
    $ipc_notes->open (123456789);
    $ipc_notes->delete ();
    $data = $ipc_notes->get ();
    $this->assertNull ($data);
    $ipc_notes->delete ();
    $data = $ipc_notes->get ();
    $this->assertNull ($data);
  }

  
}
?>

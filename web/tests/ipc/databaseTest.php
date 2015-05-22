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

 
class databaseIpcTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    $database_ipc = Database_Ipc::getInstance ();
    for ($i = 0; $i < 10; $i++) {
      $database_ipc->deleteMessage ($i);
    }
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("");
  }


  public function testTrim ()
  {
    $database_ipc = Database_Ipc::getInstance ();
    $database_ipc->trim ();
  }
  
  
  public function testStoreRetrieve ()
  {
    $database_ipc = Database_Ipc::getInstance ();
    
    $id = 1;
    $user = "phpunit";
    $channel = "channel";
    $command = "command";
    $message = "message";

    $database_ipc->storeMessage ($user, $channel, $command, $message);
   
    $data = $database_ipc->retrieveMessage ($id, $user, $channel, $command);
    $this->assertNull ($data);

    $database_ipc->storeMessage ($user, $channel, $command, $message);

    $data = $database_ipc->retrieveMessage ($id, $user, $channel, $command);
    $this->assertEquals (2, $data ['id']);
    $this->assertEquals ($message, $data ['message']);
  }
  
  
  public function testDelete ()
  {
    $database_ipc = Database_Ipc::getInstance ();
    
    $id = 1;
    $user = "phpunit";
    $channel = "channel";
    $command = "command";
    $message = "message";

    $database_ipc->storeMessage ($user, $channel, $command, $message);
    
    $data = $database_ipc->retrieveMessage (0, $user, $channel, $command);
    $this->assertNotNull ($data);
    
    $database_ipc->deleteMessage ($id);

    $data = $database_ipc->retrieveMessage (0, $user, $channel, $command);
    $this->assertNull ($data);
  }


  public function testGetFocus ()
  {
    $database_ipc = Database_Ipc::getInstance ();
    $session_logic = Session_Logic::getInstance ();
    
    $id = 1;
    $user = "phpunit";
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ($user);
    $channel = "channel";
    $command = "focus";

    $passage = $database_ipc->getFocus ();
    $this->assertEquals ("1.1.1", $passage);

    $message = "2.3.4";
    $database_ipc->storeMessage ($user, $channel, $command, $message);
    $passage = $database_ipc->getFocus ();
    $this->assertEquals ($message, $passage);

    $message = "5.6.7";
    $database_ipc->storeMessage ($user, $channel, $command, $message);
    $passage = $database_ipc->getFocus ();
    $this->assertEquals ($message, $passage);
  }


  public function testGetNote ()
  {
    $database_ipc = Database_Ipc::getInstance ();
    
    $id = 1;
    $user = "phpunit";
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ($user);
    $channel = "channel";
    $command = "opennote";

    $note = $database_ipc->getNote ();
    $this->assertNull ($note);

    $message = "12345";
    $database_ipc->storeMessage ($user, $channel, $command, $message);
    $note = $database_ipc->getNote ();
    $this->assertEquals ($message, $note ['message']);

    $message = "54321";
    $database_ipc->storeMessage ($user, $channel, $command, $message);
    $note = $database_ipc->getNote ();
    $this->assertEquals ($message, $note ['message']);
  }


  public function testNotesAlive ()
  {
    $database_ipc = Database_Ipc::getInstance ();
    
    $id = 1;
    $user = "phpunit";
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ($user);
    $channel = "channel";
    $command = "notesalive";

    $note = $database_ipc->getNotesAlive ();
    $this->assertFalse ($note);

    $message = "1";
    $database_ipc->storeMessage ($user, $channel, $command, $message);
    $note = $database_ipc->getNotesAlive ();
    $this->assertEquals ($message, $note);

    $message = "0";
    $database_ipc->storeMessage ($user, $channel, $command, $message);
    $note = $database_ipc->getNotesAlive ();
    $this->assertEquals ($message, $note);
  }
  

  public function testBibleAlive ()
  {
    $database_ipc = Database_Ipc::getInstance ();
    
    $id = 1;
    $user = "phpunit";
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ($user);
    $channel = "channel";
    $command = "biblealive";

    $alive = $database_ipc->getBibleAlive ();
    $this->assertFalse ($alive);

    $message = "1";
    $database_ipc->storeMessage ($user, $channel, $command, $message);
    $alive = $database_ipc->getBibleAlive ();
    $this->assertEquals ($message, $alive);

    $message = "0";
    $database_ipc->storeMessage ($user, $channel, $command, $message);
    $alive = $database_ipc->getBibleAlive ();
    $this->assertEquals ($message, $alive);
  }


}


?>

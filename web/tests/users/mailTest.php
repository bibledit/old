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

 
class databaseMailTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    $database_mail = Database_Mail::getInstance ();
    for ($i = 0; $i < 10; $i++) {
      $database_mail->delete ($i);
    }
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("");
  }


  public function testOptimize ()
  {
    $database_mail = Database_Mail::getInstance ();
    $database_mail->optimize ();
  }


  public function testTrim ()
  {
    $database_mail = Database_Mail::getInstance ();
    $database_mail->trim ();
  }


  public function testEmpty ()
  {
    $database_mail = Database_Mail::getInstance ();

    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("phpunit");

    $count = $database_mail->getMailCount ();
    $this->assertEquals (0, $count);
    
    $mails = $database_mail->getMails ();
    $this->assertEmpty ($mails);
    
    $mails = $database_mail->getMailsToSend ();
    $this->assertEmpty ($mails);
  }
    
  
  public function testNormalCycleOne ()
  {
    $database_mail = Database_Mail::getInstance ();

    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("phpunit");
    
    $database_mail->send ("phpunit", "subject", "body");

    $count = $database_mail->getMailCount ();
    $this->assertEquals (1, $count);

    $mails = $database_mail->getMails ();
    $this->assertEquals ("subject", $mails [0] ['subject']);
    
    $mail = $database_mail->get (1);
    $this->assertEquals ("phpunit", $mail ['username']);
    $this->assertEquals ("body", $mail ['body']);
    
    $database_mail->delete (1);

    $count = $database_mail->getMailCount ();
    $this->assertEquals (0, $count);
  }
  

  public function testNormalPostpone ()
  {
    $database_mail = Database_Mail::getInstance ();

    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("phpunit");
    
    $database_mail->send ("phpunit", "subject", "body");

    $mails = $database_mail->getMailsToSend ();
    $this->assertEquals (1, count($mails));
    
    $database_mail->postpone (1);
    $mails = $database_mail->getMailsToSend ();
    $this->assertEquals (0, count($mails));
  }
  

}


?>

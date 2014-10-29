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

 
class databaseBooksTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
  }


  public function testOptimize ()
  {
    $database_books = Database_Books::getInstance ();
    $database_books->optimize ();
  }
  
  
  public function testGetIDs ()
  {
    $database_books = Database_Books::getInstance ();
    $ids = $database_books->getIDs ();
    $count = count ($ids);
    $this->assertEquals (69, $count);
  }


  public function testGetIdFromEnglish ()
  {
    $database_books = Database_Books::getInstance ();
    $id = $database_books->getIdFromEnglish ("Exodus");
    $this->assertEquals (2, $id);
  }


  public function testGetEnglishFromId ()
  {
    $database_books = Database_Books::getInstance ();
    $name = $database_books->getEnglishFromId (3);
    $this->assertEquals ("Leviticus", $name);
  }


  public function testGetUsfmFromId ()
  {
    $database_books = Database_Books::getInstance ();
    $usfm = $database_books->getUsfmFromId (4);
    $this->assertEquals ("NUM", $usfm);
  }


  public function testGetBibleworksFromId ()
  {
    $database_books = Database_Books::getInstance ();
    $name = $database_books->getBibleworksFromId (5);
    $this->assertEquals ("Deu", $name);
  }


  public function testGetIdFromUsfm ()
  {
    $database_books = Database_Books::getInstance ();
    $id = $database_books->getIdFromUsfm ("SNG");
    $this->assertEquals (22, $id);
  }


  public function testGetIdFromOsis ()
  {
    $database_books = Database_Books::getInstance ();
    $id = $database_books->getIdFromOsis ("1Chr");
    $this->assertEquals (13, $id);
  }


  public function testGetIdFromBibleworks ()
  {
    $database_books = Database_Books::getInstance ();
    $id = $database_books->getIdFromBibleworks ("2Ki");
    $this->assertEquals (12, $id);
  }


  public function testGetIdLikeText ()
  {
    $database_books = Database_Books::getInstance ();
    $id = $database_books->getIdLikeText ("2Ki");
    $this->assertEquals (12, $id);
  }


  public function testGetIdFromOnlinebible ()
  {
    $database_books = Database_Books::getInstance ();
    $id = $database_books->getIdFromOnlinebible ("2Ki");
    $this->assertEquals (12, $id);
  }


  public function testGetOnlinebibleFromId ()
  {
    $database_books = Database_Books::getInstance ();
    $name = $database_books->getOnlinebibleFromId (5);
    $this->assertEquals ("De", $name);
  }


  public function testGetSequenceFromId ()
  {
    $database_books = Database_Books::getInstance ();
    $name = $database_books->getSequenceFromId (5);
    $this->assertEquals ("5", $name);
  }
  
  
  public function testGetType ()
  {
    $database_books = Database_Books::getInstance ();
    $type = $database_books->getType (40);
    $this->assertEquals ("nt", $type);
    $type = $database_books->getType (39);
    $this->assertEquals ("ot", $type);
    $type = $database_books->getType (0);
    $this->assertEquals ("", $type);
  }


}


?>

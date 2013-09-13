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


class databaseConsistencyTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
    $database_consistency = Database_Consistency::getInstance();
    $database_consistency->trim ();
  }


  public function testPassagesTranslations ()
  {
    $database_consistency = Database_Consistency::getInstance();
$passages = <<<EOD
Mat. 1.1
Luk. 2.2
EOD;
$translations = <<<EOD
Jesus
EOD;
    $database_consistency->storePassagesTranslations (12345, $passages, $translations);
    $result = $database_consistency->getPassages (123456);
    $this->assertEquals ("", $result);
    $result = $database_consistency->getTranslations (123456);
    $this->assertEquals ("", $result);
    $result = $database_consistency->getPassages (12345);
    $this->assertEquals ($passages, $result);
    $result = $database_consistency->getTranslations (12345);
    $this->assertEquals ($translations, $result);
  }

   
  public function testResponse ()
  {
    $database_consistency = Database_Consistency::getInstance();
$response = <<<EOD
<div>Matthew 1:1 Response</div>
EOD;
    $database_consistency->storePassagesTranslations (12345, "", "");
    $database_consistency->updateResponse (12345, $response);
    $result = $database_consistency->getResponse (123456);
    $this->assertEquals ("", $result);
    $result = $database_consistency->getResponse (12345);
    $this->assertEquals ($response, $result);
  }

   
}

?>

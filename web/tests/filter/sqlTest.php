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

 
class sqlTest extends PHPUnit_Framework_TestCase
{

  public function testSelectIdentifier ()
  {
    $this->assertEquals (" SELECT identifier ", Filter_Sql::notesSelectIdentifier ());
  }
  
  public function testOptionalFulltextSearchRelevanceStatement ()
  {
    $this->assertEquals ("", Filter_Sql::notesOptionalFulltextSearchRelevanceStatement ("Note 10"));
  }
  
  public function testFromWhereStatement ()
  {
    $this->assertEquals (" FROM notes WHERE 1 ", Filter_Sql::notesFromWhereStatement ());
  }

  public function testOptionalFulltextSearchStatement ()
  {
    $this->assertEquals (" AND cleantext LIKE '%Search query%' ", Filter_Sql::notesOptionalFulltextSearchStatement ("Search query"));
  }

  public function testOrderByRelevanceStatement ()
  {
    $this->assertEquals ("", Filter_Sql::notesOrderByRelevanceStatement ());
  }

}
?>

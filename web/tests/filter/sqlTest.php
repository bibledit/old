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
    $this->assertEquals (" ,  (MATCH (cleantext) AGAINST ('Note 10' IN NATURAL LANGUAGE MODE) * 10)  +  (MATCH (cleantext) AGAINST ('\"Note 10\"' IN BOOLEAN MODE) * 5)  +  (MATCH (cleantext) AGAINST ('Note 10*' IN BOOLEAN MODE)) + (MATCH (reversedtext) AGAINST ('01 etoN*' IN BOOLEAN MODE) * 1)  AS relevance ", Filter_Sql::notesOptionalFulltextSearchRelevanceStatement ("Note 10"));
  }
  
  public function testFromWhereStatement ()
  {
    $this->assertEquals (" FROM notes WHERE TRUE ", Filter_Sql::notesFromWhereStatement ());
  }

  public function testOptionalFulltextSearchStatement ()
  {
    $this->assertEquals (" AND (MATCH (cleantext) AGAINST ('Search query*' IN BOOLEAN MODE)) OR (MATCH (reversedtext) AGAINST ('yreuq hcraeS*' IN BOOLEAN MODE)) ", Filter_Sql::notesOptionalFulltextSearchStatement ("Search query"));
  }

  public function testOrderByRelevanceStatement ()
  {
    $this->assertEquals (" ORDER BY relevance DESC ", Filter_Sql::notesOrderByRelevanceStatement ());
  }

}
?>

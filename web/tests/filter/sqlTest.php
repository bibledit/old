<?php

 
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

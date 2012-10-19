<?php


require_once 'PHPUnit/Framework.php';

 
class eSwordTextTest extends PHPUnit_Framework_TestCase
{

  public function setUp ()
  {
  }

  
  public function tearDown ()
  {
  }


  public function testTitle()
  {
    $esword_text = new Esword_Text ("The Word of the Lord Jesus Christ");
$sql = <<<'EOD'
PRAGMA foreign_keys=OFF;
CREATE TABLE Details (Description NVARCHAR(255), Abbreviation NVARCHAR(50), Comments TEXT, Version INT, Font NVARCHAR(50), RightToLeft BOOL, OT BOOL, NT BOOL, Apocrypha BOOL, Strong BOOL);
INSERT INTO Details VALUES ('The Word of the Lord Jesus Christ', 'The Word of the Lord Jesus Christ', 'The Word of the Lord Jesus Christ', 1, 'DEFAULT', 0, 1, 1, 0, 0);
CREATE TABLE Bible (Book INT, Chapter INT, Verse INT, Scripture BLOB_TEXT);
EOD;
    $this->assertEquals ($esword_text->sql, explode ("\n", $sql));
  }


  public function testZeroReference()
  {
    $esword_text = new Esword_Text ("");
    $esword_text->addText ("The Word of God");
    $esword_text->finalize ();
$sql = <<<'EOD'
PRAGMA foreign_keys=OFF;
CREATE TABLE Details (Description NVARCHAR(255), Abbreviation NVARCHAR(50), Comments TEXT, Version INT, Font NVARCHAR(50), RightToLeft BOOL, OT BOOL, NT BOOL, Apocrypha BOOL, Strong BOOL);
INSERT INTO Details VALUES ('', '', '', 1, 'DEFAULT', 0, 1, 1, 0, 0);
CREATE TABLE Bible (Book INT, Chapter INT, Verse INT, Scripture BLOB_TEXT);
EOD;
    $this->assertEquals ($esword_text->sql, explode ("\n", $sql));
  }


}


?>

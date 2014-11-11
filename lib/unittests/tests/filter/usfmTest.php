<?php
class filterUsfmTest extends PHPUnit_Framework_TestCase // Todo C++Port
{


  
  public function testRemoveNotesOne ()
  {
    $usfm = '\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa njengomhlambi\f + Hlab. 81.5.\f*\fe + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\fe*, ohlezi \add phakathi\add* \w kwamakherubhi\w**\x + Hlab. 99.1. Eks. 25.22.\x*, khanyisa\x + Hlab. 50.2.\x*.';
    $usfm = Filter_Usfm::removeNotes ($usfm, array ("x", "f", "fe"));
    $standard = '\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa njengomhlambi, ohlezi \add phakathi\add* \w kwamakherubhi\w**, khanyisa.';
    $this->assertEquals ($standard, $usfm);
  }
  
  
  public function testInsertNotesEmpty ()
  {
    $usfm = Filter_Usfm::insertNotes (NULL, array (1, ""), 0.9);
    $this->assertNull ($usfm);
    $usfm = Filter_Usfm::insertNotes ("", array (1, ""), 0.9);
    $this->assertEquals ("", $usfm);
    $usfm = Filter_Usfm::insertNotes ('\v 1 Text.', NULL, 0.9);
    $this->assertEquals ('\v 1 Text.', $usfm);
    $usfm = Filter_Usfm::insertNotes ('\v 1 Text.', array (), 0.9);
    $this->assertEquals ('\v 1 Text.', $usfm);
  }


  public function testInsertNotesOne ()
  {
    // Test regular notes placement.
    $usfm = '\v 1 Melusi kaIsrayeli\x + Ps. 1.1\x*, beka indlebe, okhokhela uJosefa njengomhlambi, ohlezi \add phakathi\add* \w kwamakherubhi\w**, khanyisa.';
    $notes = array (
      array (54, '\x + Hlab. 81.5.\x*'),
      array (69, '\x + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\x*'),
      array (117, '\x + Hlab. 99.1. Eks. 25.22.\x*'),
      array (127, '\x + Hlab. 50.2.\x*')
    );
    $usfm = Filter_Usfm::insertNotes ($usfm, $notes, 0.9);
    $standard = '\v 1 Melusi kaIsrayeli\x + Ps. 1.1\x*, beka indlebe, okhokhela uJosefa\x + Hlab. 81.5.\x* njengomhlambi\x + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\x*, ohlezi \add phakathi\add* \w kwamakherubhi\w**\x + Hlab. 50.2.\x*\x + Hlab. 99.1. Eks. 25.22.\x*, khanyisa.';
    $this->assertEquals ($standard, $usfm);
  }


  public function testInsertNotesTwo ()
  {
    // Test notes placement where non-placed notes are added to the very end of the string.
    $usfm = '\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa njengomhlambi, ohlezi \add phakathi\add* \w kwamakherubhi\w**, khanyisa.';
    $notes = array (
      array (55, '\x + Hlab. 81.5.\x*'),
      array (69, '\x + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\x*'),
      array (117, '\x + Hlab. 99.1. Eks. 25.22.\x*'),
      array (127, '\x + Hlab. 50.2.\x*')
    );
    $usfm = Filter_Usfm::insertNotes ($usfm, $notes, 1.5);
    $standard = '\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa njengomhlambi, ohlezi \add phakathi\add*\x + Hlab. 81.5.\x* \w kwamakherubhi\w**\x + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\x*, khanyisa\x + Hlab. 50.2.\x*\x + Hlab. 99.1. Eks. 25.22.\x*.';
    $this->assertEquals ($standard, $usfm);
  }
  
  
  public function testMoveNoteOne ()
  {
$usfm = <<<'EOD'
\v 1 Zvino namazuva okutonga kwavatongi\x + Judg. 2.16.\x* nzara yakange iripo panyika. Umwe\x + Judg. 6.4,5,6.\x* murume weBheterehemu-judha akanogara\x + Judg. 17.8.\x* panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.
EOD;
    $usfm = Filter_Usfm::moveNote ($usfm, 1, 1);
$standard = <<<'EOD'
\v 1 Zvino namazuva okutonga kwavatongi nzara\x + Judg. 2.16.\x* yakange iripo panyika. Umwe\x + Judg. 6.4,5,6.\x* murume weBheterehemu-judha akanogara\x + Judg. 17.8.\x* panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.
EOD;
    $this->assertEquals ($standard, $usfm);
  }


  public function testMoveNoteTwo ()
  {
$usfm = <<<'EOD'
\v 1 Zvino namazuva okutonga kwavatongi\x + Judg. 2.16.\x* nzara yakange iripo panyika. Umwe\x + Judg. 6.4,5,6.\x* murume weBheterehemu-judha akanogara\x + Judg. 17.8.\x* panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.
EOD;
    $usfm = Filter_Usfm::moveNote ($usfm, -1, 2);
$standard = <<<'EOD'
\v 1 Zvino namazuva okutonga kwavatongi\x + Judg. 2.16.\x* nzara yakange iripo panyika\x + Judg. 6.4,5,6.\x*. Umwe murume weBheterehemu-judha akanogara\x + Judg. 17.8.\x* panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.
EOD;
    $this->assertEquals ($standard, $usfm);
  }


  public function testMoveNoteThree ()
  {
$usfm = <<<'EOD'
\v 1 Zvino namazuva okutonga kwavatongi\x + Judg. 2.16.\x* nzara yakange iripo panyika. Umwe\x + Judg. 6.4,5,6.\x* murume weBheterehemu-judha akanogara\x + Judg. 17.8.\x* panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.
EOD;
    $usfm = Filter_Usfm::moveNote ($usfm, -1, 10);
$standard = <<<'EOD'
\v 1 Zvino namazuva okutonga kwavatongi\x + Judg. 2.16.\x* nzara yakange iripo panyika. Umwe\x + Judg. 6.4,5,6.\x* murume weBheterehemu-judha akanogara\x + Judg. 17.8.\x* panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.
EOD;
    $this->assertEquals ($standard, $usfm);
  }


  public function testGetNewNotePositionOne ()
  {
$usfm = <<<'EOD'
\v 1 Zvino namazuva okutonga kwavatongi nzara yakange iripo panyika. Umwe murume weBheterehemu-judha akanogara panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.
EOD;
    $position = Filter_Usfm::getNewNotePosition ($usfm, 5, 0);
    $this->assertEquals (10, $position);
    $position = Filter_Usfm::getNewNotePosition ($usfm, 5, 1);
    $this->assertEquals (10, $position);
    $position = Filter_Usfm::getNewNotePosition ($usfm, 5, -1);
    $this->assertEquals (4, $position);
  }


  public function testGetNewNotePositionTwo ()
  {
$usfm = <<<'EOD'
\v 1 Zvino namazuva okutonga kwavatongi nzara yakange iripo panyika. Umwe murume weBheterehemu-judha akanogara panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.
EOD;
    $position = Filter_Usfm::getNewNotePosition ($usfm, 62, 0);
    $this->assertEquals (67, $position);
    $position = Filter_Usfm::getNewNotePosition ($usfm, 62, 1);
    $this->assertEquals (67, $position);
    $position = Filter_Usfm::getNewNotePosition ($usfm, 62, -1);
    $this->assertEquals (59, $position);
  }


  public function testGetNewNotePositionThree ()
  {
$usfm = <<<'EOD'
\v 1 Zvino namazuva okutonga kwavatongi nzara yakange iripo panyika. Umwe murume weBheterehemu-judha akanogara panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.
EOD;
    $position = Filter_Usfm::getNewNotePosition ($usfm, 19, 0);
    $this->assertEquals (19, $position);
    $position = Filter_Usfm::getNewNotePosition ($usfm, 19, 1);
    $this->assertEquals (28, $position);
    $position = Filter_Usfm::getNewNotePosition ($usfm, 19, -1);
    $this->assertEquals (10, $position);
  }


  public function testGetNewNotePositionFour ()
  {
$usfm = <<<'EOD'
\v 1 Zvino.
EOD;
    $position = Filter_Usfm::getNewNotePosition ($usfm, 19, 0);
    $this->assertEquals (10, $position);
    $position = Filter_Usfm::getNewNotePosition ($usfm, 19, 1);
    $this->assertEquals (10, $position);
    $position = Filter_Usfm::getNewNotePosition ($usfm, 19, -1);
    $this->assertEquals (10, $position);
  }
  
  
  public function testGetNewNotePositionFive ()
  {
    // It tests whether a note is not inserted straight after opening USFM code.
    $usfm = '\v 1 Zvino namazuva \add okutonga\add* kwavatongi nzara yakange iripo panyika.';

    $position = Filter_Usfm::getNewNotePosition ($usfm, 20, 0);
    $this->assertEquals (38, $position);

    $position = Filter_Usfm::getNewNotePosition ($usfm, 20, 1);
    $this->assertEquals (38, $position);

    $position = Filter_Usfm::getNewNotePosition ($usfm, 31, -1);
    $this->assertEquals (19, $position);
  }


  public function testGetNewNotePositionSix ()
  {
    // It tests whether a note is not inserted within another note.
    $usfm = '\v 1 Zvino namazuva\x + Gen.1.1.\x* okutonga kwavatongi nzara yakange iripo panyika.';

    $position = Filter_Usfm::getNewNotePosition ($usfm, 10, 0);
    $this->assertEquals (10, $position);

    $position = Filter_Usfm::getNewNotePosition ($usfm, 20, 0);
    $this->assertEquals (21, $position);
  }
  
  
}
?>

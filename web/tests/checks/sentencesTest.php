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


class sentencesTest extends PHPUnit_Framework_TestCase
{


  private $check;
  
  
  public function setUp ()
  {
    $this->tearDown ();
    $this->check = new Checks_Sentences ();
    $this->check->enterCapitals ("A B C D E F G H I J K L M N O P Q R S T U V W X Y Z");
    $this->check->enterSmallLetters ("a b c d e f g h i j k l m n o p q r s t u v w x y z");
    $this->check->enterEndMarks (". ! ? :");
    $this->check->enterCenterMarks (", ;");
    $this->check->enterDisregards ("( ) [ ] { } ' \" * - 0 1 2 3 4 5 6 7 8 9");
    $this->check->enterNames ("Nkosi Longnamelongnamelongname");
    $this->check->initialize ();
  }

  
  public function tearDown ()
  {
    unset ($this->check);
  }


  public function testUnknownCharacter ()
  {
    $this->check->check (array (1 => "Abc ➊ abc."));
    $results = $this->check->getResults ();
    $standard = array (array (1 => "Unknown character: ➊"));
    $this->assertEquals ($results, $standard);
  }


  public function testCapitalAfterMidSentencePunctuationMark ()
  {
    $this->check->check (array (2 => "He said, Go."));
    $results = $this->check->getResults ();
    $standard = array (array (2 => "Capital follows mid-sentence punctuation mark: He said, Go."));
    $this->assertEquals ($results, $standard);
  }


  public function testCapitalStraightAfterMidSentencePunctuationMark ()
  {
    $this->check->check (array (2 => "He said,Go."));
    $results = $this->check->getResults ();
    $standard = array (array (2 => "Capital follows straight after a mid-sentence punctuation mark: He said,Go."));
    $this->assertEquals ($results, $standard);
  }


  public function testSmallLetterStraightAfterMidSentencePunctuationMark ()
  {
    $this->check->check (array (2 => "He said,go."));
    $results = $this->check->getResults ();
    $standard = array (array (2 => "Small letter follows straight after a mid-sentence punctuation mark: He said,go."));
    $this->assertEquals ($results, $standard);
  }


  public function testTwoVersesOkay ()
  {
    $this->check->check (array (17 => "Jezus kwam naar de wereld,", 18 => "dat hij zou lijden."));
    $results = $this->check->getResults ();
    $standard = array ();
    $this->assertEquals ($results, $standard);
  }


  public function testNameAfterCommaSeveralVersesOkay ()
  {
    $this->check->check (array (
      17 => "Kwasekusithi esebakhuphele ngaphandle, yathi: Baleka ngenxa yempilo yakho, ungakhangeli ngemva kwakho, ungemi egcekeni lonke; balekela entabeni hlezi ubhujiswe.", 
      18 => "ULothi wasesithi kuwo: Kakungabi njalo, Nkosi yami.",
      19 => "Khangela-ke, inceku yakho ithole umusa emehlweni akho, ukhulisile isihawu sakho, osenze kimi, ukugcina uphila umphefumulo wami; kodwa mina ngingeke ngiphephele entabeni, hlezi ububi bunamathele kimi, besengisifa.",
      20 => "Khangela-ke, lumuzi useduze ukubalekela kuwo; futhi umncinyane. Ngicela ngibalekele kuwo (kambe kawumncinyane?) Lomphefumulo wami uphile.",
      21 => "Yasisithi kuye: Khangela, ngibemukele ubuso bakho lakulolu udaba, ukuze ngingawuchithi umuzi okhulume ngawo.",
      22 => "Phangisa, balekela kuwo; ngoba ngingeze ngenza ulutho uze ufike kuwo. Ngakho babiza ibizo lomuzi ngokuthi yiZowari."
    ));
    $results = $this->check->getResults ();
    $standard = array ();
    $this->assertEquals ($results, $standard);
  }
  
  
  public function testLongName ()
  {
    $this->check->check (array (17 => "O, Longnamelongnamelongname."));
    $results = $this->check->getResults ();
    $standard = array ();
    $this->assertEquals ($results, $standard);
  }


  public function testNoSpaceAfterFullStop ()
  {
    $this->check->check (array (2 => "He did that.He went."));
    $results = $this->check->getResults ();
    $standard = array (
      array (2 => "A letter follows straight after an end-sentence punctuation mark: did that.He went."),
      array (2 => "No capital after an end-sentence punctuation mark: id that.He went.")
    );
    $this->assertEquals ($results, $standard);
  }


  public function testCapitalFullStop ()
  {
    $this->check->check (array (2 => "He did that. he went."));
    $results = $this->check->getResults ();
    $standard = array (array (2 => "No capital after an end-sentence punctuation mark: id that. he went."));
    $this->assertEquals ($results, $standard);
  }


  public function testParagraphOne ()
  {
    $this->check->paragraphs (array (1 => "he said"), array (0));
    $results = $this->check->getResults ();
    $standard = array (
                  array (1 => "Paragraph does not start with a capital: h"),
                  array (1 => "Paragraph does not end with an end marker: d")
                );
    $this->assertEquals ($results, $standard);
  }


  public function testParagraphTwo ()
  {
    $this->check->paragraphs (array (1 => "εὐθέως"), array (0));
    $results = $this->check->getResults ();
    $standard = array (
                  array (1 => "Paragraph does not start with a capital: ε"),
                  array (1 => "Paragraph does not end with an end marker: ς")
                );
    $this->assertEquals ($results, $standard);
  }


  public function testParagraphThree ()
  {
    $this->check->paragraphs (array (1 => "Immediately εὐθέως."), array (0));
    $results = $this->check->getResults ();
    $standard = array ();
    $this->assertEquals ($results, $standard);
  }


  public function testTwoPunctuationMarksOne ()
  {
    $this->check->check (array (2 => "He did that.."));
    $results = $this->check->getResults ();
    $standard = array (array (2 => "Two punctuation marks in sequence: did that.."));
    $this->assertEquals ($results, $standard);
  }


  public function testTwoPunctuationMarksTwo ()
  {
    $this->check->check (array (2 => "He did ;. That."));
    $results = $this->check->getResults ();
    $standard = array (array (2 => "Two punctuation marks in sequence: He did ;. That."));
    $this->assertEquals ($results, $standard);
  }


}
?>

<?php


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
    $this->check->enterNames (array ("Nkosi"));
    $this->check->initializeState ();
  }

  
  public function tearDown ()
  {
    unset ($this->check);
  }


  public function testUnknownCharacter ()
  {
    $this->check->check (array (1 => "Abc ➊ abc."));
    $this->check->finalizeState ();
    $results = $this->check->getResults ();
    $standard = array (array (1 => "Unknown character: ➊"));
    $this->assertEquals ($results, $standard);
  }


  public function testCapitalAfterMidSentencePunctuationMark ()
  {
    $this->check->check (array (2 => "He said, Go."));
    $this->check->finalizeState ();
    $results = $this->check->getResults ();
    $standard = array (array (2 => "Capital follows mid-sentence punctuation mark: He said, Go."));
    $this->assertEquals ($results, $standard);
  }




  public function testCapitalStraightAfterMidSentencePunctuationMark ()
  {
    $this->check->check (array (2 => "He said,Go."));
    $this->check->finalizeState ();
    $results = $this->check->getResults ();
    $standard = array (array (2 => "Capital follows straight after a mid-sentence punctuation mark: He said,Go."));
    $this->assertEquals ($results, $standard);
  }


  public function testSmallLetterStraightAfterMidSentencePunctuationMark ()
  {
    $this->check->check (array (2 => "He said,go."));
    $this->check->finalizeState ();
    $results = $this->check->getResults ();
    $standard = array (array (2 => "Small letter follows straight after a mid-sentence punctuation mark: He said,go."));
    $this->assertEquals ($results, $standard);
  }


  public function testTwoVersesOkay ()
  {
    $this->check->check (array (17 => "Jezus kwam naar de wereld,", 18 => "dat hij zou lijden."));
    $this->check->finalizeState ();
    $results = $this->check->getResults ();
    $standard = array ();
    $this->assertEquals ($results, $standard);
  }


  public function testSeveralVersesOkay ()
  {
    $this->check->check (array (
      17 => "Kwasekusithi esebakhuphele ngaphandle, yathi: Baleka ngenxa yempilo yakho, ungakhangeli ngemva kwakho, ungemi egcekeni lonke; balekela entabeni hlezi ubhujiswe.", 
      18 => "ULothi wasesithi kuwo: Kakungabi njalo, Nkosi yami.",
      19 => "Khangela-ke, inceku yakho ithole umusa emehlweni akho, ukhulisile isihawu sakho, osenze kimi, ukugcina uphila umphefumulo wami; kodwa mina ngingeke ngiphephele entabeni, hlezi ububi bunamathele kimi, besengisifa.",
      20 => "Khangela-ke, lumuzi useduze ukubalekela kuwo; futhi umncinyane. Ngicela ngibalekele kuwo (kambe kawumncinyane?) lomphefumulo wami uphile.",
      21 => "Yasisithi kuye: Khangela, ngibemukele ubuso bakho lakulolu udaba, ukuze ngingawuchithi umuzi okhulume ngawo.",
      22 => "Phangisa, balekela kuwo; ngoba ngingeze ngenza ulutho uze ufike kuwo. Ngakho babiza ibizo lomuzi ngokuthi yiZowari."
    ));
    $this->check->finalizeState ();
    $results = $this->check->getResults ();
    $standard = array ();
    $this->assertEquals ($results, $standard);
  }


}
?>

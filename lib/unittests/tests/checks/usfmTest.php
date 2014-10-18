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


class checksUsfmTest extends PHPUnit_Framework_TestCase
{


  private $check;
  
  
  public function setUp ()
  {
    $this->tearDown ();
    $database_styles = Database_Styles::getInstance ();
    $database_styles->createStandardSheet ();
    $this->check = new Checks_Usfm ("phpunit");
    $this->check->initialize ();
  }

  
  public function tearDown ()
  {
    unset ($this->check);
  }


  public function testMalformedVerse ()
  {
$usfm = <<<'EOD'
\c 1
\p
\v 2,He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (2 => 'Malformed verse number: \v 2,He said.'));
    $this->assertEquals ($results, $standard);
  }


  public function testNewLineInUSFMGood ()
  {
$usfm = <<<'EOD'
\c 1
\p He said.
\v 1 He said.
\p He said.
\v 2 He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array ();
    $this->assertEquals ($results, $standard);
  }


  public function testNewLineInUSFMOne ()
  {
$usfm = <<<'EOD'
\c 1
\p He said.
\v 1 He said.
\
\p He said.
\v 2 He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (0 => 'New line within USFM:  \ \p He s'));
    $this->assertEquals ($results, $standard);
  }


  public function testNewLineInUSFMTwo ()
  {
$usfm = <<<'EOD'
\c 1
\p He said.
\v 1 He said.
\ 
\p He said.
\v 3 He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (0 => 'New line within USFM:  \  \p He '),
                       array (1 => 'Widow backslash: \ ')
                );
    $this->assertEquals ($results, $standard);
  }


  public function testUnknownUsfmOne ()
  {
$usfm = <<<'EOD'
\c 1
\p He said.
\v 1 He said \add something\add*.
\p,p He said.
\v 3 He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (1 => 'Marker not in stylesheet: \p,p '));
    $this->assertEquals ($results, $standard);
  }


  public function testUnknownUsfmTwoe ()
  {
$usfm = <<<'EOD'
\c 1
\pHe said.
\v 1 He said \add something\add*.
\p He said.
\v 3 He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (0 => 'Marker not in stylesheet: \pHe '));
    $this->assertEquals ($results, $standard);
  }


  public function testIdOne ()
  {
$usfm = <<<'EOD'
\id GENN
\p He said.
\v 1 He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (0 => 'Unknown ID: \id GENN'));
    $this->assertEquals ($results, $standard);
  }


  public function testIdTwo()
  {
$usfm = <<<'EOD'
\id
\p He said.
\v 1 He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (0 => 'Unknown ID: \id\p '));
    $this->assertEquals ($results, $standard);
  }


  public function testIdThree ()
  {
$usfm = <<<'EOD'
\id Gen
\p He said.
\v 1 He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (0 => 'Unknown ID: \id Gen'));
    $this->assertEquals ($results, $standard);
  }


  public function testForwardSlashOne ()
  {
$usfm = <<<'EOD'
\id GEN
\p He said.
\v 1 He said. He said something/add*.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (0 => 'Forward slash instead of backslash: /add'));
    $this->assertEquals ($results, $standard);
  }


  public function testForwardSlashTwo ()
  {
$usfm = <<<'EOD'
\id GEN
\p He said.
\v 1 /v He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (0 => 'Forward slash instead of backslash: /v'));
    $this->assertEquals ($results, $standard);
  }


  public function testCleanUsfm ()
  {
$usfm = <<<'EOD'
\id GEN
\c 35
\s UNkulunkulu ubusisa uJakobe eBhetheli
\p
\v 1 UNkulunkulu wasesithi kuJakobe: Sukuma, yenyukela eBhetheli\x + 28.13-19.\x*, uhlale khona; umenzele lapho ilathi uNkulunkulu owabonakala kuwe mhla ubaleka ebusweni bukaEsawu umnewenu\x + 27.43.\x*.
\v 2 UJakobe wasesithi kundlu yakhe\x + 18.19. Josh. 24.15.\x* lakubo bonke ababelaye: Susani onkulunkulu bezizwe abaphakathi kwenu\x + 31.19. Josh. 24.2,23. 1 Sam. 7.3.\x*, lizihlambulule, lintshintshe izembatho zenu\x + Eks. 19.10.\x*.
\v 3 Asisukume senyukele eBhetheli, ngimenzele khona uNkulunkulu ilathi owangiphendula ngosuku lokuhlupheka kwami\x + 32.7,24.\x*, njalo waba lami endleleni engahamba kuyo\x + 28.20; 31.3.\x*.
\v 4 Basebenika uJakobe bonke onkulunkulu bezizwe ababesesandleni sabo, lamacici\x + Hos. 2.13. Gen. 24.22.\x* ayesezindlebeni zabo; uJakobe wasekufihla ngaphansi \add kwesihlahla\add* \w se-okhi\w**\x + Josh. 24.26. 2 Sam. 18.9. 1 Kho. 13.14. 1 Lan. 10.12. Gen. 35.8.\x* eliseShekema.
\v 5 Basebehamba; lokwesabeka kukaNkulunkulu\x + 9.2. Eks. 15.16; 23.27. Dute. 11.25. Josh. 2.9. 2 Lan. 14.14.\x* kwakuphezu kwemizi eyayibazingelezele, njalo kabawaxotshanga amadodana kaJakobe.
\p
\v 6 UJakobe wasefika eLuzi\x + 28.19.\x*, eselizweni leKhanani, eyiBhetheli, yena labantu bonke ababelaye.
\v 7 Wasesakha khona ilathi, wayibiza indawo \add ngokuthi\add* iEli-Bhetheli\f + \fk iEli-Bhetheli: \fl okuyikuthi, \fq uNkulunkulu weBhetheli.\f*, ngoba lapho uNkulunkulu wabonakala kuye ekubalekeleni ubuso bomnewabo.
\v 8 Njalo uDebora umlizane kaRebeka wafa\x + 24.59.\x*, wangcwatshwa ezansi kweBhetheli ngaphansi kwesihlahla se-okhi\x + 13.18; 35.4.\x*, ngakho wasitha ibizo laso iAloni-Bakuthi\f + \fk iAloni-Bakuthi: \fl okuyikuthi, \fq ie-oki yokulila.\f*.
\p
\v 9 UNkulunkulu wasebuya wabonakala kuJakobe evela ePadani-Arama, wambusisa.
\v 10 UNkulunkulu wasesithi kuye: Ibizo lakho nguJakobe; ibizo lakho kalisayikuthiwa nguJakobe\x + 17.5,15.\x*, kodwa uIsrayeli kuzakuba libizo lakho. Wabiza ibizo lakhe \add ngokuthi\add* uIsrayeli\x + 32.28.\x*.
\v 11 UNkulunkulu wasesithi kuye: NginguNkulunkulu uSomandla\x + 17.1.\x*; zala wande\x + 28.3; 48.4.\x*; isizwe, yebo ibandla lezizwe kuzavela kuwe\x + 17.5,6,16; 26.4.\x*; lamakhosi azaphuma ekhalweni lwakho.
\v 12 Lelizwe engalinika uAbrahama loIsaka ngizalinika wena, lenzalweni yakho emva kwakho ngizayinika ilizwe\x + 12.7; 13.15; 17.8; 26.3; 28.13.\x*.
\v 13 UNkulunkulu wasesenyuka esuka kuye endaweni lapho ayekhuluma laye khona\x + 17.22.\x*.
\v 14 UJakobe wasemisa insika endaweni lapho ayekhuluma laye khona, insika yelitshe\x + 28.18; 31.45.\x*. Wathululela phezu kwayo umnikelo wokunathwayo\x + Nani 28.7.\x*, wathela phezu kwayo amagcobo.
\v 15 UJakobe waseyitha ibizo lendawo, lapho uNkulunkulu ayekhulume laye khona, iBhetheli\x + 28.19.\x*.
\s Ukufa kukaRasheli
\p
\v 16 Basebehamba besuka eBhetheli; kwathi kusesengummango\f + \fk ummango: \fl Heb. \fq isiqetshana somhlabathi.\f* ukuya eEfrathi, uRasheli wabeletha, wayelobunzima ekubeletheni.
\v 17 Kwasekusithi ekubeletheni kwakhe nzima, umbelethisi wathi kuye: Ungesabi, ngoba lalo uzakuba yindodana yakho\x + 30.24. 1 Sam. 4.20.\x*.
\v 18 Kwasekusithi umphefumulo wakhe usuphuma, ngoba esesifa, wayitha ibizo layo uBenoni\f + \fk uBenoni: \fl okuyikuthi, \fq indodana yosizi lwami.\f*; kodwa uyise wayibiza \add ngokuthi\add* nguBhenjamini\f + \fk uBhenjamini: \fl okuyikuthi, \fq indodana yesandla sokunene.\f*.
\v 19 URasheli wasesifa\x + 48.7.\x*, wangcwatshwa endleleni eya eEfrathi, eyiBhethelehema\x + Ruthe 1.2; 4.11. Mika 5.2. Mat. 2.6,16-18.\x*.
\v 20 UJakobe wasemisa insika phezu kwengcwaba lakhe; le yinsika yengcwaba likaRasheli kuze kube lamuhla\x + 1 Sam. 10.2.\x*.
\p
\v 21 UIsrayeli wasehamba, wamisa ithente lakhe ngaphambili komphotshongo weEderi\x + Mika 4.8.\x*.
\v 22 Kwasekusithi uIsrayeli ehlala kulelolizwe, uRubeni waya walala loBiliha\x + 49.4. 1 Lan. 5.1. Gen. 30.4; 37.2. 2 Sam. 16.22; 20.3. 1 Kor. 5.1.\x* umfazi omncinyane kayise; uIsrayeli wasekuzwa.
\s Amadodana kaJakobe lokufa kukaIsaka
\p Amadodana kaJakobe-ke ayelitshumi lambili\x + 46.8-27. Eks. 1.2-4.\x*;
\v 23 amadodana kaLeya: Izibulo likaJakobe, uRubeni, loSimeyoni loLevi loJuda loIsakari loZebuluni;
\v 24 amadodana kaRasheli: OJosefa loBhenjamini;
\v 25 lamadodana kaBiliha, incekukazi kaRasheli: ODani loNafithali;
\v 26 lamadodana kaZilipa, incekukazi kaLeya: OGadi loAsheri; la ngamadodana kaJakobe azalelwa yena ePadani-Arama.
\p
\v 27 UJakobe wasefika kuIsaka uyise eMamre\x + 13.18; 23.2,19.\x*, eKiriyathi-Arba\x + Josh. 14.15; 15.13.\x*, eyiHebroni, lapho uAbrahama ahlala khona njengowezizwe, loIsaka\x + 28.10.\x*.
\v 28 Lensuku zikaIsaka zaziyiminyaka elikhulu lamatshumi ayisificaminwembili.
\v 29 UIsaka wasehodoza, wafa, wabuthelwa ezizweni zakibo\x + 15.15; 25.8.\x*, emdala, enele ngensuku. Amadodana akhe uEsawu loJakobe asemngcwaba\x + 25.9; 49.31.\x*.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array ();
    $this->assertEquals ($results, $standard);
  }


  public function testWidowBackslashOne ()
  {
$usfm = <<<'EOD'
\id GEN
\p
\v 1 \ He said.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (1 => 'Widow backslash: \ '));
    $this->assertEquals ($results, $standard);
  }


  public function testMatchingMarkersOne ()
  {
$usfm = <<<'EOD'
\id GEN
\v 1  He said \add addition\add*.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array ();
    $this->assertEquals ($results, $standard);
  }


  public function testMatchingMarkersTwo ()
  {
$usfm = <<<'EOD'
\id GEN
\v 1  He said addition\add*.
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (1 => 'Closing marker does not match opening marker : \add*'));
    $this->assertEquals ($results, $standard);
  }


  public function testMatchingMarkersThree ()
  {
$usfm = <<<'EOD'
\id GEN
\v 1  He said \add addition\add .
EOD;
    $this->check->check ($usfm);
    $results = $this->check->getResults ();
    $standard = array (array (1 => 'Repeating opening marker: \add '));
    $this->assertEquals ($results, $standard);
  }


  public function testMatchingMarkersFour ()
  {
$usfm = <<<'EOD'
\id GEN
\v 8 Kodwa uNowa wazuza umusa emehlweni eN\nd kosi\x + 19.19.\nd*\x*.
\v 9 Kodwa uNowa wazuza umusa emehlweni eN\nd kosi\x + 19.19.\x*\nd*.
EOD;
    $this->check->check ($usfm);
    $this->check->finalize ();
    $results = $this->check->getResults ();
    $standard = array ();
    $this->assertEquals ($results, $standard);
  }


  public function testMatchingMarkersFive ()
  {
$usfm = <<<'EOD'
\id GEN
\v 8 Kodwa uNowa wazuza umusa \add emehlweni eN\nd kosi\x + 19.19.\nd*\x*.
EOD;
    $this->check->check ($usfm);
    $this->check->finalize ();
    $results = $this->check->getResults ();
    $standard = array (array (8 => "Unclosed markers: add"));
    $this->assertEquals ($results, $standard);
  }


}
?>

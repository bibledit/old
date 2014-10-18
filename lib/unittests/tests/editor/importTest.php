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

 
class editorImportTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }


  public function tearDown ()
  {
    $GLOBALS[0] = true;
    unset ($GLOBALS[0]);
  }


  public function testTextLengthOne ()
  {
$usfm = <<<'EOD'
\c 2
\p
\v 1 Kwasekuqediswa amazulu lomhlaba lalo lonke ibutho lakho\x + Dute. 4.19. Hlab. 33.6.\x*.
EOD;
    $editor_import = Editor_Import::getInstance ();
    $editor_import->load ($usfm);
    $editor_import->stylesheet ("Standard");
    $editor_import->run ();
    $this->assertEquals (61, $editor_import->textLength);
    $this->assertEquals (array (0 => 0, 1 => 2), $editor_import->verseStartOffsets);
  }


  public function testTextLengthMore ()
  {
$usfm = <<<'EOD'
\c 2
\p
\v 1 Kwasekuqediswa amazulu lomhlaba lalo lonke ibutho lakho\x + Dute. 4.19. Hlab. 33.6.\x*.
\v 2 UNkulunkulu wasewuqeda ngosuku lwesikhombisa umsebenzi wakhe abewenza. Waphumula ngosuku lwesikhombisa\x + Eks. 20.11; 31.17. Dute. 5.14. Heb. 4.4.\x* emsebenzini wakhe wonke abewenza.
\v 3 UNkulunkulu wasebusisa usuku lwesikhombisa, walungcwelisa; ngoba ngalo waphumula emsebenzini wakhe wonke, uNkulunkulu awudalayo wawenza\f + \fk wawenza: \fl Heb. \fq ukuwenza.\f*.
\s Isivande seEdeni
\p
\v 4 Lezi yizizukulwana zamazulu lezomhlaba ekudalweni kwakho\x + 1.1.\x*, mhla iN\nd kosi\nd* uNkulunkulu isenza umhlaba lamazulu,
\v 5 laso sonke isihlahlakazana sensimu, singakabi khona emhlabeni, layo yonke imibhida yeganga\x + 1.12.\x*, ingakamili; ngoba iN\nd kosi\nd* uNkulunkulu yayinganisanga izulu emhlabeni, njalo kwakungelamuntu wokulima umhlabathi.
\v 6 Kodwa kwenyuka inkungu ivela emhlabathini, yasithelela ubuso bonke bomhlabathi.
\v 7 IN\nd kosi\nd* uNkulunkulu yasibumba umuntu ngothuli oluvela emhlabathini\x + 3.19,23. Hlab. 103.14. Tshu. 12.7. 1 Kor. 15.47.\x*, yaphefumulela emakhaleni akhe umoya wempilo; umuntu wasesiba ngumphefumulo ophilayo\x + 7.22. Jobe 33.4. Isa. 2.22. 1 Kor. 15.45.\x*.
EOD;
    $editor_import = Editor_Import::getInstance ();
    $editor_import->load ($usfm);
    $editor_import->stylesheet ("Standard");
    $editor_import->run ();
    $this->assertEquals (913, $editor_import->textLength);
    $this->assertEquals (array (0 => 0, 
                                1 => 2,
                                2 => 62,
                                3 => 202,
                                4 => 359,
                                5 => 469,
                                6 => 676,
                                7 => 758), $editor_import->verseStartOffsets);
  }


  public function testSpaceAfterStartingMarker ()
  {
$usfm = <<<'EOD'
\c 1
\p
\v 2 Text \add of the \add*1st\add  second verse\add*.
EOD;
    $editor_import = Editor_Import::getInstance ();
    $editor_import->load ($usfm);
    $editor_import->stylesheet ("Standard");
    $editor_import->run ();
    $html = $editor_import->get ();
$standard = <<<'EOD'
<p class="c"><span>1</span></p>
<p class="p"><span class="v">2</span><span> </span><span>Text </span><span class="add">of the </span><span>1st</span><span class="add"> second verse</span><span>.</span></p>
EOD;
    $this->assertEquals ($standard, $html);
  }


}


?>

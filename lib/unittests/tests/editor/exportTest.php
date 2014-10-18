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

 
class editorExportTest extends PHPUnit_Framework_TestCase
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


  public function testFootnoteDeletedBody ()
  {
$html = <<<'EOD'
<p class="p"><span>The earth brought forth</span><a href="#note1" id="citation1" class="superscript">f</a><span>.</span></p>
<div id="notes">
<hr>
<p class="f"></p>
<br>
</div>
EOD;
    $editor_export = Editor_Export::getInstance ();
    $editor_export->load ($html);
    $editor_export->stylesheet ("Standard");
    $editor_export->run ();
    $usfm = $editor_export->get ();
$standard = <<<'EOD'
\p The earth brought forth.
\f +\f*
EOD;
    $this->assertEquals ($standard, trim ($usfm));
  }


  public function testFootnoteDeletedCitation ()
  {
$html = <<<'EOD'
<p class="p"><span>The earth brought forth</span><span>.</span></p>
<div id="notes">
<hr>
<p class="f"><a href="#citation1" id="note1">f</a><span> </span><span>+ </span><span class="fk">brought: </span><span class="fl">Heb. </span><span class="fq">explanation.</span></p>
</div>
EOD;
    $editor_export = Editor_Export::getInstance ();
    $editor_export->load ($html);
    $editor_export->stylesheet ("Standard");
    $editor_export->run ();
    $usfm = $editor_export->get ();
$standard = <<<'EOD'
\p The earth brought forth.
\f + \fk brought: \fl Heb. \fq explanation.\f*
EOD;
    $this->assertEquals ($standard, trim ($usfm));
  }


  public function testNonBreakingSpaces ()
  {
$html = <<<'EOD'
<p class="p"><span>The&nbsp;earth &nbsp; brought&nbsp;&nbsp;forth.</span></p>
EOD;
    $editor_export = Editor_Export::getInstance ();
    $editor_export->load ($html);
    $editor_export->stylesheet ("Standard");
    $editor_export->run ();
    $usfm = $editor_export->get ();
$standard = <<<'EOD'
\p The earth brought forth.
EOD;
    $this->assertEquals ($standard, trim ($usfm));
  }


  public function testEmbeddedTextSpansOne ()
  {
$html = <<<'EOD'
<p class="p"><span>The <span class="add"><span class="nd">Lord God</span> is calling</span> you</span><span>.</span></p>
EOD;
    $editor_export = Editor_Export::getInstance ();
    $editor_export->load ($html);
    $editor_export->stylesheet ("Standard");
    $editor_export->run ();
    $usfm = $editor_export->get ();
$standard = <<<'EOD'
\p The \add \+nd Lord God\+nd* is calling\add* you.
EOD;
    $this->assertEquals ($standard, $usfm);
  }
  

}


?>

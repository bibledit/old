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


class bibleworksTest extends PHPUnit_Framework_TestCase
{


  public function testImport1()
  {
$bwdata = <<<EOD
Hab 1:1  La sentència ... profeta <05030> Habacuc <02265>.
Hab 1:2 ¿Fins <05704> quan <0575>, Jahveh ... no [em] salvaràs <03467>(08686)? 
Hab 1:3 ¿Per què ... i la controvèrsia <04066>.
EOD;
$usfmdata = <<<EOD
\\id HAB
\\c 1
\\p
\\v 1 La sentència ... profeta <05030> Habacuc <02265>.
\\v 2 ¿Fins <05704> quan <0575>, Jahveh ... no \add em\add* salvaràs <03467>(08686)?
\\v 3 ¿Per què ... i la controvèrsia <04066>.
EOD;
    $data = Filter_Bibleworks::import ($bwdata, true);
    $this->assertEquals ($usfmdata, $data);
  }


  public function testImport2()
  {
$bwdata = <<<EOD
Hab 1:1  La sentència ... profeta <05030> Habacuc <02265>.
Hab 1:2 ¿Fins <05704> quan <0575>, Jahveh ... no [em] salvaràs <03467>(08686)? 
Hab 1:3 ¿Per què ... i la controvèrsia <04066>.
EOD;
$usfmdata = <<<EOD
\\id HAB
\\c 1
\\p
\\v 1 La sentència ... profeta  Habacuc .
\\v 2 ¿Fins  quan , Jahveh ... no \add em\add* salvaràs ?
\\v 3 ¿Per què ... i la controvèrsia .
EOD;
    $data = Filter_Bibleworks::import ($bwdata, false);
    $this->assertEquals ($usfmdata, $data);
  }


  public function testItalics1()
  {
    $data = Filter_Bibleworks::italics ("Normal text and [italics].");
    $this->assertEquals($data, 'Normal text and \add italics\add*.');
  }
  
  
  public function testItalics2()
  {
    $data = Filter_Bibleworks::italics ("Normal text, [italics], and [italics again].");
    $this->assertEquals($data, 'Normal text, \add italics\add*, and \add italics again\add*.');
  }
  
  
  public function testItalics3()
  {
    $data = Filter_Bibleworks::italics ("[Italics] and [malformed italics.");
    $this->assertEquals($data, '\add Italics\add* and [malformed italics.');
  }
  

  public function testItalics4()
  {
    $data = Filter_Bibleworks::italics ("[Italics] and malformed italics].");
    $this->assertEquals($data, '\add Italics\add* and malformed italics].');
  }
  
  
  public function testItalics5()
  {
    $data = Filter_Bibleworks::italics ("Mal]formed [italics].");
    $this->assertEquals($data, 'Mal]formed \add italics\add*.');
  }
  
  
  public function testNotes1()
  {
    $data = Filter_Bibleworks::notes ("Normal text and {notes}.");
    $this->assertEquals($data, 'Normal text and \f + notes\f*.');
  }
  
  
  public function testNotes2()
  {
    $data = Filter_Bibleworks::notes ("Normal text, {notes}, and {notes again}.");
    $this->assertEquals($data, 'Normal text, \f + notes\f*, and \f + notes again\f*.');
  }
  
  
  public function testNotes3()
  {
    $data = Filter_Bibleworks::notes ("{notes} and {malformed notes.");
    $this->assertEquals($data, '\f + notes\f* and {malformed notes.');
  }
  

  public function testNotes4()
  {
    $data = Filter_Bibleworks::notes ("{notes} and malformed notes}.");
    $this->assertEquals($data, '\f + notes\f* and malformed notes}.');
  }
  
  
  public function testNotes5()
  {
    $data = Filter_Bibleworks::notes ("Mal}formed {notes}.");
    $this->assertEquals($data, 'Mal}formed \f + notes\f*.');
  }
  
  
  public function testParenthesis1()
  {
    $malformed = array ();
    $data = Filter_Bibleworks::parenthesis ("Normal text.", $malformed);
    $this->assertEquals($data, 'Normal text.');
  }
  
  
  public function testParenthesis2()
  {
    $malformed = array ();
    $data = Filter_Bibleworks::parenthesis ("Text in parenthesis(08804).", $malformed);
    $this->assertEquals($data, 'Text in parenthesis.');
  }
  
  
  public function testParenthesis3()
  {
    $malformed = array ();
    $data = Filter_Bibleworks::parenthesis ("Text in parenthesis(08804a).", $malformed);
    $this->assertEquals($data, 'Text in parenthesis(08804a).');
    $this->assertEquals($malformed, array ('(08804a)'));
  }
  
  
  public function testParenthesis4()
  {
    $malformed = array ();
    $data = Filter_Bibleworks::parenthesis ("Text in parenthesis(08(804).", $malformed);
    $this->assertEquals($data, 'Text in parenthesis(08.');
    $this->assertEquals($malformed, array ('(08(804)'));
  }
  
  
  public function testChevrons1()
  {
    $malformed = array ();
    $data = Filter_Bibleworks::chevrons ("Normal text.", $malformed);
    $this->assertEquals($data, 'Normal text.');
  }
  
  
  public function testChevrons2()
  {
    $malformed = array ();
    $data = Filter_Bibleworks::chevrons ("Text in chevrons<01004>.", $malformed);
    $this->assertEquals($data, 'Text in chevrons.');
  }
  
  
  public function testChevrons3()
  {
    $malformed = array ();
    $data = Filter_Bibleworks::chevrons ("Text in chevrons<01004b>.", $malformed);
    $this->assertEquals($data, 'Text in chevrons.');
  }


  public function testChevrons4()
  {
    $malformed = array ();
    $data = Filter_Bibleworks::chevrons ("Text in chevrons<06030, 06031>.", $malformed);
    $this->assertEquals($data, 'Text in chevrons.');
  }


  public function testChevrons5()
  {
    $malformed = array ();
    $data = Filter_Bibleworks::chevrons ("Text in chevrons<06030; abc>.", $malformed);
    $this->assertEquals($data, 'Text in chevrons<06030; abc>.');
    $this->assertEquals($malformed, array ('<06030; abc>'));
  }

  
}


?>

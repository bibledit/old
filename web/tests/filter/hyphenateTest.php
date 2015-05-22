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

 
class filterHyphenateTest extends PHPUnit_Framework_TestCase
{

  private $firstset;
  private $secondset;
  
  public function setUp ()
  {
    $this->tearDown ();
    $this->firstset = array();
    preg_match_all('/./u', "aeiou", $this->firstset);
    $this->firstset = $this->firstset[0];
    $this->secondset = array();
    preg_match_all('/./u', "bcdfghjklmnpqrstvwxyz", $this->secondset);
    $this->secondset = $this->secondset[0];
  }

  
  public function tearDown ()
  {

  }


  public function testOne()
  {
$input = <<<'EOD'
\s \nd UNkulunkulu\nd* u\add ba\add*xwayisa ngokulunga okungokwabantu 文字ab化け
\s Ukulunga okuku\nd Kristu\nd* אבabגד kuyinzuzo אבגד ab
EOD;
    $output = Filter_Hyphenate::atTransition ($this->firstset, $this->secondset, $input);
$standard = <<<'EOD'
\s \nd UNku­lu­nku­lu\nd* u\add ba\add*­xwa­yi­sa ngo­ku­lu­nga oku­ngo­kwa­ba­ntu 文字a­b化け
\s Uku­lu­nga oku­ku\nd Kri­stu\nd* אבa­bגד ku­yi­nzu­zo אבגד ab
EOD;
    $this->assertEquals ($standard, $output);
  }


}
?>

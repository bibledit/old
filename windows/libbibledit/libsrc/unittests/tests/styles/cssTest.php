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

 
class stylesCssTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
    $database_styles = Database_Styles::getInstance ();
    $database_styles->createSheet ("phpunit");
  }


  public function tearDown ()
  {
    $database_styles = Database_Styles::getInstance ();
    $database_styles->deleteSheet ("phpunit");
  }


  public function testBasic ()
  {
    $styles_css = new Styles_Css ("phpunit");
    $styles_css->generate ();
    $css = $styles_css->css ();
    $standard = trim (file_get_contents (dirname (__FILE__) . "/basic.css"));
    $this->assertEquals ($standard, $css);
  }


  public function testExport ()
  {
    $styles_css = new Styles_Css ("phpunit");
    $styles_css->exports ();
    $styles_css->generate ();
    $css = $styles_css->css ();
    $standard = trim (file_get_contents (dirname (__FILE__) . "/exports.css"));
    $this->assertEquals ($standard, $css);
  }
  

  public function testEditor ()
  {
    $styles_css = new Styles_Css ("phpunit");
    $styles_css->editor ();
    $styles_css->generate ();
    $css = $styles_css->css ();
    $standard = trim (file_get_contents (dirname (__FILE__) . "/editor.css"));
    $this->assertEquals ($standard, $css);
  }


}


?>

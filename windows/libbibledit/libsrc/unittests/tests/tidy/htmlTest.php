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


class tidyHtmlTest extends PHPUnit_Framework_TestCase
{


  protected function setUp () 
  {
  }


  protected function tearDown ()
  {
  }


  public function testOne ()
  {
    $html = file_get_contents (__DIR__ . "/biblehub-text-john-1-1.html");
    $tidy = Tidy_Html::run ($html);
    $standard = file_get_contents (__DIR__ . "/biblehub-text-john-1-1-tidy.html");
    $this->assertEquals ($standard, $tidy);
  }


}


?>

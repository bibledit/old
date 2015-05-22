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

 
class pathsTest extends PHPUnit_Framework_TestCase
{


  public function testHtmlFileNameBible ()
  {
    $this->assertEquals ("index.html", Filter_Paths::htmlFileNameBible ());
    $this->assertEquals ("path/index.html", Filter_Paths::htmlFileNameBible ("path"));
    $this->assertEquals ("path/01-Genesis.html", Filter_Paths::htmlFileNameBible ("path", 1));
    $this->assertEquals ("01-Genesis.html", Filter_Paths::htmlFileNameBible ("", 1));
    $this->assertEquals ("path/11-1Kings.html", Filter_Paths::htmlFileNameBible ("path", 11));
    $this->assertEquals ("path/22-SongofSolomon-000.html", Filter_Paths::htmlFileNameBible ("path", 22, 0));
    $this->assertEquals ("path/33-Micah-333.html", Filter_Paths::htmlFileNameBible ("path", 33, 333));
    $this->assertEquals ("33-Micah-333.html", Filter_Paths::htmlFileNameBible ("", 33, 333));
  }
  
  
}
?>

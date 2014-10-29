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

 
class filterFileTest extends PHPUnit_Framework_TestCase
{


  public function testUnique ()
  {
    $path = tempnam (sys_get_temp_dir(), "");
    $unique1 = Filter_File::unique ($path);
    file_put_contents ($unique1, "");
    $unique2 = Filter_File::unique ($path);
    unlink ($path);
    unlink ($unique1);
    $this->assertEquals ($path . ".1", $unique1);
    $this->assertEquals ($path . ".2", $unique2);
  }
  
  
}


?>

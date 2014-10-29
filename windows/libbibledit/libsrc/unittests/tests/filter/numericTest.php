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

 
class filterNumericTest extends PHPUnit_Framework_TestCase
{

  public function testFilterEmailExtractEmail()
  {
    $this->assertEquals ("1st", Filter_Numeric::add_ordinal_number_suffix (1));
    $this->assertEquals ("2nd", Filter_Numeric::add_ordinal_number_suffix (2));
    $this->assertEquals ("3rd", Filter_Numeric::add_ordinal_number_suffix (3));
    $this->assertEquals ("4th", Filter_Numeric::add_ordinal_number_suffix (4));
    $this->assertEquals ("10th", Filter_Numeric::add_ordinal_number_suffix (10));
    $this->assertEquals ("11th", Filter_Numeric::add_ordinal_number_suffix (11));
    $this->assertEquals ("21st", Filter_Numeric::add_ordinal_number_suffix (21));
    $this->assertEquals ("22nd", Filter_Numeric::add_ordinal_number_suffix (22));
    $this->assertEquals ("33rd", Filter_Numeric::add_ordinal_number_suffix (33));
    $this->assertEquals ("44th", Filter_Numeric::add_ordinal_number_suffix (44));
    $this->assertEquals ("101st", Filter_Numeric::add_ordinal_number_suffix (101));
    $this->assertEquals ("102nd", Filter_Numeric::add_ordinal_number_suffix (102));
    $this->assertEquals ("103rd", Filter_Numeric::add_ordinal_number_suffix (103));
    $this->assertEquals ("104th", Filter_Numeric::add_ordinal_number_suffix (104));
  }

  
}
?>

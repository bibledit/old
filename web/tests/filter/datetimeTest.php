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


class filterDatetimeTest extends PHPUnit_Framework_TestCase
{


  public function setUp ()
  {
    $this->tearDown ();
  }

  
  public function tearDown ()
  {
  }


  public function testFirstWorkingDayOfMonth ()
  {
    // Sunday the 1st.
    $this->assertFalse (Filter_Datetime::isFirstWorkingDayOfMonth (1, 0));
    // Monday the 1st.
    $this->assertTrue (Filter_Datetime::isFirstWorkingDayOfMonth (1, 1));
    // Tuesday the 1st.
    $this->assertTrue (Filter_Datetime::isFirstWorkingDayOfMonth (1, 2));
    // Wednesday the 1st.
    $this->assertTrue (Filter_Datetime::isFirstWorkingDayOfMonth (1, 3));
    // Thirsday the 1st.
    $this->assertTrue (Filter_Datetime::isFirstWorkingDayOfMonth (1, 4));
    // Friday the 1st.
    $this->assertTrue (Filter_Datetime::isFirstWorkingDayOfMonth (1, 5));
    // Saturday the 1st.
    $this->assertFalse (Filter_Datetime::isFirstWorkingDayOfMonth (1, 6));
    // Sunday the 2nd.
    $this->assertFalse (Filter_Datetime::isFirstWorkingDayOfMonth (2, 0));
    // Monday the 2nd.
    $this->assertTrue (Filter_Datetime::isFirstWorkingDayOfMonth (2, 1));
    // Tuesday the 2nd.
    $this->assertFalse (Filter_Datetime::isFirstWorkingDayOfMonth (2, 2));
    // Sunday the 3nd.
    $this->assertFalse (Filter_Datetime::isFirstWorkingDayOfMonth (3, 0));
    // Monday the 3nd.
    $this->assertTrue (Filter_Datetime::isFirstWorkingDayOfMonth (3, 1));
    // Tuesday the 3nd.
    $this->assertFalse (Filter_Datetime::isFirstWorkingDayOfMonth (3, 2));
    // Sunday the 4nd.
    $this->assertFalse (Filter_Datetime::isFirstWorkingDayOfMonth (4, 0));
    // Monday the 4nd.
    $this->assertFalse (Filter_Datetime::isFirstWorkingDayOfMonth (4, 1));
    // Tuesday the 4nd.
    $this->assertFalse (Filter_Datetime::isFirstWorkingDayOfMonth (4, 2));
  }


  public function testLastBusinessDayOfMonth ()
  {
    $this->assertEquals (30, Filter_Datetime::getLastBusinessDayOfMonth (2013, 9));
    $this->assertEquals (31, Filter_Datetime::getLastBusinessDayOfMonth (2013, 10));
    $this->assertEquals (29, Filter_Datetime::getLastBusinessDayOfMonth (2013, 11));
    $this->assertEquals (31, Filter_Datetime::getLastBusinessDayOfMonth (2013, 12));
    $this->assertEquals (31, Filter_Datetime::getLastBusinessDayOfMonth (2014, 1));
    $this->assertEquals (28, Filter_Datetime::getLastBusinessDayOfMonth (2014, 2));
    $this->assertEquals (31, Filter_Datetime::getLastBusinessDayOfMonth (2014, 3));
    $this->assertEquals (30, Filter_Datetime::getLastBusinessDayOfMonth (2014, 4));
    $this->assertEquals (30, Filter_Datetime::getLastBusinessDayOfMonth (2014, 5));
    $this->assertEquals (30, Filter_Datetime::getLastBusinessDayOfMonth (2014, 6));
  }
  
  
  public function testIsBusinessDay ()
  {
    $this->assertFalse (Filter_Datetime::isBusinessDay (2013, 9, 1));
    $this->assertTrue (Filter_Datetime::isBusinessDay (2013, 9, 2));
    $this->assertTrue (Filter_Datetime::isBusinessDay (2013, 9, 3));
    $this->assertTrue (Filter_Datetime::isBusinessDay (2013, 9, 4));
    $this->assertTrue (Filter_Datetime::isBusinessDay (2013, 9, 5));
    $this->assertTrue (Filter_Datetime::isBusinessDay (2013, 9, 6));
    $this->assertFalse (Filter_Datetime::isBusinessDay (2013, 9, 7));
    $this->assertFalse (Filter_Datetime::isBusinessDay (2013, 9, 8));
    $this->assertTrue (Filter_Datetime::isBusinessDay (2013, 9, 30));
  }



}
?>

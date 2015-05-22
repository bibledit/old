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

 
class filterEmailTest extends PHPUnit_Framework_TestCase
{

  public function testFilterEmailExtractEmail()
  {
    $this->assertEquals("foo@bar.eu", Filter_Email::extractEmail ("Foo Bar <foo@bar.eu>"));
    $this->assertEquals("foo@bar.eu", Filter_Email::extractEmail ("<foo@bar.eu>"));
    $this->assertEquals("foo@bar.eu", Filter_Email::extractEmail ("foo@bar.eu"));
  }

  public function testFilterEmailExtractBody()
  {
$body = <<<EOD
body
EOD;
    $this->assertEquals("body", Filter_Email::extractBody ($body));
$body = <<<EOD

test

On Wed, 2011-03-02 at 08:26 +0100, Bibledit-Web wrote:

> test notes three


> test

On Wed, 2011-03-02 at 08:26 +0100, Bibledit-Web wrote:

>    test notes three 
EOD;
    $this->assertEquals("test", Filter_Email::extractBody ($body, "2011", "Bibledit-Web"));

  }


  
}
?>

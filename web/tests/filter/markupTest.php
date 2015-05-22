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

 
class filterMarkupTest extends PHPUnit_Framework_TestCase
{

  public function testOne ()
  {
$string = <<<EOD
Zvino uchagadzira makumbo uye Makumbo uye maKumbo uye MAKUMBO emuakasia*, ndokuaputira negoridhe.
Zvino uchaisa makumbo muzvindori panhivi dzeareka, kuti areka itakurwe nawo.
Zvindori zvichava pamupendero kuti zvive nzvimbo dzemakumbo kutakura tafura.
Zvino uchaita makumbo nematanda neMatanda nemaTANDA emuAkasia, ugoiputira negoridhe, kuti tafura itakurwe nawo.
EOD;
    $words = array ("makumbo", "akasia", "matanda");
    $result = Filter_Markup::words ($words, $string);
$standard = <<<EOD
Zvino uchagadzira <mark>makumbo</mark> uye <mark>Makumbo</mark> uye <mark>maKumbo</mark> uye <mark>MAKUMBO</mark> emu<mark>akasia</mark>*, ndokuaputira negoridhe.
Zvino uchaisa <mark>makumbo</mark> muzvindori panhivi dzeareka, kuti areka itakurwe nawo.
Zvindori zvichava pamupendero kuti zvive nzvimbo dze<mark>makumbo</mark> kutakura tafura.
Zvino uchaita <mark>makumbo</mark> ne<mark>matanda</mark> ne<mark>Matanda</mark> ne<mark>maTANDA</mark> emu<mark>Akasia</mark>, ugoiputira negoridhe, kuti tafura itakurwe nawo.
EOD;
    $this->assertEquals ($standard, $result);
  }
  
  
  public function testTwo ()
  {
$string = <<<EOD
Zvino uchagadzira makumbo uye Makumbo uye maKumbo uye MAKUMBO emuakasia*, ndokuaputira negoridhe.
Zvino uchaisa makumbo muzvindori panhivi dzeareka, kuti areka itakurwe nawo.
Zvindori zvichava pamupendero kuti zvive nzvimbo dzemakumbo kutakura tafura.
Zvino uchaita makumbo nematanda neMatanda nemaTANDA emuAkasia, ugoiputira negoridhe, kuti tafura itakurwe nawo.
EOD;
    $words = array ();
    $result = Filter_Markup::words ($words, $string);
    $this->assertEquals ($string, $result);
  }
  
  
}
?>

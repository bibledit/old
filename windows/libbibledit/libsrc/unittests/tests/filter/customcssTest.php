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

 
class filterCustomCSSTest extends PHPUnit_Framework_TestCase
{


  public function testDirection ()
  {
    $this->assertEquals ("checked", Filter_CustomCSS::directionUnspecified (100));
    $this->assertEquals ("", Filter_CustomCSS::directionUnspecified (101));

    $this->assertEquals ("checked", Filter_CustomCSS::directionLeftToRight (101));
    $this->assertEquals ("", Filter_CustomCSS::directionLeftToRight (102));

    $this->assertEquals ("checked", Filter_CustomCSS::directionRightToLeft (102));
    $this->assertEquals ("", Filter_CustomCSS::directionRightToLeft (103));

    $this->assertEquals (0, Filter_CustomCSS::directionValue (""));
    $this->assertEquals (1, Filter_CustomCSS::directionValue ("ltr"));
    $this->assertEquals (2, Filter_CustomCSS::directionValue ("rtl"));
  }

  
  public function testWritingMode ()
  {
    $this->assertEquals ("checked", Filter_CustomCSS::writingModeUnspecified (102));
    $this->assertEquals ("", Filter_CustomCSS::writingModeUnspecified (112));

    $this->assertEquals ("checked", Filter_CustomCSS::writingModeTopBottomLeftRight (112));
    $this->assertEquals ("", Filter_CustomCSS::writingModeTopBottomLeftRight (122));

    $this->assertEquals ("checked", Filter_CustomCSS::writingModeTopBottomRightLeft (122));
    $this->assertEquals ("", Filter_CustomCSS::writingModeTopBottomRightLeft (132));

    $this->assertEquals ("checked", Filter_CustomCSS::writingModeBottomTopLeftRight (132));
    $this->assertEquals ("", Filter_CustomCSS::writingModeBottomTopLeftRight (142));

    $this->assertEquals ("checked", Filter_CustomCSS::writingModeBottomTopRightLeft (142));
    $this->assertEquals ("", Filter_CustomCSS::writingModeBottomTopRightLeft (152));
  }
 
  
  public function testCss ()
  {
    $css = Filter_CustomCSS::getCss ("class", "", 0);

$standard = <<<EOD
.class
{
}
EOD;
    $this->assertEquals ($standard, $css);

    $css = Filter_CustomCSS::getCss ("class", "", 101);
$standard = <<<EOD
.class
{
direction: ltr;
}
EOD;
    $this->assertEquals ($standard, $css);


    $css = Filter_CustomCSS::getCss ("class", "", 102);
$standard = <<<EOD
.class
{
direction: rtl;
}
EOD;
    $this->assertEquals ($standard, $css);

    $css = Filter_CustomCSS::getCss ("class", "", 110);
$standard = <<<EOD
.class
{
writing-mode: tb-lr;
}
EOD;
    $this->assertEquals ($standard, $css);

    $css = Filter_CustomCSS::getCss ("class", "", 130);
$standard = <<<EOD
.class
{
writing-mode: bt-lr;
}
EOD;
    $this->assertEquals ($standard, $css);

    $css = Filter_CustomCSS::getCss ("class", "", 1322);
$standard = <<<EOD
.class
{
direction: rtl;
writing-mode: tb-rl;
}
EOD;
    $this->assertEquals ($standard, $css);

    $css = Filter_CustomCSS::getCss ("class", "sherif", 0);
$standard = <<<EOD
.class
{
font-family: sherif;
}
EOD;
    $this->assertEquals ($standard, $css);

    $css = Filter_CustomCSS::getCss ("class", "sherif", 102);
$standard = <<<EOD
.class
{
font-family: sherif;
direction: rtl;
}
EOD;
    $this->assertEquals ($standard, $css);

    $css = Filter_CustomCSS::getCss ("class", "../font.ttf", 0);
$standard = <<<EOD
@font-face
{
font-family: class;
src: url(../font.ttf);
}
.class
{
font-family: class;
}
EOD;
    $this->assertEquals ($standard, $css);
  }

  
  public function testClass ()
  {
    $class = Filter_CustomCSS::getClass ("ആഈഘലറ");
    $this->assertEquals ("customf86528", $class);
  }
  
  
}


?>

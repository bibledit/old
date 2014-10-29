<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

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


class Filter_CustomCSS
{


  public static function directionUnspecified ($value)
  {
    $value = $value % 10;
    if ($value == 0) return "checked";
    else return "";
  }


  public static function directionLeftToRight ($value)
  {
    $value = $value % 10;
    if ($value == 1) return "checked";
    else return "";
  }


  public static function directionRightToLeft ($value)
  {
    $value = $value % 10;
    if ($value == 2) return "checked";
    else return "";
  }


  public static function ltr ()
  {
    return "ltr";
  }
  

  public static function rtl ()
  {
    return "rtl";
  }
  

  public static function directionValue ($direction)
  {
    if ($direction == self::ltr ()) return 1;
    if ($direction == self::rtl ()) return 2;
    return 0;
  }


  public static function writingModeUnspecified ($value)
  {
    $value = (int) ($value / 10);
    $value = $value % 10;
    if ($value == 0) return "checked";
    else return "";
  }


  public static function writingModeTopBottomLeftRight ($value)
  {
    $value = (int) ($value / 10);
    $value = $value % 10;
    if ($value == 1) return "checked";
    else return "";
  }


  public static function writingModeTopBottomRightLeft ($value)
  {
    $value = (int) ($value / 10);
    $value = $value % 10;
    if ($value == 2) return "checked";
    else return "";
  }


  public static function writingModeBottomTopLeftRight ($value)
  {
    $value = (int) ($value / 10);
    $value = $value % 10;
    if ($value == 3) return "checked";
    else return "";
  }


  public static function writingModeBottomTopRightLeft ($value)
  {
    $value = (int) ($value / 10);
    $value = $value % 10;
    if ($value == 4) return "checked";
    else return "";
  }


  public static function tb_lr ()
  {
    return "tb-lr";
  }
  

  public static function tb_rl ()
  {
    return "tb-rl";
  }
  

  public static function bt_lr ()
  {
    return "bt-lr";
  }
  

  public static function bt_rl ()
  {
    return "bt-rl";
  }
  

  public static function writingModeValue ($mode)
  {
    if ($mode == self::tb_lr ()) return 1;
    if ($mode == self::tb_rl ()) return 2;
    if ($mode == self::bt_lr ()) return 3;
    if ($mode == self::bt_rl ()) return 4;
    return 0;
  }


  // The purpose of the function is to convert the name of the $bible into a string
  // that is acceptable as a class identifier in HTML.
  // Since a $bible can contain any Unicode character, 
  // just using the $bible as the class identifier will not work. 
  // The function solves that.
  public static function getClass ($bible)
  {
    $class = md5 ($bible);
    $class = substr ($class, 0, 6);
    $class = "custom$class";
    return $class;
  }


  // This function produces CSS based on input.
  // $class: The class for the CSS.
  // $font: The name or URL of the font to use. It may be empty.
  // $directionvalue: The value for the text direction.  
  public static function getCss ($class, $font, $directionvalue)
  {
    $css = array ();

    // If the $font has a URL, then it is a web font.
    if (($font != basename ($font)) && $font) {
      $css [] = "@font-face";
      $css [] = "{";
      $css [] = "font-family: $class;";
      $css [] = "src: url($font);";
      $css [] = "}";
      // Below, properly reference the above web font as the class.
      $font = $class;
    }
    
    $css [] = ".$class";
    $css [] = "{";

    if ($font != "") {
      $css [] = "font-family: $font;";
    }

    $direction = $directionvalue % 10;
    
    if ($direction > 0) {
      $line = "direction: ";
      if ($direction == 2) $line .= self::rtl ();
      else $line .= self::ltr ();
      $line .= ";";
      $css [] = $line;
    }

    $mode = (int) ($directionvalue / 10);
    $mode = $mode % 10;

    if ($mode > 0) {
      $line = "writing-mode: ";
      switch ($mode) {
        case 1: $line .= self::tb_lr (); break;
        case 2: $line .= self::tb_rl (); break;
        case 3: $line .= self::bt_lr (); break;
        case 4: $line .= self::bt_rl (); break;
        default: $line .= self::tb_lr (); break;
      }
      $line .= ";";
      $css [] = $line;
    }

    $css [] = "}";
 
    $css = implode ("\n", $css);       
    return $css;
  }
  

}


?>

/*
 Copyright (©) 2003-2015 Teus Benschop.
 
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


#include <filter/customcss.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/md5.h>
#include <config/globals.h>
#include <webserver/request.h>
#include <styles/logic.h>


string Filter_CustomCSS::directionUnspecified (int value)
{
  value = value % 10;
  if (value == 0) return "checked";
  else return "";
}


string Filter_CustomCSS::directionLeftToRight (int value)
{
  value = value % 10;
  if (value == 1) return "checked";
  else return "";
}


string Filter_CustomCSS::directionRightToLeft (int value)
{
  value = value % 10;
  if (value == 2) return "checked";
  else return "";
}


string Filter_CustomCSS::ltr ()
{
  return "ltr";
}


string Filter_CustomCSS::rtl ()
{
  return "rtl";
}


int Filter_CustomCSS::directionValue (string direction)
{
  if (direction == ltr ()) return 1;
  if (direction == rtl ()) return 2;
  return 0;
}


string Filter_CustomCSS::writingModeUnspecified (int value)
{
  value = (int) (value / 10);
  value = value % 10;
  if (value == 0) return "checked";
  else return "";
}


string Filter_CustomCSS::writingModeTopBottomLeftRight (int value)
{
  value = (int) (value / 10);
  value = value % 10;
  if (value == 1) return "checked";
  else return "";
}


string Filter_CustomCSS::writingModeTopBottomRightLeft (int value)
{
  value = (int) (value / 10);
  value = value % 10;
  if (value == 2) return "checked";
  else return "";
}


string Filter_CustomCSS::writingModeBottomTopLeftRight (int value)
{
  value = (int) (value / 10);
  value = value % 10;
  if (value == 3) return "checked";
  else return "";
}


string Filter_CustomCSS::writingModeBottomTopRightLeft (int value)
{
  value = (int) (value / 10);
  value = value % 10;
  if (value == 4) return "checked";
  else return "";
}


string Filter_CustomCSS::tb_lr ()
{
  return "tb-lr";
}


string Filter_CustomCSS::tb_rl ()
{
  return "tb-rl";
}


string Filter_CustomCSS::bt_lr ()
{
  return "bt-lr";
}


string Filter_CustomCSS::bt_rl ()
{
  return "bt-rl";
}


int Filter_CustomCSS::writingModeValue (string mode)
{
  if (mode == tb_lr ()) return 1;
  if (mode == tb_rl ()) return 2;
  if (mode == bt_lr ()) return 3;
  if (mode == bt_rl ()) return 4;
  return 0;
}


// The purpose of the function is to convert the name of the bible into a string
// that is acceptable as a class identifier in HTML.
// Since a bible can contain any Unicode character,
// just using the bible as the class identifier will not work.
// The function solves that.
string Filter_CustomCSS::getClass (string bible)
{
  string Class = md5 (bible);
  Class = Class.substr (0, 6);
  Class = "custom" + Class;
  return Class;
}


// This function produces CSS based on input.
// class: The class for the CSS.
// font: The name or URL of the font to use. It may be empty.
// directionvalue: The value for the text direction.
string Filter_CustomCSS::getCss (string class_, string font, int directionvalue)
{
  vector <string> css;
  
  // If the font has a URL, then it is a web font.
  if ((font != filter_url_basename (font)) && !font.empty()) {
    css.push_back ("@font-face");
    css.push_back ("{");
    css.push_back ("font-family: " + class_ + ";");
    css.push_back ("src: url(" + font + ");");
    css.push_back ("}");
    // Below, properly reference the above web font as the class.
    font = class_;
  }
  
  css.push_back ("." + class_);
  css.push_back ("{");
  
  if (font != "") {
    css.push_back ("font-family: " + font + ";");
  }
  
  int direction = directionvalue % 10;
  
  if (direction > 0) {
    string line = "direction: ";
    if (direction == 2) line += rtl ();
    else line += ltr ();
    line += ";";
    css.push_back (line);
  }
  
  int mode = (int) (directionvalue / 10);
  mode = mode % 10;
  
  if (mode > 0) {
    string line = "writing-mode: ";
    switch (mode) {
      case 1: line += tb_lr (); break;
      case 2: line += tb_rl (); break;
      case 3: line += bt_lr (); break;
      case 4: line += bt_rl (); break;
      default: line += tb_lr (); break;
    }
    line += ";";
    css.push_back (line);
  }
  
  css.push_back ("}");
  
  return filter_string_implode (css, "\n");
}

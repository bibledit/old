/*
Copyright (©) 2003-2016 Teus Benschop.

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


#include <unittests/0template.h>
#include <unittests/utilities.h>
#include <database/styles.h>
#include <database/state.h>
#include <webserver/request.h>
#include <styles/css.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/css.h>


void test_styles ()
{
  trace_unit_tests (__func__);
  Webserver_Request request;
  Database_State database_state;
  database_state.create ();

  // Create basic stylesheet.
  {
    refresh_sandbox (true);
    Database_Styles database_styles;
    database_styles.createSheet ("phpunit");
    Styles_Css styles_css = Styles_Css (&request, "phpunit");
    styles_css.generate ();
    string css = styles_css.css ();
    string standard = filter_url_file_get_contents (filter_url_create_path ("unittests", "tests", "basic.css"));
    evaluate (__LINE__, __func__, standard, css);
  }
  
  // Create stylesheet for export.
  {
    refresh_sandbox (true);
    Database_Styles database_styles;
    database_styles.createSheet ("phpunit");
    Styles_Css styles_css = Styles_Css (&request, "phpunit");
    styles_css.exports ();
    styles_css.generate ();
    string css = styles_css.css ();
    string standard = filter_string_trim (filter_url_file_get_contents (filter_url_create_path ("unittests", "tests", "exports.css")));
    evaluate (__LINE__, __func__, standard, css);
  }
  
  // Stylesheet for the Bible editor.
  {
    refresh_sandbox (true);
    Database_Styles database_styles;
    database_styles.createSheet ("phpunit");
    Styles_Css styles_css = Styles_Css (&request, "phpunit");
    styles_css.editor ();
    styles_css.generate ();
    string css = styles_css.css ();
    string standard = filter_string_trim (filter_url_file_get_contents (filter_url_create_path ("unittests", "tests", "editor.css")));
    evaluate (__LINE__, __func__, standard, css);
  }
  
  // Direction
  {
    evaluate (__LINE__, __func__, "checked", Filter_Css::directionUnspecified (100));
    evaluate (__LINE__, __func__, "", Filter_Css::directionUnspecified (101));
    
    evaluate (__LINE__, __func__, "checked", Filter_Css::directionLeftToRight (101));
    evaluate (__LINE__, __func__, "", Filter_Css::directionLeftToRight (102));
    
    evaluate (__LINE__, __func__, "checked", Filter_Css::directionRightToLeft (102));
    evaluate (__LINE__, __func__, "", Filter_Css::directionRightToLeft (103));
    
    evaluate (__LINE__, __func__, 0, Filter_Css::directionValue (""));
    evaluate (__LINE__, __func__, 1, Filter_Css::directionValue ("ltr"));
    evaluate (__LINE__, __func__, 2, Filter_Css::directionValue ("rtl"));
  }

  // Writing Mode
  {
    evaluate (__LINE__, __func__, "checked", Filter_Css::writingModeUnspecified (102));
    evaluate (__LINE__, __func__, "", Filter_Css::writingModeUnspecified (112));
    
    evaluate (__LINE__, __func__, "checked", Filter_Css::writingModeTopBottomLeftRight (112));
    evaluate (__LINE__, __func__, "", Filter_Css::writingModeTopBottomLeftRight (122));
    
    evaluate (__LINE__, __func__, "checked", Filter_Css::writingModeTopBottomRightLeft (122));
    evaluate (__LINE__, __func__, "", Filter_Css::writingModeTopBottomRightLeft (132));
    
    evaluate (__LINE__, __func__, "checked", Filter_Css::writingModeBottomTopLeftRight (132));
    evaluate (__LINE__, __func__, "", Filter_Css::writingModeBottomTopLeftRight (142));
    
    evaluate (__LINE__, __func__, "checked", Filter_Css::writingModeBottomTopRightLeft (142));
    evaluate (__LINE__, __func__, "", Filter_Css::writingModeBottomTopRightLeft (152));
  }

  // CSS.
  {
    string css = Filter_Css::getCss ("class", "", 0);
    
    string standard =
    ".class\n"
    "{\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("class", "", 101);
    standard =
    ".class\n"
    "{\n"
    "direction: ltr;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("class", "", 102);
    standard =
    ".class\n"
    "{\n"
    "direction: rtl;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("class", "", 110);
    standard =
    ".class\n"
    "{\n"
    "writing-mode: tb-lr;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("CLass", "", 130);
    standard =
    ".CLass\n"
    "{\n"
    "writing-mode: bt-lr;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("CLass", "", 1322);
    standard =
    ".CLass\n"
    "{\n"
    "direction: rtl;\n"
    "writing-mode: tb-rl;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("Class", "sherif", 0);
    standard =
    ".Class\n"
    "{\n"
    "font-family: sherif;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("Class", "sherif", 102);
    standard =
    ".Class\n"
    "{\n"
    "font-family: sherif;\n"
    "direction: rtl;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("classs", "../font.ttf", 0);
    standard =
    "@font-face\n"
    "{\n"
    "font-family: classs;\n"
    "src: url(../font.ttf);\n"
    "}\n"
    ".classs\n"
    "{\n"
    "font-family: classs;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
  }

  // Class.
  {
    string clss = Filter_Css::getClass ("ആഈഘലറ");
    evaluate (__LINE__, __func__, "customf86528", clss);
  }

  refresh_sandbox (true);
}

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


#include <unittests/string.h>
#include <unittests/utilities.h>
#include <filter/string.h>


void test_string ()
{
  trace_unit_tests (__func__);
  
  // Test filter_string_str_replace.
  {
    // Shows that std::string handles UTF-8 well for simple operations.
    evaluate (__LINE__, __func__, "⇊⇦", filter_string_str_replace ("⇖", "", "⇊⇖⇦"));
    // Exercise the replacement counter.
    int counter = 0;
    evaluate (__LINE__, __func__, "a", filter_string_str_replace ("bc", "", "abc", &counter));
    evaluate (__LINE__, __func__, 1, counter);
  }

  // Test array_unique, a C++ equivalent for PHP's array_unique function.
  {
    vector <string> reference;
    reference.push_back ("aaa");
    reference.push_back ("b");
    reference.push_back ("zzz");
    reference.push_back ("x");
    reference.push_back ("yyy");
    reference.push_back ("k");
    vector <string> input;
    input.push_back ("aaa");
    input.push_back ("b");
    input.push_back ("aaa");
    input.push_back ("b");
    input.push_back ("aaa");
    input.push_back ("zzz");
    input.push_back ("x");
    input.push_back ("x");
    input.push_back ("yyy");
    input.push_back ("k");
    input.push_back ("k");
    vector <string> output = array_unique (input);
    evaluate (__LINE__, __func__, reference, output);
  }

  // Test array_unique, a C++ equivalent for PHP's array_unique function.
  {
    vector <int> reference;
    reference.push_back (111);
    reference.push_back (2);
    reference.push_back (999);
    reference.push_back (7);
    reference.push_back (888);
    reference.push_back (5);
    vector <int> input;
    input.push_back (111);
    input.push_back (2);
    input.push_back (111);
    input.push_back (2);
    input.push_back (111);
    input.push_back (999);
    input.push_back (7);
    input.push_back (7);
    input.push_back (888);
    input.push_back (5);
    input.push_back (5);
    vector <int> output = array_unique (input);
    evaluate (__LINE__, __func__, reference, output);
  }

  // Test filter_string_array_diff, a C++ equivalent for PHP's filter_string_array_diff function.
  {
    vector <string> reference;
    reference.push_back ("aaa");
    reference.push_back ("zzz");
    reference.push_back ("b");
    vector <string> from;
    from.push_back ("aaa");
    from.push_back ("bbb");
    from.push_back ("ccc");
    from.push_back ("zzz");
    from.push_back ("b");
    from.push_back ("x");
    vector <string> against;
    against.push_back ("bbb");
    against.push_back ("ccc");
    against.push_back ("x");
    vector <string> output = filter_string_array_diff (from, against);
    evaluate (__LINE__, __func__, reference, output);
  }

  // Test filter_string_array_diff, a C++ equivalent for PHP's filter_string_array_diff function.
  {
    vector <int> reference;
    reference.push_back (111);
    reference.push_back (999);
    reference.push_back (2);
    vector <int> from;
    from.push_back (111);
    from.push_back (222);
    from.push_back (333);
    from.push_back (999);
    from.push_back (2);
    from.push_back (8);
    vector <int> against;
    against.push_back (222);
    against.push_back (333);
    against.push_back (8);
    vector <int> output = filter_string_array_diff (from, against);
    evaluate (__LINE__, __func__, reference, output);
  }

  // Test for array_intersect, a C++ equivalent for PHP's array_intersect function.
  {
    vector <int> one;
    vector <int> two;
    one = {1};
    two = {2};
    evaluate (__LINE__, __func__, {}, array_intersect (one, two));
    one = {1, 2, 3};
    two = {2, 3, 4};
    evaluate (__LINE__, __func__, {2, 3}, array_intersect (one, two));
    one = {1, 2, 3, 4, 5};
    two = {2, 3, 4};
    evaluate (__LINE__, __func__, {2, 3, 4}, array_intersect (one, two));
  }
  
  // Test hex2bin and bin2hex as equivalents to PHP's functions.
  {
    string bin = "This is a 123 test.";
    string hex = "5468697320697320612031323320746573742e";
    evaluate (__LINE__, __func__, hex, bin2hex (bin));
    evaluate (__LINE__, __func__, bin, hex2bin (hex));
    bin = "סֶ	א	ב	ױ";
    hex = "d7a1d6b609d79009d79109d7b1";
    evaluate (__LINE__, __func__, hex, bin2hex (bin));
    evaluate (__LINE__, __func__, bin, hex2bin (hex));
    bin.clear ();
    hex.clear ();
    evaluate (__LINE__, __func__, hex, bin2hex (bin));
    evaluate (__LINE__, __func__, bin, hex2bin (hex));
    hex = "a";
    evaluate (__LINE__, __func__, "", bin2hex (bin));
    evaluate (__LINE__, __func__, bin, hex2bin (hex));
  }

  // Test string modifiers.
  {
    evaluate (__LINE__, __func__, "", filter_string_trim ("  "));
    evaluate (__LINE__, __func__, "", filter_string_trim (""));
    evaluate (__LINE__, __func__, "xx", filter_string_trim ("\t\nxx\n\r"));
    evaluate (__LINE__, __func__, "", filter_string_ltrim ("  "));
    evaluate (__LINE__, __func__, "", filter_string_ltrim (""));
    evaluate (__LINE__, __func__, "xx\n\r", filter_string_ltrim ("xx\n\r"));
    evaluate (__LINE__, __func__, "xx  ", filter_string_ltrim ("  xx  "));
    evaluate (__LINE__, __func__, "0000012345", filter_string_fill ("12345", 10, '0'));
  }
  {
    evaluate (__LINE__, __func__, true, filter_string_is_numeric ("1"));
    evaluate (__LINE__, __func__, true, filter_string_is_numeric ("1234"));
    evaluate (__LINE__, __func__, false, filter_string_is_numeric ("X"));
    evaluate (__LINE__, __func__, false, filter_string_is_numeric ("120X"));
  }
  {
    evaluate (__LINE__, __func__, 0, convert_to_int (""));
    evaluate (__LINE__, __func__, 123, convert_to_int ("123"));
    evaluate (__LINE__, __func__, 123, convert_to_int ("123xx"));
    evaluate (__LINE__, __func__, 0, convert_to_int ("xxx123xx"));
  }
  {
    evaluate (__LINE__, __func__, true, unicode_string_is_valid ("valid"));
    evaluate (__LINE__, __func__, true, unicode_string_is_valid ("בְּרֵאשִׁית, בָּרָא אֱלֹהִים, אֵת הַשָּׁמַיִם, וְאֵת הָאָרֶץ"));
  }
  {
    vector <string> haystack = {"needle"};
    string needle = "needle";
    evaluate (__LINE__, __func__, true, in_array (needle, haystack));
    needle.clear ();
    evaluate (__LINE__, __func__, false, in_array (needle, haystack));
    haystack.clear ();
    evaluate (__LINE__, __func__, false, in_array (needle, haystack));
    needle = "needle";
    evaluate (__LINE__, __func__, false, in_array (needle, haystack));
    evaluate (__LINE__, __func__, true, in_array (1, {1, 2, 3}));
    evaluate (__LINE__, __func__, false, in_array (1, {2, 3}));
  }

  // Test random number generator.
  {
    int floor = 100000;
    int ceiling = 999999;
    int r1 = filter_string_rand (floor, ceiling);
    if ((r1 < floor) || (r1 > ceiling)) evaluate (__LINE__, __func__, "Random generator out of bounds", convert_to_string (r1));
    int r2 = filter_string_rand (floor, ceiling);
    if ((r2 < floor) || (r2 > ceiling)) evaluate (__LINE__, __func__, "Random generator out of bounds", convert_to_string (r2));
    if (r1 == r2) evaluate (__LINE__, __func__, "Random generator should generate different values", convert_to_string (r1) + " " + convert_to_string (r2));
  }

  // Convert html to plain text.
  {
    string html =
    "<p>author</p>\n"
    "<p>Text 1<div>Text 2</div><div>Text 3</div></p>";
    string plain =
    "author\n"
    "Text 1\n"
    "Text 2\n"
    "Text 3";
    evaluate (__LINE__, __func__, plain, filter_string_html2text (html));
  }
  {
    string html =
    "<p>writer (15 Nov):</p>\n"
    "<p>Second note.<div>Second matter.</div><div>A second round is needed.</div></p>\n"
    "<p>Here is <b>bold</b>, and here is <i>italics</i>.<div>Here is <sup>superscript</sup>&nbsp;and here is <sub>subscript</sub>.</div><div style=\"text-align: center;\">Centered</div><div style=\"text-align: left;\">Left justified</div><div style=\"text-align: left;\"><ol><li>Numbered list.</li></ol>No numbered text.</div><div style=\"text-align: left;\">Link to <a href=\"http://google.nl\">http://google.nl</a>.</div><div style=\"text-align: left;\">Here follows an image:&nbsp;<img src=\"http://localhost/image\">.<br></div><h1>Header 1</h1><div>Normal text again below the header.</div></p>\n";
    string plain =
    "writer (15 Nov):\n"
    "Second note.\n"
    "Second matter.\n"
    "A second round is needed.\n"
    "Here is bold, and here is italics.\n"
    "Here is superscript and here is subscript.\n"
    "Centered\n"
    "Left justified\n"
    "Numbered list.\n"
    "No numbered text.\n"
    "Link to http://google.nl.\n"
    "Here follows an image: .\n"
    "Header 1\n"
    "Normal text again below the header.\n";
    evaluate (__LINE__, __func__, filter_string_trim (plain), filter_string_trim (filter_string_html2text (html)));
  }
  {
    string html =
    "test notes four\n"
    "\n"
    "Logbook:\n"
    "\n";
    string plain =
    "test notes fourLogbook:";
    evaluate (__LINE__, __func__, filter_string_trim (plain), filter_string_trim (filter_string_html2text (html)));
  }
  {
    string html =
    "Line one.<BR>\n"
    "\n"
    "Line two.<BR>\n"
    "\n"
    "Line three.<BR>\n";
    string plain =
    "Line one.\n"
    "Line two.\n"
    "Line three.\n";
    evaluate (__LINE__, __func__, filter_string_trim (plain), filter_string_trim (filter_string_html2text (html)));
  }

  // Email address extraction.
  {
    evaluate (__LINE__, __func__, "foo@bar.eu", filter_string_extract_email ("Foo Bar <foo@bar.eu>"));
    evaluate (__LINE__, __func__, "foo@bar.eu", filter_string_extract_email ("<foo@bar.eu>"));
    evaluate (__LINE__, __func__, "foo@bar.eu", filter_string_extract_email ("foo@bar.eu"));
    
    string body = "body";
    evaluate (__LINE__, __func__, body, filter_string_extract_body (body));
    
    body =
    "\n"
    "test\n"
    "\n"
    "On Wed, 2011-03-02 at 08:26 +0100, Bibledit wrote:\n"
    "\n"
    "> test notes three\n"
    "\n"
    "\n"
    "> test\n"
    "\n"
    "On Wed, 2011-03-02 at 08:26 +0100, Bibledit wrote:\n"
    "\n"
    ">    test notes three \n";
    evaluate (__LINE__, __func__, "test", filter_string_extract_body (body, "2011", "Bibledit"));
  }
}

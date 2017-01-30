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
}

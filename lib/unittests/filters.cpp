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


#include <unittests/unittest.h>
#include <unittests/utilities.h>
#include <unittests/libraries.h>
#include <config/libraries.h>
#include <library/bibledit.h>
#include <database/config/user.h>
#include <database/styles.h>
#include <database/search.h>
#include <database/books.h>
#include <database/config/bible.h>
#include <config/globals.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/roles.h>
#include <filter/md5.h>
#include <filter/usfm.h>
#include <filter/archive.h>
#include <filter/text.h>
#include <filter/customcss.h>
#include <filter/bibleworks.h>
#include <filter/diff.h>
#include <filter/abbreviations.h>
#include <session/logic.h>
#include <text/text.h>
#include <esword/text.h>
#include <onlinebible/text.h>
#include <html/text.h>
#include <odf/text.h>
#include <styles/logic.h>
#include <styles/css.h>


void test_filters_test1 ()
{
  {
    // Filter_Roles.
    evaluate (__LINE__, __func__, 3, Filter_Roles::consultant ());
    evaluate (__LINE__, __func__, 1, Filter_Roles::lowest ());
  }
  {
    // mkdir including parents.
    string directory = filter_url_create_path (testing_directory, "a", "b");
    filter_url_mkdir (directory);
    string path = filter_url_create_path (directory, "c");
    string contents = "unittest";
    filter_url_file_put_contents (path, contents);
    evaluate (__LINE__, __func__, contents, filter_url_file_get_contents (path));
  }
  {
    // C++ md5 function as compared to PHP's version.
    evaluate (__LINE__, __func__, "1f3870be274f6c49b3e31a0c6728957f", md5 ("apple"));
  }
  {
    // Test str_replace.
    // Shows that std::string handles UTF-8 well for simple operations. 
    evaluate (__LINE__, __func__, "⇊⇦", filter_string_str_replace ("⇖", "", "⇊⇖⇦"));
  }
  {
    // Test filter_string_array_unique, a C++ equivalent for PHP's array_unique function.
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
    vector <string> output = filter_string_array_unique (input);
    evaluate (__LINE__, __func__, reference, output);
  }
  {
    // Test filter_string_array_diff, a C++ equivalent for PHP's filter_string_array_diff function.
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
  {
    // Test filter_url_escape_shell_argument.
    evaluate (__LINE__, __func__, "'argument'", filter_url_escape_shell_argument ("argument"));
    evaluate (__LINE__, __func__, "'argu\\'ment'", filter_url_escape_shell_argument ("argu'ment"));
  }
}


void test_filters_test2 ()
{
  {
    // Test string modifiers.
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
    // Test URL decoder.
    evaluate (__LINE__, __func__, "Store settings", filter_url_urldecode ("Store+settings"));
    evaluate (__LINE__, __func__, "test@mail", filter_url_urldecode ("test%40mail"));
    evaluate (__LINE__, __func__, "ᨀab\\d@a", filter_url_urldecode ("%E1%A8%80ab%5Cd%40a"));
  }
  {
    // Test dirname and basename functions.
    evaluate (__LINE__, __func__, ".", filter_url_dirname (""));
    evaluate (__LINE__, __func__, ".", filter_url_dirname ("/"));
    evaluate (__LINE__, __func__, ".", filter_url_dirname ("dir/"));
    evaluate (__LINE__, __func__, ".", filter_url_dirname ("/dir"));
    evaluate (__LINE__, __func__, "foo", filter_url_dirname ("foo/bar"));
    evaluate (__LINE__, __func__, "/foo", filter_url_dirname ("/foo/bar"));
    evaluate (__LINE__, __func__, "/foo", filter_url_dirname ("/foo/bar/"));
    evaluate (__LINE__, __func__, "a.txt", filter_url_basename ("/a.txt"));
    evaluate (__LINE__, __func__, "txt", filter_url_basename ("/txt/"));
    evaluate (__LINE__, __func__, "foo.bar", filter_url_basename ("/path/to/foo.bar"));
    evaluate (__LINE__, __func__, "foo.bar", filter_url_basename ("foo.bar"));
  }
  {
    // Test the date and time related functions.
    int month = filter_string_date_numerical_month ();
    if ((month < 1) || (month > 12)) evaluate (__LINE__, __func__, "current month", convert_to_string (month));
    int year = filter_string_date_numerical_year ();
    if ((year < 2014) || (year > 2050)) evaluate (__LINE__, __func__, "current year", convert_to_string (year));
    struct timeval tv;
    gettimeofday (&tv, NULL);
    int reference_second = tv.tv_sec;
    int actual_second = filter_string_date_seconds_since_epoch ();
    if (abs (actual_second - reference_second) > 1) evaluate (__LINE__, __func__, reference_second, actual_second);
    int usecs = filter_string_date_numerical_microseconds ();
    if ((usecs < 0) || (usecs > 1000000)) evaluate (__LINE__, __func__, "0-1000000", convert_to_string (usecs));
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
}


void test_filters_test3 ()
{
  // Test the USFM filter functions.
  {
    evaluate (__LINE__, __func__, "", usfm_one_string (""));
    evaluate (__LINE__, __func__, "\\id GEN", usfm_one_string ("\\id GEN\n"));
    evaluate (__LINE__, __func__, "\\v 10 text", usfm_one_string ("\\v 10\ntext"));
    evaluate (__LINE__, __func__, "\\v 10\\v 11", usfm_one_string ("\\v 10\n\\v 11"));
    evaluate (__LINE__, __func__, "\\v 10 text\\p\\v 11", usfm_one_string ("\\v 10 text\n\\p\\v 11"));
  }
  {
    evaluate (__LINE__, __func__, { "\\id ", "GEN", "\\c ", "10" }, usfm_get_markers_and_text ("\\id GEN\\c 10"));
    evaluate (__LINE__, __func__, { "noise", "\\id ", "GEN", "\\c ", "10" }, usfm_get_markers_and_text ("noise\\id GEN\\c 10"));
    evaluate (__LINE__, __func__, { "\\p", "\\v ", "1 In ", "\\add ", "the", "\\add*" }, usfm_get_markers_and_text ("\\p\\v 1 In \\add the\\add*"));
    evaluate (__LINE__, __func__, { "\\v ", "2 Text ", "\\add ", "of the ", "\\add*", "1st", "\\add ", "second verse", "\\add*", "." }, usfm_get_markers_and_text ("\\v 2 Text \\add of the \\add*1st\\add second verse\\add*."));
    evaluate (__LINE__, __func__, { "\\p", "\\v ", "1 In ", "\\+add ", "the", "\\+add*" }, usfm_get_markers_and_text ("\\p\\v 1 In \\+add the\\+add*"));
  }
  {
    evaluate (__LINE__, __func__, "", usfm_get_marker (""));
    evaluate (__LINE__, __func__, "id", usfm_get_marker ("\\id GEN"));
    evaluate (__LINE__, __func__, "add", usfm_get_marker ("\\add insertion"));
    evaluate (__LINE__, __func__, "add", usfm_get_marker ("\\add"));
    evaluate (__LINE__, __func__, "add", usfm_get_marker ("\\add*"));
    evaluate (__LINE__, __func__, "add", usfm_get_marker ("\\add*\\add"));
    evaluate (__LINE__, __func__, "add", usfm_get_marker ("\\+add"));
    evaluate (__LINE__, __func__, "add", usfm_get_marker ("\\+add*"));
  }
  {
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();

    evaluate (__LINE__, __func__, 0, usfm_import ("", "Standard").size());
    vector <BookChapterData> import2 = usfm_import ("\\id MIC\n\\c 1\n\\s Heading\n\\p\n\\v 1 Verse one.", "Standard");
    evaluate (__LINE__, __func__, 2, import2.size());
    if (import2.size () == 2) {
      evaluate (__LINE__, __func__, 33, import2 [0].book);
      evaluate (__LINE__, __func__, 0, import2 [0].chapter);
      evaluate (__LINE__, __func__, "\\id MIC", import2 [0].data);
      evaluate (__LINE__, __func__, 33, import2 [1].book);
      evaluate (__LINE__, __func__, 1, import2 [1].chapter);
      evaluate (__LINE__, __func__, "\\c 1\n\\s Heading\n\\p\n\\v 1 Verse one.", import2 [1].data);
    } else evaluate (__LINE__, __func__, "executing tests", "skipping tests");

    evaluate (__LINE__, __func__, {0, 1, 2}, usfm_get_verse_numbers ("\\v 1 test\\v 2 test"));
  }
}


void test_filters_test_usfm1 ()
{
  {
    string usfm = "\\id MIC";
    evaluate (__LINE__, __func__, 0, usfm_linenumber_to_versenumber (usfm, 0));
    usfm = "\\id MIC\n\\v 1 Verse";
    evaluate (__LINE__, __func__, 1, usfm_linenumber_to_versenumber (usfm, 1));
    usfm = "\\v 1 Verse";
    evaluate (__LINE__, __func__, 1, usfm_linenumber_to_versenumber (usfm, 0));
    usfm = "\\p\n\\v 3 Verse 3 (out of order).\n\\v 1 Verse 1. \n\\v 2 Verse 1.";
    evaluate (__LINE__, __func__, 0, usfm_linenumber_to_versenumber (usfm, 0));
    evaluate (__LINE__, __func__, 3, usfm_linenumber_to_versenumber (usfm, 1));
    evaluate (__LINE__, __func__, 1, usfm_linenumber_to_versenumber (usfm, 2));
    evaluate (__LINE__, __func__, 2, usfm_linenumber_to_versenumber (usfm, 3));

    usfm = "\\id MIC";
    evaluate (__LINE__, __func__, 0, usfm_offset_to_versenumber (usfm, 0));
    evaluate (__LINE__, __func__, 0, usfm_offset_to_versenumber (usfm, 7));
    evaluate (__LINE__, __func__, 0, usfm_offset_to_versenumber (usfm, 17));

    usfm = "\\id MIC\n\\v 1 Verse";
    evaluate (__LINE__, __func__, 0, usfm_offset_to_versenumber (usfm, 7));
    evaluate (__LINE__, __func__, 1, usfm_offset_to_versenumber (usfm, 8));

    usfm = "\\v 1 Verse";
    evaluate (__LINE__, __func__, 1, usfm_offset_to_versenumber (usfm, 0));
    evaluate (__LINE__, __func__, 1, usfm_offset_to_versenumber (usfm, 2));

    usfm = "\\p\n\\v 3 Verse 3 (out of order).\n\\v 1 Verse 1.\n\\v 2 Verse 2.";

    evaluate (__LINE__, __func__, 0, usfm_offset_to_versenumber (usfm, 0));
    evaluate (__LINE__, __func__, 0, usfm_offset_to_versenumber (usfm, 1));

    evaluate (__LINE__, __func__, 0, usfm_offset_to_versenumber (usfm, 2));
    evaluate (__LINE__, __func__, 3, usfm_offset_to_versenumber (usfm, 3));
    evaluate (__LINE__, __func__, 3, usfm_offset_to_versenumber (usfm, 4));

    evaluate (__LINE__, __func__, 3, usfm_offset_to_versenumber (usfm, 31));
    evaluate (__LINE__, __func__, 1, usfm_offset_to_versenumber (usfm, 32));
    evaluate (__LINE__, __func__, 1, usfm_offset_to_versenumber (usfm, 33));

    evaluate (__LINE__, __func__, 1, usfm_offset_to_versenumber (usfm, 45));
    evaluate (__LINE__, __func__, 2, usfm_offset_to_versenumber (usfm, 46));
    evaluate (__LINE__, __func__, 2, usfm_offset_to_versenumber (usfm, 47));

    usfm = "\\p\n\\v 1 Verse 1.\n\\v 2 Verse 2.\n\\v 3 Verse 3.";
    evaluate (__LINE__, __func__, 3, usfm_versenumber_to_offset (usfm, 1));
    evaluate (__LINE__, __func__, 17, usfm_versenumber_to_offset (usfm, 2));
    evaluate (__LINE__, __func__, 31, usfm_versenumber_to_offset (usfm, 3));
    evaluate (__LINE__, __func__, 44, usfm_versenumber_to_offset (usfm, 4));
    evaluate (__LINE__, __func__, 44, usfm_versenumber_to_offset (usfm, 5));

    usfm = "\\p\n\\v 1 One";
    evaluate (__LINE__, __func__, "\\v 1 One", usfm_get_verse_text (usfm, 1));
    evaluate (__LINE__, __func__, "\\p", usfm_get_verse_text (usfm, 0));
    evaluate (__LINE__, __func__, "", usfm_get_verse_text (usfm, 2));

    usfm = "\\c 1\n\\s Isibingelelo\n\\p\n\\v 1 Umdala\n\\p\n\\v 2 Sithandwa\n\\v 3 Ngoba\n\\v 4 Kangilantokozo\n\\s Inkathazo\n\\p\n\\v 5 Sithandwa\n\\v 6 abafakazele\n\\v 7 Ngoba\n\\v 8 Ngakho\n\\p\n\\v 9 Ngabhalela\n\\v 10 Ngakho\n\\p\n\\v 11 Sithandwa\n\\v 12 NgoDemetriyu\n\\s Isicino\n\\p\n\\v 13 Bengilezinto\n\\v 14 kodwa\n\\p Ukuthula";
    string result = "\\c 1\n\\s Isibingelelo\n\\p";
    evaluate (__LINE__, __func__, result, usfm_get_verse_text (usfm, 0));
    result = "\\v 1 Umdala\n\\p";
    evaluate (__LINE__, __func__, result, usfm_get_verse_text (usfm, 1));
    result = "\\v 12 NgoDemetriyu\n\\s Isicino\n\\p";
    evaluate (__LINE__, __func__, result, usfm_get_verse_text (usfm, 12));
  }
}


void test_filters_test_usfm2 ()
{
  {
    evaluate (__LINE__, __func__, true, usfm_is_usfm_marker ("\\id"));
    evaluate (__LINE__, __func__, true, usfm_is_usfm_marker ("\\c "));
    evaluate (__LINE__, __func__, false, usfm_is_usfm_marker ("c"));
    evaluate (__LINE__, __func__, true, usfm_is_usfm_marker ("\\add "));
    evaluate (__LINE__, __func__, true, usfm_is_usfm_marker ("\\add*"));
    evaluate (__LINE__, __func__, true, usfm_is_usfm_marker ("\\+add*"));
  }
  {
    evaluate (__LINE__, __func__, true, usfm_is_opening_marker ("\\id"));
    evaluate (__LINE__, __func__, true, usfm_is_opening_marker ("\\c "));
    evaluate (__LINE__, __func__, false, usfm_is_opening_marker ("\\c*"));
    evaluate (__LINE__, __func__, true, usfm_is_opening_marker ("\\+add "));
    evaluate (__LINE__, __func__, false, usfm_is_opening_marker ("\\+add*"));
  }
  {
    evaluate (__LINE__, __func__, false, usfm_is_embedded_marker ("\\add"));
    evaluate (__LINE__, __func__, false, usfm_is_embedded_marker ("\\add*"));
    evaluate (__LINE__, __func__, true, usfm_is_embedded_marker ("\\+add"));
    evaluate (__LINE__, __func__, true, usfm_is_embedded_marker ("\\+add "));
    evaluate (__LINE__, __func__, true, usfm_is_embedded_marker ("\\+add*"));
  }
  {
    evaluate (__LINE__, __func__, "GEN", usfm_get_book_identifier ({ "\\id", "GEN" }, 0));
    evaluate (__LINE__, __func__, "XXX", usfm_get_book_identifier ({ "\\id", "GEN" }, 1));
    evaluate (__LINE__, __func__, "GE", usfm_get_book_identifier ({ "\\id", "GE" }, 0));
    evaluate (__LINE__, __func__, "GEN", usfm_get_book_identifier ({ "\\id", "GENxxx" }, 0));
    evaluate (__LINE__, __func__, "GEN", usfm_get_book_identifier ({ "", "GENxxx" }, 0));
  }
  {
    evaluate (__LINE__, __func__, "1", usfm_peek_verse_number ("1"));
    evaluate (__LINE__, __func__, "1", usfm_peek_verse_number ("1 "));
    evaluate (__LINE__, __func__, "1a", usfm_peek_verse_number ("1a"));
    evaluate (__LINE__, __func__, "2-3", usfm_peek_verse_number ("2-3"));
    evaluate (__LINE__, __func__, "2b,3", usfm_peek_verse_number ("2b,3"));
    evaluate (__LINE__, __func__, "2b,3,", usfm_peek_verse_number ("2b,3, 4"));
    evaluate (__LINE__, __func__, "2a-3b", usfm_peek_verse_number ("2a-3b And he said"));
  }
}


void test_filters_test5 ()
{
  {
    evaluate (__LINE__, __func__, "\\id ", usfm_get_opening_usfm ("id"));
    evaluate (__LINE__, __func__, "\\add ", usfm_get_opening_usfm ("add"));
    evaluate (__LINE__, __func__, "\\add ", usfm_get_opening_usfm ("add", false));
    evaluate (__LINE__, __func__, "\\+add ", usfm_get_opening_usfm ("add", true));
  }
  {
    evaluate (__LINE__, __func__, "\\wj*", usfm_get_closing_usfm ("wj"));
    evaluate (__LINE__, __func__, "\\add*", usfm_get_closing_usfm ("add"));
    evaluate (__LINE__, __func__, "\\add*", usfm_get_closing_usfm ("add", false));
    evaluate (__LINE__, __func__, "\\+add*", usfm_get_closing_usfm ("add", true));
  }
  {
    string usfm = "\\c 1\n\\s Isibingelelo\n\\p\n\\v 1 Umdala\n\\p\n\\v 2 Sithandwa\n\\v 3 Ngoba\n\\v 4 Kangilantokozo\n\\s Inkathazo\n\\p\n\\v 5 Sithandwa\n\\v 6 abafakazele\n\\v 7 Ngoba\n\\v 8 Ngakho\n\\p\n\\v 9 Ngabhalela\n\\v 10 Ngakho\n\\p\n\\v 11 Sithandwa\n\\v 12 NgoDemetriyu\n\\s Isicino\n\\p\n\\v 13 Bengilezinto\n\\v 14 kodwa\n\\p Ukuthula";
    evaluate (__LINE__, __func__, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 }, usfm_get_verse_numbers (usfm));
    usfm = ""
    "\\c 80\n"
    "\\s Umkhuleko wokusizwa kukaIsrayeli\n"
    "\\d Kumqondisi wokuhlabelela. NgeShoshanimi. Ubufakazi. Isihlabelelo sikaAsafi\n"
    "\\p\n"
    "\\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa\\x + Hlab. 81.5.\\x* njengomhlambi\\x + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\\x*, ohlezi \\add phakathi\\add* \\w kwamakherubhi\\w**\\x + Hlab. 99.1. Eks. 25.22.\\x*, khanyisa\\x + Hlab. 50.2.\\x*.\n"
    "\\v 2 Phambi kukaEfrayimi loBhenjamini loManase\\x + Nani 2.18-23.\\x* vusa amandla akho, uze ube lusindiso lwethu\\x + Hlab. 35.23.\\x*.\n"
    "\\p\n"
    "\\v 3 Siphendule, Nkulunkulu\\x + 80.7,14,19. Hlab. 60.1. 85.4. Lilo 5.21.\\x*, wenze ubuso bakho bukhanye, ngakho sizasindiswa\\x + Nani 6.25. Hlab. 4.6.\\x*.\n"
    "\\p\n"
    "\\v 4 \nnd kosi\nd* Nkulunkulu wamabandla\\x + Ps 59.5. 84.8.\\x*, koze kube nini uthukuthelela umkhuleko wabantu bakho\\x + Hlab. 74.10.\\x*?\n"
    "\\v 5 Ubenze badla isinkwa sezinyembezi\\x + Hlab. 42.3. Hlab. 102.9.\\x*, wabanathisa izinyembezi ngesilinganiso\\x + Isa. 40.12.\\x*.\n"
    "\\v 6 Usenza sibe yingxabano kubomakhelwane bethu, lezitha zethu ziyahlekisana \\add ngathi\\x + Hlab. 44.13. 79.4.\\x*\\add*.\n"
    "\\p\n"
    "\\v 7 Siphendule, Nkulunkulu wamabandla, wenze ubuso bakho bukhanye, ngakho sizasindiswa\\x + 80.7,14,19. Hlab. 60.1. 85.4. Lilo 5.21.\\x*.\n"
    "\\p\n"
    "\\v 8 Waliletha ivini livela eGibhithe\\x + Isa. 5.1-7. 27.2. Jer. 2.21. 12.10. Hez. 15.6. 17.6. 19.10. Mat. 21.33. Mark. 12.1. Luka 20.9. Joha. 15.1-6.\\x*, wazixotsha izizwe\\x + Hlab. 78.55.\\x*, walihlanyela lona\\x + Hlab. 44.2.\\x*.\n"
    "\\v 9 Walungisa \\add indawo\\add* phambi kwalo\\x + Gen. 24.31. Josh. 24.12.\\x*, wagxilisa impande zalo, laze lagcwala umhlaba.\n"
    "\\v 10 Izintaba zembeswa ngomthunzi walo, lezingatsha zalo zi\\add njenge\\add*misedari kaNkulunkulu.\n"
    "\\v 11 Lanabisela ingatsha zalo elwandle\\x + Jobe 14.9.\\x*, lamahlumela alo\\x + Jobe 8.16.\\x* emfuleni\\x + Hlab. 72.8.\\x*.\n"
    "\\v 12 Uyibhobozeleni imiduli yalo\\x + Hlab. 89.40,41. Isa. 5.5.\\x*, ukuze balikhe bonke abadlula ngendlela\\x + Hlab. 89.40,41. Isa. 5.5.\\x*?\n"
    "\\v 13 Ingulube yasehlathini iyalihlikiza\\x + Jer. 5.6.\\x*, lenyamazana yeganga iyalidla\\x + Hlab. 50.11.\\x*.\n"
    "\\p\n"
    "\\v 14 Nkulunkulu wamabandla, akubuyele\\x + 80.3.\\x*, ukhangele phansi usemazulwini\\x + Isa. 63.15.\\x*, ubone, wethekelele lelivini\\x + Hlab. 84.9.\\x*,\n"
    "\\v 15 ngitsho isivini isandla sakho sokunene esasihlanyelayo, lendodana\\x + Gen. 49.22.\\x* \\add o\\add*waziqinisela \\add yona\\x + 80.17. Isa.44.14.\\x*\\add*.\n"
    "\\v 16 Sitshisiwe ngomlilo\\x + Isa. 33.12.\\x*, saqunyelwa phansi\\x + Isa. 33.12.\\x*; bayabhubha ngokukhuza kobuso bakho\\x + Hlab. 76.6. Hlab. 39.11.\\x*.\n"
    "\\v 17 Isandla sakho kasibe phezu komuntu\\x + Hlab. 89.21. Luka 1.66.\\x* wesandla sakho sokunene\\x + 80.15.\\x*, phezu kwendodana yomuntu\\x + Hlab. 8.4,5.\\x*, \\add o\\add*ziqinisele \\add yona\\x + 80.17. Isa. 44.14.\\x*\\x + Hlab. 89.21. Luka 1.66.\\x*\\add*.\n"
    "\\v 18 Ngakho kasiyikubuyela emuva sisuke kuwe; sivuselele, khona sizabiza ibizo lakho\\x + Hlab. 71.20.\\x*.\n"
    "\\p\n"
    "\\v 19 \nnd kosi\nd*, Nkulunkulu wamabandla, siphendule, wenze ubuso bakho bukhanye, ngakho sizasindiswa\\x + 80.3,7.\\x*.\n";
    evaluate (__LINE__, __func__, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 }, usfm_get_verse_numbers (usfm));
  }
  {
    string usfm = "\\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa\\x + Hlab. 81.5.\\x* njengomhlambi\\x + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\\x*, ohlezi \\add phakathi\\add* \\w kwamakherubhi\\w**\\x + Hlab. 99.1. Eks. 25.22.\\x*, khanyisa\\x + Hlab. 50.2.\\x*.";
    vector <UsfmNote> xrefs = usfm_extract_notes (usfm, {"x"});
    evaluate (__LINE__, __func__, 4, xrefs.size());
    if (xrefs.size () == 4) {
      evaluate (__LINE__, __func__, 55, xrefs[0].offset);
      evaluate (__LINE__, __func__, "\\x + Hlab. 81.5.\\x*", xrefs[0].data);
      evaluate (__LINE__, __func__, 69, xrefs[1].offset);
      evaluate (__LINE__, __func__, "\\x + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\\x*", xrefs[1].data);
      evaluate (__LINE__, __func__, 117, xrefs[2].offset);
      evaluate (__LINE__, __func__, "\\x + Hlab. 99.1. Eks. 25.22.\\x*", xrefs[2].data);
      evaluate (__LINE__, __func__, 127, xrefs[3].offset);
      evaluate (__LINE__, __func__, "\\x + Hlab. 50.2.\\x*", xrefs[3].data);
    }
  }
  {
    string usfm = "\\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa\\f + Hlab. 81.5.\\f* njengomhlambi\\fe + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\\fe*, ohlezi \\add phakathi\\add* \\w kwamakherubhi\\w**\\x + Hlab. 99.1. Eks. 25.22.\\x*, khanyisa\\x + Hlab. 50.2.\\x*.";
    vector <UsfmNote> notes = usfm_extract_notes (usfm, { "x", "f", "fe" });
    evaluate (__LINE__, __func__, 4, notes.size());
    if (notes.size () == 4) {
      evaluate (__LINE__, __func__, 55, notes[0].offset);
      evaluate (__LINE__, __func__, "\\f + Hlab. 81.5.\\f*", notes[0].data); 
      evaluate (__LINE__, __func__, 69, notes[1].offset);
      evaluate (__LINE__, __func__, "\\fe + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\\fe*", notes[1].data);
      evaluate (__LINE__, __func__, 117, notes[2].offset);
      evaluate (__LINE__, __func__, "\\x + Hlab. 99.1. Eks. 25.22.\\x*", notes[2].data);
      evaluate (__LINE__, __func__, 127, notes[3].offset);
      evaluate (__LINE__, __func__, "\\x + Hlab. 50.2.\\x*", notes[3].data);
    }
  }
  {
    string usfm = "\\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa njengomhlambi\\f + Hlab. 81.5.\\f*\\fe + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\\fe*, ohlezi \\add phakathi\\add* \\w kwamakherubhi\\w**\\x + Hlab. 99.1. Eks. 25.22.\\x*, khanyisa\\x + Hlab. 50.2.\\x*.";
    vector <UsfmNote> notes = usfm_extract_notes (usfm, { "x", "f", "fe" });
    evaluate (__LINE__, __func__, 4, notes.size());
    if (notes.size () == 4) {
      evaluate (__LINE__, __func__, 69, notes[0].offset);
      evaluate (__LINE__, __func__, "\\f + Hlab. 81.5.\\f*", notes[0].data);
      evaluate (__LINE__, __func__, 69, notes[1].offset);
      evaluate (__LINE__, __func__, "\\fe + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\\fe*", notes[1].data);
      evaluate (__LINE__, __func__, 117, notes[2].offset);
      evaluate (__LINE__, __func__, "\\x + Hlab. 99.1. Eks. 25.22.\\x*", notes[2].data);
      evaluate (__LINE__, __func__, 127, notes[3].offset);
      evaluate (__LINE__, __func__, "\\x + Hlab. 50.2.\\x*", notes[3].data);
    }
  }
  {
    string usfm = "\\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa njengomhlambi\\f + Hlab. 81.5.\\f*\\fe + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\\fe*, ohlezi \\add phakathi\\add* \\w kwamakherubhi\\w**\\x + Hlab. 99.1. Eks. 25.22.\\x*, khanyisa\\x + Hlab. 50.2.\\x*.";
    usfm = usfm_remove_notes (usfm, {"x", "f", "fe"});
    string standard = "\\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa njengomhlambi, ohlezi \\add phakathi\\add* \\w kwamakherubhi\\w**, khanyisa.";
    evaluate (__LINE__, __func__, standard, usfm);
  }
}


void test_filters_test6 ()
{
  {
    // Test inserting empty notes
    UsfmNote usfmnote (1, "");
    string usfm = usfm_insert_notes ("", {usfmnote}, 0.9);
    evaluate (__LINE__, __func__, "", usfm);

    usfmnote = UsfmNote (1, "");
    usfm = usfm_insert_notes ("", {usfmnote}, 0.9);
    evaluate (__LINE__, __func__, "", usfm);

    usfmnote = UsfmNote (1, "");
    usfm = usfm_insert_notes ("\\v 1 Text.", {}, 0.9);
    evaluate (__LINE__, __func__, "\\v 1 Text.", usfm);
  }
  {
    // Test regular notes placement.
    string usfm = "\\v 1 Melusi kaIsrayeli\\x + Ps. 1.1\\x*, beka indlebe, okhokhela uJosefa njengomhlambi, ohlezi \\add phakathi\\add* \\w kwamakherubhi\\w**, khanyisa.";
    vector <UsfmNote> notes = {
      {54, "\\x + Hlab. 81.5.\\x*"},
      {69, "\\x + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\\x*"},
      {117, "\\x + Hlab. 99.1. Eks. 25.22.\\x*"},
      {127, "\\x + Hlab. 50.2.\\x*"}
    };
    usfm = usfm_insert_notes (usfm, notes, 0.9);
    string standard = "\\v 1 Melusi kaIsrayeli\\x + Ps. 1.1\\x*, beka indlebe, okhokhela uJosefa\\x + Hlab. 81.5.\\x* njengomhlambi\\x + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\\x*, ohlezi \\add phakathi\\add* \\w kwamakherubhi\\w**\\x + Hlab. 50.2.\\x*\\x + Hlab. 99.1. Eks. 25.22.\\x*, khanyisa.";
    evaluate (__LINE__, __func__, standard, usfm);
  }
  {
    // Test notes placement where non-placed notes are added to the very end of the string.
    string usfm = "\\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa njengomhlambi, ohlezi \\add phakathi\\add* \\w kwamakherubhi\\w**, khanyisa.";
    vector <UsfmNote> notes = {
      {55, "\\x + Hlab. 81.5.\\x*"},
      {69, "\\x + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\\x*"},
      {117, "\\x + Hlab. 99.1. Eks. 25.22.\\x*"},
      {127, "\\x + Hlab. 50.2.\\x*"}
    };
    usfm = usfm_insert_notes (usfm, notes, 1.5);
    string standard = "\\v 1 Melusi kaIsrayeli, beka indlebe, okhokhela uJosefa njengomhlambi, ohlezi \\add phakathi\\add*\\x + Hlab. 81.5.\\x* \\w kwamakherubhi\\w**\\x + Gen. 48.15. 49.24. Hlab. 77.20. Hlab. 95.7.\\x*, khanyisa\\x + Hlab. 50.2.\\x*\\x + Hlab. 99.1. Eks. 25.22.\\x*.";
    evaluate (__LINE__, __func__, standard, usfm);
  }
  {
    // Test moving note.
    string usfm = "\\v 1 Zvino namazuva okutonga kwavatongi\\x + Judg. 2.16.\\x* nzara yakange iripo panyika. Umwe\\x + Judg. 6.4,5,6.\\x* murume weBheterehemu-judha akanogara\\x + Judg. 17.8.\\x* panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.";
    usfm = usfm_move_note (usfm, 1, 1);
    string standard = "\\v 1 Zvino namazuva okutonga kwavatongi nzara\\x + Judg. 2.16.\\x* yakange iripo panyika. Umwe\\x + Judg. 6.4,5,6.\\x* murume weBheterehemu-judha akanogara\\x + Judg. 17.8.\\x* panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.";
    evaluate (__LINE__, __func__, standard, usfm);
  }
  {
    // Test moving note.
    string usfm = "\\v 1 Zvino namazuva okutonga kwavatongi\\x + Judg. 2.16.\\x* nzara yakange iripo panyika. Umwe\\x + Judg. 6.4,5,6.\\x* murume weBheterehemu-judha akanogara\\x + Judg. 17.8.\\x* panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.";
    usfm = usfm_move_note (usfm, -1, 2);
    string standard = "\\v 1 Zvino namazuva okutonga kwavatongi\\x + Judg. 2.16.\\x* nzara yakange iripo panyika\\x + Judg. 6.4,5,6.\\x*. Umwe murume weBheterehemu-judha akanogara\\x + Judg. 17.8.\\x* panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.";
    evaluate (__LINE__, __func__, standard, usfm);
  }
  {
    // Test moving note.
    string usfm = "\\v 1 Zvino namazuva okutonga kwavatongi\\x + Judg. 2.16.\\x* nzara yakange iripo panyika. Umwe\\x + Judg. 6.4,5,6.\\x* murume weBheterehemu-judha akanogara\\x + Judg. 17.8.\\x* panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.";
    usfm = usfm_move_note (usfm, -1, 10);
    string standard = "\\v 1 Zvino namazuva okutonga kwavatongi\\x + Judg. 2.16.\\x* nzara yakange iripo panyika. Umwe\\x + Judg. 6.4,5,6.\\x* murume weBheterehemu-judha akanogara\\x + Judg. 17.8.\\x* panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.";
    evaluate (__LINE__, __func__, standard, usfm);
  }
  {
    // Test getting new note position.
    string usfm = "\\v 1 Zvino namazuva okutonga kwavatongi nzara yakange iripo panyika. Umwe murume weBheterehemu-judha akanogara panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.";
    int position = usfm_get_new_note_position (usfm, 5, 0);
    evaluate (__LINE__, __func__, 10, position);
    position = usfm_get_new_note_position (usfm, 5, 1);
    evaluate (__LINE__, __func__, 10, position);
    position = usfm_get_new_note_position (usfm, 5, -1);
    evaluate (__LINE__, __func__, 4, position);
  }
  {
    // Test getting new note position.
    string usfm = "\\v 1 Zvino namazuva okutonga kwavatongi nzara yakange iripo panyika. Umwe murume weBheterehemu-judha akanogara panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.";
    int position = usfm_get_new_note_position (usfm, 62, 0);
    evaluate (__LINE__, __func__, 67, position);
    position = usfm_get_new_note_position (usfm, 62, 1);
    evaluate (__LINE__, __func__, 67, position);
    position = usfm_get_new_note_position (usfm, 62, -1);
    evaluate (__LINE__, __func__, 59, position);
  }
  {
    // Test getting new note position.
    string usfm = "\\v 1 Zvino namazuva okutonga kwavatongi nzara yakange iripo panyika. Umwe murume weBheterehemu-judha akanogara panyika yaMoabhu, iye nomukadzi wake navanakomana vake vaviri.";
    int position = usfm_get_new_note_position (usfm, 19, 0);
    evaluate (__LINE__, __func__, 19, position);
    position = usfm_get_new_note_position (usfm, 19, 1);
    evaluate (__LINE__, __func__, 28, position);
    position = usfm_get_new_note_position (usfm, 19, -1);
    evaluate (__LINE__, __func__, 10, position);
  }
  {
    // Test getting new note position.
    string usfm = "\\v 1 Zvino.";
    int position = usfm_get_new_note_position (usfm, 19, 0);
    evaluate (__LINE__, __func__, 10, position);
    position = usfm_get_new_note_position (usfm, 19, 1);
    evaluate (__LINE__, __func__, 10, position);
    position = usfm_get_new_note_position (usfm, 19, -1);
    evaluate (__LINE__, __func__, 10, position);
  }
  {
    // Test whether a note is not inserted straight after opening USFM code.
    string usfm = "\\v 1 Zvino namazuva \\add okutonga\\add* kwavatongi nzara yakange iripo panyika.";

    int position = usfm_get_new_note_position (usfm, 20, 0);
    evaluate (__LINE__, __func__, 38, position);

    position = usfm_get_new_note_position (usfm, 20, 1);
    evaluate (__LINE__, __func__, 38, position);

    position = usfm_get_new_note_position (usfm, 31, -1);
    evaluate (__LINE__, __func__, 19, position);
  }
  {
    // It tests whether a note is not inserted within another note.
    string usfm = "\\v 1 Zvino namazuva\\x + Gen.1.1.\\x* okutonga kwavatongi nzara yakange iripo panyika.";

    int position = usfm_get_new_note_position (usfm, 10, 0);
    evaluate (__LINE__, __func__, 10, position);

    position = usfm_get_new_note_position (usfm, 20, 0);
    evaluate (__LINE__, __func__, 21, position);
  }
}


void test_filters_export1 ()
{
  {
    // Test object Text_Text.
    Text_Text text_text = Text_Text ();
    text_text.addtext ("text one");
    evaluate (__LINE__, __func__, "text one", text_text.get ());
  
    text_text = Text_Text ();
    text_text.paragraph ("paragraph1");
    text_text.paragraph ("paragraph2");
    evaluate (__LINE__, __func__, "paragraph1\nparagraph2", text_text.get ());
  
    text_text = Text_Text ();
    text_text.paragraph ("paragraph");
    evaluate (__LINE__, __func__, "paragraph", text_text.line ());
  }
  {
    // Test Esword_Text title.
    Esword_Text esword_text = Esword_Text ("The Word of the Lord Jesus Christ");
    vector <string> sql = {
      {"PRAGMA foreign_keys=OFF;"},
      {"PRAGMA synchronous=OFF;"},
      {"CREATE TABLE Details (Description NVARCHAR(255), Abbreviation NVARCHAR(50), Comments TEXT, Version INT, Font NVARCHAR(50), RightToLeft BOOL, OT BOOL, NT BOOL, Apocrypha BOOL, Strong BOOL);"},
      {"INSERT INTO Details VALUES ('The Word of the Lord Jesus Christ', 'The Word of the Lord Jesus Christ', 'The Word of the Lord Jesus Christ', 1, 'DEFAULT', 0, 1, 1, 0, 0);"},
      {"CREATE TABLE Bible (Book INT, Chapter INT, Verse INT, Scripture TEXT);"}
    };
    evaluate (__LINE__, __func__, sql, esword_text.get_sql ());
  }
  // Test e-Sword text zero reference.
  {
    Esword_Text esword_text = Esword_Text ("");
    esword_text.addText ("The Word of God");
    esword_text.finalize ();
    vector <string> sql = {
      {"PRAGMA foreign_keys=OFF;"},
      {"PRAGMA synchronous=OFF;"},
      {"CREATE TABLE Details (Description NVARCHAR(255), Abbreviation NVARCHAR(50), Comments TEXT, Version INT, Font NVARCHAR(50), RightToLeft BOOL, OT BOOL, NT BOOL, Apocrypha BOOL, Strong BOOL);"},
      {"INSERT INTO Details VALUES ('', '', '', 1, 'DEFAULT', 0, 1, 1, 0, 0);"},
      {"CREATE TABLE Bible (Book INT, Chapter INT, Verse INT, Scripture TEXT);"},
      {"INSERT INTO Bible VALUES (0, 0, 0, 'The Word of God');"},
      {"CREATE INDEX BookChapterVerseIndex ON Bible (Book, Chapter, Verse);"}
    };
    evaluate (__LINE__, __func__, sql, esword_text.get_sql ());
  }
  // Test e-Sword converter John 2:3
  {
    Esword_Text esword_text = Esword_Text ("");
    esword_text.newBook (43);
    esword_text.newChapter (2);
    esword_text.newVerse (3);
    esword_text.addText ("In the beginning was the Word, and the Word was with God, and the Word was God.");
    esword_text.finalize ();
    vector <string> sql = {
      {"PRAGMA foreign_keys=OFF;"},
      {"PRAGMA synchronous=OFF;"},
      {"CREATE TABLE Details (Description NVARCHAR(255), Abbreviation NVARCHAR(50), Comments TEXT, Version INT, Font NVARCHAR(50), RightToLeft BOOL, OT BOOL, NT BOOL, Apocrypha BOOL, Strong BOOL);"},
      {"INSERT INTO Details VALUES ('', '', '', 1, 'DEFAULT', 0, 1, 1, 0, 0);"},
      {"CREATE TABLE Bible (Book INT, Chapter INT, Verse INT, Scripture TEXT);"},
      {"INSERT INTO Bible VALUES (43, 2, 3, 'In the beginning was the Word, and the Word was with God, and the Word was God.');"},
      {"CREATE INDEX BookChapterVerseIndex ON Bible (Book, Chapter, Verse);"}
    };
    evaluate (__LINE__, __func__, sql, esword_text.get_sql ());
  }
  // Test e-Sword converter fragmented text
  {
    Esword_Text esword_text = Esword_Text ("");
    esword_text.newBook (43);
    esword_text.newChapter (1);
    esword_text.newVerse (1);
    esword_text.addText ("In the beginning was the Word");
    esword_text.addText (", and the Word was with God");
    esword_text.addText (", and the Word was God.");
    esword_text.finalize ();
    vector <string> sql = {
      {"PRAGMA foreign_keys=OFF;"},
      {"PRAGMA synchronous=OFF;"},
      {"CREATE TABLE Details (Description NVARCHAR(255), Abbreviation NVARCHAR(50), Comments TEXT, Version INT, Font NVARCHAR(50), RightToLeft BOOL, OT BOOL, NT BOOL, Apocrypha BOOL, Strong BOOL);"},
      {"INSERT INTO Details VALUES ('', '', '', 1, 'DEFAULT', 0, 1, 1, 0, 0);"},
      {"CREATE TABLE Bible (Book INT, Chapter INT, Verse INT, Scripture TEXT);"},
      {"INSERT INTO Bible VALUES (43, 1, 1, 'In the beginning was the Word, and the Word was with God, and the Word was God.');"},
      {"CREATE INDEX BookChapterVerseIndex ON Bible (Book, Chapter, Verse);"}
    };
    evaluate (__LINE__, __func__, sql, esword_text.get_sql ());
  }
}

void test_filters_export2 ()
{
  // Test e-Sword converter switch reference.
  {
    Esword_Text esword_text = Esword_Text ("");
    esword_text.newBook (1);
    esword_text.newChapter (2);
    esword_text.newVerse (3);
    esword_text.addText ("But as many as received him, to them gave he power to become the sons of God, even to them that believe on his name.");
    esword_text.newBook (4);
    esword_text.newChapter (5);
    esword_text.newVerse (6);
    esword_text.addText ("Which were born, not of blood, nor of the will of the flesh, nor of the will of man, but of God.");
    esword_text.finalize ();
    vector <string> sql = {
      {"PRAGMA foreign_keys=OFF;"},
      {"PRAGMA synchronous=OFF;"},
      {"CREATE TABLE Details (Description NVARCHAR(255), Abbreviation NVARCHAR(50), Comments TEXT, Version INT, Font NVARCHAR(50), RightToLeft BOOL, OT BOOL, NT BOOL, Apocrypha BOOL, Strong BOOL);"},
      {"INSERT INTO Details VALUES ('', '', '', 1, 'DEFAULT', 0, 1, 1, 0, 0);"},
      {"CREATE TABLE Bible (Book INT, Chapter INT, Verse INT, Scripture TEXT);"},
      {"INSERT INTO Bible VALUES (1, 2, 3, 'But as many as received him, to them gave he power to become the sons of God, even to them that believe on his name.');"},
      {"INSERT INTO Bible VALUES (4, 5, 6, 'Which were born, not of blood, nor of the will of the flesh, nor of the will of man, but of God.');"},
      {"CREATE INDEX BookChapterVerseIndex ON Bible (Book, Chapter, Verse);"}
    };
    evaluate (__LINE__, __func__, sql, esword_text.get_sql ());
  }
  // Test e-Sword converter create module.
  {
    Esword_Text esword_text = Esword_Text ("");
    esword_text.addText ("In the beginning was the Word, and the Word was with God, and the Word was God.");
    esword_text.finalize ();
    string filename = "/tmp/module.bblx";
    esword_text.createModule (filename);
    int filesize = filter_url_filesize (filename);
    evaluate (__LINE__, __func__, 4096, filesize);
    filter_url_unlink (filename);
  }
  // Test class OnlineBible_Text.
  {
    OnlineBible_Text onlinebible_text = OnlineBible_Text ();
    onlinebible_text.addText ("No verse given, so discard this.");
    onlinebible_text.newVerse (2, 2, 2);
    onlinebible_text.addText ("Text for Exodus 2:2, not verse 2-6a.");
    onlinebible_text.storeData ();
    onlinebible_text.addText ("Verse was stored, no new verse given, so discard this.");
    string filename = "/tmp/OLBTextTest1.exp";
    onlinebible_text.save (filename);
    string standard = filter_url_file_get_contents (filter_url_create_root_path ("unittests", "tests", "onlinebible1.exp"));
    string result = filter_url_file_get_contents (filename);
    evaluate (__LINE__, __func__, standard, result);
    filter_url_unlink (filename);
  }
  {
    OnlineBible_Text onlinebible_text = OnlineBible_Text ();
    onlinebible_text.addNote ();
    onlinebible_text.addText ("Discard this note text because no verse has been given yet.");
    onlinebible_text.closeCurrentNote ();
    onlinebible_text.newVerse (1, 2, 3);
    onlinebible_text.addNote ();
    onlinebible_text.addText ("Output this note text.");
    onlinebible_text.closeCurrentNote ();
    string filename = "/tmp/OLBTextTest2.exp";
    onlinebible_text.save (filename);
    string standard = filter_url_file_get_contents (filter_url_create_root_path ("unittests", "tests", "onlinebible2.exp"));
    string result = filter_url_file_get_contents (filename);
    evaluate (__LINE__, __func__, standard, result);
    filter_url_unlink (filename);
  }
}

void test_filters_test8 ()
{
  // Test Html_Text paragraphs.
  {
    Html_Text html_text = Html_Text ("TestOne");
    html_text.newParagraph ();
    evaluate (__LINE__, __func__, "", html_text.currentParagraphStyle);
    html_text.addText ("Paragraph One");
    evaluate (__LINE__, __func__, "Paragraph One", html_text.currentParagraphContent);
    html_text.newParagraph ();
    evaluate (__LINE__, __func__, "", html_text.currentParagraphContent);
    html_text.addText ("Paragraph Two");
    evaluate (__LINE__, __func__, "Paragraph Two", html_text.currentParagraphContent);
    html_text.newHeading1 ("Heading One");
    evaluate (__LINE__, __func__, "", html_text.currentParagraphContent);
    html_text.newParagraph ();
    html_text.addText ("Paragraph Three");
    string html = html_text.getInnerHtml ();
    string standard =
"    <p>\n"
"      <span>Paragraph One</span>\n"
"    </p>\n"
"    <p>\n"
"      <span>Paragraph Two</span>\n"
"    </p>\n"
"    <h1>Heading One</h1>\n"
"    <p>\n"
"      <span>Paragraph Three</span>\n"
"    </p>\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (html));
  }
  // Test Html_Text automatic paragraph.
  {
    Html_Text html_text = Html_Text ("TestTwo");
    html_text.addText ("Should create new paragraph automatically");
    string html = html_text.getInnerHtml ();
    string standard = 
"    <p>\n"
"      <span>Should create new paragraph automatically</span>\n"
"    </p>\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (html));
  }
  // Test Html_Text basic note
  {
    Html_Text html_text = Html_Text ("TestThree");
    html_text.newParagraph ();
    html_text.addText ("Text1");
    html_text.addNote ("†", "");
    html_text.addNoteText ("Note1.");
    html_text.addText (".");
    string html = html_text.getInnerHtml ();
    string standard = 
    "    <p>\n"
    "      <span>Text1</span>\n"
    "      <a href=\"#note1\" id=\"citation1\" class=\"superscript\">†</a>\n"
    "      <span>.</span>\n"
    "    </p>\n"
    "    <div>\n"
    "      <p class=\"\">\n"
    "        <a href=\"#citation1\" id=\"note1\">†</a>\n"
    "        <span> </span>\n"
    "        <span>Note1.</span>\n"
    "      </p>\n"
    "    </div>\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (html));
  }
  // Test Html_Text getInnerHtml ()
  {
    Html_Text html_text = Html_Text ("test");
    html_text.newParagraph ();
    html_text.addText ("Paragraph One");
    html_text.newParagraph ();
    html_text.addText ("Paragraph Two");
    string html = html_text.getInnerHtml ();
    string standard = 
    "    <p>\n"
    "      <span>Paragraph One</span>\n"
    "    </p>\n"
    "    <p>\n"
    "      <span>Paragraph Two</span>\n"
    "    </p>\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (html));
  }
  // Test Html_Text basic formatted note ()
  {
    Database_Styles_Item style;
    Html_Text html_text = Html_Text ("");
    html_text.newParagraph ();
    html_text.addText ("Text");
    html_text.addNote ("𐌰", "f");
    style.marker = "add";
    html_text.openTextStyle (style, true, false);
    html_text.addNoteText ("Add");
    html_text.closeTextStyle (true, false);
    html_text.addNoteText ("normal");
    html_text.addText (".");
    string html = html_text.getInnerHtml ();
    string standard = 
    "    <p>\n"
    "      <span>Text</span>\n"
    "      <a href=\"#note1\" id=\"citation1\" class=\"superscript\">𐌰</a>\n"
    "      <span>.</span>\n"
    "    </p>\n"
    "    <div>\n"
    "      <p class=\"f\">\n"
    "        <a href=\"#citation1\" id=\"note1\">𐌰</a>\n"
    "        <span> </span>\n"
    "        <span class=\"add\">Add</span>\n"
    "        <span>normal</span>\n"
    "      </p>\n"
    "    </div>\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (html));
  }
  // Test Html_Text embedded formatted note
  {
    Database_Styles_Item style;
    Html_Text html_text = Html_Text ("");
    html_text.newParagraph ();
    html_text.addText ("text");
    html_text.addNote ("𐌰", "f");
    style.marker = "add";
    html_text.openTextStyle (style, true, false);
    html_text.addNoteText ("add");
    style.marker = "nd";
    html_text.openTextStyle (style, true, true);
    html_text.addNoteText ("nd");
    html_text.closeTextStyle (true, false);
    html_text.addNoteText ("normal");
    html_text.addText (".");
    string html = html_text.getInnerHtml ();
    string standard = 
"    <p>\n"
"      <span>text</span>\n"
"      <a href=\"#note1\" id=\"citation1\" class=\"superscript\">𐌰</a>\n"
"      <span>.</span>\n"
"    </p>\n"
"    <div>\n"
"      <p class=\"f\">\n"
"        <a href=\"#citation1\" id=\"note1\">𐌰</a>\n"
"        <span> </span>\n"
"        <span class=\"add\">add</span>\n"
"        <span class=\"add nd\">nd</span>\n"
"        <span>normal</span>\n"
"      </p>\n"
"    </div>\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (html));
  }
}


void test_filters_test9 ()
{
  // Prepare for testing the archive functions.  
  string file1 = "/tmp/testarchive1";
  string file2 = "/tmp/testarchive2";
  string data1;
  string data2;
  for (unsigned int i = 0; i < 1000; i++) {
    data1.append ("Data One\n");
    data2.append ("Data Two\n");
  }
  filter_url_file_put_contents (file1, data1);
  filter_url_file_put_contents (file2, data2);
  {
    // Test zip compression of one file.
    string zipfile = filter_archive_zip_file (file1);
    evaluate (__LINE__, __func__, true, filter_url_file_exists (zipfile));
    evaluate (__LINE__, __func__, 223, filter_url_filesize (zipfile));
    filter_url_unlink (zipfile);
    // Test compressing a non-existing file.
    zipfile = filter_archive_zip_file ("xxxxx");
    evaluate (__LINE__, __func__, "", zipfile);
  }
  // Test Filter_Archive zip folder.
  {
    string folder = filter_url_tempfile();
    filter_url_mkdir (folder);
    filter_url_file_put_contents (folder + "/file1", data1);
    filter_url_file_put_contents (folder + "/file2", data2);
    // Test zip compression.
    string zipfile = filter_archive_zip_folder (folder);
    evaluate (__LINE__, __func__, true, filter_url_file_exists (zipfile));
    evaluate (__LINE__, __func__, 396, filter_url_filesize (zipfile));
    // Clean up the mess.
    filter_url_unlink (zipfile);
    filter_url_rmdir (folder);
  }
  // Test Filter_Archive unzip.
  {
    string zipfile = filter_archive_zip_file (file1);
    // Test unzip.
    string folder = filter_archive_unzip (zipfile);
    evaluate (__LINE__, __func__, true, filter_url_file_exists (zipfile));
    evaluate (__LINE__, __func__, 9000, filter_url_filesize (folder + "/testarchive1"));
    filter_url_unlink (zipfile);
    filter_url_rmdir (folder);
    // Test that unzipping garbage returns NULL.
    folder = filter_archive_unzip ("xxxxx");
    evaluate (__LINE__, __func__, "", folder);
  }
  // Test Filter_Archive tar gzip file.
  {
    // Test gzipped tarball compression.
    string tarball = filter_archive_tar_gzip_file (file1);
    evaluate (__LINE__, __func__, true, filter_url_file_exists (tarball));
    int size = filter_url_filesize (tarball);
    if ((size < 155) || (size > 180)) evaluate (__LINE__, __func__, "between 155 and 180", convert_to_string (size));
    // Clean up tarball from /tmp folder.
    filter_url_unlink (tarball);
    // Test that compressing a non-existing file returns NULL.
    tarball = filter_archive_tar_gzip_file ("xxxxx");
    evaluate (__LINE__, __func__, "", tarball);
  }
  // Test Filter Archive Tar Gzip Folder.
  {
    string folder = filter_url_tempfile ();
    filter_url_mkdir (folder);
    filter_url_file_put_contents (folder + "/file1", data1);
    filter_url_file_put_contents (folder + "/file2", data2);
    // Test compression.
    string tarball = filter_archive_tar_gzip_folder (folder);
    evaluate (__LINE__, __func__, true, filter_url_file_exists (tarball));
    int size = filter_url_filesize (tarball);
    if ((size < 235) || (size > 260)) evaluate (__LINE__, __func__, "between 235 and 260", convert_to_string (size));
    // Clean up.
    filter_url_unlink (tarball);
    filter_url_rmdir (folder);
    // Test that compressing a non-existing folder returns NULL.
    //tarball = filter_archive_tar_gzip_folder (folder + "/x");
    //evaluate (__LINE__, __func__, "", tarball);
  }
  // Test Filter Archive Untargz.
  {
    string tarball = filter_archive_tar_gzip_file (file1);
    // Test decompression.
    string folder = filter_archive_untar_gzip (tarball);
    evaluate (__LINE__, __func__, true, filter_url_file_exists (folder));
    filter_url_rmdir (folder);
    folder = filter_archive_uncompress (tarball);
    evaluate (__LINE__, __func__, true, filter_url_file_exists (folder));
    evaluate (__LINE__, __func__, 9000, filter_url_filesize (folder + "/testarchive1"));
    filter_url_rmdir (folder);
    filter_url_unlink (tarball);
    // Test that unzipping garbage returns NULL.
    folder = filter_archive_untar_gzip ("xxxxx");
    evaluate (__LINE__, __func__, "", folder);
  }
  // Clear up data used for the archive tests.
  refresh_sandbox (false);
  filter_url_unlink (file1);
  filter_url_unlink (file2);
}


void test_filters_test10 ()
{
  string OdfTextTestDotOdt = "/tmp/OdfTextTest.odt";
  string Odt2TxtOutput = "/tmp/Odt2TxtOutput.txt";
  // Test Odf converter paragraphs.
  {
    Odf_Text odf_text = Odf_Text ("phpunit");
    odf_text.createPageBreakStyle ();
    odf_text.newParagraph ();
    evaluate (__LINE__, __func__, "Standard", odf_text.currentParagraphStyle);
    odf_text.addText ("Paragraph One");
    evaluate (__LINE__, __func__, "Paragraph One", odf_text.currentParagraphContent);
    odf_text.newParagraph ();
    evaluate (__LINE__, __func__, "", odf_text.currentParagraphContent);
    odf_text.addText ("Paragraph Two");
    evaluate (__LINE__, __func__, "Paragraph Two", odf_text.currentParagraphContent);
    odf_text.newHeading1 ("Heading One");
    evaluate (__LINE__, __func__, "", odf_text.currentParagraphContent);
    odf_text.newPageBreak ();
    odf_text.newParagraph ();
    odf_text.addText ("Paragraph Three");
    odf_text.save (OdfTextTestDotOdt);
    string command = "odt2txt " + OdfTextTestDotOdt + " > " + Odt2TxtOutput;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (Odt2TxtOutput);
    string standard = ""
    "  Paragraph One\n"
    "\n"
    "  Paragraph Two\n"
    "\n"
    "  Heading One\n"
    "  ===========\n"
    "\n"
    "  Paragraph Three\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (OdfTextTestDotOdt);
  filter_url_unlink (Odt2TxtOutput);
  // Test Automatic Paragraph
  {
    Odf_Text odf_text = Odf_Text ("phpunit");
    odf_text.addText ("Should create new paragraph automatically");
    odf_text.save (OdfTextTestDotOdt);
    string command = "odt2txt " + OdfTextTestDotOdt + " > " + Odt2TxtOutput;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (Odt2TxtOutput);
    string standard = ""
"Should create new paragraph automatically\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (OdfTextTestDotOdt);
  filter_url_unlink (Odt2TxtOutput);
  // Test Basic Note
  {
    Odf_Text odf_text = Odf_Text ("phpunit");
    odf_text.newParagraph ();
    odf_text.addText ("Text");
    odf_text.addNote ("†", "");
    odf_text.addNoteText ("Note");
    odf_text.addText (".");
    odf_text.save (OdfTextTestDotOdt);
    string command = "odt2txt " + OdfTextTestDotOdt + " > " + Odt2TxtOutput;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (Odt2TxtOutput);
    string standard = ""
    "  Text†\n"
    "\n"
    "  Note\n"
    "\n"
    "  .\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (OdfTextTestDotOdt);
  filter_url_unlink (Odt2TxtOutput);
  // TestBasicFormattedText
  {
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();
    Database_Styles_Item add = database_styles.getMarkerData ("Standard", "add");
    Odf_Text odf_text = Odf_Text ("phpunit");
    odf_text.newParagraph ();
    odf_text.addText ("text");
    odf_text.openTextStyle (add, false, false);
    odf_text.addText ("add");
    odf_text.closeTextStyle (false, false);
    odf_text.addText ("normal");
    odf_text.addText (".");
    odf_text.save (OdfTextTestDotOdt);
    string command = "odt2txt " + OdfTextTestDotOdt + " > " + Odt2TxtOutput;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (Odt2TxtOutput);
    string standard = "textaddnormal.";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (OdfTextTestDotOdt);
  filter_url_unlink (Odt2TxtOutput);
  // Test Basic Formatted Note
  {
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();
    Database_Styles_Item add = database_styles.getMarkerData ("Standard", "add");
    Odf_Text odf_text = Odf_Text ("phpunit");
    odf_text.newParagraph ();
    odf_text.addText ("Text");
    odf_text.addNote ("𐌰", "f");
    odf_text.openTextStyle (add, true, false);
    odf_text.addNoteText ("Add");
    odf_text.closeTextStyle (true, false);
    odf_text.addNoteText ("normal");
    odf_text.addText (".");
    odf_text.save (OdfTextTestDotOdt);
    string command = "odt2txt " + OdfTextTestDotOdt + " > " + Odt2TxtOutput;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (Odt2TxtOutput);
    string standard = ""
      "  Text𐌰\n"
      "\n"
      "  Addnormal\n"
      "\n"
      "  .\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (OdfTextTestDotOdt);
  filter_url_unlink (Odt2TxtOutput);
  // Test Embedded Formatted Text.
  {
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();
    Database_Styles_Item add = database_styles.getMarkerData ("Standard", "add");
    add.italic = ooitOn;
    add.bold = 0;
    add.underline = 0;
    add.smallcaps = 0;
    add.superscript = false;
    add.color = "#000000";
    Database_Styles_Item nd = database_styles.getMarkerData ("Standard", "nd");
    nd.italic = 0;
    nd.bold = 0;
    nd.underline = 0;
    nd.smallcaps = ooitOn;
    nd.superscript = false;
    nd.color = "#000000";
    Odf_Text odf_text = Odf_Text ("phpunit");
    odf_text.newParagraph ();
    odf_text.addText ("text");
    odf_text.openTextStyle (add, false, false);
    odf_text.addText ("add");
    odf_text.openTextStyle (nd, false, true);
    odf_text.addText ("nd");
    odf_text.closeTextStyle (false, false);
    odf_text.addText ("normal");
    odf_text.addText (".");
    odf_text.save (OdfTextTestDotOdt);
    string command = "odt2txt " + OdfTextTestDotOdt + " > " + Odt2TxtOutput;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (Odt2TxtOutput);
    string standard = "textaddndnormal.";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (OdfTextTestDotOdt);
  filter_url_unlink (Odt2TxtOutput);
  // Test Embedded Formatted Note.
  {
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();
    Database_Styles_Item add = database_styles.getMarkerData ("Standard", "add");
    add.italic = ooitOn;
    add.bold = 0;
    add.underline = 0;
    add.smallcaps = 0;
    add.superscript = false;
    add.color = "#000000";
    Database_Styles_Item nd = database_styles.getMarkerData ("Standard", "nd");
    nd.italic = 0;
    nd.bold = 0;
    nd.underline = 0;
    nd.smallcaps = ooitOn;
    nd.superscript = false;
    nd.color = "#000000";
    Odf_Text odf_text = Odf_Text ("phpunit");
    odf_text.newParagraph ();
    odf_text.addText ("text");
    odf_text.addNote ("𐌰", "f");
    odf_text.openTextStyle (add, true, false);
    odf_text.addNoteText ("add");
    odf_text.openTextStyle (nd, true, true);
    odf_text.addNoteText ("nd");
    odf_text.closeTextStyle (true, false);
    odf_text.addNoteText ("normal");
    odf_text.addText (".");
    odf_text.save (OdfTextTestDotOdt);
    string command = "odt2txt " + OdfTextTestDotOdt + " > " + Odt2TxtOutput;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (Odt2TxtOutput);
    string standard = ""
    "  text𐌰\n"
    "\n"
    "  addndnormal\n"
    "\n"
    "  .\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (OdfTextTestDotOdt);
  filter_url_unlink (Odt2TxtOutput);
  // Test paragraph formatting.
  {
    Database_Styles database_styles = Database_Styles ();
    database_styles.create ();
    Database_Styles_Item d = database_styles.getMarkerData ("Standard", "d");
    Odf_Text odf_text = Odf_Text ("phpunit");
    odf_text.createParagraphStyle (d.marker, d.fontsize, d.italic, d.bold, d.underline, d.smallcaps, d.justification, d.spacebefore, d.spaceafter, d.leftmargin, d.rightmargin, d.firstlineindent, true, false);
    odf_text.newParagraph ("d");
    odf_text.addText ("Paragraph with d style");
    odf_text.newParagraph ("d");
    odf_text.addText ("Paragraph with d style at first, then Standard");
    odf_text.updateCurrentParagraphStyle ("Standard");
    odf_text.save (OdfTextTestDotOdt);
    string command = "odt2txt " + OdfTextTestDotOdt + " > " + Odt2TxtOutput;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (Odt2TxtOutput);
    string standard = ""
    "  Paragraph with d style\n"
    "\n"
    "  Paragraph with d style at first, then Standard\n"
    "";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (OdfTextTestDotOdt);
  filter_url_unlink (Odt2TxtOutput);
}


void test_filters_test11 ()
{
  string TextTestOdt  = "/tmp/TextTest.odt";
  string TextTestHtml = "/tmp/TextTest.html";
  string TextTestTxt  = "/tmp/TextTest.txt";
  string bible = "phpunit";

  // The unittests depend on known settings and values.
  Database_Config_Bible::setExportChapterDropCapsFrames (bible, true);
  Database_Styles database_styles = Database_Styles ();
  database_styles.create ();

  // Test extraction of all sorts of information from USFM code
  // Test basic formatting into OpenDocument.
  {
    string usfm = ""
      "\\id GEN\n"
      "\\h Header\n"
      "\\h1 Header1\n"
      "\\h2 Header2\n"
      "\\h3 Header3\n"
      "\\toc1 The Book of Genesis\n"
      "\\toc2 Genesis\n"
      "\\toc3 Gen\n"
      "\\cl Chapter\n"
      "\\c 1\n"
      "\\cp Ⅰ\n"
      "\\p\n"
      "\\v 1 Text chapter 1\n"
      "\\c 2\n"
      "\\cp ②\n"
      "\\h Header4\n"
      "\\p\n"
      "\\v 2 Text chapter 2\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.odf_text_standard = new Odf_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    // Check that it finds the running headers.
    int desiredRunningHeaders = 5;
    int actualRunningHeaders = filter_text.runningHeaders.size();
    evaluate (__LINE__, __func__, desiredRunningHeaders, actualRunningHeaders);
    if (actualRunningHeaders == desiredRunningHeaders) {
      evaluate (__LINE__, __func__, 1, filter_text.runningHeaders[0].book);
      evaluate (__LINE__, __func__, 0, filter_text.runningHeaders[0].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.runningHeaders[0].verse);
      evaluate (__LINE__, __func__, "h", filter_text.runningHeaders[0].marker);
      evaluate (__LINE__, __func__, "Header", filter_text.runningHeaders[0].value);
      evaluate (__LINE__, __func__, 1, filter_text.runningHeaders[1].book);
      evaluate (__LINE__, __func__, 0, filter_text.runningHeaders[1].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.runningHeaders[1].verse);
      evaluate (__LINE__, __func__, "h1", filter_text.runningHeaders[1].marker);
      evaluate (__LINE__, __func__, "Header1", filter_text.runningHeaders[1].value);
      evaluate (__LINE__, __func__, 1, filter_text.runningHeaders[2].book);
      evaluate (__LINE__, __func__, 0, filter_text.runningHeaders[2].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.runningHeaders[2].verse);
      evaluate (__LINE__, __func__, "h2", filter_text.runningHeaders[2].marker);
      evaluate (__LINE__, __func__, "Header2", filter_text.runningHeaders[2].value);
      evaluate (__LINE__, __func__, 1, filter_text.runningHeaders[3].book);
      evaluate (__LINE__, __func__, 0, filter_text.runningHeaders[3].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.runningHeaders[3].verse);
      evaluate (__LINE__, __func__, "h3", filter_text.runningHeaders[3].marker);
      evaluate (__LINE__, __func__, "Header3", filter_text.runningHeaders[3].value);
      evaluate (__LINE__, __func__, 1, filter_text.runningHeaders[4].book);
      evaluate (__LINE__, __func__, 2, filter_text.runningHeaders[4].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.runningHeaders[4].verse);
      evaluate (__LINE__, __func__, "h", filter_text.runningHeaders[4].marker);
      evaluate (__LINE__, __func__, "Header4", filter_text.runningHeaders[4].value);
    }
    // Check on Table of Contents items.
    int desiredlongTOCs = 1;
    int actuallongTOCs = filter_text.longTOCs.size();
    evaluate (__LINE__, __func__, desiredlongTOCs, actuallongTOCs);
    if (desiredlongTOCs == actuallongTOCs) {
      evaluate (__LINE__, __func__, 1, filter_text.longTOCs[0].book);
      evaluate (__LINE__, __func__, 0, filter_text.longTOCs[0].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.longTOCs[0].verse);
      evaluate (__LINE__, __func__, "toc1", filter_text.longTOCs[0].marker);
      evaluate (__LINE__, __func__, "The Book of Genesis", filter_text.longTOCs[0].value);
    }
    int desiredshortTOCs = 1;
    int actualshortTOCs = filter_text.shortTOCs.size();
    evaluate (__LINE__, __func__, desiredshortTOCs, actualshortTOCs);
    if (desiredlongTOCs == actuallongTOCs) {
      evaluate (__LINE__, __func__, 1, filter_text.shortTOCs[0].book);
      evaluate (__LINE__, __func__, 0, filter_text.shortTOCs[0].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.shortTOCs[0].verse);
      evaluate (__LINE__, __func__, "toc2", filter_text.shortTOCs[0].marker);
      evaluate (__LINE__, __func__, "Genesis", filter_text.shortTOCs[0].value);
    }
    // Check book abbreviation.
    int desiredbookAbbreviations = 1;
    int actualbookAbbreviations = filter_text.bookAbbreviations.size();
    evaluate (__LINE__, __func__, desiredbookAbbreviations, actualbookAbbreviations);
    if (desiredlongTOCs == actuallongTOCs) {
      evaluate (__LINE__, __func__, 1, filter_text.bookAbbreviations[0].book);
      evaluate (__LINE__, __func__, 0, filter_text.bookAbbreviations[0].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.bookAbbreviations[0].verse);
      evaluate (__LINE__, __func__, "toc3", filter_text.bookAbbreviations[0].marker);
      evaluate (__LINE__, __func__, "Gen", filter_text.bookAbbreviations[0].value);
    }
    // Check chapter specials.
    int desiredchapterLabels = 1;
    int actualchapterLabels = filter_text.chapterLabels.size();
    evaluate (__LINE__, __func__, desiredchapterLabels, actualchapterLabels);
    if (desiredlongTOCs == actuallongTOCs) {
      evaluate (__LINE__, __func__, 1, filter_text.chapterLabels[0].book);
      evaluate (__LINE__, __func__, 0, filter_text.chapterLabels[0].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.chapterLabels[0].verse);
      evaluate (__LINE__, __func__, "cl", filter_text.chapterLabels[0].marker);
      evaluate (__LINE__, __func__, "Chapter", filter_text.chapterLabels[0].value);
    }
    int desiredpublishedChapterMarkers = 2;
    int actualpublishedChapterMarkers = filter_text.publishedChapterMarkers.size();
    evaluate (__LINE__, __func__, desiredpublishedChapterMarkers, actualpublishedChapterMarkers);
    if (desiredlongTOCs == actuallongTOCs) {
      evaluate (__LINE__, __func__, 1, filter_text.publishedChapterMarkers[0].book);
      evaluate (__LINE__, __func__, 1, filter_text.publishedChapterMarkers[0].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.publishedChapterMarkers[0].verse);
      evaluate (__LINE__, __func__, "cp", filter_text.publishedChapterMarkers[0].marker);
      evaluate (__LINE__, __func__, "Ⅰ", filter_text.publishedChapterMarkers[0].value);
      evaluate (__LINE__, __func__, 1, filter_text.publishedChapterMarkers[1].book);
      evaluate (__LINE__, __func__, 2, filter_text.publishedChapterMarkers[1].chapter);
      evaluate (__LINE__, __func__, "0", filter_text.publishedChapterMarkers[1].verse);
      evaluate (__LINE__, __func__, "cp", filter_text.publishedChapterMarkers[1].marker);
      evaluate (__LINE__, __func__, "②", filter_text.publishedChapterMarkers[1].value);
    }
    // OpenDocument output.
    filter_text.odf_text_standard->save (TextTestOdt);
    string command = "odt2txt " + TextTestOdt + " > " + TextTestTxt;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (TextTestTxt);
    odt = filter_string_str_replace ("  ", "", odt);
    string standard = ""
    "\n"
    "Header4 Ⅰ\n"
    "=========\n"
    "\n"
    "[-- Image: frame1 --]\n"
    "\n"
    "Ⅰ\n"
    "\n"
    "Text chapter 1\n"
    "\n"
    "Header4 ②\n"
    "=========\n"
    "\n"
    "[-- Image: frame2 --]\n"
    "\n"
    "②\n"
    "\n"
    "Text chapter 2\n"
    "\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (TextTestOdt);
  filter_url_unlink (TextTestHtml);
  // There are two books here. This normally gives one new page between these two books.
  // Test that basic USFM code gets transformed correctly.
  {
    string usfm =
      "\\id GEN\n"
      "\\ide XYZ\n"
      "\\c 1\n"
      "\\p Text Genesis 1\n"
      "\\c 2\n"
      "\\p Text Genesis 2\n"
      "\\id MAT\n"
      "\\c 1\n"
      "\\p Text Matthew 1\n"
      "\\c 2\n"
      "\\p Text Matthew 2\n"
      "\\rem Comment\n"
      "\\xxx Unknown markup\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.odf_text_standard = new Odf_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    filter_text.odf_text_standard->save (TextTestOdt);
    string command = "odt2txt " + TextTestOdt + " > " + TextTestTxt;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (TextTestTxt);
    odt = filter_string_str_replace ("  ", "", odt);
    string standard = ""
      "\n"
      "Genesis 1\n"
      "=========\n"
      "\n"
      "Text Genesis 1\n"
      "\n"
      "Genesis 2\n"
      "=========\n"
      "\n"
      "Text Genesis 2\n"
      "\n"
      "Matthew 1\n"
      "=========\n"
      "\n"
      "Text Matthew 1\n"
      "\n"
      "Matthew 2\n"
      "=========\n"
      "\n"
      "Text Matthew 2\n"
      "\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
    evaluate (__LINE__, __func__, {"Genesis 0:0 Text encoding indicator not supported. Encoding is always in UTF8: \\ide XYZ", "Matthew 2:0 Unknown marker \\xxx, formatting error: Unknown markup"}, filter_text.fallout);
    evaluate (__LINE__, __func__, {"Matthew 2:0 Comment: \\rem Comment"}, filter_text.info);
  }
  // Test transformation of verse numbers and text following.
  {
    string usfm = 
      "\\id GEN\n"
      "\\v 1 Verse One.\n"
      "\\p Paragraph One.\n"
      "\\v 2 Verse Two.\n"
      "\\p\n"
      "\\v 3 Verse Three.\n"
      "\\v 4 Verse Four.\n"
      "\\v 5 Verse Five.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.odf_text_standard = new Odf_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    filter_text.odf_text_standard->save (TextTestOdt);
    string command = "odt2txt " + TextTestOdt + " > " + TextTestTxt;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (TextTestTxt);
    odt = filter_string_str_replace ("  ", "", odt);
    string standard = ""
      "\n"
      "1" + get_en_space () + "Verse One.\n"
      "\n"
      "Paragraph One. 2" + get_en_space () + "Verse Two.\n"
      "\n"
      "3" + get_en_space () + "Verse Three. 4" + get_en_space () + "Verse Four. 5" + get_en_space () + "Verse Five.\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  // Test footnotes and cross references.
  {
    string usfm = 
      "\\id GEN\n"
      "\\v 1 Text 1\\x + \\xt Isa. 1.1.\\x*\\x - \\xt Isa. 2.2.\\x*\\x + \\xt Isa. 3.3.\\x*, text 2\\f + \\fk Word1: \\fl Heb. \\fq Explanation1.\\f*\\f + \\fk Word2: \\fl Heb. \\fq Explanation2.\\f*, text3.\\f + \\fk Test: \\fl Heb. \\fq Note at the very end.\\f*\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.odf_text_standard = new Odf_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    filter_text.odf_text_standard->save (TextTestOdt);
    string command = "odt2txt " + TextTestOdt + " > " + TextTestTxt;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (TextTestTxt);
    odt = filter_string_str_replace ("  ", "", odt);
    string standard = ""
      "\n"
      "1" + get_en_space () + "Text 1a\n"
      "\n"
      "Isa. 1.1.\n"
      "\n"
      "Isa. 2.2.\n"
      "\n"
      "b\n"
      "\n"
      "Isa. 3.3.\n"
      "\n"
      ", text 21\n"
      "\n"
      "Word1: Heb. Explanation1.\n"
      "\n"
      "2\n"
      "\n"
      "Word2: Heb. Explanation2.\n"
      "\n"
      ", text3.3\n"
      "\n"
      "Test: Heb. Note at the very end.\n"
      "\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (TextTestOdt);
  filter_url_unlink (TextTestHtml);
  filter_url_unlink (TextTestTxt);
}


void test_filters_test12 ()
{
  Database_Styles database_styles = Database_Styles ();
  database_styles.create ();
  string TextTestOdt  = "/tmp/TextTest.odt";
  string TextTestHtml = "/tmp/TextTest.html";
  string TextTestTxt  = "/tmp/TextTest.txt";
  string bible = "phpunit";
  // Test Clear Text Export 1
  {
    string usfm = 
      "\\id GEN\n"
      "\\h Genesis\n"
      "\\toc2 Genesis\n"
      "\\mt2 The book of\n"
      "\\mt Genesis\n"
      "\\c 1\n"
      "\\p\n"
      "\\v 1 In the be\\x + \\xt Isa. 1.1.\\x*\\x - \\xt Isa. 2.2.\\x*ginning, God created\\f + \\fk Word1: \\fl Heb. \\fq Explanation1.\\f*\\f + \\fk Word2: \\fl Heb. \\fq Explanation2.\\f* the heavens and the earth.\\f + \\fk Test: \\fl Heb. \\fq Note at the very end.\\f*\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.text_text = new Text_Text ();
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    string output = filter_text.text_text->get ();
    string standard = 
      "The book of\n"
      "Genesis\n"
      "1\n"
      "1 In the beginning, God created the heavens and the earth.\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (output));
  }
  // Test Clear Text Export 2
  {
    string usfm = 
      "\\id GEN\n"
      "\\c 1\n"
      "\\p\n"
      "\\v 1 Chapter 1, verse one.\n"
      "\\v 2 Verse two.\n"
      "\\c 2\n"
      "\\p\n"
      "\\v 1 Chapter 2, verse one.\n"
      "\\v 2 Verse two.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.text_text = new Text_Text ();
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    string output = filter_text.text_text->get ();
    string standard = 
      "1\n"
      "1 Chapter 1, verse one. 2 Verse two.\n"
      "2\n"
      "1 Chapter 2, verse one. 2 Verse two.\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (output));
  }
  // Test Verses Headings 1
  {
    string usfm =
      "\\id GEN\n"
      "\\c 1\n"
      "\\p\n"
      "\\v 1 Verse one.\n"
      "\\v 2 Verse two.\n"
      "\\s Heading one\n"
      "\\p\n"
      "\\v 3 Verse three\n"
      "\\p\n"
      "\\s Heading two\n"
      "\\p\n"
      "\\v 4 Verse four.\n"
      "\\v 5 Verse five.\n"
      "\\c 2\n"
      "\\s Heading three\n"
      "\\p\n"
      "\\v 1 Verse one.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.initializeHeadingsAndTextPerVerse ();
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    map <int, string> output = filter_text.verses_headings;
    map <int, string> standard = { {0, "Heading three"}, {2, "Heading one"}, {3, "Heading two"} };
    evaluate (__LINE__, __func__, standard, output);
  }
  // Test Verses Headings 2
  {
    string usfm = 
      "\\id GEN\n"
      "\\c 1\n"
      "\\p\n"
      "\\v 1 Verse one.\n"
      "\\s \\s Usuku lweN\\nd kosi\\nd* luyeza masinyane\n"
      "\\p\n"
      "\\v 2 Verse two\n"
      "\\p\n"
      "\\s Heading \\add two\\add*\n"
      "\\p\n"
      "\\v 3 Verse three\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.initializeHeadingsAndTextPerVerse ();
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    map <int, string> output = filter_text.verses_headings;
    map <int, string> standard = { {1, "Usuku lweNkosi luyeza masinyane"}, {2, "Heading two"} };
    evaluate (__LINE__, __func__, standard, output);
  }
  // Test Verses Text 1
  {
    string usfm = 
      "\\id GEN\n"
      "\\c 1\n"
      "\\p\n"
      "\\v 2 Verse\\x + \\xt Isa. 1.1.\\x* two.\n"
      "\\v 3 Verse three\\x + \\xt Isa. 1.1.\\x*.\n"
      "\\s Heading one\n"
      "\\p\n"
      "\\v 4 Verse four.\n"
      "\\p\n"
      "\\s Heading two\n"
      "\\p\n"
      "\\v 5 Verse five.\n"
      "\\v 6 Verse six.\n"
      "\\c 2\n"
      "\\s Heading three\n"
      "\\p\n"
      "\\v 1 Verse one\\x + \\xt Isa. 1.1.\\x*.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.initializeHeadingsAndTextPerVerse ();
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    map <int, string> output = filter_text.getVersesText ();
    map <int, string> standard = {
      {1, "Verse one."},
      {2, "Verse two."},
      {3, "Verse three."},
      {4, "Verse four."},
      {5, "Verse five."},
      {6, "Verse six."},
    };
    evaluate (__LINE__, __func__, standard, output);
  }
  // Test Verses Text 2
  {
    string usfm = 
      "\\c 15\n"
      "\\s Heading\n"
      "\\p\n"
      "\\v 1 He said:\n"
      "\\p I will sing to the Lord.\n"
      "\\v 2 The Lord is my strength.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.initializeHeadingsAndTextPerVerse ();
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    map <int, string> output = filter_text.getVersesText ();
    map <int, string> standard = {
      {1, "He said: I will sing to the Lord."},
      {2, "The Lord is my strength." }
    };
    evaluate (__LINE__, __func__, standard, output);
  }
  // Test Paragraph Positions 1
  {
    string usfm = 
      "\\c 1\n"
      "\\s Heading\n"
      "\\p\n"
      "\\v 1 He said:\n"
      "\\p I will sing to the Lord.\n"
      "\\v 2 The Lord is my strength.\n"
      "\\p I trust in Him.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.initializeHeadingsAndTextPerVerse ();
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    vector <int> output = filter_text.paragraph_start_positions;
    vector <int> standard = {0, 9, 58};
    evaluate (__LINE__, __func__, standard, output);
  }
  // Test Embedded Text 1
  {
    string usfm = 
      "\\c 1\n"
      "\\p\n"
      "\\v 1 He said: I will sing \\add to the \\+nd Lord\\+nd*\\add*.\n"
      "\\v 2 The \\nd Lord\\nd* is my strength.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.initializeHeadingsAndTextPerVerse ();
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    map <int, string> output = filter_text.getVersesText ();
    map <int, string> standard = {
      {1, "He said: I will sing to the Lord."},
      {2, "The Lord is my strength."}
    };
    evaluate (__LINE__, __func__, standard, output);
  }
  // Test Embedded Html One
  {
    // Open character style, and embedded character style, and close both normally.
    string usfm =
      "\\c 1\n"
      "\\p\n"
      "\\v 1 I will sing \\add to the \\+nd Lord\\+nd*\\add*.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.html_text_standard = new Html_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    string html = filter_text.html_text_standard->getInnerHtml ();
    string standard = 
      "    <p class=\"p\">\n"
      "      <span class=\"v\">1</span>\n"
      "      <span> I will sing </span>\n"
      "      <span class=\"add\">to the </span>\n"
      "      <span class=\"add nd\">Lord</span>\n"
      "      <span>.</span>\n"
      "    </p>\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (html));
  }
  // Test Embedded Html Two
  {
    // Open character style, open embedded character style, close embedded one, then close the outer one.
    string usfm = 
      "\\c 1\n"
      "\\p\n"
      "\\v 1 I will sing \\add to the \\+nd Lord\\+nd* God\\add*.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.html_text_standard = new Html_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    string html = filter_text.html_text_standard->getInnerHtml ();
    string standard = 
      "    <p class=\"p\">\n"
      "      <span class=\"v\">1</span>\n"
      "      <span> I will sing </span>\n"
      "      <span class=\"add\">to the </span>\n"
      "      <span class=\"add nd\">Lord</span>\n"
      "      <span class=\"add\"> God</span>\n"
      "      <span>.</span>\n"
      "    </p>\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (html));
  }
  // Test Embedded Html Three.
  {
    // Open character style, open embedded character style, then closing the outer one closes the embedded one also.
    string usfm = 
      "\\c 1\n"
      "\\p\n"
      "\\v 1 I will sing \\add to the \\+nd Lord\\add*.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.html_text_standard = new Html_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    string html = filter_text.html_text_standard->getInnerHtml ();
    string standard = 
      "    <p class=\"p\">\n"
      "      <span class=\"v\">1</span>\n"
      "      <span> I will sing </span>\n"
      "      <span class=\"add\">to the </span>\n"
      "      <span class=\"add nd\">Lord</span>\n"
      "      <span>.</span>\n"
      "    </p>\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (html));
  }
  // Test Embedded Open Document One
  {
    // Open character style, and embedded character style, and close both normally.
    string usfm = 
      "\\id GEN\n"
      "\\c 1\n"
      "\\p\n"
      "\\v 1 I will sing \\add to the \\+nd Lord\\+nd*\\add*.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.odf_text_standard = new Odf_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    filter_text.odf_text_standard->save (TextTestOdt);
    string command = "odt2txt " + TextTestOdt + " > " + TextTestTxt;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (TextTestTxt);
    odt = filter_string_str_replace ("  ", "", odt);
    string standard = ""
      "Genesis 1\n"
      "=========\n"
      "\n"
      "1 I will sing to the Lord.\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  // Exercise bits in document to generate text and note citations.
  {
    string usfm = 
      "\\id GEN\n"
      "\\v 1 Text 1\\x + \\xt Isa. 1.1.\\x* text\\f + \\fk Word: \\fl Heb. \\fq Explanation1.\\f* text\\fe + \\fk Word: \\fl Heb. \\fq Explanation1.\\fe*.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.odf_text_text_and_note_citations = new Odf_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    filter_text.odf_text_text_and_note_citations->save (TextTestOdt);
    string command = "odt2txt " + TextTestOdt + " > " + TextTestTxt;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (TextTestTxt);
    string standard = ""
      "1 Text 1a text1 text1.";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  // TestEmbeddedOpenDocumentTwo
  {
    // Open character style, open embedded character style, close embedded one, then close the outer one.
    string usfm = 
      "\\id GEN\n"
      "\\c 1\n"
      "\\p\n"
      "\\v 1 I will sing \\add to the \\+nd Lord\\+nd* God\\add*.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.odf_text_standard = new Odf_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    filter_text.odf_text_standard->save (TextTestOdt);
    string command = "odt2txt " + TextTestOdt + " > " + TextTestTxt;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (TextTestTxt);
    odt = filter_string_str_replace ("  ", "", odt);
    string standard = ""
      "Genesis 1\n"
      "=========\n"
      "\n"
      "1 I will sing to the Lord God.\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  // Test EmbeddedOpenDocumentThree
  {
    // Open character style, open embedded character style, then closing the outer one closes the embedded one also.
    string usfm = 
      "\\id GEN\n"
      "\\c 1\n"
      "\\p\n"
      "\\v 1 I will sing \\add to the \\+nd Lord\\add*.\n";
    Filter_Text filter_text = Filter_Text (bible);
    filter_text.odf_text_standard = new Odf_Text (bible);
    filter_text.addUsfmCode (usfm);
    filter_text.run ("Standard");
    filter_text.odf_text_standard->save (TextTestOdt);
    string command = "odt2txt " + TextTestOdt + " > " + TextTestTxt;
    int ret = system (command.c_str());
    string odt;
    if (ret == 0) odt = filter_url_file_get_contents (TextTestTxt);
    odt = filter_string_str_replace ("  ", "", odt);
    string standard = ""
      "Genesis 1\n"
      "=========\n"
      "\n"
      "1 I will sing to the Lord.\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (odt));
  }
  filter_url_unlink (TextTestOdt);
  filter_url_unlink (TextTestHtml);
  filter_url_unlink (TextTestTxt);
}


void test_filters_test13 () // Todo
{
  // Unicode tests.
  evaluate (__LINE__, __func__, 4, unicode_string_length ("test"));
  evaluate (__LINE__, __func__, 4, unicode_string_length ("ᨁᨃᨅᨕ"));

  string hebrew = "אָבּגּדּהּ";
  evaluate (__LINE__, __func__, "st1234", unicode_string_substr ("test1234", 2));
  evaluate (__LINE__, __func__, "גּדּהּ", unicode_string_substr (hebrew, 2));
  evaluate (__LINE__, __func__, "", unicode_string_substr (hebrew, 5));
  evaluate (__LINE__, __func__, "", unicode_string_substr (hebrew, 6));
  evaluate (__LINE__, __func__, "test", unicode_string_substr ("test123456", 0, 4));
  evaluate (__LINE__, __func__, "12", unicode_string_substr ("test123456", 4, 2));
  evaluate (__LINE__, __func__, "גּדּ", unicode_string_substr (hebrew, 2, 2));
  evaluate (__LINE__, __func__, "גּדּהּ", unicode_string_substr (hebrew, 2, 10));

  string needle = "דּ";
  evaluate (__LINE__, __func__, 3, unicode_string_strpos ("012345", "3"));
  evaluate (__LINE__, __func__, 5, unicode_string_strpos ("012345", "5"));
  evaluate (__LINE__, __func__, 0, unicode_string_strpos ("012345", "0"));
  evaluate (__LINE__, __func__, -1, unicode_string_strpos ("012345", "6"));
  evaluate (__LINE__, __func__, 3, unicode_string_strpos (hebrew, needle));
  evaluate (__LINE__, __func__, 3, unicode_string_strpos (hebrew, needle, 3));
  evaluate (__LINE__, __func__, -1, unicode_string_strpos (hebrew, needle, 4));
  evaluate (__LINE__, __func__, -1, unicode_string_strpos ("", "3"));
  
  evaluate (__LINE__, __func__, 2, unicode_string_strpos_case_insensitive ("AbCdEf", "c"));
  evaluate (__LINE__, __func__, 2, unicode_string_strpos_case_insensitive ("AbCdEf", "cD"));
  evaluate (__LINE__, __func__, -1, unicode_string_strpos_case_insensitive ("AbCdEf", "ce"));

  evaluate (__LINE__, __func__, "test1234", unicode_string_casefold ("test1234"));
  evaluate (__LINE__, __func__, "test1234", unicode_string_casefold ("TEST1234"));
  
  // Test unique filename.
  string filename = "/tmp/unique";
  filter_url_file_put_contents (filename, "");
  string filename1 = filter_url_unique_path (filename);
  filter_url_file_put_contents (filename1, "");
  evaluate (__LINE__, __func__, "/tmp/unique.1", filename1);
  string filename2 = filter_url_unique_path (filename);
  filter_url_file_put_contents (filename2, "");
  evaluate (__LINE__, __func__, "/tmp/unique.2", filename2);
  filter_url_unlink (filename);
  filter_url_unlink (filename1);
  filter_url_unlink (filename2);
}


void test_filters_test14 ()
{
  int floor = 100000;
  int ceiling = 999999;
  int r1 = filter_string_rand (floor, ceiling);
  if ((r1 < floor) || (r1 > ceiling)) evaluate (__LINE__, __func__, "Random generator out of bounds", convert_to_string (r1));
  int r2 = filter_string_rand (floor, ceiling);
  if ((r2 < floor) || (r2 > ceiling)) evaluate (__LINE__, __func__, "Random generator out of bounds", convert_to_string (r2));
  if (r1 == r2) evaluate (__LINE__, __func__, "Random generator should generate different values", convert_to_string (r1) + " " + convert_to_string (r2));
}


void test_filters_test15 ()
{
  Passage passage = Passage ();
  evaluate (__LINE__, __func__, "", passage.bible);
  evaluate (__LINE__, __func__, 0, passage.book);
  evaluate (__LINE__, __func__, 0, passage.chapter);
  evaluate (__LINE__, __func__, "", passage.verse);
  passage = Passage ("bible", 1, 2, "3");
  evaluate (__LINE__, __func__, "bible", passage.bible);
  evaluate (__LINE__, __func__, 1, passage.book);
  evaluate (__LINE__, __func__, 2, passage.chapter);
  evaluate (__LINE__, __func__, "3", passage.verse);
  evaluate (__LINE__, __func__, true, passage.equal (passage));
  Passage passage2 = Passage ("bible", 1, 2, "4");
  evaluate (__LINE__, __func__, false, passage.equal (passage2));
}


void test_filters_passage1 ()
{
  // Convert Passage to/from text.
  {
    Passage input = Passage ("עברית", 1, 2, "3");
    string text = input.to_text ();
    Passage output = Passage::from_text (text);
    evaluate (__LINE__, __func__, true, input.equal (output));
    input = Passage ("ελληνικά", 5, 4, "0");
    text = input.to_text ();
    output = Passage::from_text (text);
    evaluate (__LINE__, __func__, true, input.equal (output));
  }
  // PassageDisplay
  {
    evaluate (__LINE__, __func__, "Genesis 10:2", filter_passage_display (1, 10, "2"));
    evaluate (__LINE__, __func__, "Exodus 11:2", filter_passage_display (2, 11, "2"));
  }
  // PassagesDisplayInline
  {
    evaluate (__LINE__, __func__, "Genesis 10:2", filter_passage_display_inline ({Passage ("", 1, 10, "2")}));
    evaluate (__LINE__, __func__, "", filter_passage_display_inline ({}));
    evaluate (__LINE__, __func__, "Genesis 10:2 | Exodus 777:777", filter_passage_display_inline ({ Passage ("", 1, 10, "2"), Passage ("", 2, 777, "777")}));
    evaluate (__LINE__, __func__, "Unknown 10:2 | Exodus 777:777", filter_passage_display_inline ({ Passage ("", -1, 10, "2"), Passage ("", 2, 777, "777")}));
  }
  // PassagesDisplayMultiline.
  {
    evaluate (__LINE__, __func__, "Genesis 10:2\n", filter_passage_display_multiline ({Passage ("", 1, 10, "2")}));
    evaluate (__LINE__, __func__, "", filter_passage_display_inline ({}));
    evaluate (__LINE__, __func__, "Genesis 10:2\nExodus 777:777\n", filter_passage_display_multiline ({ Passage ("", 1, 10, "2"), Passage ("", 2, 777, "777")}));
    evaluate (__LINE__, __func__, "Unknown 10:2\nExodus 777:777\n", filter_passage_display_multiline ({ Passage ("", -1, 10, "2"), Passage ("", 2, 777, "777")}));
  }
  // PassageInteger ()
  {
    Passage standard = Passage ("", 6, 4, "2");
    int numeric = filter_passage_to_integer (standard);
    Passage passage = filter_integer_to_passage (numeric);
    evaluate (__LINE__, __func__, true, standard.equal (passage));
  }
  // InterpretBookEnglish
  {
    evaluate (__LINE__, __func__, 1, filter_passage_interpret_book ("Genesis"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1 Corinthians"));
    evaluate (__LINE__, __func__, 22, filter_passage_interpret_book ("Song of Solomon"));
    evaluate (__LINE__, __func__, 60, filter_passage_interpret_book ("I Peter"));
    evaluate (__LINE__, __func__, 63, filter_passage_interpret_book ("II John"));
    evaluate (__LINE__, __func__, 64, filter_passage_interpret_book ("III John"));
  }
  // InterpretBookUSFM
  {
    evaluate (__LINE__, __func__, 1, filter_passage_interpret_book ("GEN"));
    evaluate (__LINE__, __func__, 67, filter_passage_interpret_book ("FRT"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1CO"));
  }
  // InterpretBookOSIS
  {
    evaluate (__LINE__, __func__, 1, filter_passage_interpret_book ("Gen"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1Cor"));
    evaluate (__LINE__, __func__, 22, filter_passage_interpret_book ("Song"));
  }
  // InterpretBookBibleWorks
  {
    evaluate (__LINE__, __func__, 1,  filter_passage_interpret_book ("Gen"));
    evaluate (__LINE__, __func__, 2,  filter_passage_interpret_book ("Exod"));
    evaluate (__LINE__, __func__, 3,  filter_passage_interpret_book ("Lev"));
    evaluate (__LINE__, __func__, 4,  filter_passage_interpret_book ("Num"));
    evaluate (__LINE__, __func__, 5,  filter_passage_interpret_book ("Deut"));
    evaluate (__LINE__, __func__, 6,  filter_passage_interpret_book ("Jos"));
    evaluate (__LINE__, __func__, 7,  filter_passage_interpret_book ("Jdg"));
    evaluate (__LINE__, __func__, 8,  filter_passage_interpret_book ("Ruth"));
    evaluate (__LINE__, __func__, 9,  filter_passage_interpret_book ("1 Sam"));
    evaluate (__LINE__, __func__, 10, filter_passage_interpret_book ("2 Sam"));
    evaluate (__LINE__, __func__, 11, filter_passage_interpret_book ("1 Ki"));
    evaluate (__LINE__, __func__, 12, filter_passage_interpret_book ("2 Ki"));
    evaluate (__LINE__, __func__, 13, filter_passage_interpret_book ("1 Chr"));
    evaluate (__LINE__, __func__, 14, filter_passage_interpret_book ("2 Chr"));
    evaluate (__LINE__, __func__, 15, filter_passage_interpret_book ("Ezr"));
    evaluate (__LINE__, __func__, 16, filter_passage_interpret_book ("Neh"));
    evaluate (__LINE__, __func__, 17, filter_passage_interpret_book ("Est"));
    evaluate (__LINE__, __func__, 18, filter_passage_interpret_book ("Job"));
    evaluate (__LINE__, __func__, 19, filter_passage_interpret_book ("Ps"));
    evaluate (__LINE__, __func__, 20, filter_passage_interpret_book ("Prov"));
    evaluate (__LINE__, __func__, 21, filter_passage_interpret_book ("Eccl"));
    evaluate (__LINE__, __func__, 22, filter_passage_interpret_book ("Cant"));
    evaluate (__LINE__, __func__, 22, filter_passage_interpret_book ("Sol"));
    evaluate (__LINE__, __func__, 23, filter_passage_interpret_book ("Isa"));
    evaluate (__LINE__, __func__, 24, filter_passage_interpret_book ("Jer"));
    evaluate (__LINE__, __func__, 25, filter_passage_interpret_book ("Lam"));
    evaluate (__LINE__, __func__, 26, filter_passage_interpret_book ("Ezek"));
    evaluate (__LINE__, __func__, 27, filter_passage_interpret_book ("Dan"));
    evaluate (__LINE__, __func__, 28, filter_passage_interpret_book ("Hos"));
    evaluate (__LINE__, __func__, 29, filter_passage_interpret_book ("Joel"));
    evaluate (__LINE__, __func__, 30, filter_passage_interpret_book ("Amos"));
    evaluate (__LINE__, __func__, 31, filter_passage_interpret_book ("Obad"));
    evaluate (__LINE__, __func__, 32, filter_passage_interpret_book ("Jon"));
    evaluate (__LINE__, __func__, 33, filter_passage_interpret_book ("Mic"));
    evaluate (__LINE__, __func__, 34, filter_passage_interpret_book ("Nah"));
    evaluate (__LINE__, __func__, 35, filter_passage_interpret_book ("Hab"));
    evaluate (__LINE__, __func__, 36, filter_passage_interpret_book ("Zeph"));
    evaluate (__LINE__, __func__, 37, filter_passage_interpret_book ("Hag"));
    evaluate (__LINE__, __func__, 38, filter_passage_interpret_book ("Zech"));
    evaluate (__LINE__, __func__, 39, filter_passage_interpret_book ("Mal"));
    evaluate (__LINE__, __func__, 40, filter_passage_interpret_book ("Matt"));
    evaluate (__LINE__, __func__, 41, filter_passage_interpret_book ("Mk"));
    evaluate (__LINE__, __func__, 42, filter_passage_interpret_book ("Lk"));
    evaluate (__LINE__, __func__, 43, filter_passage_interpret_book ("Jn"));
    evaluate (__LINE__, __func__, 44, filter_passage_interpret_book ("Acts"));
    evaluate (__LINE__, __func__, 45, filter_passage_interpret_book ("Rom"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1 Co"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1Co"));
    evaluate (__LINE__, __func__, 47, filter_passage_interpret_book ("2 Co"));
    evaluate (__LINE__, __func__, 48, filter_passage_interpret_book ("Gal"));
    evaluate (__LINE__, __func__, 49, filter_passage_interpret_book ("Eph"));
    evaluate (__LINE__, __func__, 50, filter_passage_interpret_book ("Phil"));
    evaluate (__LINE__, __func__, 51, filter_passage_interpret_book ("Col"));
    evaluate (__LINE__, __func__, 52, filter_passage_interpret_book ("1 Thess"));
    evaluate (__LINE__, __func__, 53, filter_passage_interpret_book ("2 Thess"));
    evaluate (__LINE__, __func__, 54, filter_passage_interpret_book ("1 Tim"));
    evaluate (__LINE__, __func__, 55, filter_passage_interpret_book ("2 Tim"));
    evaluate (__LINE__, __func__, 56, filter_passage_interpret_book ("Tit"));
    evaluate (__LINE__, __func__, 57, filter_passage_interpret_book ("Phlm"));
    evaluate (__LINE__, __func__, 58, filter_passage_interpret_book ("Heb"));
    evaluate (__LINE__, __func__, 59, filter_passage_interpret_book ("Jas"));
    evaluate (__LINE__, __func__, 60, filter_passage_interpret_book ("1 Pet"));
    evaluate (__LINE__, __func__, 61, filter_passage_interpret_book ("2 Pet"));
    evaluate (__LINE__, __func__, 62, filter_passage_interpret_book ("1 Jn"));
    evaluate (__LINE__, __func__, 63, filter_passage_interpret_book ("2 Jn"));
    evaluate (__LINE__, __func__, 64, filter_passage_interpret_book ("3 Jn"));
    evaluate (__LINE__, __func__, 65, filter_passage_interpret_book ("Jude"));
    evaluate (__LINE__, __func__, 66, filter_passage_interpret_book ("Rev"));
  }
}


void test_filters_passage2 ()
{
  // InterpretBookOnlineBibleAbbreviations.
  {
    evaluate (__LINE__, __func__, 1, filter_passage_interpret_book ("Ge"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1Co"));
    evaluate (__LINE__, __func__, 22, filter_passage_interpret_book ("So"));
  }  
  // InterpretBookPartialNames.
  {
    evaluate (__LINE__, __func__, 1, filter_passage_interpret_book ("G"));
    evaluate (__LINE__, __func__, 1, filter_passage_interpret_book ("g"));
    evaluate (__LINE__, __func__, 1, filter_passage_interpret_book ("ge"));
    evaluate (__LINE__, __func__, 1, filter_passage_interpret_book ("gene"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1 Cori"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1 cori"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1 corint"));
    evaluate (__LINE__, __func__, 46, filter_passage_interpret_book ("1cor"));
    evaluate (__LINE__, __func__, 22, filter_passage_interpret_book ("song"));
    evaluate (__LINE__, __func__, 22, filter_passage_interpret_book ("song of"));
    evaluate (__LINE__, __func__, 22, filter_passage_interpret_book ("song of sol"));
    evaluate (__LINE__, __func__, 11, filter_passage_interpret_book ("1ki"));
  }
  // Explode Passage
  {
    Passage passage = filter_passage_explode_passage ("Genesis 2:2");
    evaluate (__LINE__, __func__, 1, passage.book);
    evaluate (__LINE__, __func__, 2, passage.chapter);
    evaluate (__LINE__, __func__, "2", passage.verse);
    
    passage = filter_passage_explode_passage ("1 Corinth. 2:2");
    evaluate (__LINE__, __func__, 46, passage.book);
    evaluate (__LINE__, __func__, 2, passage.chapter);
    evaluate (__LINE__, __func__, "2", passage.verse);
    
    passage = filter_passage_explode_passage ("Song of Sol. 2:2");
    evaluate (__LINE__, __func__, 22, passage.book);
    evaluate (__LINE__, __func__, 2, passage.chapter);
    evaluate (__LINE__, __func__, "2", passage.verse);
   
    passage = filter_passage_explode_passage ("Revelation 2:2");
    evaluate (__LINE__, __func__, 66, passage.book);
    evaluate (__LINE__, __func__, 2, passage.chapter);
    evaluate (__LINE__, __func__, "2", passage.verse);
  }
  // InterpretPassage
  {
    Passage currentPassage = Passage ("", 2, 4, "6");

    Passage standard = Passage ("", 1, 2, "3");
    Passage output = filter_passage_interpret_passage (currentPassage, " Genesis 2 3");
    evaluate (__LINE__, __func__, true, standard.equal (output));

    standard = Passage ("", 46, 2, "3");
    output = filter_passage_interpret_passage (currentPassage, "1  Corinthians 2:3");
    evaluate (__LINE__, __func__, true, standard.equal (output));

    standard = Passage ("", 22, 2, "3");
    output = filter_passage_interpret_passage (currentPassage, "Song of Solomon 2.3");
    evaluate (__LINE__, __func__, true, standard.equal (output));

    standard = currentPassage;
    output = filter_passage_interpret_passage (currentPassage, "");
    evaluate (__LINE__, __func__, true, standard.equal (output));

    standard = Passage ("", 0, 1, "1");
    output = filter_passage_interpret_passage (currentPassage, "Genesis Exodus");
    evaluate (__LINE__, __func__, true, standard.equal (output));

    standard = Passage ("", 2, 1, "1");
    output = filter_passage_interpret_passage (currentPassage, "Exodus");
    evaluate (__LINE__, __func__, true, standard.equal (output));

    standard = Passage ("", 2, 4, "11");
    output = filter_passage_interpret_passage (currentPassage, "11");
    evaluate (__LINE__, __func__, true, standard.equal (output));

    standard = Passage ("", 2, 11, "14");
    output = filter_passage_interpret_passage (currentPassage, "11 14");
    evaluate (__LINE__, __func__, true, standard.equal (output));

    standard = Passage ("", 22, 2, "1");
    output = filter_passage_interpret_passage (currentPassage, "Song of Solomon 2");
    evaluate (__LINE__, __func__, true, standard.equal (output));
  }
}


void test_filters_passage3 ()
{
  // Sequence And Range None.
  {
    vector <string> standard = {"Exod. 30:4"};
    vector <string> output = filter_passage_handle_sequences_ranges ("Exod. 30:4");
    evaluate (__LINE__, __func__, standard, output);
  }
  // Sequence
  {
    vector <string> standard = {"Exod. 37:5", "14", "28"};
    vector <string> output = filter_passage_handle_sequences_ranges ("Exod. 37:5, 14, 28");
    evaluate (__LINE__, __func__, standard, output);
  }
  // Range
  {
    vector <string> standard = {"Exod. 37:5", "14", "15", "16"};
    vector <string> output = filter_passage_handle_sequences_ranges ("Exod. 37:5, 14 - 16");
    evaluate (__LINE__, __func__, standard, output);
  }
  // Sequence And Range One
  {
    vector <string> standard = {"Exod. 37:4", "5", "14", "15", "27", "28", "30", "40"};
    vector <string> output = filter_passage_handle_sequences_ranges ("Exod. 37:4 - 5, 14 - 15, 27 - 28, 30, 40");
    evaluate (__LINE__, __func__, standard, output);
  }
  // Sequence And Range Two
  {
    vector <string> standard = {"Exod. 25:13", "14", "27", "28"};
    vector <string> output = filter_passage_handle_sequences_ranges ("Exod. 25:13-14, 27-28");
    evaluate (__LINE__, __func__, standard, output);
  }
  // Test ordered books.
  {
    refresh_sandbox (true);
    Database_Bibles database_bibles = Database_Bibles ();
    Database_Search database_search = Database_Search ();
    database_search.create ();
    
    string bible = "php unit";

    // No ordering.
    database_bibles.createBible (bible);
    database_bibles.storeChapter (bible, 1, 1, "data");
    database_bibles.storeChapter (bible, 2, 1, "data");
    database_bibles.storeChapter (bible, 3, 1, "data");
    database_bibles.storeChapter (bible, 4, 1, "data");
    database_bibles.storeChapter (bible, 5, 1, "data");
    vector <int> books = filter_passage_get_ordered_books (bible);
    evaluate (__LINE__, __func__, {1, 2, 3, 4, 5}, books);

    // Existing books re-ordered.
    Database_Config_Bible::setBookOrder (bible, "1 3 2 5 4");
    books = filter_passage_get_ordered_books (bible);
    evaluate (__LINE__, __func__, {1, 3, 2, 5, 4}, books);

    // Some books ordered, and Bible has extra books: These are to be added to the end.
    Database_Config_Bible::setBookOrder (bible, "1 3 2");
    books = filter_passage_get_ordered_books (bible);
    evaluate (__LINE__, __func__, {1, 3, 2, 4, 5}, books);

    // More books ordered than in Bible: Remove the extra ones.
    Database_Config_Bible::setBookOrder (bible, "1 3 2 5 4 6");
    books = filter_passage_get_ordered_books (bible);
    evaluate (__LINE__, __func__, {1, 3, 2, 5, 4}, books);
  }
}


void test_filter_string_text2html ()
{
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
      "Here is superscript and here is subscript.\n"
      "Centered\n"
      "Left justified\n"
      "Numbered list.\n"
      "No numbered text.\n"
      "Link to http://google.nl.\n"
      "Here follows an image: .\n"
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
}


void test_email ()
{
  evaluate (__LINE__, __func__, true, filter_url_email_is_valid ("user@web.site"));
  evaluate (__LINE__, __func__, false, filter_url_email_is_valid ("user@website"));
  evaluate (__LINE__, __func__, false, filter_url_email_is_valid (" user@web.site"));
  evaluate (__LINE__, __func__, false, filter_url_email_is_valid ("user @ web.site"));

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


void test_stat ()
{
  // Testing is_dir.
  string path = filter_url_create_root_path ("git");
  evaluate (__LINE__, __func__, true, filter_url_is_dir (path));
  path = filter_url_create_root_path ("setup", "index.html");
  evaluate (__LINE__, __func__, false, filter_url_is_dir (path));
  
  string directory = filter_url_create_root_path ("tmp");
  string file1 = filter_url_create_path (directory, "1");
  string file2 = filter_url_create_path (directory, "2");
  filter_url_file_put_contents (file1, "x");
  filter_url_file_put_contents (file2, "x");

  // Testing checking for and setting write permissions.
  evaluate (__LINE__, __func__, true, filter_url_get_write_permission (directory));
  evaluate (__LINE__, __func__, true, filter_url_get_write_permission (file1));
  evaluate (__LINE__, __func__, true, filter_url_get_write_permission (file2));
  
  chmod (directory.c_str(), S_IRUSR);
  chmod (file1.c_str(), S_IRUSR);
  chmod (file2.c_str(), S_IRUSR);

  evaluate (__LINE__, __func__, false, filter_url_get_write_permission (directory));
  evaluate (__LINE__, __func__, false, filter_url_get_write_permission (file1));
  evaluate (__LINE__, __func__, false, filter_url_get_write_permission (file2));

  filter_url_set_write_permission (directory);
  filter_url_set_write_permission (file1);
  filter_url_set_write_permission (file2);

  evaluate (__LINE__, __func__, true, filter_url_get_write_permission (directory));
  evaluate (__LINE__, __func__, true, filter_url_get_write_permission (file1));
  evaluate (__LINE__, __func__, true, filter_url_get_write_permission (file2));
}


void test_replace ()
{
  evaluate (__LINE__, __func__, "ABXEFG", substr_replace ("ABCDEFG", "X", 2, 2));
  evaluate (__LINE__, __func__, "ABX", substr_replace ("ABCDEFG", "X", 2, 5));
  evaluate (__LINE__, __func__, "ABXG", substr_replace ("ABCDEFG", "X", 2, 4));
}


// Tests for the filters in the filter folder.
void test_filters ()
{
  refresh_sandbox (true);
  test_filters_test1 ();
  test_filters_test2 ();
  test_filters_test3 ();
  test_filters_test_usfm1 ();
  test_filters_test_usfm2 ();
  test_filters_test5 ();
  test_filters_test6 ();
  test_filters_export1 ();
  test_filters_export2 ();
  test_filters_test8 ();
  test_filters_test9 ();
  test_filters_test10 ();
  refresh_sandbox (true);
  test_filters_test11 ();
  refresh_sandbox (true);
  test_filters_test12 ();
  refresh_sandbox (true);
  test_filters_test13 ();
  test_filters_test14 ();
  test_filters_test15 ();
  test_filters_passage1 ();
  test_filters_passage2 ();
  test_filters_passage3 ();
  test_filter_string_text2html ();
  test_email ();
  test_stat ();
  test_replace ();
}


void test_styles_css ()
{
  Webserver_Request request;
  // Basic.
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.createSheet ("phpunit");
    Styles_Css styles_css = Styles_Css (&request, "phpunit");
    styles_css.generate ();
    string css = styles_css.css ();
    string standard = filter_string_trim (filter_url_file_get_contents (filter_url_create_path ("unittests", "tests", "basic.css")));
    evaluate (__LINE__, __func__, standard, css);
  }
  // Export.
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.createSheet ("phpunit");
    Styles_Css styles_css = Styles_Css (&request, "phpunit");
    styles_css.exports ();
    styles_css.generate ();
    string css = styles_css.css ();
    string standard = filter_string_trim (filter_url_file_get_contents (filter_url_create_path ("unittests", "tests", "exports.css")));
    evaluate (__LINE__, __func__, standard, css);
  }
  // Editor.
  {
    refresh_sandbox (true);
    Database_Styles database_styles = Database_Styles ();
    database_styles.createSheet ("phpunit");
    Styles_Css styles_css = Styles_Css (&request, "phpunit");
    styles_css.editor ();
    styles_css.generate ();
    string css = styles_css.css ();
    string standard = filter_string_trim (filter_url_file_get_contents (filter_url_create_path ("unittests", "tests", "editor.css")));
    evaluate (__LINE__, __func__, standard, css);
  }
}


void test_filter_custom_css ()
{
  // Direction
  {
    evaluate (__LINE__, __func__, "checked", Filter_CustomCSS::directionUnspecified (100));
    evaluate (__LINE__, __func__, "", Filter_CustomCSS::directionUnspecified (101));
    
    evaluate (__LINE__, __func__, "checked", Filter_CustomCSS::directionLeftToRight (101));
    evaluate (__LINE__, __func__, "", Filter_CustomCSS::directionLeftToRight (102));
    
    evaluate (__LINE__, __func__, "checked", Filter_CustomCSS::directionRightToLeft (102));
    evaluate (__LINE__, __func__, "", Filter_CustomCSS::directionRightToLeft (103));
    
    evaluate (__LINE__, __func__, 0, Filter_CustomCSS::directionValue (""));
    evaluate (__LINE__, __func__, 1, Filter_CustomCSS::directionValue ("ltr"));
    evaluate (__LINE__, __func__, 2, Filter_CustomCSS::directionValue ("rtl"));
  }
  // Writing Mode
  {
    evaluate (__LINE__, __func__, "checked", Filter_CustomCSS::writingModeUnspecified (102));
    evaluate (__LINE__, __func__, "", Filter_CustomCSS::writingModeUnspecified (112));
    
    evaluate (__LINE__, __func__, "checked", Filter_CustomCSS::writingModeTopBottomLeftRight (112));
    evaluate (__LINE__, __func__, "", Filter_CustomCSS::writingModeTopBottomLeftRight (122));
    
    evaluate (__LINE__, __func__, "checked", Filter_CustomCSS::writingModeTopBottomRightLeft (122));
    evaluate (__LINE__, __func__, "", Filter_CustomCSS::writingModeTopBottomRightLeft (132));
    
    evaluate (__LINE__, __func__, "checked", Filter_CustomCSS::writingModeBottomTopLeftRight (132));
    evaluate (__LINE__, __func__, "", Filter_CustomCSS::writingModeBottomTopLeftRight (142));
    
    evaluate (__LINE__, __func__, "checked", Filter_CustomCSS::writingModeBottomTopRightLeft (142));
    evaluate (__LINE__, __func__, "", Filter_CustomCSS::writingModeBottomTopRightLeft (152));
  }
  // Css
  {
    string css = Filter_CustomCSS::getCss ("class", "", 0);
    
    string standard =
    ".class\n"
    "{\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_CustomCSS::getCss ("class", "", 101);
    standard =
    ".class\n"
    "{\n"
    "direction: ltr;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_CustomCSS::getCss ("class", "", 102);
    standard =
    ".class\n"
    "{\n"
    "direction: rtl;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_CustomCSS::getCss ("class", "", 110);
    standard =
    ".class\n"
    "{\n"
    "writing-mode: tb-lr;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_CustomCSS::getCss ("CLass", "", 130);
    standard =
    ".CLass\n"
    "{\n"
    "writing-mode: bt-lr;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_CustomCSS::getCss ("CLass", "", 1322);
    standard =
    ".CLass\n"
    "{\n"
    "direction: rtl;\n"
    "writing-mode: tb-rl;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_CustomCSS::getCss ("Class", "sherif", 0);
    standard =
    ".Class\n"
    "{\n"
    "font-family: sherif;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_CustomCSS::getCss ("Class", "sherif", 102);
    standard =
    ".Class\n"
    "{\n"
    "font-family: sherif;\n"
    "direction: rtl;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_CustomCSS::getCss ("classs", "../font.ttf", 0);
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
    string clss = Filter_CustomCSS::getClass ("ആഈഘലറ");
    evaluate (__LINE__, __func__, "customf86528", clss);
  }
}


void test_filter_bibleworks ()
{
  // Import1
  {
    string bwdata =
    "Hab 1:1  La sentència ... profeta <05030> Habacuc <02265>.\n"
    "Hab 1:2 ¿Fins <05704> quan <0575>, Jahveh ... no [em] salvaràs <03467>(08686)?\n"
    "Hab 1:3 ¿Per què ... i la controvèrsia <04066>.\n";
    string usfmdata =
    "\\id HAB\n"
    "\\c 1\n"
    "\\p\n"
    "\\v 1 La sentència ... profeta <05030> Habacuc <02265>.\n"
    "\\v 2 ¿Fins <05704> quan <0575>, Jahveh ... no \\add em\\add* salvaràs <03467>(08686)?\n"
    "\\v 3 ¿Per què ... i la controvèrsia <04066>.";
    string data = Filter_Bibleworks::import (bwdata, true);
    evaluate (__LINE__, __func__, usfmdata, data);
  }
  // Import2
  {
    string bwdata =
    "Hab 1:1  La sentència ... profeta <05030> Habacuc <02265>.\n"
    "Hab 1:2 ¿Fins <05704> quan <0575>, Jahveh ... no [em] salvaràs <03467>(08686)?\n"
    "Hab 1:3 ¿Per què ... i la controvèrsia <04066>.\n";
    string usfmdata =
    "\\id HAB\n"
    "\\c 1\n"
    "\\p\n"
    "\\v 1 La sentència ... profeta  Habacuc .\n"
    "\\v 2 ¿Fins  quan , Jahveh ... no \\add em\\add* salvaràs ?\n"
    "\\v 3 ¿Per què ... i la controvèrsia .";
    string data = Filter_Bibleworks::import (bwdata, false);
    evaluate (__LINE__, __func__, usfmdata, data);
  }
  // Italics1
  {
    string data = Filter_Bibleworks::italics ("Normal text and [italics].");
    evaluate (__LINE__, __func__, data, "Normal text and \\add italics\\add*.");
  }
  // Italics2
  {
    string data = Filter_Bibleworks::italics ("Normal text, [italics], and [italics again].");
    evaluate (__LINE__, __func__, data, "Normal text, \\add italics\\add*, and \\add italics again\\add*.");
  }
  // Italics3
  {
    string data = Filter_Bibleworks::italics ("[Italics] and [malformed italics.");
    evaluate (__LINE__, __func__, data, "\\add Italics\\add* and [malformed italics.");
  }
  // Italics4
  {
    string data = Filter_Bibleworks::italics ("[Italics] and malformed italics].");
    evaluate (__LINE__, __func__, data, "\\add Italics\\add* and malformed italics].");
  }
  // Italics5
  {
    string data = Filter_Bibleworks::italics ("Mal]formed [italics].");
    evaluate (__LINE__, __func__, data, "Mal]formed \\add italics\\add*.");
  }
  // Notes1
  {
    string data = Filter_Bibleworks::notes ("Normal text and {notes}.");
    evaluate (__LINE__, __func__, data, "Normal text and \\f + notes\\f*.");
  }
  // Notes2
  {
    string data = Filter_Bibleworks::notes ("Normal text, {notes}, and {notes again}.");
    evaluate (__LINE__, __func__, data, "Normal text, \\f + notes\\f*, and \\f + notes again\\f*.");
  }
  // Notes3
  {
    string data = Filter_Bibleworks::notes ("{notes} and {malformed notes.");
    evaluate (__LINE__, __func__, data, "\\f + notes\\f* and {malformed notes.");
  }
  // Notes4
  {
    string data = Filter_Bibleworks::notes ("{notes} and malformed notes}.");
    evaluate (__LINE__, __func__, data, "\\f + notes\\f* and malformed notes}.");
  }
  // Notes5
  {
    string data = Filter_Bibleworks::notes ("Mal}formed {notes}.");
    evaluate (__LINE__, __func__, data, "Mal}formed \\f + notes\\f*.");
  }
  // Parenthesis1
  {
    vector <string >malformed;
    string data = Filter_Bibleworks::parenthesis ("Normal text.", malformed);
    evaluate (__LINE__, __func__, data, "Normal text.");
  }
  // Parenthesis2.
  {
    vector <string> malformed;
    string data = Filter_Bibleworks::parenthesis ("Text in parenthesis(08804).", malformed);
    evaluate (__LINE__, __func__, data, "Text in parenthesis.");
  }
  // Parenthesis3
  {
    vector <string> malformed;
    string data = Filter_Bibleworks::parenthesis ("Text in parenthesis(08804a).", malformed);
    evaluate (__LINE__, __func__, data, "Text in parenthesis(08804a).");
    evaluate (__LINE__, __func__, malformed, {"(08804a)"});
  }
  // Parenthesis4
  {
    vector <string> malformed;
    string data = Filter_Bibleworks::parenthesis ("Text in parenthesis(08(804).", malformed);
    evaluate (__LINE__, __func__, data, "Text in parenthesis(08.");
    evaluate (__LINE__, __func__, malformed, {"(08(804)"});
  }
  // Chevrons1
  {
    vector <string> malformed;
    string data = Filter_Bibleworks::chevrons ("Normal text.", malformed);
    evaluate (__LINE__, __func__, data, "Normal text.");
  }
  // Chevrons2
  {
    vector <string> malformed;
    string data = Filter_Bibleworks::chevrons ("Text in chevrons<01004>.", malformed);
    evaluate (__LINE__, __func__, data, "Text in chevrons.");
  }
  // Chevrons3
  {
    vector <string> malformed;
    string data = Filter_Bibleworks::chevrons ("Text in chevrons<01004b>.", malformed);
    evaluate (__LINE__, __func__, data, "Text in chevrons.");
  }
  // Chevrons4()
  {
    vector <string> malformed;
    string data = Filter_Bibleworks::chevrons ("Text in chevrons<06030, 06031>.", malformed);
    evaluate (__LINE__, __func__, data, "Text in chevrons.");
  }
  // Chevrons5
  {
    vector <string> malformed;
    string data = Filter_Bibleworks::chevrons ("Text in chevrons<06030; abc>.", malformed);
    evaluate (__LINE__, __func__, data, "Text in chevrons<06030; abc>.");
    evaluate (__LINE__, __func__, malformed, {"<06030; abc>"});
  }
}


void test_filter_diff ()
{
  // Diff 1
  {
    string output = filter_diff_diff ("Old text", "New text");
    string standard = "<span style=\"text-decoration: line-through;\">Old</span> <span style=\"font-weight: bold;\">New</span> text";
    evaluate (__LINE__, __func__, standard, output);
  }
  // Diff 2
  {
    string output = filter_diff_diff ("this is really old text", "and this is new text");
    string standard = "<span style=\"font-weight: bold;\">and</span> this is <span style=\"text-decoration: line-through;\">really</span> <span style=\"text-decoration: line-through;\">old</span> <span style=\"font-weight: bold;\">new</span> text";
    evaluate (__LINE__, __func__, standard, output);
  }
  // Similarity 1.
  {
    int similarity = filter_diff_similarity ("Old text", "New text");
    evaluate (__LINE__, __func__, 45, similarity);
  }
  // Similarity 2.
  {
    int similarity = filter_diff_similarity ("New text", "New text");
    evaluate (__LINE__, __func__, 100, similarity);
  }
  // Similarity 3.
  {
    int similarity = filter_diff_similarity ("ABCDEFGH", "IJKLMNOPQRST");
    evaluate (__LINE__, __func__, 0, similarity);
  }
  // Similarity 4.
  {
    string first =
    "\\c 29\n"
    "\\s Kugadzwa kwevapristi\n"
    "\\p\n"
    "\\v 1 Zvino ichi ndicho chinhu chauchavaitira kuvaita vatsvene, kuti vandishumire pabasa reupristi: Tora nzombe imwe diki; nemakondohwe maviri asina chaanopomerwa,\n"
    "\\v 2 nechingwa chisina kuviriswa, nemakeke asina kuviriswa akakanyiwa nemafuta, nezvingwa zvitete zvisina kuviriswa zvine mafuta; uchazviita neupfu hwakatsetseka hwegorosi.\n"
    "\\v 3 Zvino uchazviisa mudengu rimwe, ugopinda nazvo zviri mudengu, pamwe nenzombe diki nemakondohwe maviri.\n"
    "\\v 4 NaAroni nevanakomana vake uchauya navo kumukova wetende rekusangana, ugovashambidza nemvura;\n"
    "\\v 5 uchatorawo hanzu ugopfekedza Aroni nguvo refu, nejazi reefodhi, neefodhi, nechidzitiro chechipfuva; uye umusunge chiuno nebhanhire reefodhi rakarukwa neumhizha;\n"
    "\\v 6 uisewo ngundu pamusoro wake, nekuisa korona tsvene pangundu.\n"
    "\\v 7 Zvino uchatora mafuta ekuzodza, ugodira pamusoro wake, ugomuzodza.\n"
    "\\v 8 Nevanakomana vake uchavaswededza pedo ugovapfekedza majazi.\n"
    "\\v 9 Uchavasungawo zviuno nebhanhire, Aroni nevanakomana vake, uvasungirire nguwani; uye basa reupristi richava ravo chive chimiso nekusingaperi; uye uchaeresa Aroni nevanakomana vake.\n"
    "\\v 10 Zvino uchauisa nzombe pamberi petende rekusangana, uye Aroni nevanakomana vake vachaisa maoko avo pamusoro pemusoro wenzombe.\n"
    "\\v 11 Zvino uchabaya nzombe pamberi paJehovha pamukova wetende rekusangana.\n"
    "\\v 12 Zvino uchatora ropa renzombe, ugoisa panyanga dzearitari nemunwe wako, ndokudururira ropa rese mujinga memuzasi mearitari:\n"
    "\\v 13 Zvino uchatora mafuta ese anofukidza zvemukati, nebutiro riri pamusoro pechiropa, neitsvo mbiri, nemafuta ari padziri uzvipise pamusoro pearitari.\n"
    "\\v 14 Asi nyama yenzombe, nedehwe rayo, nechizvizvi chayo, uchapisa nemoto kunze kwemisasa; chipiriso chezvivi.\n"
    "\\p\n"
    "\\v 15 Uchatorawo gondohwe rimwe, uye Aroni nevanakomana vake vachaisa maoko avo pamusoro pemusoro wegondohwe,\n"
    "\\v 16 zvino uchauraya gondohwe ndokutora ropa raro ndokusasa kupoteredza aritari.\n"
    "\\v 17 Zvino uchaguranya gondohwe ndokusuka zvemukati zvaro nemakumbo aro ugoisa panhindi dzaro nepamusoro waro.\n"
    "\\v 18 Zvino uchapisa gondohwe rese paaritari; chipiriso chinopiswa kuna Jehovha, hwema hunonhuhwira, chipiriso chinoitirwa Jehovha, nemoto.\n"
    "\\v 19 Zvino uchatora gondohwe rechipiri, uye Aroni nevanakomana vake vagoisa maoko avo pamusoro pemusoro wegondohwe.\n"
    "\\v 20 Ipapo uchauraya gondohwe ndokutora paropa raro, ndokuisa pamucheto wezasi wenzeve yerudyi yaAroni, nepamicheto yezasi yenzeve dzerudyi dzevanakomana vake, nepagunwe reruoko rwerudyi, nepagumwe guru retsoka yavo yerudyi, nekusasawo ropa paaritari kupoteredza.\n"
    "\\v 21 Zvino uchatora paropa riri paaritari nepamafuta ekuzodza ugosasa pamusoro paAroni napanguvo dzake nepamusoro pevanakomana vake, nepanguvo dzevanakomana vake pamwe naye, agoitwa mutsvene iye nenguvo dzake nevanakomana vake nenguvo dzevanakomana vake pamwe naye.\n"
    "\\v 22 Zvino utorewo mamwe mafuta egondohwe, nebemhe raro, namafuta anofukidza ura, namafuta ari pamusoro pechiropa, netsvo mbiri, namafuta ari padziri, nebandauko rorudyi (nokuti igondohwe rokugadza naro),\n"
    "\\v 23 nebundu rimwe rechingwa, nechingwa chimwe chidiki chinamafuta, nechingwa chimwe chitete chinobva mudengu rezvingwa zvisina kuviriswa, zviri pamberi paJehovha.\n"
    "\\v 24 Zvino uzviise zvese pamaoko aAroni napamaoko avanakomana vake, vagozvizunguzira, chive chipo chinozunguzirwa pamberi paJehovha.\n"
    "\\v 25 Zvino uzvibvise pamaoko avo, uzvipise paaritari pamusoro pechipiriso chinopiswa, kuti chive chipo chinonhuhwira pamberi paJehovha; chipiriso chakaitirwa Jehovha pamoto.\n"
    "\\v 26 Utorewo chityu chegondohwe rokugadza naro raAroni, urizunguzire pamberi paJehovha, kuti chive chipo chinozunguzirwa; ndiwo mugove wako.\n"
    "\\v 27 Zvino chityu chinozunguzirwa uchiite chitsvene, nebandauko rechipiriso chinosimudzwa, chinozunguzirwa nekusimudzwa, zvegondohwe rokugadza naro, kuti zvive zvaAroni nezvavanakomana vake.\n"
    "\\v 28 Izvozvo zvinofanira kupiwa Aroni navanakomana vake navana vaIsraeri nokusingaperi nokuti chipiriso chinosimudzwa, chinobva kuvana vaIsraeri pazvibairo zvezvipiriso zvavo zvokuyananisa, chive chipiriso chavo chinosimudzirwa Jehovha.\n"
    "\\p\n"
    "\\v 29 Nguvo dzaAroni dzichava dzavanakomana vake vanomutevera, kuti vazodzwe vakadzipfeka, vagadzwe vakadzipfeka.\n"
    "\\v 30 Mwanakomana wake anopinda paupristi panzvimbo yake, anofanira kudzipfeka mazuva manomwe, kana achipinda mutende rekusangana kushumira panzvimbo tsvene.\n"
    "\\p\n"
    "\\v 31 Zvino unofanira kutora gondohwe rokugadza naro, ubike nyama yaro panzvimbo tsvene.\n"
    "\\v 32 Aroni navanakomana vake ngavadye nyama yegondohwe, nechingwa chiri mudengu pamukova wetende rokusangana.\n"
    "\\v 33 Vanofanira kudya izvozvo zvaiyananiswa nazvo, pakugadzwa nokuitwa vatsvene kwavo; asi mweni ngaarege kuzvidya, nokuti zvitsvene.\n"
    "\\v 34 Kana imwe nyama yokugadza nayo ikasara, kana chimwe chingwa, kusvikira mangwana, unofanira kupisa zvasara nomoto; hazvifaniri kudyiwa, nokuti zvitsvene.\n"
    "\\p\n"
    "\\v 35 Unofanira kuitira Aroni navanakomana vake saizvozvo, zvese sezvandakuraira; uite mazuva manomwe uchivagadza.\n"
    "\\v 36 Zuva rimwe nerimwe unofanira kubaira nzombe yechipiriso chezvivi, chokuyananisa nacho; unofanira kunatsa aritari, kana uchiiyananisira; uizodze kuti uiite tsvene.\n"
    "\\v 37 Unofanira kuita mazuva manomwe pakuyananisira aritari, nokuiita tsvene; kuti aritari ive tsvene kwazvo, uye zvese zvinobata paaritari zvinofanira kuva zvitsvene.\n"
    "\\s Zvibairo zvezuva rimwe nerimwe\n"
    "\\p\n"
    "\\v 38 Zvino ndizvo zvaunofanira kubaira paaritari: Zuva rimwe nerimwe nguva dzese makwayana maviri.\n"
    "\\v 39 Rimwe gwayana unofanira kuribaira mangwanani, rimwe gwayana unofanira kuribaira madekwani.\n"
    "\\v 40 Pamwe chete negwayana rimwe unofanira kuisa chipiriso choupfu hwakatsetseka hwakasvika pachegumi cheefa hwakakanyiwa nechechina chehini yamafuta akasviniwa nechechina chehini yewaini, chive chipiriso chinodururwa.\n"
    "\\v 41 Rimwe gwayana uribaire madekwani, uriitire sezvawakaitira chipiriso choupfu chamangwanani, nezvawakaitira chipiriso charo chinomwiwa, kuti chive chipiriso chinonhuhwira, chipiriso chakaitirwa Jehovha pamoto.\n"
    "\\v 42 Chinofanira kuva chipiriso chinopiswa nguva dzese pamarudzi enyu ese, pamukova wetende rekusangana pamberi paJehovha, pandichasangana nemi, nditaurepo newe.\n"
    "\\v 43 Ndipo pandichasangana nevana vaIsraeri, uye tende richaitwa dzvene nekubwinya kwangu.\n"
    "\\v 44 Tende rokusangana nearitari ndichazviita zvitsvene; uye Aroni navanakomana vake ndichavaita vatsvene, kuti vandishumire pabasa roupristi.\n"
    "\\v 45 Ndichagara pakati pavana vaIsraeri, kuti ndive Mwari wavo.\n"
    "\\v 46 Vachaziva kuti ndini Jehovha wavo, wakavabudisa panyika kuti ndigare pakati pavo; ndini Jehovha Mwari wavo.\n";
    
    string second =
    "\\c 29\n"
    "\\s Kugadzwa kwevapristi\n"
    "\\p\n"
    "\\v 1 Zvino ichi ndicho chinhu chauchavaitira kuvaita vatsvene, kuti vandishumire pabasa reupristi: Tora nzombe imwe diki; nemakondohwe maviri asina chaanopomerwa,\n"
    "\\v 2 nechingwa chisina kuviriswa, nemakeke asina kuviriswa akakanyiwa nemafuta, nezvingwa zvitete zvisina kuviriswa zvine mafuta; uchazviita neupfu hwakatsetseka hwegorosi.\n"
    "\\v 3 Zvino uchazviisa mudengu rimwe, ugopinda nazvo zviri mudengu, pamwe nenzombe diki nemakondohwe maviri.\n"
    "\\v 4 NaAroni nevanakomana vake uchauya navo kumukova wetende rekusangana, ugovashambidza nemvura;\n"
    "\\v 5 uchatorawo hanzu ugopfekedza Aroni nguvo refu, nejazi reefodhi, neefodhi, nechidzitiro chechipfuva; uye umusunge chiuno nebhanhire reefodhi rakarukwa neumhizha;\n"
    "\\v 6 uisewo ngundu pamusoro wake, nekuisa korona tsvene pangundu.\n"
    "\\v 7 Zvino uchatora mafuta ekuzodza, ugodira pamusoro wake, ugomuzodza.\n"
    "\\v 8 Nevanakomana vake uchavaswededza pedo ugovapfekedza majazi.\n"
    "\\v 9 Uchavasungawo zviuno nebhanhire, Aroni nevanakomana vake, uvasungirire nguwani; uye basa reupristi richava ravo chive chimiso nekusingaperi; uye uchaeresa Aroni nevanakomana vake.\n"
    "\\v 10 Zvino uchauisa nzombe pamberi petende rekusangana, uye Aroni nevanakomana vake vachaisa maoko avo pamusoro pemusoro wenzombe.\n"
    "\\v 11 Zvino uchabaya nzombe pamberi paJehovha pamukova wetende rekusangana.\n"
    "\\v 12 Zvino uchatora ropa renzombe, ugoisa panyanga dzearitari nemunwe wako, ndokudururira ropa rese mujinga memuzasi mearitari:\n"
    "\\v 13 Zvino uchatora mafuta ese anofukidza zvemukati, nebutiro riri pamusoro pechiropa, neitsvo mbiri, nemafuta ari padziri uzvipise pamusoro pearitari.\n"
    "\\v 14 Asi nyama yenzombe, nedehwe rayo, nechizvizvi chayo, uchapisa nemoto kunze kwemisasa; chipiriso chezvivi.\n"
    "\\p\n"
    "\\v 15 Uchatorawo gondohwe rimwe, uye Aroni nevanakomana vake vachaisa maoko avo pamusoro pemusoro wegondohwe,\n"
    "\\v 16 zvino uchauraya gondohwe ndokutora ropa raro ndokusasa kupoteredza aritari.\n"
    "\\v 17 Zvino uchaguranya gondohwe ndokusuka zvemukati zvaro nemakumbo aro ugoisa panhindi dzaro nepamusoro waro.\n"
    "\\v 18 Zvino uchapisa gondohwe rese paaritari; chipiriso chinopiswa kuna Jehovha, hwema hunonhuhwira, chipiriso chinoitirwa Jehovha, nemoto.\n"
    "\\v 19 Zvino uchatora gondohwe rechipiri, uye Aroni nevanakomana vake vagoisa maoko avo pamusoro pemusoro wegondohwe.\n"
    "\\v 20 Ipapo uchauraya gondohwe ndokutora paropa raro, ndokuisa pamucheto wezasi wenzeve yerudyi yaAroni, nepamicheto yezasi yenzeve dzerudyi dzevanakomana vake, nepagunwe reruoko rwerudyi, nepagumwe guru retsoka yavo yerudyi, nekusasawo ropa paaritari kupoteredza.\n"
    "\\v 21 Zvino uchatora paropa riri paaritari nepamafuta ekuzodza ugosasa pamusoro paAroni napanguvo dzake nepamusoro pevanakomana vake, nepanguvo dzevanakomana vake pamwe naye, agoitwa mutsvene iye nenguvo dzake nevanakomana vake nenguvo dzevanakomana vake pamwe naye.\n"
    "\\v 22 Zvino utorewo mamwe mafuta egondohwe, nebemhe raro, namafuta anofukidza ura, namafuta ari pamusoro pechiropa, netsvo mbiri, namafuta ari padziri, nebandauko rorudyi (nokuti igondohwe rokugadza naro),\n"
    "\\v 23 nebundu rimwe rechingwa, nechingwa chimwe chidiki chinamafuta, nechingwa chimwe chitete chinobva mudengu rezvingwa zvisina kuviriswa, zviri pamberi paJehovha.\n"
    "\\v 24 Zvino uzviise zvese pamaoko aAroni napamaoko avanakomana vake, vagozvizunguzira, chive chipo chinozunguzirwa pamberi paJehovha.\n"
    "\\v 25 Zvino uzvibvise pamaoko avo, uzvipise paaritari pamusoro pechipiriso chinopiswa, kuti chive chipo chinonhuhwira pamberi paJehovha; chipiriso chakaitirwa Jehovha pamoto.\n"
    "\\v 26 Utorewo chityu chegondohwe rokugadza naro raAroni, urizunguzire pamberi paJehovha, kuti chive chipo chinozunguzirwa; ndiwo mugove wako.\n"
    "\\v 27 Zvino chityu chinozunguzirwa uchiite chitsvene, nebandauko rechipiriso chinosimudzwa, chinozunguzirwa nekusimudzwa, zvegondohwe rokugadza naro, kuti zvive zvaAroni nezvavanakomana vake.\n"
    "\\v 28 Izvozvo zvinofanira kupiwa Aroni navanakomana vake navana vaIsraeri nokusingaperi nokuti chipiriso chinosimudzwa, chinobva kuvana vaIsraeri pazvibairo zvezvipiriso zvavo zvokuyananisa, chive chipiriso chavo chinosimudzirwa Jehovha.\n"
    "\\p\n"
    "\\v 29 Nguvo dzaAroni dzichava dzavanakomana vake vanomutevera, kuti vazodzwe vakadzipfeka, vagadzwe vakadzipfeka.\n"
    "\\v 30 Mwanakomana wake anopinda paupristi panzvimbo yake, anofanira kudzipfeka mazuva manomwe, kana achipinda mutende rekusangana kushumira panzvimbo tsvene.\n"
    "\\p\n"
    "\\v 31 Zvino unofanira kutora gondohwe rokugadza naro, ubike nyama yaro panzvimbo tsvene.\n"
    "\\v 32 Aroni navanakomana vake ngavadye nyama yegondohwe, nechingwa chiri mudengu pamukova wetende rokusangana.\n"
    "\\v 33 Vanofanira kudya izvozvo zvaiyananiswa nazvo, pakugadzwa nokuitwa vatsvene kwavo; asi mweni ngaarege kuzvidya, nokuti zvitsvene.\n"
    "\\v 34 Kana imwe nyama yokugadza nayo ikasara, kana chimwe chingwa, kusvikira mangwana, unofanira kupisa zvasara nomoto; hazvifaniri kudyiwa, nokuti zvitsvene.\n"
    "\\p\n"
    "\\v 35 Unofanira kuitira Aroni navanakomana vake saizvozvo, zvese sezvandakuraira; uite mazuva manomwe uchivagadza.\n"
    "\\v 36 Zuva rimwe nerimwe unofanira kubaira nzombe yechipiriso chezvivi, chokuyananisa nacho; unofanira kunatsa aritari, kana uchiiyananisira; uizodze kuti uiite tsvene.\n"
    "\\v 37 Unofanira kuita mazuva manomwe pakuyananisira aritari, nokuiita tsvene; kuti aritari ive tsvene kwazvo, uye zvese zvinobata paaritari zvinofanira kuva zvitsvene.\n"
    "\\s Zvibairo zvezuva rimwe nerimwe\n"
    "\\p\n"
    "\\v 41 Rimwe gwayana uribaire madekwani, uriitire sezvawakaitira chipiriso choupfu chamangwanani, nezvawakaitira chipiriso charo chinomwiwa, kuti chive chipiriso chinonhuhwira, chipiriso chakaitirwa Jehovha pamoto.\n"
    "\\v 42 Chinofanira kuva chipiriso chinopiswa nguva dzese pamarudzi enyu ese, pamukova wetende rekusangana pamberi paJehovha, pandichasangana nemi, nditaurepo newe.\n"
    "\\v 43 Ndipo pandichasangana nevana vaIsraeri, uye tende richaitwa dzvene nekubwinya kwangu.\n"
    "\\v 44 Tende rokusangana nearitari ndichazviita zvitsvene; uye Aroni navanakomana vake ndichavaita vatsvene, kuti vandishumire pabasa roupristi.\n"
    "\\v 45 Ndichagara pakati pavana vaIsraeri, kuti ndive Mwari wavo.\n"
    "\\v 46 Vachaziva kuti ndini Jehovha wavo, wakavabudisa panyika kuti ndigare pakati pavo; ndini Jehovha Mwari wavo.\n";
    int similarity = filter_diff_similarity (first, second);
    evaluate (__LINE__, __func__, 94, similarity);
  }
}


void test_filter_abbreviations ()
{
  // Read ()
  {
    string input =
    "Psalms = Ps.\n"
    "\n"
    "Exodus = Exod.\n"
    "\n"
    "Exodu = Exod.\n"
    "\n"
    "Psalms = Psa.\n"
    "\n"
    "Joshua =\n"
    "\n";
    map <string, int> output = filter_abbreviations_read (input);
    map <string, int> standard = { make_pair ("Ps.", 19), make_pair ("Exod.", 2), make_pair ("Psa.", 19) };
    evaluate (__LINE__, __func__, standard, output);
  }
  // Display ()
  {
    string input =
    "Psalms = Ps.\n"
    "\n"
    "Exodus = Exod.\n"
    "\n";
    string output = filter_abbreviations_display (input);
    string standard =
    "Genesis = \n"
    "Exodus = Exod.\n"
    "Leviticus = \n"
    "Numbers = \n"
    "Deuteronomy = \n"
    "Joshua = \n"
    "Judges = \n"
    "Ruth = \n"
    "1 Samuel = \n"
    "2 Samuel = \n"
    "1 Kings = \n"
    "2 Kings = \n"
    "1 Chronicles = \n"
    "2 Chronicles = \n"
    "Ezra = \n"
    "Nehemiah = \n"
    "Esther = \n"
    "Job = \n"
    "Psalms = Ps.\n"
    "Proverbs = \n"
    "Ecclesiastes = \n"
    "Song of Solomon = \n"
    "Isaiah = \n"
    "Jeremiah = \n"
    "Lamentations = \n"
    "Ezekiel = \n"
    "Daniel = \n"
    "Hosea = \n"
    "Joel = \n"
    "Amos = \n"
    "Obadiah = \n"
    "Jonah = \n"
    "Micah = \n"
    "Nahum = \n"
    "Habakkuk = \n"
    "Zephaniah = \n"
    "Haggai = \n"
    "Zechariah = \n"
    "Malachi = \n"
    "Matthew = \n"
    "Mark = \n"
    "Luke = \n"
    "John = \n"
    "Acts = \n"
    "Romans = \n"
    "1 Corinthians = \n"
    "2 Corinthians = \n"
    "Galatians = \n"
    "Ephesians = \n"
    "Philippians = \n"
    "Colossians = \n"
    "1 Thessalonians = \n"
    "2 Thessalonians = \n"
    "1 Timothy = \n"
    "2 Timothy = \n"
    "Titus = \n"
    "Philemon = \n"
    "Hebrews = \n"
    "James = \n"
    "1 Peter = \n"
    "2 Peter = \n"
    "1 John = \n"
    "2 John = \n"
    "3 John = \n"
    "Jude = \n"
    "Revelation = \n"
    "Front Matter = \n"
    "Back Matter = \n"
    "Other Material = ";
    evaluate (__LINE__, __func__, standard, output);
  }
}

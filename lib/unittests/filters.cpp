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


#include <unittests/unittest.h>
#include <unittests/utilities.h>
#include <unittests/tests1.h>
#include <unittests/database_config.h>
#include <config/libraries.h>
#include <library/bibledit.h>
#include <database/config/user.h>
#include <database/styles.h>
#include <database/search.h>
#include <database/books.h>
#include <config/globals.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/roles.h>
#include <filter/md5.h>
#include <filter/usfm.h>
#include <filter/archive.h>
#include <session/logic.h>
#include <text/text.h>
#include <esword/text.h>
#include <onlinebible/text.h>
#include <html/text.h>
#include <odf/text.h>


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
    // Test filter_string_array_diff, a C++ equivalent for PHP's array_diff function.
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
    evaluate (__LINE__, __func__, ".", get_dirname (""));
    evaluate (__LINE__, __func__, ".", get_dirname ("/"));
    evaluate (__LINE__, __func__, ".", get_dirname ("dir/"));
    evaluate (__LINE__, __func__, ".", get_dirname ("/dir"));
    evaluate (__LINE__, __func__, "foo", get_dirname ("foo/bar"));
    evaluate (__LINE__, __func__, "/foo", get_dirname ("/foo/bar"));
    evaluate (__LINE__, __func__, "/foo", get_dirname ("/foo/bar/"));
    evaluate (__LINE__, __func__, "a.txt", get_basename ("/a.txt"));
    evaluate (__LINE__, __func__, "txt", get_basename ("/txt/"));
    evaluate (__LINE__, __func__, "foo.bar", get_basename ("/path/to/foo.bar"));
    evaluate (__LINE__, __func__, "foo.bar", get_basename ("foo.bar"));
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
    Database_Books database_books = Database_Books ();
    database_books.create ();

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


void test_filters_test4 ()
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



void test_filters_test7 ()
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
    if ((size < 155) || (size > 165)) evaluate (__LINE__, __func__, "between 155 and 165", convert_to_string (size));
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
    if ((size < 235) || (size > 245)) evaluate (__LINE__, __func__, "between 235 and 245", convert_to_string (size));
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
}


void test_filters_test10 ()
{
  string OdfTextTestDotOdt = "/tmp/OdfTextTest.odt";
  string temporary_folder = "/tmp";
  // Test Odf converter paragraphs.
  {
    Odf_Text odf_text = Odf_Text ("phpunit");
    /*
    $odf_text->createPageBreakStyle ();
    $odf_text->newParagraph ();
    $this->assertEquals ("Standard", $odf_text->currentParagraphStyle);
    $odf_text->addText ("Paragraph One");
    $this->assertEquals ("Paragraph One", $odf_text->currentParagraphContent);
    $odf_text->newParagraph ();
    $this->assertEquals ("", $odf_text->currentParagraphContent);
    $odf_text->addText ("Paragraph Two");
    $this->assertEquals ("Paragraph Two", $odf_text->currentParagraphContent);
    $odf_text->newHeading1 ("Heading One");
    $this->assertEquals ("", $odf_text->currentParagraphContent);
    $odf_text->newPageBreak ();
    $odf_text->newParagraph ();
    $odf_text->addText ("Paragraph Three");
    */
    odf_text.save (OdfTextTestDotOdt);
    /*
    $odt = shell_exec ("odt2txt /tmp/OdfTextTest.odt");
$standard = <<<'EOD'
Paragraph One

Paragraph Two

Heading One
===========

Paragraph Three
EOD;
    $this->assertEquals ($standard, trim ($odt));
    */
  }
  exit (0); // Todo
/* Todo



  public function testAutomaticParagraph ()
  {
    $odf_text = new Odf_Text ("phpunit");
    $odf_text->addText ("Should create new paragraph automatically");
    $odf_text->save ("/tmp/OdfTextTest.odt");
    $odt = shell_exec ("odt2txt /tmp/OdfTextTest.odt");
$standard = <<<'EOD'
Should create new paragraph automatically
EOD;
    $this->assertEquals ($standard, trim ($odt));
  }


  public function testBasicNote ()
  {
    $odf_text = new Odf_Text ("phpunit");
    $odf_text->newParagraph ();
    $odf_text->addText ("Text");
    $odf_text->addNote ("†", "");
    $odf_text->addNoteText ("Note");
    $odf_text->addText (".");
    $odf_text->save ("/tmp/OdfTextTest.odt");
    $odt = shell_exec ("odt2txt /tmp/OdfTextTest.odt");
$standard = <<<'EOD'
Text†

Note

.
EOD;
    $this->assertEquals ($standard, trim ($odt));
  }


  public function testBasicFormattedText ()
  {
    $styles_logic = Styles_Logic::getInstance ();
    $database_styles = Database_Styles::getInstance ();
    $add = $database_styles->getMarkerData ("Standard", "add");
    $odf_text = new Odf_Text ("phpunit");
    $odf_text->newParagraph ();
    $odf_text->addText ("text");
    $odf_text->openTextStyle ($add, false, false);
    $odf_text->addText ("add");
    $odf_text->closeTextStyle (false, false);
    $odf_text->addText ("normal");
    $odf_text->addText (".");
    $odf_text->save ("/tmp/OdfTextTest.odt");
    $odt = shell_exec ("odt2txt /tmp/OdfTextTest.odt");
$standard = <<<'EOD'
textaddnormal.
EOD;
    $this->assertEquals ($standard, trim ($odt));
  }


  public function testBasicFormattedNote ()
  {
    $styles_logic = Styles_Logic::getInstance ();
    $database_styles = Database_Styles::getInstance ();
    $add = $database_styles->getMarkerData ("Standard", "add");
    $odf_text = new Odf_Text ("phpunit");
    $odf_text->newParagraph ();
    $odf_text->addText ("Text");
    $odf_text->addNote ("𐌰", "f");
    $odf_text->openTextStyle ($add, true, false);
    $odf_text->addNoteText ("Add");
    $odf_text->closeTextStyle (true, false);
    $odf_text->addNoteText ("normal");
    $odf_text->addText (".");
    $odf_text->save ("/tmp/OdfTextTest.odt");
    $odt = shell_exec ("odt2txt /tmp/OdfTextTest.odt");
$standard = <<<'EOD'
Text𐌰

Addnormal

.
EOD;
    $this->assertEquals ($standard, trim ($odt));
  }


  public function testEmbeddedFormattedText ()
  {
    $styles_logic = Styles_Logic::getInstance ();
    $add = array ("marker" => "add", "italic" => ooitOn, "bold" => NULL, "underline" => NULL, "smallcaps" => NULL, "superscript" => false, "color" => "000000");
    $nd = array ("marker" => "nd", "italic" => NULL, "bold" => NULL, "underline" => NULL, "smallcaps" => ooitOn, "superscript" => false, "color" => "000000");
    $odf_text = new Odf_Text ("phpunit");
    $odf_text->newParagraph ();
    $odf_text->addText ("text");
    $odf_text->openTextStyle ($add, false, false);
    $odf_text->addText ("add");
    $odf_text->openTextStyle ($nd, false, true);
    $odf_text->addText ("nd");
    $odf_text->closeTextStyle (false, false);
    $odf_text->addText ("normal");
    $odf_text->addText (".");
    $odf_text->save ("/tmp/OdfTextTest.odt");
    $odt = shell_exec ("odt2txt /tmp/OdfTextTest.odt");
$standard = <<<'EOD'
textaddndnormal.
EOD;
    $this->assertEquals ($standard, trim ($odt));
  }


  public function testEmbeddedFormattedNote ()
  {
    $styles_logic = Styles_Logic::getInstance ();
    $add = array ("marker" => "add", "italic" => ooitOn, "bold" => NULL, "underline" => NULL, "smallcaps" => NULL, "superscript" => false, "color" => "000000");
    $nd = array ("marker" => "nd", "italic" => NULL, "bold" => NULL, "underline" => NULL, "smallcaps" => ooitOn, "superscript" => false, "color" => "000000");
    $odf_text = new Odf_Text ("phpunit");
    $odf_text->newParagraph ();
    $odf_text->addText ("text");
    $odf_text->addNote ("𐌰", "f");
    $odf_text->openTextStyle ($add, true, false);
    $odf_text->addNoteText ("add");
    $odf_text->openTextStyle ($nd, true, true);
    $odf_text->addNoteText ("nd");
    $odf_text->closeTextStyle (true, false);
    $odf_text->addNoteText ("normal");
    $odf_text->addText (".");
    $odf_text->save ("/tmp/OdfTextTest.odt");
    $odt = shell_exec ("odt2txt /tmp/OdfTextTest.odt");
$standard = <<<'EOD'
text𐌰

addndnormal

.
EOD;
    $this->assertEquals ($standard, trim ($odt));
  }



*/
}


// Tests for the filters in the filter folder.
void test_filters ()
{
  refresh_sandbox (true);
  test_filters_test1 ();
  test_filters_test2 ();
  test_filters_test3 ();
  test_filters_test4 ();
  test_filters_test5 ();
  test_filters_test6 ();
  test_filters_test7 ();
  test_filters_test8 ();
  test_filters_test9 ();
  test_filters_test10 ();
  refresh_sandbox (true);
}



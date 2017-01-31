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


#include <unittests/unittest.h>
#include <unittests/utilities.h>
#include <config/libraries.h>
#include <library/bibledit.h>
#include <database/config/user.h>
#include <database/styles.h>
#include <database/books.h>
#include <database/config/bible.h>
#include <database/modifications.h>
#include <database/state.h>
#include <database/git.h>
#include <database/login.h>
#include <database/logs.h>
#include <config/globals.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/roles.h>
#include <filter/usfm.h>
#include <filter/archive.h>
#include <filter/text.h>
#include <filter/css.h>
#include <filter/diff.h>
#include <filter/abbreviations.h>
#include <filter/git.h>
#include <filter/merge.h>
#include <filter/date.h>
#include <filter/shell.h>
#include <filter/memory.h>
#include <session/logic.h>
#include <text/text.h>
#include <esword/text.h>
#include <onlinebible/text.h>
#include <html/text.h>
#include <odf/text.h>
#include <styles/logic.h>
#include <styles/css.h>
#include <ipc/notes.h>
#include <client/logic.h>
#include <bible/logic.h>
#include <developer/logic.h>
#include <ldap/logic.h>


// Todo move into smaller units.


void test_filter_merge ()
{
  trace_unit_tests (__func__);
  
}


void test_filter_tidy ()
{
  trace_unit_tests (__func__);
  
  string folder = filter_url_create_root_path ("unittests", "tests");
  string html = filter_url_file_get_contents (filter_url_create_path (folder, "/biblehub-john-1-1.html"));
  vector <string> tidy = filter_string_explode (html_tidy (html), '\n');
  evaluate (__LINE__, __func__, 752, (int)tidy.size());
}


void test_ipc_notes ()
{
  trace_unit_tests (__func__);
  
  // Initialize.
  refresh_sandbox (true);
  Webserver_Request request;
  request.database_users ()->create ();
  request.session_logic ()->setUsername ("phpunit");

  // There should be no note identifier.
  int identifier = Ipc_Notes::get (&request);
  evaluate (__LINE__, __func__, 0, identifier);

  // Test opening note.
  Ipc_Notes::open (&request, 123456789);
  identifier = Ipc_Notes::get (&request);
  evaluate (__LINE__, __func__, 123456789, identifier);
  
  // Test trimming.
  request.database_ipc()->trim ();
  identifier = Ipc_Notes::get (&request);
  evaluate (__LINE__, __func__, 123456789, identifier);

  // Test deleting note once.
  Ipc_Notes::open (&request, 123456789);
  Ipc_Notes::erase (&request);
  identifier = Ipc_Notes::get (&request);
  evaluate (__LINE__, __func__, 0, identifier);

  // Test deleting two notes.
  Ipc_Notes::open (&request, 123456789);
  Ipc_Notes::open (&request, 123456789);
  Ipc_Notes::erase (&request);
  identifier = Ipc_Notes::get (&request);
  evaluate (__LINE__, __func__, 0, identifier);
  Ipc_Notes::erase (&request);
  identifier = Ipc_Notes::get (&request);
  evaluate (__LINE__, __func__, 0, identifier);
}


void test_filter_date ()
{
  trace_unit_tests (__func__);
  
  // First Business Day Of Month
  {
    // Sunday the 1st.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (1, 0));
    // Monday the 1st.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (1, 1));
    // Tuesday the 1st.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (1, 2));
    // Wednesday the 1st.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (1, 3));
    // Thirsday the 1st.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (1, 4));
    // Friday the 1st.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (1, 5));
    // Saturday the 1st.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (1, 6));
    // Sunday the 2nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (2, 0));
    // Monday the 2nd.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (2, 1));
    // Tuesday the 2nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (2, 2));
    // Sunday the 3nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (3, 0));
    // Monday the 3nd.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (3, 1));
    // Tuesday the 3nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (3, 2));
    // Sunday the 4nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (4, 0));
    // Monday the 4nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (4, 1));
    // Tuesday the 4nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (4, 2));
  }
  // Last Business Day Of Month
  {
    evaluate (__LINE__, __func__, 30, filter_date_get_last_business_day_of_month (2013, 9));
    evaluate (__LINE__, __func__, 31, filter_date_get_last_business_day_of_month (2013, 10));
    evaluate (__LINE__, __func__, 29, filter_date_get_last_business_day_of_month (2013, 11));
    evaluate (__LINE__, __func__, 31, filter_date_get_last_business_day_of_month (2013, 12));
    evaluate (__LINE__, __func__, 31, filter_date_get_last_business_day_of_month (2014, 1));
    evaluate (__LINE__, __func__, 28, filter_date_get_last_business_day_of_month (2014, 2));
    evaluate (__LINE__, __func__, 31, filter_date_get_last_business_day_of_month (2014, 3));
    evaluate (__LINE__, __func__, 30, filter_date_get_last_business_day_of_month (2014, 4));
    evaluate (__LINE__, __func__, 30, filter_date_get_last_business_day_of_month (2014, 5));
    evaluate (__LINE__, __func__, 30, filter_date_get_last_business_day_of_month (2014, 6));
  }
  // Is Business Day
  {
    evaluate (__LINE__, __func__, false, filter_date_is_business_day (2013, 9, 1));
    evaluate (__LINE__, __func__, true, filter_date_is_business_day (2013, 9, 2));
    evaluate (__LINE__, __func__, true, filter_date_is_business_day (2013, 9, 3));
    evaluate (__LINE__, __func__, true, filter_date_is_business_day (2013, 9, 4));
    evaluate (__LINE__, __func__, true, filter_date_is_business_day (2013, 9, 5));
    evaluate (__LINE__, __func__, true, filter_date_is_business_day (2013, 9, 6));
    evaluate (__LINE__, __func__, false, filter_date_is_business_day (2013, 9, 7));
    evaluate (__LINE__, __func__, false, filter_date_is_business_day (2013, 9, 8));
    evaluate (__LINE__, __func__, true, filter_date_is_business_day (2013, 9, 30));
    evaluate (__LINE__, __func__, false, filter_date_is_business_day (2015, 3, 1));
    evaluate (__LINE__, __func__, false, filter_date_is_business_day (2015, 2, 32));
    
  }
  // Seonds since Unix epoch.
  {
    int year, month, day, seconds;

    year = 2011;
    month = 2;
    day = 5;
    seconds = filter_date_seconds_since_epoch (year, month, day);
    evaluate (__LINE__, __func__, year, filter_date_numerical_year (seconds));
    evaluate (__LINE__, __func__, month, filter_date_numerical_month (seconds));
    evaluate (__LINE__, __func__, day, filter_date_numerical_month_day (seconds));

    year = 2015;
    month = 3;
    day = 15;
    seconds = filter_date_seconds_since_epoch (year, month, day);
    evaluate (__LINE__, __func__, year, filter_date_numerical_year (seconds));
    evaluate (__LINE__, __func__, month, filter_date_numerical_month (seconds));
    evaluate (__LINE__, __func__, day, filter_date_numerical_month_day (seconds));
    
    year = 2030;
    month = 12;
    day = 31;
    seconds = filter_date_seconds_since_epoch (year, month, day);
    evaluate (__LINE__, __func__, year, filter_date_numerical_year (seconds));
    evaluate (__LINE__, __func__, month, filter_date_numerical_month (seconds));
    evaluate (__LINE__, __func__, day, filter_date_numerical_month_day (seconds));
  }
}


void test_filter_string ()
{
  trace_unit_tests (__func__);
  
  {
    string input = "<span>Praise the LORD&#xB6;, all &amp; you nations</span>";
    string output = convert_xml_character_entities_to_characters (input);
    string standard = filter_string_str_replace ("&#xB6;", "¶", input);
    evaluate (__LINE__, __func__, standard, output);

    input = "<span>Praise the LORD &#x5D0; all you nations</span>";
    output = convert_xml_character_entities_to_characters (input);
    standard = filter_string_str_replace ("&#x5D0;", "א", input);
    evaluate (__LINE__, __func__, standard, output);
    
    input = "Username";
    output = encrypt_decrypt ("key", input);
    output = encrypt_decrypt ("key", output);
    evaluate (__LINE__, __func__, input, output);

    input = "בְּרֵאשִׁ֖ית בָּרָ֣א אֱלֹהִ֑ים אֵ֥ת הַשָּׁמַ֖יִם וְאֵ֥ת הָאָֽרֶץ";
    output = encrypt_decrypt ("בְּרֵאשִׁ֖ית", input);
    output = encrypt_decrypt ("בְּרֵאשִׁ֖ית", output);
    evaluate (__LINE__, __func__, input, output);
  }

  {
    string one = get_new_random_string ();
    this_thread::sleep_for (chrono::milliseconds (10));
    string two = get_new_random_string ();
    evaluate (__LINE__, __func__, 32, one.length ());
    evaluate (__LINE__, __func__, true, one != two);
  }

  {
    evaluate (__LINE__, __func__, 4, (int)unicode_string_length ("test"));
    evaluate (__LINE__, __func__, 4, (int)unicode_string_length ("ᨁᨃᨅᨕ"));
  }

  {
    string hebrew = "אָבּגּדּהּ";
    evaluate (__LINE__, __func__, "st1234", unicode_string_substr ("test1234", 2));
    evaluate (__LINE__, __func__, "גּדּהּ", unicode_string_substr (hebrew, 2));
    evaluate (__LINE__, __func__, "", unicode_string_substr (hebrew, 5));
    evaluate (__LINE__, __func__, "", unicode_string_substr (hebrew, 6));
    evaluate (__LINE__, __func__, "test", unicode_string_substr ("test123456", 0, 4));
    evaluate (__LINE__, __func__, "12", unicode_string_substr ("test123456", 4, 2));
    evaluate (__LINE__, __func__, "גּדּ", unicode_string_substr (hebrew, 2, 2));
    evaluate (__LINE__, __func__, "גּדּהּ", unicode_string_substr (hebrew, 2, 10));
  }
  
  {
    string hebrew = "אָבּגּדּהּ";
    string needle = "דּ";
    evaluate (__LINE__, __func__, 3, (int)unicode_string_strpos ("012345", "3"));
    evaluate (__LINE__, __func__, 5, (int)unicode_string_strpos ("012345", "5"));
    evaluate (__LINE__, __func__, 0, (int)unicode_string_strpos ("012345", "0"));
    evaluate (__LINE__, __func__, -1, (int)unicode_string_strpos ("012345", "6"));
    evaluate (__LINE__, __func__, 3, (int)unicode_string_strpos (hebrew, needle));
    evaluate (__LINE__, __func__, 3, (int)unicode_string_strpos (hebrew, needle, 3));
    evaluate (__LINE__, __func__, -1, (int)unicode_string_strpos (hebrew, needle, 4));
    evaluate (__LINE__, __func__, -1, (int)unicode_string_strpos ("", "3"));
  }
  
  {
    evaluate (__LINE__, __func__, 2, (int)unicode_string_strpos_case_insensitive ("AbCdEf", "c"));
    evaluate (__LINE__, __func__, 2, (int)unicode_string_strpos_case_insensitive ("AbCdEf", "cD"));
    evaluate (__LINE__, __func__, -1, (int)unicode_string_strpos_case_insensitive ("AbCdEf", "ce"));
  }
  
  {
    evaluate (__LINE__, __func__, "test1234", unicode_string_casefold ("test1234"));
    evaluate (__LINE__, __func__, "test1234", unicode_string_casefold ("TEST1234"));
    evaluate (__LINE__, __func__, "θεος", unicode_string_casefold ("Θεος"));
    evaluate (__LINE__, __func__, "α α β β", unicode_string_casefold ("Α α Β β"));
    evaluate (__LINE__, __func__, "אָבּגּדּהּ", unicode_string_casefold ("אָבּגּדּהּ"));
  }

  {
    evaluate (__LINE__, __func__, "TEST1234", unicode_string_uppercase ("test1234"));
    evaluate (__LINE__, __func__, "TEST1234", unicode_string_uppercase ("TEST1234"));
    evaluate (__LINE__, __func__, "ΘΕΟΣ", unicode_string_uppercase ("Θεος"));
    evaluate (__LINE__, __func__, "Α Α Β Β", unicode_string_uppercase ("Α α Β β"));
    evaluate (__LINE__, __func__, "אָבּגּדּהּ", unicode_string_uppercase ("אָבּגּדּהּ"));
  }

  {
    evaluate (__LINE__, __func__, "ABCDEFG", unicode_string_transliterate ("ABCDEFG"));
    evaluate (__LINE__, __func__, "Ιησου Χριστου", unicode_string_transliterate ("Ἰησοῦ Χριστοῦ"));
    evaluate (__LINE__, __func__, "אבגדה", unicode_string_transliterate ("אָבּגּדּהּ"));
  }
  
  {
    vector <string> needles;
    needles = filter_string_search_needles ("ABC", "one abc two ABc three aBc four");
    evaluate (__LINE__, __func__, { "abc", "ABc", "aBc" }, needles);
    needles = filter_string_search_needles ("abc", "one abc two ABc three aBc four");
    evaluate (__LINE__, __func__, { "abc", "ABc", "aBc" }, needles);
  }
  
  {
    evaluate (__LINE__, __func__, false, unicode_string_is_punctuation ("A"));
    evaluate (__LINE__, __func__, false, unicode_string_is_punctuation ("z"));
    evaluate (__LINE__, __func__, true, unicode_string_is_punctuation ("."));
    evaluate (__LINE__, __func__, true, unicode_string_is_punctuation (","));
  }
  
  {
    evaluate (__LINE__, __func__, false, convert_to_bool ("0"));
    evaluate (__LINE__, __func__, false, convert_to_bool ("false"));
    evaluate (__LINE__, __func__, false, convert_to_bool ("FALSE"));
    evaluate (__LINE__, __func__, true, convert_to_bool ("1"));
    evaluate (__LINE__, __func__, true, convert_to_bool ("true"));
    evaluate (__LINE__, __func__, true, convert_to_bool ("TRUE"));
  }
  
  {
    evaluate (__LINE__, __func__, 21109, unicode_string_convert_to_codepoint ("創"));
    evaluate (__LINE__, __func__, 97, unicode_string_convert_to_codepoint ("a"));
  }
  
  {
    // Check that the function to desanitize html no longer corrupts UTF-8.
    string html = "<p>“Behold”, from “הִנֵּה”, means look at</p>";
    string desanitized = filter_string_desanitize_html (html);
    evaluate (__LINE__, __func__, html, desanitized);
  }
}


void test_filter_shell ()
{
  trace_unit_tests (__func__);
  
  evaluate (__LINE__, __func__, true, filter_shell_is_present ("zip"));
  evaluate (__LINE__, __func__, false, filter_shell_is_present ("xxxxx"));
  
  string output;
  int result;

  result = filter_shell_vfork (output, "", "ls", "-l");
  evaluate (__LINE__, __func__, 0, result);
  if (output.find ("unittest") == string::npos) {
    evaluate (__LINE__, __func__, "Supposed to list files", output);
  }

  result = filter_shell_vfork (output, "/", "ls", "-l");
  evaluate (__LINE__, __func__, 0, result);
  if (output.find ("tmp") == string::npos) {
    evaluate (__LINE__, __func__, "Supposed to list folder /", output);
  }
}


void test_filter_memory ()
{
  trace_unit_tests (__func__);
  
  int available = filter_memory_percentage_available ();
  (void) available;
}


void test_filter_ldap ()
{
  trace_unit_tests (__func__);
  
  refresh_sandbox (true);

  // Copy the default LDAP server configuration into place.
  string ldap_txt = filter_url_create_root_path ("config", "ldap.txt");
  string ldap_conf = filter_url_create_root_path ("config", "ldap.conf");
  filter_url_file_cp (ldap_txt, ldap_conf);
  // Initialize LDAP configuration.
  ldap_logic_initialize ();
  
  // Authenticate a user and check the results.
  string user = "boyle";
  string password = "password";
  bool okay;
  bool access;
  string email;
  int role;
  okay = ldap_logic_fetch (user, password, access, email, role, false);
  evaluate (__LINE__, __func__, true, okay);
  evaluate (__LINE__, __func__, true, access);
  evaluate (__LINE__, __func__, "boyle@ldap.forumsys.com", email);
  evaluate (__LINE__, __func__, Filter_Roles::guest (), role);

  // Check there is one journal entry as a result of authenticating a user.
  string last = "0";
  vector <string> logs = Database_Logs::get (last);
  evaluate (__LINE__, __func__, 1, logs.size ());

  // Clear LDAP settings.
  ldap_logic_clear ();
  refresh_sandbox (false);
}


void test_filter_dev ()
{
  trace_unit_tests (__func__);
}

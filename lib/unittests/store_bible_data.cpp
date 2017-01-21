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


#include <unittests/store_bible_data.h>
#include <unittests/utilities.h>
#include <webserver/request.h>
#include <database/state.h>
#include <database/login.h>
#include <filter/usfm.h>
#include <filter/string.h>


void test_store_bible_data_safely_setup (Webserver_Request * request, string usfm)
{
  refresh_sandbox (true);
  Database_State::create ();
  Database_Login::create ();
  request->database_bibles()->createBible ("phpunit");
  request->database_bibles()->storeChapter ("phpunit", 1, 1, usfm);
}


void test_store_bible_data ()
{
  trace_unit_tests (__func__);
  
  Webserver_Request request;
  test_store_bible_data_safely_setup (&request, "");
  request.database_users ()->create ();
  request.session_logic ()->setUsername ("phpunit");
  string usfm =
  "\\c 1\n"
  "\\p\n"
  "\\v 1 Verse 1.\n"
  "\\v 2 Verse 2.\n"
  "\\p\n"
  "\\v 3 Verse 3.\n"
  "\\v 4 Verse 4.\n"
  "\\v 5 Verse 5.";
  {
    // Safely store a chapter.
    test_store_bible_data_safely_setup (&request, usfm);
    string data =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 Verse 1.\n"
    "\\v 2 Verse 2.\n"
    "\\v 3 Verse 3.\n"
    "\\v 4 Verse 4.\n"
    "\\v 5 Verse 5.";
    string explanation;
    string stored = usfm_safely_store_chapter (&request, "phpunit", 1, 1, data, explanation);
    evaluate (__LINE__, __func__, "", stored);
    evaluate (__LINE__, __func__, "", explanation);
    string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
    evaluate (__LINE__, __func__, data, result);
    refresh_sandbox (false);
  }
  {
    // Safely store chapter.
    test_store_bible_data_safely_setup (&request, usfm);
    string data =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 Verse 1.\n"
    "\\v 2 Verse 2.\n"
    "\\p\n"
    "\\v 3 Verse 3.\n"
    "\\v 4 Verse 4.";
    string explanation;
    string stored = usfm_safely_store_chapter (&request, "phpunit", 1, 1, data, explanation);
    evaluate (__LINE__, __func__, "", stored);
    evaluate (__LINE__, __func__, "", explanation);
    string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
    evaluate (__LINE__, __func__, data, result);
    refresh_sandbox (false);
  }
  {
    // Safely store chapter with length error
    test_store_bible_data_safely_setup (&request, usfm);
    string data =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 Verse 1.\n"
    "\\v 2 Verse 2.\n"
    "\\v 3 Verse 3.\n";
    string explanation;
    string stored = usfm_safely_store_chapter (&request, "phpunit", 1, 1, data, explanation);
    evaluate (__LINE__, __func__, "Text length differs too much", stored);
    evaluate (__LINE__, __func__, "The text was not saved for safety reasons. The length differs 37% from the existing text. Make smaller changes and save more often. Or relax the restriction in the editing settings. See menu Settings - Personalize.", explanation);
    string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
    evaluate (__LINE__, __func__, usfm, result);
    refresh_sandbox (false);
  }
  // SafeStoreChapterSimilarity
  {
    test_store_bible_data_safely_setup (&request, usfm);
    string data =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 Verse 1.\n"
    "\\v 3 Verse 3.\n"
    "\\v 2 Verse 2.\n"
    "\\v 4 Verse 4.\n";
    string explanation;
    string stored = usfm_safely_store_chapter (&request, "phpunit", 1, 1, data, explanation);
    evaluate (__LINE__, __func__, "Text content differs too much", stored);
    evaluate (__LINE__, __func__, "The text was not saved for safety reasons. The new text is 54% similar to the existing text. Make smaller changes and save more often. Or relax the restriction in the editing settings. See menu Settings - Personalize.", explanation);
    string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
    evaluate (__LINE__, __func__, usfm, result);
    refresh_sandbox (false);
  }
  // SafeStoreChapterNoChange
  {
    test_store_bible_data_safely_setup (&request, usfm);
    string explanation;
    int currentId = request.database_bibles()->getChapterId ("phpunit", 1, 1);
    string stored = usfm_safely_store_chapter (&request, "phpunit", 1, 1, usfm, explanation);
    evaluate (__LINE__, __func__, "", stored);
    evaluate (__LINE__, __func__, "", explanation);
    string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
    evaluate (__LINE__, __func__, usfm, result);
    int currentId2 = request.database_bibles()->getChapterId ("phpunit", 1, 1);
    evaluate (__LINE__, __func__, currentId, currentId2);
    refresh_sandbox (false);
  }
  // Safely store verse 0 without a change.
  {
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data =
      "\\c 1\n"
      "\\p\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 0, data, explanation, false);
      evaluate (__LINE__, __func__, "", stored);
      evaluate (__LINE__, __func__, "", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      evaluate (__LINE__, __func__, usfm, result);
      refresh_sandbox (false);
    }
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data =
      "\\c 1\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 0, data, explanation, true);
      evaluate (__LINE__, __func__, "", stored);
      evaluate (__LINE__, __func__, "", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      evaluate (__LINE__, __func__, usfm, result);
      refresh_sandbox (false);
    }
  }
  // Safely store verse 0 with a change.
  {
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data =
      "\\c 1\n"
      "\\p xx\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 0, data, explanation, false);
      evaluate (__LINE__, __func__, "", stored);
      evaluate (__LINE__, __func__, "", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      string newusfm = filter_string_str_replace ("\\p\n\\v 1", "\\p xx\n\\v 1", usfm);
      evaluate (__LINE__, __func__, newusfm, result);
      refresh_sandbox (false);
    }
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data =
      "\\c 1x\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 0, data, explanation, true);
      evaluate (__LINE__, __func__, "", stored);
      evaluate (__LINE__, __func__, "", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      string newusfm = filter_string_str_replace ("c 1", "c 1x", usfm);
      evaluate (__LINE__, __func__, newusfm, result);
      refresh_sandbox (false);
    }
  }
  // Safely store verse 2 with a change.
  {
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data = "\\v 2 Verse two.\n\\p\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 2, data, explanation, false);
      evaluate (__LINE__, __func__, "", stored);
      evaluate (__LINE__, __func__, "", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      string newusfm = filter_string_str_replace ("Verse 2", "Verse two", usfm);
      evaluate (__LINE__, __func__, newusfm, result);
      refresh_sandbox (false);
    }
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data = "\\v 2 Verse two.\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 2, data, explanation, true);
      evaluate (__LINE__, __func__, "", stored);
      evaluate (__LINE__, __func__, "", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      string newusfm = filter_string_str_replace ("Verse 2", "Verse two", usfm);
      evaluate (__LINE__, __func__, newusfm, result);
      refresh_sandbox (false);
    }
  }
  // Safely store verse 3 with a change.
  {
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data = "\\v 3 Verse three.\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 3, data, explanation, false);
      evaluate (__LINE__, __func__, "", stored);
      evaluate (__LINE__, __func__, "", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      string newusfm = filter_string_str_replace ("Verse 3", "Verse three", usfm);
      evaluate (__LINE__, __func__, newusfm, result);
      refresh_sandbox (false);
    }
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data = "\\p\n\\v 3 Verse three.\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 3, data, explanation, true);
      evaluate (__LINE__, __func__, "", stored);
      evaluate (__LINE__, __func__, "", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      string newusfm = filter_string_str_replace ("Verse 3", "Verse three", usfm);
      evaluate (__LINE__, __func__, newusfm, result);
      refresh_sandbox (false);
    }
  }
  // Safely store the USFM for verse two to verse one: Fails.
  {
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data = "\\v 2 Verse 2.\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 1, data, explanation, false);
      evaluate (__LINE__, __func__, "Verse mismatch", stored);
      evaluate (__LINE__, __func__, "The USFM contains verse(s) 2 while it wants to save to verse 1", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      evaluate (__LINE__, __func__, usfm, result);
      refresh_sandbox (false);
    }
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data = "\\v 2 Verse 2.\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 1, data, explanation, true);
      evaluate (__LINE__, __func__, "Verse mismatch", stored);
      evaluate (__LINE__, __func__, "The USFM contains verse(s) 2 while it wants to save to verse 1", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      evaluate (__LINE__, __func__, usfm, result);
      refresh_sandbox (false);
    }
  }
  // Safely store a verse with too much length difference: Fails.
  {
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data = "\\v 2 Verse two two to to two.\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 2, data, explanation, false);
      evaluate (__LINE__, __func__, "Text length differs too much", stored);
      evaluate (__LINE__, __func__, "The text was not saved for safety reasons. The length differs 81% from the existing text. Make smaller changes and save more often. Or relax the restriction in the editing settings. See menu Settings - Personalize.", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      evaluate (__LINE__, __func__, usfm, result);
      refresh_sandbox (false);
    }
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data = "\\v 2 Verse two two two.\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 2, data, explanation, true);
      evaluate (__LINE__, __func__, "Text length differs too much", stored);
      evaluate (__LINE__, __func__, "The text was not saved for safety reasons. The length differs 76% from the existing text. Make smaller changes and save more often. Or relax the restriction in the editing settings. See menu Settings - Personalize.", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      evaluate (__LINE__, __func__, usfm, result);
      refresh_sandbox (false);
    }
  }
  // Safely store a verse with too much of content difference: Fails.
  {
    {
      test_store_bible_data_safely_setup (&request, usfm);
      request.database_config_user ()->setEditingAllowedDifferenceVerse (40);
      string data = "\\v 2 vERSE 2.\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 2, data, explanation, false);
      evaluate (__LINE__, __func__, "Text content differs too much", stored);
      evaluate (__LINE__, __func__, "The text was not saved for safety reasons. The new text is 38% similar to the existing text. Make smaller changes and save more often. Or relax the restriction in the editing settings. See menu Settings - Personalize.", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      evaluate (__LINE__, __func__, usfm, result);
      refresh_sandbox (false);
    }
    {
      test_store_bible_data_safely_setup (&request, usfm);
      request.database_config_user ()->setEditingAllowedDifferenceVerse (40);
      string data = "\\v 2 vERSE 2.\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 2, data, explanation, true);
      evaluate (__LINE__, __func__, "Text content differs too much", stored);
      evaluate (__LINE__, __func__, "The text was not saved for safety reasons. The new text is 44% similar to the existing text. Make smaller changes and save more often. Or relax the restriction in the editing settings. See menu Settings - Personalize.", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      evaluate (__LINE__, __func__, usfm, result);
      refresh_sandbox (false);
    }
  }
  // Safely store USFM without any verse to verse 2: Fails.
  {
    {
      test_store_bible_data_safely_setup (&request, usfm);
      request.database_config_user ()->setEditingAllowedDifferenceVerse (40);
      string data = "\\p Verse 2.\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 2, data, explanation, false);
      evaluate (__LINE__, __func__, "Missing verse number", stored);
      evaluate (__LINE__, __func__, "The USFM contains no verse information", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      evaluate (__LINE__, __func__, usfm, result);
      refresh_sandbox (false);
    }
    {
      test_store_bible_data_safely_setup (&request, usfm);
      request.database_config_user ()->setEditingAllowedDifferenceVerse (40);
      string data = "\\p Verse 2.\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 2, data, explanation, true);
      evaluate (__LINE__, __func__, "Missing verse number", stored);
      evaluate (__LINE__, __func__, "The USFM contains no verse information", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      evaluate (__LINE__, __func__, usfm, result);
      refresh_sandbox (false);
    }
  }
  // Safely store USFM with two verses: Fails.
  {
    {
      test_store_bible_data_safely_setup (&request, usfm);
      request.database_config_user ()->setEditingAllowedDifferenceVerse (40);
      string data = "\\v 2 Verse 2.\n\\v 3 3";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 2, data, explanation, false);
      evaluate (__LINE__, __func__, "Cannot overwrite another verse", stored);
      evaluate (__LINE__, __func__, "The USFM contains verse(s) 0 2 3 which would overwrite a fragment that contains verse(s) 0 2", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      evaluate (__LINE__, __func__, usfm, result);
      refresh_sandbox (false);
    }
    {
      test_store_bible_data_safely_setup (&request, usfm);
      request.database_config_user ()->setEditingAllowedDifferenceVerse (40);
      string data = "\\v 2 Verse 2.\n\\v 3 3";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 2, data, explanation, true);
      evaluate (__LINE__, __func__, "Cannot overwrite another verse", stored);
      evaluate (__LINE__, __func__, "The USFM contains verse(s) 0 2 3 which would overwrite a fragment that contains verse(s) 0 2", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      evaluate (__LINE__, __func__, usfm, result);
      refresh_sandbox (false);
    }
  }
  // The USFM is going to have combined verses.
  usfm =
  "\\c 1\n"
  "\\p\n"
  "\\v 1 Verse 1.\n"
  "\\v 2-3 Verse 2 and 3.\n"
  "\\p\n"
  "\\v 4-5 Verse 4 and 5.\n"
  "\\v 6 Verse 6.";
  // Safely store combined verse without any change.
  {
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data = "\\v 2-3 Verse 2 and 3.\n\\p\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 2, data, explanation, false);
      evaluate (__LINE__, __func__, "", stored);
      evaluate (__LINE__, __func__, "", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      evaluate (__LINE__, __func__, usfm, result);
    }
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data = "\\v 2-3 Verse 2 and 3.\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 2, data, explanation, true);
      evaluate (__LINE__, __func__, "", stored);
      evaluate (__LINE__, __func__, "", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      evaluate (__LINE__, __func__, usfm, result);
    }
  }
  // Safely store combined verse before the \p with a change.
  {
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data = "\\v 2-3 Verse 2 andx 3.\n\\p\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 3, data, explanation, false);
      evaluate (__LINE__, __func__, "", stored);
      evaluate (__LINE__, __func__, "", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      string newusfm = filter_string_str_replace ("2 and 3", "2 andx 3", usfm);
      evaluate (__LINE__, __func__, newusfm, result);
      refresh_sandbox (false);
    }
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data = "\\v 2-3 Verse 2 andx 3.\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 3, data, explanation, true);
      evaluate (__LINE__, __func__, "", stored);
      evaluate (__LINE__, __func__, "", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      string newusfm = filter_string_str_replace ("2 and 3", "2 andx 3", usfm);
      evaluate (__LINE__, __func__, newusfm, result);
      refresh_sandbox (false);
    }
  }
  // Safely store combined verse after the \p with a change.
  {
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data = "\\v 4-5 Verse 4 andx 5.\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 4, data, explanation, false);
      evaluate (__LINE__, __func__, "", stored);
      evaluate (__LINE__, __func__, "", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      string newusfm = filter_string_str_replace ("4 and 5", "4 andx 5", usfm);
      evaluate (__LINE__, __func__, newusfm, result);
      refresh_sandbox (false);
    }
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data = "\\p\n\\v 4-5 Verse 4 andx 5.\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 4, data, explanation, true);
      evaluate (__LINE__, __func__, "", stored);
      evaluate (__LINE__, __func__, "", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      string newusfm = filter_string_str_replace ("4 and 5", "4 andx 5", usfm);
      evaluate (__LINE__, __func__, newusfm, result);
      refresh_sandbox (false);
    }
  }
  // Safely store combined verse with a change and wrong verses: Fails.
  {
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data = "\\v 2-4 Verse 2 andx 3.\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 3, data, explanation, false);
      evaluate (__LINE__, __func__, "Cannot overwrite another verse", stored);
      evaluate (__LINE__, __func__, "The USFM contains verse(s) 0 2 3 4 which would overwrite a fragment that contains verse(s) 0 2 3", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      evaluate (__LINE__, __func__, usfm, result);
      refresh_sandbox (false);
    }
    {
      test_store_bible_data_safely_setup (&request, usfm);
      string data = "\\v 2-4 Verse 2 andx 3.\n";
      string explanation;
      string stored = usfm_safely_store_verse (&request, "phpunit", 1, 1, 3, data, explanation, true);
      evaluate (__LINE__, __func__, "Cannot overwrite another verse", stored);
      evaluate (__LINE__, __func__, "The USFM contains verse(s) 0 2 3 4 which would overwrite a fragment that contains verse(s) 0 2 3", explanation);
      string result = request.database_bibles()->getChapter ("phpunit", 1, 1);
      evaluate (__LINE__, __func__, usfm, result);
      refresh_sandbox (false);
    }
  }
}

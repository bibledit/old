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


#include <unittests/libraries.h>
#include <unittests/utilities.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <filter/md5.h>
#include <filter/usfm.h>
#include <filter/diff.h>
#include <filter/memory.h>
#include <filter/date.h>
#include <flate/flate.h>
#include <config/globals.h>
#include <database/config/general.h>
#include <database/config/bible.h>
#include <database/logs.h>
#include <database/sqlite.h>
#include <database/users.h>
#include <database/books.h>
#include <database/versifications.h>
#include <database/state.h>
#include <database/login.h>
#include <database/notes.h>
#include <database/modifications.h>
#include <checksum/logic.h>
#include <editor/html2usfm.h>
#include <editor/usfm2html.h>
#include <workspace/logic.h>
#include <client/logic.h>
#include <styles/logic.h>
#include <checks/sentences.h>
#include <checks/versification.h>
#include <checks/usfm.h>
#include <checks/verses.h>
#include <checks/pairs.h>
#include <manage/hyphenate.h>
#include <search/logic.h>
#include <jsonxx/jsonxx.h>
#include <related/logic.h>
#include <editone/logic.h>
#include <webserver/http.h>
#include <demo/logic.h>
#include <search/rebibles.h>
#include <search/renotes.h>
#include <changes/modifications.h>
#include <checks/run.h>
#include <export/usfm.h>
#include <export/textusfm.h>
#include <export/odt.h>
#include <export/esword.h>
#include <export/onlinebible.h>
#include <export/quickbible.h>
#include <sword/logic.h>
#include <tasks/logic.h>
#include <resource/logic.h>
#include <user/logic.h>
#include <rss/logic.h>


using namespace jsonxx;


// Test the logic used in the visual verse editor.
void test_editone_logic ()
{
  trace_unit_tests (__func__);

  string stylesheet = styles_logic_standard_sheet ();
  string directory = filter_url_create_root_path ("unittests", "tests");

  // Prefix.
  {
    string usfm = filter_url_file_get_contents (filter_url_create_path (directory, "edit_one_1.usfm"));
    string html;
    string last_paragraph_style;
    editone_logic_prefix_html_stage_one (usfm, stylesheet, html, last_paragraph_style);
    string standard = filter_url_file_get_contents (filter_url_create_path (directory, "edit_one_1.html"));
    evaluate (__LINE__, __func__, standard, html);
    evaluate (__LINE__, __func__, "p", last_paragraph_style);
  }
  
  // Editable verse text.
  {
    // Convert USFM to html.
    string usfm = filter_url_file_get_contents (filter_url_create_path (directory, "edit_one_2.usfm"));
    string html;
    string last_paragraph_style;
    string focused_verse_applied_p_style;
    editone_old_logic_editable_html ("p", usfm, stylesheet, html, last_paragraph_style, focused_verse_applied_p_style);
    string standard = filter_url_file_get_contents (filter_url_create_path (directory, "edit_one_2.html"));
    evaluate (__LINE__, __func__, standard, html);
    evaluate (__LINE__, __func__, "p", last_paragraph_style);
    evaluate (__LINE__, __func__, "p", focused_verse_applied_p_style);
    
    // Convert the html back to USFM again.
    string round_tripped_usfm = editone_old_logic_html_to_usfm (stylesheet, html, focused_verse_applied_p_style);
    evaluate (__LINE__, __func__, usfm, round_tripped_usfm);
  }
  
  // Editable verse text including a \b.
  {
    // Convert USFM to html.
    string usfm = filter_url_file_get_contents (filter_url_create_path (directory, "edit_one_4.usfm"));
    string html;
    string last_paragraph_style;
    string focused_verse_applied_p_style;
    editone_old_logic_editable_html ("p", usfm, stylesheet, html, last_paragraph_style, focused_verse_applied_p_style);
    string standard = filter_url_file_get_contents (filter_url_create_path (directory, "edit_one_4.html"));
    evaluate (__LINE__, __func__, standard, html);
    evaluate (__LINE__, __func__, "q1", last_paragraph_style);
    evaluate (__LINE__, __func__, "p", focused_verse_applied_p_style);
    
    // Convert the html back to USFM again.
    string round_tripped_usfm = editone_old_logic_html_to_usfm (stylesheet, html, focused_verse_applied_p_style);
    evaluate (__LINE__, __func__, usfm, round_tripped_usfm);
  }

  // Suffix.
  {
    string usfm = filter_url_file_get_contents (filter_url_create_path (directory, "edit_one_3.usfm"));
    string html;
    string last_paragraph_style;
    editone_logic_suffix_html ("q1", usfm, stylesheet, html);
    string standard = filter_url_file_get_contents (filter_url_create_path (directory, "edit_one_3.html"));
    evaluate (__LINE__, __func__, standard, html);
    evaluate (__LINE__, __func__, "", last_paragraph_style);
  }
  
  // Removing notes from the prefix and appending them to the notes in the suffix.
  {
    string prefix;
    string suffix;
    string standard;
    prefix = filter_url_file_get_contents (filter_url_create_path (directory, "editone1prefix1.html"));
    suffix = filter_url_file_get_contents (filter_url_create_path (directory, "editone1suffix1.html"));
    editone_logic_move_notes (prefix, suffix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone1prefix2.html"));
    evaluate (__LINE__, __func__, standard, prefix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone1suffix2.html"));
    evaluate (__LINE__, __func__, standard, suffix);
  }

  // Prefix without notes, so moving nothing to the notes in the suffix.
  {
    string prefix;
    string suffix;
    string standard;
    prefix = filter_url_file_get_contents (filter_url_create_path (directory, "editone2prefix1.html"));
    suffix = filter_url_file_get_contents (filter_url_create_path (directory, "editone2suffix1.html"));
    editone_logic_move_notes (prefix, suffix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone2prefix2.html"));
    evaluate (__LINE__, __func__, standard, prefix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone2suffix2.html"));
    evaluate (__LINE__, __func__, standard, suffix);
  }
  
  // Move notes from the prefix to a suffix that does not have notes of its own.
  {
    string prefix;
    string suffix;
    string standard;
    prefix = filter_url_file_get_contents (filter_url_create_path (directory, "editone3prefix1.html"));
    suffix = filter_url_file_get_contents (filter_url_create_path (directory, "editone3suffix1.html"));
    editone_logic_move_notes (prefix, suffix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone3prefix2.html"));
    evaluate (__LINE__, __func__, standard, prefix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone3suffix2.html"));
    evaluate (__LINE__, __func__, standard, suffix);
  }
  
  // Test that a empty prefix works fine when trying to move notes from prefix to suffix.
  {
    string prefix;
    string suffix;
    string standard;
    prefix = filter_url_file_get_contents (filter_url_create_path (directory, "editone4prefix1.html"));
    suffix = filter_url_file_get_contents (filter_url_create_path (directory, "editone4suffix1.html"));
    editone_logic_move_notes (prefix, suffix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone4prefix2.html"));
    evaluate (__LINE__, __func__, standard, prefix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone4suffix2.html"));
    evaluate (__LINE__, __func__, standard, suffix);
  }
  
  // Test that notes from the prefix get moved even to an empty suffix.
  {
    string prefix;
    string suffix;
    string standard;
    prefix = filter_url_file_get_contents (filter_url_create_path (directory, "editone5prefix1.html"));
    suffix = filter_url_file_get_contents (filter_url_create_path (directory, "editone5suffix1.html"));
    editone_logic_move_notes (prefix, suffix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone5prefix2.html"));
    evaluate (__LINE__, __func__, standard, prefix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone5suffix2.html"));
    evaluate (__LINE__, __func__, standard, suffix);
  }
}


void test_http ()
{
  trace_unit_tests (__func__);

  string host, line;
  
  line = "192.168.1.139:8080";
  host = http_parse_host (line);
  evaluate (__LINE__, __func__, "192.168.1.139", host);

  line = "localhost:8080";
  host = http_parse_host (line);
  evaluate (__LINE__, __func__, "localhost", host);

  line = "[::1]:8080";
  host = http_parse_host (line);
  evaluate (__LINE__, __func__, "[::1]", host);

  line = "[fe80::601:25ff:fe07:6801]:8080";
  host = http_parse_host (line);
  evaluate (__LINE__, __func__, "[fe80::601:25ff:fe07:6801]", host);
}


bool test_memory_run = false;
uint64_t max_memory_usage = 0;
void test_memory_record ()
{
  uint64_t maximum = 0;
  do {
    uint64_t total = filter_memory_total_usage ();
    if (total > maximum) maximum = total;
    this_thread::sleep_for (chrono::microseconds (10));
  } while (test_memory_run);
  max_memory_usage = maximum;
}


void test_memory ()
{
  return;
  
  // Measure maximum memory usage of tasks that normally run in the background.
  trace_unit_tests (__func__);

  // Creating search index for one Bible.
  refresh_sandbox (false);
  {
    Database_State::create ();
    demo_create_sample_bible ();
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    search_reindex_bibles (true);
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " search_reindex_bibles" << endl;
  }

  // Creating search index consultation notes.
  refresh_sandbox (false);
  {
    Database_State::create ();
    Database_Notes database_notes (NULL);
    database_notes.create ();
    for (int i = 0; i < 100; i++) {
      database_notes.storeNewNote ("bible", i, i, i, "summary", "contents", true);
    }
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    search_reindex_notes ();
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " search_reindex_notes" << endl;
  }
  
  // Generating change notifications.
  refresh_sandbox (false);
  {
    Database_State::create ();
    Database_Users database_users;
    database_users.create ();
    Database_Login::create ();
    demo_create_sample_bible ();
    Webserver_Request request;
    request.session_logic ()->setUsername ("admin");
    request.database_config_user ()->setGenerateChangeNotifications (true);
    request.database_config_user ()->setUserChangesNotification (true);
    request.database_config_user ()->setUserChangesNotificationsOnline (true);
    request.database_config_user ()->setContributorChangesNotificationsOnline (true);
    Database_Bibles database_bibles;
    Database_Modifications database_modifications;
    string bible = demo_sample_bible_name ();
    for (int book = 1; book <= 1; book++) {
      for (int chapter = 1; chapter <= 1; chapter++) {
        string usfm = database_bibles.getChapter (bible, book, chapter);
        usfm = filter_string_str_replace ("the", "THE", usfm);
        database_modifications.storeTeamDiff (bible, book, chapter);
        database_bibles.storeChapter (bible, book, chapter, usfm);
      }
    }
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    changes_modifications ();
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    uint64_t after_usage = filter_memory_total_usage ();
    cout << max_memory_usage - basic_usage << " changes_modifications, leakage " << after_usage - basic_usage << endl;
  }

  // Running checks.
  refresh_sandbox (false);
  {
    Database_State::create ();
    demo_create_sample_bible ();
    string bible = demo_sample_bible_name ();
    Database_Config_Bible::setCheckDoubleSpacesUsfm (bible, true);
    Database_Config_Bible::setCheckFullStopInHeadings (bible, true);
    Database_Config_Bible::setCheckSpaceBeforePunctuation (bible, true);
    Database_Config_Bible::setCheckSentenceStructure (bible, true);
    Database_Config_Bible::setCheckParagraphStructure (bible, true);
    Database_Config_Bible::setCheckBooksVersification (bible, true);
    Database_Config_Bible::setCheckChaptesVersesVersification (bible, true);
    Database_Config_Bible::setCheckWellFormedUsfm (bible, true);
    Database_Config_Bible::setCheckMissingPunctuationEndVerse (bible, true);
    Database_Config_Bible::setCheckPatterns (bible, true);
    Database_Config_Bible::setCheckMatchingPairs (bible, true);
    Database_Check database_check;
    database_check.create ();
    Database_Users database_users;
    database_users.create ();
    Database_Versifications database_versifications;
    database_versifications.create ();
    database_versifications.defaults ();
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    checks_run (bible);
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " checks_run" << endl;
  }

  // Exporting Bible to USFM.
  refresh_sandbox (false);
  {
    Database_State::create ();
    demo_create_sample_bible ();
    string bible = demo_sample_bible_name ();
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    export_usfm (bible, false);
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " export_usfm" << endl;
  }

  // Exporting Bible book to USFM and plain text.
  refresh_sandbox (false);
  {
    Database_State::create ();
    demo_create_sample_bible ();
    string bible = demo_sample_bible_name ();
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    export_text_usfm_book (bible, 1, false);
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " export_text_usfm_book" << endl;
  }

  // Exporting Bible book to Open Document.
  refresh_sandbox (false);
  {
    Database_State::create ();
    demo_create_sample_bible ();
    string bible = demo_sample_bible_name ();
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    export_odt_book (bible, 1, false);
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " export_odt_book" << endl;
  }

  // Exporting Bible to eSword.
  refresh_sandbox (false);
  {
    Database_State::create ();
    demo_create_sample_bible ();
    string bible = demo_sample_bible_name ();
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    export_esword (bible, false);
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " export_esword" << endl;
  }

  // Exporting Bible to Online Bible.
  refresh_sandbox (false);
  {
    Database_State::create ();
    demo_create_sample_bible ();
    string bible = demo_sample_bible_name ();
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    export_onlinebible (bible, false);
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " export_onlinebible" << endl;
  }

  // Exporting Bible to Quick Bible.
  refresh_sandbox (false);
  {
    Database_State::create ();
    demo_create_sample_bible ();
    string bible = demo_sample_bible_name ();
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    export_quickbible (bible, false);
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " export_quickbible" << endl;
  }

  // Refreshing SWORD module list.
  refresh_sandbox (false);
  {
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    sword_logic_refresh_module_list ();
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " sword_logic_refresh_module_list" << endl;
  }
  
  // Done.
  refresh_sandbox (false);
  
  /*
   
   First iteration displaying memory usage in bytes per function:
   40960 search_reindex_bibles
   8192 search_reindex_notes
   113590272 changes_modifications
   8364032 checks_run
   40960 export_usfm
   339968 export_text_usfm_book
   929792 export_odt_book
   30154752 export_esword
   15814656 export_onlinebible
   118784 export_quickbible
   20480 sword_logic_refresh_module_list
   This is on macOS, but valgrind on Linux gives different values, lower values.
  
  */
}


void test_tasks_logic ()
{
  trace_unit_tests (__func__);
  
  refresh_sandbox (true);
  tasks_logic_queue ("task1");
  tasks_logic_queue ("task3");
  tasks_logic_queue ("task4", { "parameter1", "parameter2" });
  evaluate (__LINE__, __func__, true, tasks_logic_queued ("task1"));
  evaluate (__LINE__, __func__, false, tasks_logic_queued ("task2"));
  evaluate (__LINE__, __func__, false, tasks_logic_queued ("task1", { "parameter" }));
  evaluate (__LINE__, __func__, true, tasks_logic_queued ("task4"));
  evaluate (__LINE__, __func__, true, tasks_logic_queued ("task4", { "parameter1" }));
  evaluate (__LINE__, __func__, true, tasks_logic_queued ("task4", { "parameter1", "parameter2" }));
  evaluate (__LINE__, __func__, false, tasks_logic_queued ("task4", { "parameter1", "parameter3" }));
  evaluate (__LINE__, __func__, false, tasks_logic_queued ("task4", { "parameter2" }));
  
}


void test_biblegateway ()
{
  trace_unit_tests (__func__);
  
  string resource;
  int book;
  string text;

  resource = "Библия, ревизирано издание (BPB)";

  book = 1; // Genesis.

  text = resource_logic_bible_gateway_get (resource, book, 1, 1);
  evaluate (__LINE__, __func__, "В началото Бог сътвори небето и земята.", text);

  resource = "21st Century King James Version (KJ21)";

  book = 34; // Nahum.
  
  text = resource_logic_bible_gateway_get (resource, book, 1, 0);
  evaluate (__LINE__, __func__, "", text);

  text = resource_logic_bible_gateway_get (resource, book, 1, 1);
  evaluate (__LINE__, __func__, "The burden of Nineveh. The book of the vision of Nahum the Elkoshite.", text);

  text = resource_logic_bible_gateway_get (resource, book, 1, 15);
  evaluate (__LINE__, __func__, "Behold upon the mountains the feet of Him that bringeth good tidings, that publisheth peace! O Judah, keep thy solemn feasts, perform thy vows; for the wicked shall no more pass through thee; he is utterly cut off.", text);

  text = resource_logic_bible_gateway_get (resource, book, 1, 16);
  evaluate (__LINE__, __func__, "", text);

  book = 64; // 3 John.
  
  text = resource_logic_bible_gateway_get (resource, book, 1, 0);
  evaluate (__LINE__, __func__, "", text);
  
  text = resource_logic_bible_gateway_get (resource, book, 1, 1);
  evaluate (__LINE__, __func__, "The Elder, Unto the well-beloved Gaius, whom I love in the truth:", text);

  text = resource_logic_bible_gateway_get (resource, book, 1, 14);
  evaluate (__LINE__, __func__, "but I trust I shall shortly see thee, and we shall speak face to face. Peace be to thee. Our friends salute thee. Greet the friends by name.", text);

  text = resource_logic_bible_gateway_get (resource, book, 1, 15);
  evaluate (__LINE__, __func__, "", text);
  
  resource = "Expanded Bible (EXB)";

  book = 34; // Nahum.

  text = resource_logic_bible_gateway_get (resource, book, 1, 0);
  evaluate (__LINE__, __func__, "", text);

  text = resource_logic_bible_gateway_get (resource, book, 1, 1);
  evaluate (__LINE__, __func__, "This is the ·message [oracle; burden] for the city of Nineveh [C the capital of the Assyrian empire]. This is the book of the vision of Nahum, ·who was from the town of Elkosh [L the Elkoshite; C of uncertain location].The Lord Is Angry with Nineveh", text);

  text = resource_logic_bible_gateway_get (resource, book, 1, 2);
  evaluate (__LINE__, __func__, "The Lord is a ·jealous [zealous] God [Ex. 20:5; 34:14; Deut. 4:24; 5:9; Josh. 24:19] who ·punishes [avenges]; the Lord ·punishes [avenges] and is filled with ·anger [wrath]. The Lord ·punishes [takes vengeance on] ·those who are against him [his adversaries/enemies], and he ·stays angry with [or vents his wrath against] his enemies.", text);

  text = resource_logic_bible_gateway_get (resource, book, 1, 11);
  evaluate (__LINE__, __func__, "Someone has come from ·Nineveh [L you] who ·makes evil plans [plots evil] against the Lord and gives wicked ·advice [counsel; strategy].", text);

  text = resource_logic_bible_gateway_get (resource, book, 1, 12);
  evaluate (__LINE__, __func__, "This is what the Lord says: “Although Assyria ·is strong [or has allies] and has many people, it will be ·defeated [destroyed] and ·brought to an end [pass away]. Although I have ·made you suffer, Judah [L afflicted you], I will ·make you suffer [afflict you] no more.", text);
}


void test_rss_feed ()
{
#ifdef HAVE_CLOUD
  trace_unit_tests (__func__);
  refresh_sandbox (true);
  
  string path = rss_logic_xml_path ();
  Database_Config_General::setSiteURL ("http://localhost:8080/");
  
  // Write two items.
  Database_Config_General::setMaxRssFeedItems (10);
  rss_logic_update_xml ({ "titleone", "titletwo" }, { "authorone", "authortwo" }, { "description one", "description two"} );
  evaluate (__LINE__, __func__, 828, filter_url_filesize (path));
  
  // Set maximum number of items to 0: Should remove the file.
  Database_Config_General::setMaxRssFeedItems (0);
  rss_logic_update_xml ({ "titleone", "titletwo" }, { "authorone", "authortwo" }, { "description one", "description two"} );
  evaluate (__LINE__, __func__, 0, filter_url_filesize (path));

  // Add many entries and clipping their number.
  Database_Config_General::setMaxRssFeedItems (10);
  vector <string> titles;
  vector <string> authors;
  vector <string> descriptions;
  for (size_t i = 0; i < 100; i++) {
    titles.push_back ("title " + convert_to_string (i));
    authors.push_back ("author " + convert_to_string (i));
    descriptions.push_back ("description " + convert_to_string (i));
  }
  rss_logic_update_xml (titles, authors, descriptions);
  evaluate (__LINE__, __func__, 2508, filter_url_filesize (path));
#endif
}


void test_libraries_dev ()
{
  trace_unit_tests (__func__);
}

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
#include <unittests/libraries.h>
#include <unittests/filters.h>
#include <unittests/databases.h>
#include <config/libraries.h>
#include <library/bibledit.h>
#include <database/config/user.h>
#include <database/styles.h>
#include <database/books.h>
#include <database/state.h>
#include <config/globals.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/roles.h>
#include <filter/md5.h>
#include <filter/usfm.h>
#include <session/logic.h>


int main (int argc, char **argv) 
{
  (void) argc;
  (void) argv;

#ifdef HAVE_UNITTESTS

  cout << "Running unittests" << endl;

  // Directory where the unit tests will run.
  testing_directory = "/tmp/bibledit-unittests";  
  filter_url_mkdir (testing_directory);
  refresh_sandbox (true);
  config_globals_document_root = testing_directory;

  // Number of failed unit tests.
  error_count = 0;
  
  // Flag for unit tests.
  config_globals_unit_testing = true;

  test_filters_usfm5 (); refresh_sandbox (true); exit (0);
  
  // Run the tests.
  test_database_config_general ();
  test_database_config_bible ();
  test_database_config_user ();
  test_sqlite ();
  test_database_logs ();
  test_filters_various1 ();
  test_filters_various2 ();
  test_filters_usfm1 ();
  test_filters_usfm2 ();
  test_filters_usfm3 ();
  test_filters_usfm4 ();
  test_filters_usfm5 ();
  test_filters_export1 ();
  test_filters_export2 ();
  test_html_text ();
  test_filters_archive ();
  test_odf_text ();
  test_filter_text1 ();
  test_filter_text2 ();
  test_filter_url1 ();
  test_filter_string_rand ();
  test_filter_passage1 ();
  test_filter_passage2 ();
  test_filter_passage3 ();
  test_filter_passage4 ();
  test_email ();
  test_stat ();
  test_replace ();
  test_database_users ();
  test_session_logic ();
  test_empty_folders ();
  test_flate ();
  test_database_styles ();
  test_database_bibles ();
  test_database_books ();
  test_database_bibleactions ();
  test_database_check ();
  test_database_localization ();
  test_database_confirm ();
  test_database_ipc ();
  test_database_jobs ();
  test_database_kjv ();
  test_database_oshb ();
  test_database_sblgnt ();
  test_database_sprint ();
  test_database_mail ();
  test_database_navigation ();
  test_database_resources ();
  test_database_usfmresources ();
  test_database_mappings ();
  test_database_noteactions ();
  test_database_versifications ();
  test_database_modifications_user ();
  test_database_modifications_team ();
  test_database_modifications_notifications ();
  test_database_notes ();
  test_database_volatile ();
  test_database_state ();
  test_database_imageresources ();
  test_styles_css ();
  test_filter_custom_css ();
  test_filter_diff ();
  test_filter_abbreviations ();
  test_checksum_logic ();
  test_store_bible_data ();
  test_editor_html2usfm ();
  test_editor_usfm2html ();
  test_editor_roundtrip ();
  test_editor_development ();
  test_filter_markup ();
  test_workbench_logic ();
  test_filter_git ();
  test_filter_merge ();
  test_filter_tidy ();
  test_ipc_notes ();
  test_check_verses ();
  test_filter_date ();
  test_client_logic ();
  test_check_sentences ();
  test_check_versification ();
  test_check_usfm ();
  test_filter_url2  ();
  test_hyphenate ();
  test_filter_string ();
  test_database_noteassignment ();
  test_database_strong ();
  test_database_morphgnt ();
  test_database_etcbc4 ();
  test_database_hebrewlexicon ();
  test_filter_string_text2html ();
  test_database_cache ();
  test_filter_archive ();
  test_search_logic ();
  test_filter_shell ();
  test_database_login ();
  test_database_privileges ();
  test_json ();
  test_database_git ();
  test_filter_url3 ();
  test_database_userresources ();

  
  // Output possible journal entries.
  refresh_sandbox (true);
  
  // Test results.  
  if (error_count == 0) cout << "All tests passed" << endl;
  else cout << "Number of failures: " << error_count << endl;

  // Ready.
  return (error_count == 0) ? 0 : 1;

#else
  
  cout << "Unit tests are disabled" << endl;
  cout << "Enable them through ./configure --enable-unittests" << endl;
  return 0;

#endif
}



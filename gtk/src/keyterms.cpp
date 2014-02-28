/*
 ** Copyright (©) 2003-2013 Teus Benschop.
 **  
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


#include "keyterms.h"
#include "utilities.h"
#include "gwrappers.h"
#include <sqlite3.h>
#include "directories.h"
#include "books.h"
#include "bible.h"
#include "sqlite_reader.h"
#include "import.h"
#include "progresswindow.h"
#include "unixwrappers.h"
#include <glib.h>
#include "tiny_utilities.h"
#include "settings.h"
#include "htmlwriter.h"


ustring keyterms_get_user_filename()
// Gives the filename for the user-created keyterms database.
{
  return gw_build_filename(directories_get_templates_user(), "keyterms.sql");
}


ustring keyterms_get_package_filename()
// Gives the filename for the keyterms database that comes with bibledit.
{
  return gw_build_filename(directories_get_package_data(), "keyterms.sql");
}


ustring keyterms_get_filename ()
// Gives the standard filename, or the user file if it's there.
{
  ustring filename = keyterms_get_user_filename();
  if (!g_file_test (filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    filename = keyterms_get_package_filename();
  }
  return filename;
}


void keyterms_ensure_user_database()
{
  // Bail out if the user database exists.
  if (g_file_test (keyterms_get_user_filename().c_str(), G_FILE_TEST_IS_REGULAR)) {
    return;
  }
  
  // Copy the package database to the user one.
  unix_cp (keyterms_get_package_filename(), keyterms_get_user_filename());
  chmod(keyterms_get_user_filename().c_str(), 00666);
}


void keyterms_clean_reference_line (ustring& line)
// Cleans common clutter from a line that has a reference.
{
  /*
    Some files may have references that are specially formed. Examples:
    Exo 08:22(E26)
    Gen 08:20ab
    Gen 22:09 (2x)
    These specialties are removed as being clutter.
  */
  line = trim (line);
  size_t pos = line.find ("(");
  if (pos != string::npos) {
    line.erase (pos, 10);
    line = trim (line);
  }
  for (unsigned int i = 0; i < 3; i++) {
    pos = line.find_last_of ("abc");
    if (pos != string::npos) {
      if (pos > 5) {
        line.erase (pos, 10);
        line = trim (line);
      }
    }
  }
}


void keyterms_import_textfile_flush(sqlite3 * db, unsigned int category_id, ustring& keyterm, vector <ustring>& comments, vector <Reference>& references)
{
  // Only store if there's a keyterm..
  if (!keyterm.empty()) {
    
    int keyterm_id = keyterms_retrieve_highest_id ("keyterm") + 1;

    // Variables needed to access the database.
    int rc;
    char *error = NULL;
    char *sql;

    // Clean keyterm.
    keyterm = double_apostrophy(keyterm);

    // Clean comment.
    ustring comment;
    for (unsigned int i = 0; i < comments.size(); i++) {
      if (!comment.empty())
        comment.append("\n");
      comment.append(comments[i]);
    }
    comment = double_apostrophy(comment);

    // Store the keyterm.
    sql = g_strdup_printf("insert into keyterm values (%d, %d, '%s', '%s', '%s');", keyterm_id, category_id, keyterm.c_str(), keyterm.casefold().c_str(), comment.c_str());
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    g_free(sql);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));

    // Store the references belonging to the keyword.
    for (unsigned int i = 0; i < references.size(); i++) {
      sql = g_strdup_printf("insert into reference values (%d, %d, %d, %d);", keyterm_id, references[i].book, references[i].chapter, convert_to_int (references[i].verse));
      rc = sqlite3_exec(db, sql, NULL, NULL, &error);
      g_free(sql);
      if (rc)
        throw runtime_error(sqlite3_errmsg(db));
    }

  }
  keyterm.clear();
  comments.clear();
  references.clear();
}


void keyterms_import_textfile_flags_down(bool & flag1, bool & flag2, bool & flag3)
{
  flag1 = false;
  flag2 = false;
  flag3 = false;
}


ustring keyterms_reference_start_markup ()
{
  return "_reference_start_";
}


ustring keyterms_reference_end_markup ()
{
  return "_reference_end_";
}


void keyterms_import_textfile(const ustring & textfile, ustring category)
// Imports a keyterms textfile.
{
  // Ensure the db is there.
  keyterms_ensure_user_database();

  // Variables.
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {

    // The id to use for the category.
    int category_id = keyterms_retrieve_highest_id ("category") + 1;
    
    // Read the text.
    ReadText rt(textfile);

    // Open the database.
    rc = sqlite3_open(keyterms_get_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    char *sql;
    sql = g_strdup_printf("PRAGMA synchronous=OFF;");
    rc = sqlite3_exec(db, sql, NULL, NULL, &error); 
    g_free(sql);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));

    // Store the category of these keyterms.
    category = double_apostrophy(category);
    sql = g_strdup_printf("insert into category values (%d, '%s');", category_id, category.c_str());
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    g_free(sql);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));

    // Storage for our data.
    ustring keyterm;
    vector <ustring> comments;
    vector <Reference> references;

    // Flags for reading our data.
    bool readkeyterm = false;
    bool readcomments = false;
    bool readreferences = false;

    unsigned int previousbook = 0;
    unsigned int previouschapter = 0;

    // Go through the lines.
    for (unsigned int i = 1; i < rt.lines.size(); i++) {
      try {
        ustring line(rt.lines[i]);
        // Skip comments and empty lines.
        if (line.empty())
          continue;
        if (line.find("#") != string::npos)
          continue;
        if (line.find("//") != string::npos)
          continue;
        if (line.substr(0, 1) == "[") {
          keyterms_import_textfile_flags_down(readkeyterm, readcomments, readreferences);
        }
        if (readkeyterm) {
          keyterm = line;
          keyterms_import_textfile_flags_down(readkeyterm, readcomments, readreferences);
        }
        if (readcomments) {
          comments.push_back(line);
        }
        if (readreferences) {
          // Clean line.
          keyterms_clean_reference_line (line);
          // Store the reference.
          Reference reference(0);
          if (reference_discover(previousbook, previouschapter, "0", line, reference.book, reference.chapter, reference.verse)) {
            references.push_back(reference);
            comments.push_back (keyterms_reference_start_markup () + reference.human_readable ("") + keyterms_reference_end_markup ());
          }
        }
        if (line == "[keyterm]") {
          keyterms_import_textfile_flush(db, category_id, keyterm, comments, references);
          readkeyterm = true;
        }
        if (line == "[comments]") {
          readcomments = true;
        }
        if (line == "[references]") {
          readreferences = true;
        }
      }
      catch(exception & ex) {
        ustring msg = "Skipping: ";
        msg.append(ex.what());
        gw_critical (msg);
      }

    }
    // Flush possible remaining keyterm.
    keyterms_import_textfile_flush(db, category_id, keyterm, comments, references);
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
}


void keyterms_import_otkey_db(const ustring& textfile, ustring category)
{
  // Ensure the db is there.
  keyterms_ensure_user_database();

  // Some variables we need.
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {

    // The id to use for the category.
    int category_id = keyterms_retrieve_highest_id ("category") + 1;
    
    // Read the text.
    ReadText rt(textfile);

    // Open the database.
    rc = sqlite3_open(keyterms_get_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    char *sql;
    sql = g_strdup_printf("PRAGMA synchronous=OFF;");
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    g_free(sql);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));

    // Store the category of these keyterms.
    category = double_apostrophy(category);
    sql = g_strdup_printf("insert into category values (%d, '%s');", category_id, category.c_str());
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    g_free(sql);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
 
    // Storage for our data.
    ustring keyterm;
    vector <ustring> comments;
    vector <Reference> references;

    // Reference discovery variables.
    unsigned int previousbook = 0;
    unsigned int previouschapter = 0;

    // Go through all the lines.
    for (unsigned int i = 1; i < rt.lines.size(); i++) {
      try {
        // Get the line, skip if empty.
        ustring line(rt.lines[i]);
        if (line.empty())
          continue;

        // The kind of information that the line contains.
        bool key_line = (line.find("\\key") == 0);
        bool ref_line = (line.find("\\ref") == 0);
        
        // Keyterm.
        if (key_line) {
          keyterms_import_textfile_flush(db, category_id, keyterm, comments, references);
          keyterm.clear();
          keyterm = trim(line.substr(4, 1000));
        }

        // Reference.
        else if (ref_line) {
          ustring ref = line.substr(4, 1000);
          keyterms_clean_reference_line (ref);
          Reference reference(0);
          if (reference_discover(previousbook, previouschapter, "0", ref, reference.book, reference.chapter, reference.verse)) {
            reference.verse = number_in_string(reference.verse);
            references.push_back(reference);
            comments.push_back (keyterms_reference_start_markup () + reference.human_readable ("") + keyterms_reference_end_markup ());
          }
        }
        
        // Anything else is treated as a comment.
        else {
          comments.push_back (line);
        }
      }
      catch(exception & ex) {
        ustring msg = "Skipping: ";
        msg.append(ex.what());
        gw_critical (msg);
      }

    }
    // Flush remaining data.
    keyterms_import_textfile_flush(db, category_id, keyterm, comments, references);

  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
}


void keyterms_import_ktref_db(const ustring& textfile, ustring category)
{
  // Ensure the db is there.
  keyterms_ensure_user_database();

  // Some variables we need.
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {

    // The id to use for the category.
    int category_id = keyterms_retrieve_highest_id ("category") + 1;
    
    // Open the database.
    rc = sqlite3_open(keyterms_get_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    char *sql;
    sql = g_strdup_printf("PRAGMA synchronous=OFF;");
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    g_free(sql);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));

    // Store the category of these keyterms.
    category = double_apostrophy(category);
    sql = g_strdup_printf("insert into category values (%d, '%s');", category_id, category.c_str());
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    g_free(sql);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));

    // Read and clean the input text. 
    // Lines with text that does not start with a backslash will be joined to the previous one.
    vector <ustring> lines;
    {
      ReadText rt(textfile);
      for (unsigned int i = 0; i < rt.lines.size(); i++) {
        ustring line = rt.lines[i];
        line = trim (line);
        if ((line.find("\\") == 0) || (line.empty())) {
          lines.push_back(line);
        } else {
          lines[lines.size() - 1].append(" " + line);
        }
      }
    }

    // Remove data before the first real keyword.
    {
      bool process = false;
      vector <ustring> lines2;
      for (unsigned int i = 1; i < lines.size(); i++) {
        ustring line(lines[i]);
        if (line.find("\\en ") != string::npos)
          process = true;
        if (line.find("en AAA") != string::npos)
          process = false;
        if (process) {
          if (!line.empty()) {
            lines2.push_back (line);
          }
        }
      }
      lines = lines2;
    }

    // Storage for our data.
    ustring keyterm;
    vector <ustring> comments;
    vector <Reference> references;

    // Reference discovery variables.
    unsigned int previousbook = 0;
    unsigned int previouschapter = 0;

    // Go through all the lines.
    for (unsigned int i = 0; i < lines.size(); i++) {
      try {
        // Get the line.
        ustring line(lines[i]);

        // The kind of information that the line contains.
        bool key_line = (line.find("\\en ") == 0);
        bool ref_line = (line.find("\\ref ") == 0);
        
        // Keyterm.
        if (key_line) {
          keyterms_import_textfile_flush(db, category_id, keyterm, comments, references);
          keyterm.clear();
          keyterm = trim(line.substr(3, 1000));
        }

        // Reference.
        else if (ref_line) {
          ustring ref = line.substr(5, 10000);
          size_t pos = ref.find("(");
          if (pos != string::npos) {
            comments.push_back(ref);
          }
          while (pos != string::npos) {
            size_t pos2 = ref.find(")");
            ref.erase(pos, pos2 - pos + 1);
            pos = ref.find("(");
          }
          replace_text(ref, ")", "");
          vector < ustring > refs;
          {
            Parse parse1(ref, false, ";");
            for (unsigned int i1 = 0; i1 < parse1.words.size(); i1++) {
              Parse parse2(parse1.words[i1], false, ",");
              for (unsigned int i2 = 0; i2 < parse2.words.size(); i2++) {
                refs.push_back(parse2.words[i2]);
              }
            }
          }
          for (unsigned int i2 = 0; i2 < refs.size(); i2++) {
            Reference reference(0);
            if (reference_discover(previousbook, previouschapter, "0", refs[i2], reference.book, reference.chapter, reference.verse)) {
              reference.verse = number_in_string(reference.verse);
              references.push_back(reference);
              comments.push_back (keyterms_reference_start_markup () + reference.human_readable ("") + keyterms_reference_end_markup ());
              previousbook = reference.book;
              previouschapter = reference.chapter;
            }
          }
        }
        
        // Anything else is treated as a comment.
        else {
          comments.push_back (line);
        }
      }
      catch(exception & ex) {
        ustring msg = "Skipping: ";
        msg.append(ex.what());
        gw_critical (msg);
      }

    }

    // Flush remaining data.
    keyterms_import_textfile_flush(db, category_id, keyterm, comments, references);
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
}


void keyterms_import_ktbh_txt_comments(ustring line, vector < ustring > &comments)
// Make standard modifications to the comment, and store it.
{
  if (line.find( "\\notfr ") == 0)
    return;
  if (line.find( "\\notsp ") == 0)
    return;
  if (line.find( "\\notpt ") == 0)
    return;
  if (line.find( "\\notxx ") == 0)
    return;
  replace_text(line, "\\strong", "Strong's number:");
  replace_text(line, "\\pos", "Part of speech:");
  replace_text(line, "\\incpos", "Part of speech (inc):");
  replace_text(line, "\\freq", "Frequency:");
  replace_text(line, "\\sub", "Cognate(s):");
  replace_text(line, "\\alt", "Variant Spelling(s):");
  replace_text(line, "\\see", "See entry:");
  replace_text(line, "\\intro", "Abstract for Busy Translators:");
  replace_text(line, "\\tnotes", "Notes for Translators:");
  replace_text(line, "\\hebraist", "Hebraists' Comments:");
  replace_text(line, "\\altint", "Alternative interpretations:");
  replace_text(line, "\\evalev", "Evaluating Evidence:");
  replace_text(line, "\\qumran", "Qumran:");
  replace_text(line, "\\nt", "New Testament Evidence:");
  replace_text(line, "\\comp", "Comparison of Meanings and Uses:");
  replace_text(line, "\\dfnotes", "Domain and Frame Notes:");
  replace_text(line, "\\addinfo", "Additional Information:");
  replace_text(line, "\\not ", "Note: ");
  replace_text(line, "\\frame", "Frame for Meaning:");
  replace_text(line, "\\domain", "Intrinsic Domains for Meaning:");
  replace_text(line, "\\stem", "Verb Stem:");
  replace_text(line, "\\gram", "Grammar Notes:");
  replace_text(line, "\\eventfr", "Participant Roles for Meaning:");
  replace_text(line, "\\syntagc", "Meaning Comments:");
  replace_text(line, "\\framec", "Frame for Use:");
  replace_text(line, "\\domainc", "Contextual Domains for Use:");
  replace_text(line, "\\eventfrc", "Participant Roles for Use:");
  replace_text(line, "\\syntagcc", "Use Comments:");
  replace_text(line, "\\keyverse", "Key Verse:");
  replace_text(line, "\\eval", "Evaluation of Translations:");
  replace_text(line, "\\lxx", "Key Septuagint:");
  replace_text(line, "\\trans", "Translation:");
  replace_text(line, "\\paradigc", "Keyword:");
  replace_text(line, "\\colc", "Collocating Term:");
  replace_text(line, "\\pgloss", "Gloss:");
  replace_text(line, "\\ver ", "Version: ");
  replace_text(line, "\\dat ", "Date: ");
  replace_text(line, "\\level2", "Level 2:");
  replace_text(line, "\\level3", "Level 3:");
  replace_text(line, "\\level4", "Level 4:");
  replace_text(line, "\\meaning", "Meaning:");
  replace_text(line, "\\context", "Context:");
  replace_text(line, "|h", "");
  replace_text(line, "|h*", "");
  replace_text(line, "|g", "");
  replace_text(line, "|g*", "");
  replace_text(line, "|n", "");
  replace_text(line, "|n*", "");
  replace_text(line, "|e", "");
  replace_text(line, "|e*", "");
  replace_text(line, "|r", "");
  replace_text(line, "|r*", "");
  replace_text(line, "|t", "");
  replace_text(line, "|t*", "");
  replace_text(line, "|s", "");
  replace_text(line, "|s*", "");
  replace_text(line, "", "");
  comments.push_back(line);
}


void keyterms_import_ktbh_txt_references(ustring line, vector <Reference> &references, vector <ustring>& comments)
/*
 The references in the KTBH database are stored in a special way.
 Reinier de Blois gave the code to extract a reference:
 digit 1: calculate the ansi code and subtract 32 from it (e.g. ! = 1 = Genesis).
 digit 2: if " store number 90.
 digit 3: calculate the ansi code and subtract 32 from it; add the stored number of digit 2 to it (e.g. !! = chapter 1; "! = chapter 91).
 digit 4: if " store number 90.
 digit 5: calculate the ansi code and subtract 32 from it; add the stored number of digit 4 to it (e.g. !! = verse 1; "! = verse 91).
 digit 6: calculate the ansi code and subtract 32 from it (position of word within verse).
 digit 7: calculate the ansi code and subtract 32 from it (position of morpheme within word).
 */
{
  Parse parse(line, false);
  for (unsigned int i = 0; i < parse.words.size(); i++) {
    if (parse.words[i].length() < 5)
      throw runtime_error(line);
    gunichar *digit1;
    digit1 = g_utf8_to_ucs4_fast(parse.words[i].substr(0, 1).c_str(), -1, NULL);
    *digit1 -= 32;
    gunichar digit2 = 0;
    if (parse.words[i].substr(1, 1) == "\"")
      digit2 = 90;
    gunichar *digit3;
    digit3 = g_utf8_to_ucs4_fast(parse.words[i].substr(2, 1).c_str(), -1, NULL);
    *digit3 -= 32;
    gunichar digit4 = 0;
    if (parse.words[i].substr(3, 1) == "\"")
      digit4 = 90;
    gunichar *digit5;
    digit5 = g_utf8_to_ucs4_fast(parse.words[i].substr(4, 1).c_str(), -1, NULL);
    *digit5 -= 32;
    Reference reference(*digit1, digit2 + *digit3, convert_to_string(digit4 + *digit5));
    references.push_back(reference);
    comments.push_back (keyterms_reference_start_markup () + reference.human_readable ("") + keyterms_reference_end_markup ());
    g_free(digit1);
    g_free(digit3);
    g_free(digit5);
  }
}


void keyterms_import_ktbh_txt(const ustring& textfile, ustring category)
{
  // Ensure the db is there.
  keyterms_ensure_user_database();

  // Some variables we need.
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {

    // The id to use for the category.
    int category_id = keyterms_retrieve_highest_id ("category") + 1;
    
    // Open the database.
    rc = sqlite3_open(keyterms_get_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    char *sql;
    sql = g_strdup_printf("PRAGMA synchronous=OFF;");
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    g_free(sql);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));

    // Store the category of these keyterms.
    category = double_apostrophy(category);
    sql = g_strdup_printf("insert into category values (%d, '%s');", category_id, category.c_str());
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    g_free(sql);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));

    // Read the text. Any lines not starting with \ should be joined to the previous one.
    vector <ustring> lines;
    {
      ReadText rt(textfile, true);
      for (unsigned int i = 0; i < rt.lines.size(); i++) {
        ustring line = rt.lines[i];
        if (!line.empty()) {
          if (rt.lines[i].find("\\") == 0) {
            lines.push_back(rt.lines[i]);
          } else {
            lines[lines.size() - 1].append(" " + rt.lines[i]);
          }
        }
      }
    }

    // Storage for our data.
    ustring keyterm;
    vector <ustring> comments;
    vector <Reference> references;

    // Go through all the lines.
    ProgressWindow progresswindow ("Importing", false);
    progresswindow.set_iterate (0, 1, lines.size());
    for (unsigned int i = 0; i < lines.size(); i++) {
      progresswindow.iterate();
      try {

        // Get the line.
        ustring line(lines[i]);

        // Hebrew keyterm: store.
        if (line.find("\\heb ") == 0) {
          keyterms_import_textfile_flush(db, category_id, keyterm, comments, references);
          keyterm = "(" + line.substr(5, 1000) + ")";
        }
        
        // Gloss: store and add to keyterm.
        else if (line.find("\\gloss ") == 0) {
          ustring gloss = line.substr(7, 1000);
          keyterm.insert(0, gloss + " ");
        }

        // Reference: store.
        else if (line.find("\\ref ") == 0) {
          ustring ref = line.substr(5, 10000);
          keyterms_import_ktbh_txt_references(ref, references, comments);
        }

        // Key reference.
        else if (line.find("\\keyref ") == 0) {
          ustring ref = line.substr(8, 10000);
          comments.push_back ("Key Reference:");
          keyterms_import_ktbh_txt_references(ref, references, comments);
        }
        
        // References.
        else if (line.find("\\refsc ") == 0) {
          comments.push_back ("References:");
          ustring ref = line.substr(7, 10000);
          keyterms_import_ktbh_txt_references(ref, references, comments);
        }
        
        // Anything else is treated as a comment.
        else {
          keyterms_import_ktbh_txt_comments(line, comments);
        }
      }
      catch(exception & ex) {
        ustring msg = "Skipping: ";
        msg.append(ex.what());
        cout << msg << endl;
      }

    }
    // Flush remaining data.
    keyterms_import_textfile_flush(db, category_id, keyterm, comments, references);

  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
}


vector <ustring> keyterms_get_categories()
// Retrieves the different categories from the database.
{
  sqlite3 *db;
  int rc;
  char *error = NULL;
  SqliteReader reader(0);
  try {
    rc = sqlite3_open(keyterms_get_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    char *sql;
    sql = g_strdup_printf("select name from category;");
    rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
    g_free(sql);
    if (rc) {
      throw runtime_error(sqlite3_errmsg(db));
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
  return reader.ustring0;
}


void keyterms_delete_collection (const ustring& collection)
// Delete the collection from the database. 
// If all collections have been deleted, it reverts to the original database of the package.
{
  // Ensure we work with the user database.
  keyterms_ensure_user_database ();

  sqlite3 *db;
  sqlite3_open(keyterms_get_filename().c_str(), &db);
  sqlite3_busy_timeout(db, 1000);
  char *sql;

  // Retrieve the id of the collection to delete.
  int collection_id = 0;
  {
    SqliteReader reader(0);
    sql = g_strdup_printf("select id from category where name = '%s';", double_apostrophy(collection).c_str());
    sqlite3_exec(db, sql, reader.callback, &reader, NULL);
    g_free(sql);
    if (!reader.ustring0.empty())
      collection_id = convert_to_int(reader.ustring0[0]);
  }

  // Retrieve a list of keyterm ids to be deleted.
  vector <int> keyterm_ids;
  {
    SqliteReader reader(0);
    sql = g_strdup_printf("select id from keyterm where category = %d;", collection_id);
    sqlite3_exec(db, sql, reader.callback, &reader, NULL);
    g_free(sql);
    for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
      keyterm_ids.push_back (convert_to_int (reader.ustring0[i]));
    }
  }  

  // Delete the category.
  sql = g_strdup_printf("delete from category where name = '%s';", double_apostrophy(collection).c_str());
  sqlite3_exec(db, sql, NULL, NULL, NULL);
  g_free(sql);

  // Delete the keyterms.
  sql = g_strdup_printf("delete from keyterm where category = %d;", collection_id);
  sqlite3_exec(db, sql, NULL, NULL, NULL);
  g_free(sql);
  
  // Delete the relevant references.
  for (unsigned int i = 0; i < keyterm_ids.size(); i++) {
    sql = g_strdup_printf("delete from reference where id = %d;", keyterm_ids[i]);
    sqlite3_exec(db, sql, NULL, NULL, NULL);
    g_free(sql);
  }

  sqlite3_close(db);
  
  // If there are no categories left, revert to the package database.
  if (keyterms_get_categories ().empty ()) {
    unlink (keyterms_get_user_filename().c_str());
  }
}


void keyterms_get_terms(const ustring& searchterm, const ustring& collection, vector <ustring>& terms, vector <unsigned int>& ids)
// Depending on the searchterm and collection, get the keyterms, together with 
// their levels, parents, and ids. If the searchterm is empty, give them all.
// If the collection is empty, get them from all collections.
{
  // Variables we need.
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    // Open database, readers.
    rc = sqlite3_open(keyterms_get_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    SqliteReader readerterms(0);
    SqliteReader readercollection(0);
    // See whether to search in collections.
    unsigned int collection_id = 0;
    if (!collection.empty()) {
      char *sql;
      sql = g_strdup_printf("select id from category where name = '%s';", double_apostrophy(collection).c_str());
      rc = sqlite3_exec(db, sql, readercollection.callback, &readercollection, &error);
      g_free(sql);
      if (rc)
        throw runtime_error(sqlite3_errmsg(db));
      if (!readercollection.ustring0.empty())
        collection_id = convert_to_int(readercollection.ustring0[0]);
    }
    // Assemble query expression.
    ustring sql("select id, name from keyterm");
    if (!searchterm.empty() || collection_id)
      sql.append(" where");
    if (!searchterm.empty()) {
      sql.append(" (namecf glob ('*" + searchterm.casefold() + "*')");
      sql.append(" or comment glob ('*" + searchterm.casefold() + "*'))");
    }
    if (!searchterm.empty() && collection_id)
      sql.append(" and");
    if (collection_id) {
      sql.append(" category = " + convert_to_string(collection_id));
    }
    sql.append(";");
    // Query the db.    
    rc = sqlite3_exec(db, sql.c_str(), readerterms.callback, &readerterms, &error);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    // Return results.
    for (unsigned int i = 0; i < readerterms.ustring0.size(); i++) {
      terms.push_back(readerterms.ustring1[i]);
      ids.push_back(convert_to_int(readerterms.ustring0[i]));
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
}


bool keyterms_get_term(unsigned int id, ustring & term)
{
  bool result = false;
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    rc = sqlite3_open(keyterms_get_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    SqliteReader reader(0);
    char *sql;
    sql = g_strdup_printf("select name from keyterm where id = %d;", id);
    rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
    g_free(sql);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    if (!reader.ustring0.empty()) {
      term = reader.ustring0[0];
      result = true;
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
  return result;
}


bool keyterms_get_data(unsigned int id, ustring & category, ustring & comments, vector < Reference > &references)
{
  bool result = false;
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    rc = sqlite3_open(keyterms_get_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    char *sql;
    SqliteReader reader1(0);
    sql = g_strdup_printf("select category, comment from keyterm where id = %d;", id);
    rc = sqlite3_exec(db, sql, reader1.callback, &reader1, &error);
    g_free(sql);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    if (!reader1.ustring0.empty()) {
      comments = reader1.ustring1[0];
      result = true;
    }
    SqliteReader reader2(0);
    sql = g_strdup_printf("select name from category where id = %d;", convert_to_int(reader1.ustring0[0]));
    rc = sqlite3_exec(db, sql, reader2.callback, &reader2, &error);
    g_free(sql);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    if (!reader2.ustring0.empty()) {
      category = reader2.ustring0[0];
    }
    SqliteReader reader3(0);
    sql = g_strdup_printf("select book, chapter, verse from reference where id = %d;", id);
    rc = sqlite3_exec(db, sql, reader3.callback, &reader3, &error);
    g_free(sql);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    for (unsigned int i = 0; i < reader3.ustring0.size(); i++) {
      Reference reference(convert_to_int(reader3.ustring0[i]), convert_to_int(reader3.ustring1[i]), reader3.ustring2[i]);
      references.push_back(reference);
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
  return result;
}


ustring keyterms_renderings_filename(const ustring & project)
// Returns the filename for the database to store user data.
// If the database is not there, it will be created on the fly.
{
  ustring filename = gw_build_filename(directories_get_projects(), project,
                                       "keyterms-renderings.sql");
  if (!g_file_test(filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    sqlite3 *db;
    int rc;
    char *error = NULL;
    try {
      rc = sqlite3_open(filename.c_str(), &db);
      if (rc)
        throw runtime_error(sqlite3_errmsg(db));
      sqlite3_busy_timeout(db, 1000);
      char *sql;
      sql = g_strdup_printf("create table renderings (keyword string, collection string, rendering string, wholeword integer, casesensitive integer);");
      rc = sqlite3_exec(db, sql, NULL, NULL, &error);
      g_free(sql);
      if (rc)
        throw runtime_error(sqlite3_errmsg(db));
    }
    catch(exception & ex) {
      gw_critical(ex.what());
    }
    sqlite3_close(db);
  }
  return filename;
}


void keyterms_retrieve_renderings(const ustring & project, const ustring & keyterm, const ustring & collection, vector < ustring > &renderings, vector < bool > &wholewords, vector < bool > &casesensitives)
{
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    rc = sqlite3_open(keyterms_renderings_filename(project).c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    char *sql;
    SqliteReader reader(0);
    sql = g_strdup_printf("select rendering, wholeword, casesensitive from renderings where keyword = '%s' and collection = '%s';", double_apostrophy(keyterm).c_str(), double_apostrophy(collection).c_str());
    rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
    g_free(sql);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
      renderings.push_back(reader.ustring0[i]);
      wholewords.push_back(convert_to_bool(reader.ustring1[i]));
      casesensitives.push_back(convert_to_bool(reader.ustring2[i]));
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
}


void keyterms_store_renderings(const ustring & project, const ustring & keyterm, const ustring & collection, const vector < ustring > &renderings, const vector < bool > &wholewords, const vector < bool > &casesensitives)
{
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    rc = sqlite3_open(keyterms_renderings_filename(project).c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    char *sql = g_strdup_printf("delete from renderings where keyword = '%s' and collection = '%s';", double_apostrophy(keyterm).c_str(), double_apostrophy(collection).c_str());
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    g_free(sql);
    for (unsigned int i = 0; i < renderings.size(); i++) {
      ustring s1 = double_apostrophy(keyterm);
      ustring s2 = double_apostrophy(collection);
      ustring s3 = double_apostrophy(renderings[i]);
      sql = g_strdup_printf("insert into renderings values ('%s', '%s', '%s', %d, %d);", s1.c_str(), s2.c_str(), s3.c_str(), wholewords[i], casesensitives[i]);
      rc = sqlite3_exec(db, sql, NULL, NULL, &error);
      g_free(sql);
      if (rc)
        throw runtime_error(sqlite3_errmsg(db));
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
}


void keyterms_export(const ustring & directory, bool gui)
{
  return;
  
  // This might need to be updated to use the user-database too.
  // GUI.
  ProgressWindow *progresswindow = NULL;
  if (gui)
    progresswindow = new ProgressWindow("Exporting keyterms", false);
  // Go through the categories.  
  vector < ustring > categories = keyterms_get_categories();
  for (unsigned int cat = 0; cat < categories.size(); cat++) {
    // Output data.
    ustring paratext_filename = gw_build_filename(directory, categories[cat] + ".lex");
    replace_text(paratext_filename, " ", "_");
    replace_text(paratext_filename, "'", "_");
    vector < ustring > paratext_lines;
    // Get all data for the category.
    vector < ustring > terms;
    vector < unsigned int >ids;
    keyterms_get_terms("", categories[cat], terms, ids);
    // GUI.
    if (gui) {
      progresswindow->set_iterate(0, 1, ids.size());
      progresswindow->set_text(categories[cat]);
    }
    // Go through the terms.
    for (unsigned int id = 0; id < ids.size(); id++) {
      // GUI
      if (gui)
        progresswindow->iterate();
      // Process the full data of the keyterm.
      ustring comments;
      vector < Reference > references;
      keyterms_get_data(ids[id], categories[cat], comments, references);
      ustring key("\\key ");
      key.append(terms[id]);
      paratext_lines.push_back(key);
      paratext_lines.push_back("\\keynote ");
      replace_text(comments, "\n", "</p><p>");
      paratext_lines.push_back("\\optional <p>" + comments + "</p>");
      for (unsigned int rf = 0; rf < references.size(); rf++) {
        ustring reference("\\ref ");
        reference.append(books_id_to_paratext(references[rf].book));
        reference.append(" ");
        reference.append(convert_to_string(references[rf].chapter));
        reference.append(":");
        reference.append(references[rf].verse);
        paratext_lines.push_back(reference);
      }
      paratext_lines.push_back("");
    }
    // Store output
    write_lines(paratext_filename, paratext_lines);
  }
  // Destroy GUI.
  if (progresswindow)
    delete progresswindow;
}


vector <int> keyterms_get_terms_in_verse(const Reference & reference)
{
  vector < int >terms;
  sqlite3 *db;
  sqlite3_open(keyterms_get_filename().c_str(), &db);
  sqlite3_busy_timeout(db, 1000);
  SqliteReader reader(0);
  char *sql = g_strdup_printf("select id from reference where book = %d and chapter = %d and verse = '%s';", reference.book, reference.chapter, reference.verse.c_str());
  sqlite3_exec(db, sql, reader.callback, &reader, NULL);
  g_free(sql);
  for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
    terms.push_back(convert_to_int(reader.ustring0[i]));
  }
  sqlite3_close(db);
  return terms;
}


deque <ustring> keyterms_rendering_retrieve_terms(const ustring & project, const ustring & rendering)
// This retrieves the keyterms that have the given rendering in them.
{
  deque < ustring > keyterms;

  // Open database, readers.
  sqlite3 *db;
  sqlite3_open(keyterms_renderings_filename(project).c_str(), &db);
  sqlite3_busy_timeout(db, 1000);
  SqliteReader reader(0);
  // Do the search.
  char *sql = g_strdup_printf("select keyword from renderings where rendering glob ('*%s*');", double_apostrophy(rendering).c_str());
  sqlite3_exec(db, sql, reader.callback, &reader, NULL);
  g_free(sql);
  for (unsigned int i = 0; i < reader.ustring0.size(); i++)
    keyterms.push_back(reader.ustring0[i]);
  sqlite3_close(db);
  return keyterms;
}


int keyterms_retrieve_highest_id (const gchar * table)
// This retrieves the highest id from "table".
{
  int id = 0;
  sqlite3 *db;
  sqlite3_open(keyterms_get_filename().c_str(), &db);
  sqlite3_busy_timeout(db, 1000);
  SqliteReader reader(0);
  char *sql;
  sql = g_strdup_printf("select id from '%s' order by id desc;", table);
  sqlite3_exec(db, sql, reader.callback, &reader, NULL);
  g_free(sql);
  if (reader.ustring0.size() > 0)
    id = convert_to_int(reader.ustring0[0]);
  sqlite3_close(db);
  return id;
}


void keyterms_export_renderings (const ustring& collection, bool include_terms_wo_renderings)
// Exports keyterms and their renderings to html.
// collection: May restrict the export to one collection.
// include_terms_wo_rendering: Whether to include the keyterms that have no renderings.
{
  // The project.
  extern Settings * settings;
  ustring project = settings->genconfig.project_get();

  // Storage for the keyterms and their renderings.
  vector <ustring> all_keyterms;
  vector <VectorUstring> all_renderings;

  // Get keyterms and renderings.
  {
    vector <ustring> collections = keyterms_get_categories();
    for (unsigned int i = 0; i < collections.size(); i++) {
      if (!collection.empty()) {
        if (collection != collections[i]) {
          continue;
        }
      }
      vector <unsigned int> ids;
      vector <ustring> keyterms;
      vector <VectorUstring> renderings;
      keyterms_get_terms ("", collections[i], keyterms, ids);
      for (unsigned int i2 = 0; i2 < keyterms.size(); i2++) {
        vector <ustring> rendering;
        vector <bool> dummy;
        keyterms_retrieve_renderings(project, keyterms[i2], collections[i], rendering, dummy, dummy);
        renderings.push_back (rendering);
      }
      for (unsigned int i2 = 0; i2 < ids.size(); i2++) {
        all_keyterms.push_back (keyterms[i2]);
        all_renderings.push_back (renderings[i2]);
      }
    }
  }

  // Start the document.
  HtmlWriter htmlwriter ("Keywords", false, true, false);
  htmlwriter.heading (1, "Keywords");
  
  // Column headers.
  vector <ustring> column_headers;
  column_headers.push_back ("Keyword");
  column_headers.push_back ("Renderings");
  
  // Cell content.
  vector <VectorUstring> cell_content;
  for (unsigned int i = 0; i < all_keyterms.size(); i++) {
    vector <ustring> row;
    vector <ustring> renderings = all_renderings[i];
    if (renderings.empty ()) {
      if (include_terms_wo_renderings) {
        row.push_back (all_keyterms[i]);
      }
    } else {
      for (unsigned int i2 = 0; i2 < renderings.size(); i2++) {
        ustring keyterm;
        if (i2 == 0) {
          keyterm = all_keyterms[i];
        }
        row.push_back (keyterm);
        row.push_back (renderings[i2]);
      }
    }
    if (!row.empty()) {
      cell_content.push_back (row);
    }
  }
  
  // Write the table.
  htmlwriter.table ("Keywords",  column_headers, cell_content, "", NULL, 12);
}



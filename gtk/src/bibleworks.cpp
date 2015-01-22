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


#include "libraries.h"
#include "bibleworks.h"
#include "utilities.h"
#include "clean.h"
#include "usfmtools.h"
#include "bible.h"
#include "books.h"
#include "gwrappers.h"
#include "directories.h"
#include "unixwrappers.h"
#include "tiny_utilities.h"
#include "dialogradiobutton.h"
#include "localizedbooks.h"
#include "categorize.h"
#include "projectutils.h"
#include "progresswindow.h"
#include "gtkwrappers.h"
#include "shell.h"
#include <sqlite3.h>


ustring bibleworks_exported_file_get_bookname(const ustring & filename)
// Retrieves the bookname from a file exported by BibleWorks. Though there can
// be several books in such a file, this functions retrieves only the name
// of the first book.
{
#define MYMAX 10
  ReadText rt(filename, true, false);
  unsigned int maximum = MYMAX;
  maximum = CLAMP(maximum, 0, rt.lines.size());
  unsigned int goodline = 0;
  ustring bookname;
  try {
    for (unsigned int i = 0; i < maximum; i++) {
      de_byte_order_mark (rt.lines[i]);
      ustring bookabbreviation = rt.lines[i].substr(0, 3);
      unsigned int id = books_bibleworks_to_id(bookabbreviation);
      if (id) {
        goodline++;
        bookname = books_id_to_english(id);
      }
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  if (goodline != MYMAX)
    bookname.clear();
  return bookname;
#undef MYMAX
}


bool bibleworks_clipboard_file_okay (const ustring& filename)
// Looks whether a file as copied from BibleWorks through the clipboard looks okay.
{
#define MYMAX 10
  ReadText rt(filename, true, false);
  unsigned int maximum = MYMAX;
  maximum = CLAMP(maximum, 0, rt.lines.size());
  for (unsigned int i = 0; i < maximum; i++) {
    if (bibleworks_clipboard_file_line_get_extract_book_id (rt.lines[i]) == 0)
      return false;
  }
  return true;
#undef MYMAX
}


unsigned int bibleworks_clipboard_file_line_get_extract_book_id (ustring& line)
// Gets the id of a book from a line of a BibleWorks database copied through the clipboard.
// The amount of text that make up the book is removed from the line.
// Normally a line of text would look like this:
// SCR Matthew 1:1  Βίβλος γενέσεως Ἰησοῦ Χριστοῦ, υἱοῦ Δαβὶδ, υἱοῦ Ἀβραάμ.
// or:
// SCR 1 Corinthians 1:1  Παῦλος κλητὸς ἀπόστολος Ἰησοῦ Χριστοῦ διὰ θελήματος Θεοῦ, καὶ Σωσθένης ὁ ἀδελφός,
{
  // Remove whitespace from the start of the line.
  while (line.substr (0, 1) == " ") 
    line.erase (0, 1);
  // Remove the module abbreviation.
  size_t pos = line.find (" ");
  if (pos == string::npos)
    return 0;
  line.erase (0, ++pos);
  // Get the name of the book.
  vector <unsigned int> ids = books_type_to_ids (btUnknown);
  for (unsigned int i = 0; i < ids.size(); i++) {
    ustring english_name = books_id_to_english (ids[i]);
    if (line.find (english_name) == 0) {
      line.erase (0, english_name.length());
      return ids[i];
    }
  }
  return 0;
}


void import_check_bibleworks_file (vector <ustring>& filenames, vector <unsigned int>& bookids, const ustring& bible, vector <ustring>& messages)
// Checks the file exported from BibleWorks and meant to be imported as a Bible.
{
  // Check whether there's only one file selected.
  if (messages.empty()) {
    if (filenames.size() > 1) {
      messages.push_back ("You have selected more than one file");
    }
  }

  // Check that the BibleWorks file is a valid one.
  if (messages.empty()) {
    ustring english_name = bibleworks_exported_file_get_bookname(filenames[0]);
    unsigned int id = books_english_to_id (english_name);
    if (id == 0) {
      messages.push_back ("The file cannot be recognized as coming from BibleWorks");
    }
  }

  // Check that the Bible to import the file into is empty.
  if (messages.empty()) {
    vector <unsigned int> books = project_get_books (bible);
    if (!books.empty()) {
      messages.push_back ("You try to import data into a Bible that is not empty");
    }
  }
}


void import_bibleworks_text_file (const ustring& file, const ustring& bible, vector <ustring>& messages)
// Imports a bibleworks text file.
{
  // Read the file.
  ReadText rt (file, true, false);

  // If there's nothing to import, bail out.
  if (rt.lines.empty()) {
    messages.push_back ("The file is empty");
  }

  // Divide the input into separate bits for each book.
  vector <VectorUstring> bookdata;
  if (messages.empty()) {
    try {
      ustring previousbook;
      vector < ustring > booklines;
      de_byte_order_mark (rt.lines[0]);
      for (unsigned int i = 0; i < rt.lines.size(); i++) {
        ustring currentbook = rt.lines[i].substr(0, 3);
        if (i == 0) {
          previousbook = currentbook;
        }
        if (currentbook != previousbook) {
          bookdata.push_back (booklines);
          booklines.clear();
          previousbook = currentbook;
        }
        booklines.push_back(rt.lines[i]);
      }
      bookdata.push_back (booklines);
    }
    catch(exception & ex) {
      messages.push_back(ex.what());
    }
  }

  // Import each book.
  if (messages.empty ()) {
    ProgressWindow progresswindow ("Importing", false);
    progresswindow.set_iterate (0, 1, bookdata.size());
    for (unsigned int i = 0; i < bookdata.size(); i++) {
      progresswindow.iterate ();
      try {

        // Input and output data.
        vector <ustring> rawlines = bookdata[i];
        vector <ustring> usfmlines;

        // Get the name of the book.
        unsigned int book_id = books_bibleworks_to_id(rawlines[0].substr(0, 3));
        if (book_id == 0) {
          messages.push_back ("Unknown book: " + rawlines[0]);
          return;
        }

        // Store USFM id.
        ustring usfmid = books_id_to_paratext (book_id);
        usfmlines.push_back("\\id " + usfmid);

        // Convert the BibleWorks lines to USFM code.
        ustring previouschapter = "0";
        for (unsigned int i = 0; i < rawlines.size(); i++) {
          // Convert chapter information.
          ustring line = rawlines[i];
          line.erase(0, 4);
          ustring currentchapter = number_in_string(line);
          line.erase(0, currentchapter.length() + 1);
          if (currentchapter != previouschapter) {
            usfmlines.push_back("\\c " + currentchapter);
            usfmlines.push_back("\\p");
            previouschapter = currentchapter;
          }
          // Convert verse data.
          usfmlines.push_back("\\v " + line);
        }

        // Store into the Bible.
        CategorizeChapterVerse ccv(usfmlines);
        project_store_book(bible, book_id, ccv);
        
      }
      catch(exception & ex) {
        messages.push_back(ex.what());
      }
    }
  }

}


void check_bibleworks_source_language (vector <ustring>& filenames, vector <ustring>& messages)
{
  // Check that two files were selected.
  if (filenames.size() != 2) {
    messages.push_back ("Please select two files for this import");
    return;
  }

  // Check that both files are valid.
  for (unsigned int i = 0; i < 2; i++) {
    if (!bibleworks_clipboard_file_okay (filenames[i])) {
      messages.push_back ("File " + filenames[i] + " cannot be recognized as a textfile coming from BibleWorks");
    }
  }

  // If the second file does not have the "@" character, it is not the morphology, then swap the filenames.
  {
    ReadText rt (filenames[1]);
    if (!rt.lines.empty()) {
      if (rt.lines[0].find ("@") == string::npos) {
        ustring filename = filenames[0];
        filenames[0] = filenames[1];
        filenames[1] = filename;
      }
    }
  }

  // Check that the first file has the text.
  {
    ReadText rt (filenames[0]);
    if (!rt.lines.empty()) {
      if (rt.lines[0].find ("@") != string::npos) {
        messages.push_back ("File " + filenames[0] + " should have the normal text");
      }
    }
  }

  // Check that the second file has the morphology.
  {
    ReadText rt (filenames[1]);
    if (!rt.lines.empty()) {
      if (rt.lines[0].find ("@") == string::npos) {
        messages.push_back ("File " + filenames[1] + " should have the morphology");
      }
    }
  }
}


typedef struct
{
  const char *input;
  const char *output;
} bibleworks_greek_record;


// A table of double ascii characters and their Greek UTF8 equivalents.
bibleworks_greek_record bibleworks_greek_table_2[] =
{
  { "i,", "ί" },
  { "e,", "έ" },
  { "VI", "Ἰ" },
  { "u/", "ῦ" },
  { "i`", "ἱ" },
  { "i.", "ὶ" },
  { "VA", "Ἀ" },
  { "a,", "ά" },
  { "a.", "ὰ" },
  { "o.", "ὸ" },
  { "e.", "ὲ" },
  
  
  
};


// A table of single ascii characters and their Greek UTF8 equivalents.
bibleworks_greek_record bibleworks_greek_table_1[] =
{
  { "B", "Β" },
  { "b", "β" },
  { "l", "λ" },
  { "o", "ο" },
  { "j", "ς" },
  { "g", "γ" },
  { "e", "ε" },
  { "n", "ν" },
  { "s", "σ" },
  { "w", "ω" },
  { "h", "η" },
  { "C", "Χ" },
  { "r", "ρ" },
  { "t", "τ" },
  { "u", "υ" },
  { "D", "Δ" },
  { "a", "α" },
  { "d", "δ" },
  { "(", "," },
  { "m", "μ" },
  { "Å", "." },
  { " ", " " },
  { "i", "ι" },
  { "v", "γ" },
  { "k", "κ" },
  { "\\", "·" },


  
};



ustring convert_bibleworks_greek (ustring line)
{
  ustring outputline;
  while (!line.empty()) {
    ustring character;
    bool converted = false;
    // Convert the combined characters.
    character = line.substr (0, 2);
    for (unsigned int i = 0; i < sizeof(bibleworks_greek_table_2) / sizeof(*bibleworks_greek_table_2); i++) {
      if (!converted) {
        if (character == bibleworks_greek_table_2[i].input) {
          outputline.append (bibleworks_greek_table_2[i].output);
          line.erase (0, 2);
          converted = true;
        }
      }
    }
    // Convert the single character.
    if (!converted) {
      character = line.substr (0, 1);
      for (unsigned int i = 0; i < sizeof(bibleworks_greek_table_1) / sizeof(*bibleworks_greek_table_1); i++) {
        if (!converted) {
          if (character == bibleworks_greek_table_1[i].input) {
            outputline.append (bibleworks_greek_table_1[i].output);
            line.erase (0, 1);
            converted = true;
          }
        }
      }
    }
    // Message if the conversion didn't work out.
    if (!converted) {
      gw_critical ("Output so far: " + outputline + " - unhandled character: " + character + " - input stream: " + line);
      outputline.append (character);
      line.erase (0, 1);
    }
  }
  return outputline;  
}


void bibleworks_define_parsing_case (ustring& parsing, ustring& definition)
// Parse the case.
{
  ustring case_code = parsing.substr (0, 1);
  parsing.erase (0, 1);
  if (case_code == "n") {
    definition.append (" nominative");
  } 
  if (case_code == "g") {
    definition.append (" genitive");
  }
  if (case_code == "d") {
    definition.append (" dative");
  }
  if (case_code == "a") {
    definition.append (" accusative");
  }
  if (case_code == "v") {
    definition.append (" vocative");
  }
}


void bibleworks_define_parsing_gender (ustring& parsing, ustring& definition)
// Parse the gender.
{
  ustring gender_code = parsing.substr (0, 1);
  parsing.erase (0, 1);
  if (gender_code == "m") {
    definition.append (" masculine");
  } 
  if (gender_code == "f") {
    definition.append (" feminine");
  }
  if (gender_code == "n") {
    definition.append (" neuter");
  }
}


void bibleworks_define_parsing_pronoun (ustring& parsing, ustring& definition)
// Parse the extra bits of the pronoun.
{
  ustring code = parsing.substr (0, 1);
  parsing.erase (0, 1);
  if (code == "r") {
    definition.append (" relative");
  }
  if (code == "e") {
    definition.append (" reciprocal");
  }
  if (code == "d") {
    definition.append (" demonstrative");
  }
  if (code == "c") {
    definition.append (" correlative");
  } 
  if (code == "q") {
    definition.append (" interrogative");
  }
  if (code == "i") {
    definition.append (" indefinite");
  }
  if (code == "o") {
    definition.append (" correlative/interrogative");
  }
  if (code == "x") {
    definition.append (" reflexive");
  }
  if (code == "s") {
    definition.append (" possessive");
  }
  if (code == "p") {
    definition.append (" personal");
  }
}


void bibleworks_define_parsing_indeclinable_form_suffix (ustring& parsing, ustring& definition)
// Parse the suffix of the indeclinable form.
{
  if (parsing == "a")
    definition.append (" Aramaic word");
  if (parsing == "h")
    definition.append (" Hebrew word");
  if (parsing == "p")
    definition.append (" indeclinable noun");
  if (parsing == "n")
    definition.append (" indeclinable numeral");
  if (parsing == "l")
    definition.append (" indeclinable letter");
  if (parsing == "o")
    definition.append (" indeclinable other");
}


void bibleworks_define_parsing_conjunction_suffix (ustring& parsing, ustring& definition)
// Parse the suffix of the conjunction.
{
  if (parsing == "c")
    definition.append (" contracted");
  if (parsing == "n")
    definition.append (" not contracted");
}


void bibleworks_define_parsing_adjective_suffix (ustring& parsing, ustring& definition)
// Parse the suffix of the adjective.
{
  if (parsing == "c")
    definition.append (" comparative");
  if (parsing == "s")
    definition.append (" superlative");
  if (parsing == "n")
    definition.append (" no degree");
}


void bibleworks_define_parsing_particle_suffix (ustring& parsing, ustring& definition)
// Parse the suffix of the particle.
{
  if (parsing == "i")
    definition.append (" interrogative");
  if (parsing == "n")
    definition.append (" negative");
  if (parsing == "o")
    definition.append (" other");
}


void bibleworks_define_parsing_adverb_suffix (ustring& parsing, ustring& definition)
// Parse the suffix of the adverb.
{
  if (parsing == "c")
    definition.append (" comparative");
  if (parsing == "s")
    definition.append (" superlative");
  if (parsing == "i")
    definition.append (" interrogative");
  if (parsing == "n")
    definition.append (" normal");
}


void bibleworks_define_parsing_mood (ustring& parsing, ustring& definition)
// Parse the mood of verbs.
{
  ustring mood = parsing.substr (0, 1);  
  parsing.erase (0, 1);
  if (mood == "i")
    definition.append (" indicative");
  if (mood == "s")
    definition.append (" subjunctive");
  if (mood == "o")
    definition.append (" optative");
  if (mood == "d")
    definition.append (" imperative");
  if (mood == "n")
    definition.append (" infinitive");
  if (mood == "p")
    definition.append (" participle");
}


void bibleworks_define_parsing_tense (ustring& parsing, ustring& definition)
// Parse the tense of verbs.
{
  ustring tense = parsing.substr (0, 1);  
  parsing.erase (0, 1);
  if (tense == "p")
    definition.append (" present");
  if (tense == "i")
    definition.append (" imperfect");
  if (tense == "f")
    definition.append (" future");
  if (tense == "a")
    definition.append (" aorist");
  if (tense == "x")
    definition.append (" perfect");
  if (tense == "y")
    definition.append (" pluperfect");
}


void bibleworks_define_parsing_voice (ustring& parsing, ustring& definition)
// Parse the voice of verbs.
{
  ustring voice = parsing.substr (0, 1);  
  parsing.erase (0, 1);
  if (voice == "a")
    definition.append (" active");
  if (voice == "m")
    definition.append (" middle");
  if (voice == "p")
    definition.append (" passive");
  if (voice == "e")
    definition.append (" middle or passive");
  if (voice == "d")
    definition.append (" middle deponent");
  if (voice == "o")
    definition.append (" passive deponent");
  if (voice == "n")
    definition.append (" middle or passive deponent");
  if (voice == "q")
    definition.append (" impersonal active");
  if (voice == "x")
    definition.append (" no voice stated");
}


void bibleworks_define_parsing_person (ustring& parsing, ustring& definition)
// This looks in the "parsing" whether the person is given. 
// If so, it adds the description to the "definition" and removes the relevant code from the "parsing".
{
  ustring person = parsing.substr (0, 1);
  bool person_found = true;
  if (person == "1") {
    definition.append (" first person");
  } else if (person == "2") {
    definition.append (" second person");
  } else if (person == "3") {
    definition.append (" third person");
  } else {
    person_found = false;
  }
  if (person_found) {
    parsing.erase (0, 1);
  }
}


void bibleworks_define_parsing_number (ustring& parsing, ustring& definition)
// Parse the number.
{
  ustring number = parsing.substr (0, 1);
  bool remove_code = true;
  if (number == "s") {
    definition.append (" singular");
  } else if (number == "p") {
    definition.append (" plural");
  } else {
    remove_code = false;
  }
  if (remove_code) {
    parsing.erase (0, 1);
  }
}


bool bibleworks_define_parsing (ustring parsing, ustring& definition)
// Tries to define the parsing as coming from BibleWorks.
// Returns true if it managed.
// The definitions were assembled by trial and error, by searching for a parsing, then looking what BibleWorks gave for it.
{
  // A parsing as imported from BibleWorks should start with the @ character.
  if (parsing.substr (0, 1) != "@") {
    return false;
  }
  parsing.erase (0, 1);
  ustring prefix = parsing.substr (0, 1);
  parsing.erase (0, 1);

  if (prefix == "n") {
    definition = "noun";
    bibleworks_define_parsing_case (parsing, definition);
    bibleworks_define_parsing_gender (parsing, definition);
    bibleworks_define_parsing_number (parsing, definition);
    return true;
  }

  if (prefix == "v") {
    definition = "verb";
    // Parsing of the verb may follow different routes depending on the length of the parsing.
    size_t length_after_v = parsing.length();
    if (length_after_v == 3) {
      bibleworks_define_parsing_mood (parsing, definition);
      bibleworks_define_parsing_tense (parsing, definition);
      bibleworks_define_parsing_voice (parsing, definition);
    }
    if (length_after_v == 4) {
      bibleworks_define_parsing_mood (parsing, definition);
      bibleworks_define_parsing_tense (parsing, definition);
      bibleworks_define_parsing_voice (parsing, definition);
    }
    if (length_after_v == 5) {
      bibleworks_define_parsing_mood (parsing, definition);
      bibleworks_define_parsing_tense (parsing, definition);
      bibleworks_define_parsing_voice (parsing, definition);
      bibleworks_define_parsing_person (parsing, definition);
      bibleworks_define_parsing_number (parsing, definition);
    }
    if (length_after_v == 6) {
      // These are the participles.
      bibleworks_define_parsing_mood (parsing, definition);
      bibleworks_define_parsing_tense (parsing, definition);
      bibleworks_define_parsing_voice (parsing, definition);
      bibleworks_define_parsing_case (parsing, definition);
      bibleworks_define_parsing_gender (parsing, definition);
      bibleworks_define_parsing_number (parsing, definition);
    }
    return true;
  }

  if (prefix == "a") {
    definition = "adjective";
    bibleworks_define_parsing_case (parsing, definition);
    bibleworks_define_parsing_gender (parsing, definition);
    bibleworks_define_parsing_number (parsing, definition);
    bibleworks_define_parsing_adjective_suffix (parsing, definition);
    return true;
  }

  if (prefix == "d") {
    definition = "definite article";
    bibleworks_define_parsing_case (parsing, definition);
    bibleworks_define_parsing_gender (parsing, definition);
    bibleworks_define_parsing_number (parsing, definition);
    return true;
  }

  if (prefix == "p") {
    definition = "preposition";
    return true;
  }

  if (prefix == "c") {
    definition = "conjunction";
    return true;
  }

  if (prefix == "x") {
    definition = "particle or disjunctive particle";
    bibleworks_define_parsing_particle_suffix (parsing, definition);
    return true;
  }

  if (prefix == "i") {
    definition = "interjection";
    return true;
  }

  if (prefix == "b") {
    definition = "adverb";
    bibleworks_define_parsing_adverb_suffix (parsing, definition);
    return true;
  }

  if (prefix == "r") {
    definition = "pronoun";
    bibleworks_define_parsing_pronoun (parsing, definition);
    bibleworks_define_parsing_case (parsing, definition);
    bibleworks_define_parsing_gender (parsing, definition);
    bibleworks_define_parsing_number (parsing, definition);
    return true;
  }

  if (prefix == "t") {
    definition = "indeclinable form";
    bibleworks_define_parsing_indeclinable_form_suffix (parsing, definition);
    return true;
  }

  if (prefix == "q") {
    definition = "conjunction or conjunctive part";
    bibleworks_define_parsing_conjunction_suffix (parsing, definition);
    return true;
  }

  return false;
}


bool bibleworks_is_running ()
// Returns whether BibleWorks runs.
// The reason this was introduced is so that older and future versions of BibleWorks are likely to be recognized.
{
  bool running = false;
  vector <ustring> processes = list_processes ();
  for (unsigned int i = 0; i < processes.size(); i++) {
    ustring process = lowerCase (processes[i]);
    size_t pos = process.find ("0.exe");
    if (pos != string::npos) {
      process.erase (0, pos - 6);
      pos = process.find ("bw");
      running = (pos != string::npos);
    }
  }
  return running;
}


bool bibleworks_reference_get_decode (ustring response, Reference& reference)
{
  // If BibleWorks was not running it might get started at this point, but the value it returns is empty as a result of a timeout.
  if (response.empty())
    return false;
  // The response could be, e.g.: "OK Jer 39:10" (without the quotes).
  replace_text (response, ":", " ");
  Parse parse (response);
  if (parse.words.size() != 4)
    return false;
  reference.book = books_bibleworks_to_id (parse.words[1]);
  reference.chapter = convert_to_int (parse.words[2]);
  reference.verse = parse.words[3];  
  return true;
}

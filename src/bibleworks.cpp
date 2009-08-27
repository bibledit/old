/*
** Copyright (©) 2003-2009 Teus Benschop.
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
  size_t pos = line.find (" ");
  if (pos == string::npos)
    return 0;
  line.erase (0, ++pos);
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


void import_bibleworks_source_language (vector <ustring>& files, const ustring& name, BibleWorksTextConversionType conversion, vector <ustring>& messages)
{
  // Read the text file and the morphology file.
  // Check that the data looks right.
  ReadText rt_text (files[0], true, false);
  if (rt_text.lines.size() < 100) {
    messages.push_back ("There was not enough data to import");
    return;
  }
  ReadText rt_morphology (files[1], true, false);
  if (rt_morphology.lines.size() != rt_text.lines.size()) {
    messages.push_back ("Both files do not contain the same number of verses");
    return;
  }

  // Go through the data.
  ProgressWindow progresswindow ("Importing", false);
  progresswindow.set_iterate (0, 1, rt_text.lines.size());
  for (unsigned int i = 0; i < rt_text.lines.size(); i++) {
    progresswindow.iterate();
    if (i > 2)
      continue;

    // The data per line.
    ustring text_line = trim(rt_text.lines[i]);
    if (text_line.empty())
      continue;
    ustring morphology_line = rt_morphology.lines[i];
    
    // Extract the reference from the line.
    Reference reference (0);
    {
      reference.book = bibleworks_clipboard_file_line_get_extract_book_id (text_line);
      if (reference.book == 0) {
        messages.push_back ("Unknown book: " + text_line);
        return;
      }
      text_line.erase (0, 1);
      bibleworks_clipboard_file_line_get_extract_book_id (morphology_line);
      morphology_line.erase (0, 1);
      ustring chapter = number_in_string(text_line);
      text_line.erase(0, chapter.length() + 1);
      morphology_line.erase(0, chapter.length() + 1);
      reference.chapter = convert_to_int (chapter);
      reference.verse = number_in_string (text_line);
      text_line.erase (0, reference.verse.length());
      morphology_line.erase (0, reference.verse.length());
      text_line = trim (text_line);
      morphology_line = trim (morphology_line);
    }



    

  }
}


typedef struct
{
  const char *input;
  const char *output;
} bibleworks_greek_record;


// A table of double ascii characters and their Greek UTF8 equivalents.
typeof (bibleworks_greek_record) bibleworks_greek_table_2[] =
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
typeof (bibleworks_greek_record) bibleworks_greek_table_1[] =
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

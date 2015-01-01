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


#include <filter/bibleworks.h>
#include <filter/string.h>
#include <locale/translate.h>
#include <database/books.h>


/**
 * Imports BibleWorks $data as USFM code.
 * This is $data fit for the BibleWorks Version Database Compiler.
 * It a string of USFM code.
 * $keepTags: Whether to keep the grammatical tags in the BibleWorks text.
 * These are the tags between () or <>.
 */
string Filter_Bibleworks::import (string data, bool keepTags)
{
  // Storage for the generated USFM.
  vector <string> usfm;
  
  // The data comes as one string. Make it an array.
  vector <string> vdata = filter_string_explode (data, '\n');
  
  // Book / chapter trackers.
  string currentBibleWorksBookAbbreviation = "";
  int currentChapter = 0;
  
  // Go through each line of data to be imported.
  for (auto & line : vdata) {
    line = filter_string_trim (line);
    
    // Get the name of the book and remove the text fragment.
    // Upon encountering a new book, it generates USFM code for it.
    string bookAbbreviation = line.substr (0, 3);
    line = line.substr (3, 10000);
    line = filter_string_trim (line);
    if (bookAbbreviation != currentBibleWorksBookAbbreviation) {
      currentBibleWorksBookAbbreviation = bookAbbreviation;
      currentChapter = 0;
      int bookID = filter_passage_interpret_book (bookAbbreviation);
      string book = Database_Books::getUsfmFromId (bookID);
      usfm.push_back ("\\id " + book);
    }
    
    // Get the chapter number and remove the text fragment.
    // Upon encountering a new chapter, it generates USFM code for it.
    int chapter = convert_to_int (line);
    line = line.substr (convert_to_string (chapter).length() + 1, 10000);
    line = filter_string_trim (line);
    if (chapter != currentChapter) {
      currentChapter = chapter;
      usfm.push_back ("\\c " + convert_to_string (currentChapter));
      usfm.push_back ("\\p");
    }
    
    // Get the verse number and remove the text fragment and whitespace.
    int verse = convert_to_int (line);
    line = line.substr (convert_to_string (verse).length (), 10000);
    line = filter_string_trim (line);
    
    // Convert markup for italics and footnotes.
    line = italics (line);
    line = notes (line);
    
    // Deal with the grammatical tags.
    if (!keepTags) {
      vector <string> malformed;
      line = parenthesis (line, malformed);
      line = chevrons (line, malformed);
    }
    
    // Output the verse.
    usfm.push_back ("\\v " + convert_to_string (verse) +  " " + line);
  }
  
  return filter_string_implode (usfm, "\n");
}


/**
 * Converts the italics in BibleWorks format to italics in USFM format.
 * BibleWorks gives the italics between square brackets, like so: [italics].
 * It returns the converted $line.
 */
string Filter_Bibleworks::italics (string line)
{
  size_t startPosition = line.find ("[");
  while (startPosition != string::npos) {
    size_t endPosition = line.find ("]", startPosition);
    if (endPosition != string::npos) {
      line = substr_replace (line, "\\add*", endPosition, 1);
      line = substr_replace (line, "\\add ", startPosition, 1);
    }
    startPosition = line.find ("[", startPosition + 1);
  }
  return line;
}


/**
 * Converts the notes in BibleWorks format to footnotes in USFM format.
 * BibleWorks gives the notes between curly brackets, like so: [italics].
 * It returns the converted $line.
 */
string Filter_Bibleworks::notes (string line)
{
  size_t startPosition = line.find ("{");
  while (startPosition != string::npos) {
    size_t endPosition = line.find ("}", startPosition);
    if (endPosition != string::npos) {
      line = substr_replace (line, "\\f*", endPosition, 1);
      line = substr_replace (line, "\\f + ", startPosition, 1);
    }
    startPosition = line.find ("{", startPosition + 1);
  }
  return line;
}



/**
 * Removes numericals in parenthesis.
 * The numericals in BibleWorks text contain grammatical tags.
 * Example: (08804)
 * It will only remove well-formed tags.
 * The malformed tags are not removed, and added to array $malformed.
 * It returns the $line without the grammatical tags in parenthesis.
 */
string Filter_Bibleworks::parenthesis (string line, vector <string>& malformed)
{
  size_t startPosition = line.find ("(");
  while (startPosition != string::npos) {
    size_t endPosition = line.find (")", startPosition);
    if (endPosition != string::npos) {
      string text = line.substr (startPosition + 1, endPosition - startPosition - 1);
      if (number_in_string (text) == text) {
        line = substr_replace (line, "", startPosition, endPosition - startPosition + 1);
      } else {
        malformed.push_back (line.substr (startPosition, endPosition - startPosition + 1));
      }
    }
    startPosition = line.find ("(", startPosition + 1);
  }
  return line;
}


/**
 * Removes grammatical tags between chevrons.
 * Example: <06030a>
 * It will only remove well-formed tags.
 * The malformed tags are not removed, and added to array $malformed.
 * It returns the $line without the grammatical tags between chevrons.
 */
string Filter_Bibleworks::chevrons (string line, vector <string>& malformed)
{
  size_t startPosition = line.find ("<");
  while (startPosition != string::npos) {
    size_t endPosition = line.find (">", startPosition);
    if (endPosition != string::npos) {
      string text = line.substr (startPosition + 1, endPosition - startPosition - 1);
      // Tags may contgain an "a" or a "b", e.g.: <06030a>. Remove them.
      text = filter_string_str_replace ("a", "", text);
      text = filter_string_str_replace ("b", "", text);
      // Tags may also contain commas (,) or spaces ( ), e.g. <06030, 06031>
      text = filter_string_str_replace (",", "", text);
      text = filter_string_str_replace (" ", "", text);
      // Remove numerical tags, e.g.: <06030>
      if (number_in_string  (text) == text) {
        line = substr_replace (line, "", startPosition, endPosition - startPosition + 1);
      } else {
        malformed.push_back (line.substr (startPosition, endPosition - startPosition + 1));
      }
    }
    startPosition = line.find ("<", startPosition + 1);
  }
  return line;
}


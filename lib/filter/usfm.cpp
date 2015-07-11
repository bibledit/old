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


#include <filter/usfm.h>
#include <filter/string.h>
#include <filter/diff.h>
#include <database/books.h>
#include <database/styles.h>
#include <database/logs.h>
#include <database/config/bible.h>
#include <styles/logic.h>
#include <webserver/request.h>
#include <bible/logic.h>


BookChapterData::BookChapterData (int book_in, int chapter_in, string data_in)
{
  book = book_in;
  chapter = chapter_in;
  data = data_in;
}


UsfmNote::UsfmNote (int offset_in, string data_in)
{
  offset = offset_in;
  data = data_in;
}


// Returns the string $usfm as one long string.
// $usfm may contain new lines, but the resulting long string won't.
string usfm_one_string (string usfm)
{
  string long_string = "";
  vector <string> usfm_lines = filter_string_explode (usfm, '\n');
  for (string & line : usfm_lines) {
    line = filter_string_trim (line);
    // Skip empty line.
    if (line != "") {
      // The line will be appended to the output line.
      // If it does not start with a backslash (\), a space is inserted first.
      size_t pos = line.find ("\\");
      if (pos != 0) {
        if (long_string != "") long_string += " ";
      }
      long_string += line;
    }
  }
  return long_string;
}


// Returns the string $code as an array alternating between marker and text.
// Example, input is:   \id GEN
//                      \c 10
//             ...
// Output would be:     array ("\id ", "GEN", "\c ", "10", ...)
// If $code does not start with a marker, this becomes visible in the output too.
vector <string> usfm_get_markers_and_text (string code)
{
  vector <string> markers_and_text;
  code = filter_string_str_replace ("\n\\", "\\", code); // New line followed by backslash: leave new line out.
  code = filter_string_str_replace ("\n", " ", code); // New line only: change to space, according to the USFM specification.
  // No removal of double spaces, because it would remove an opening marker (which already has its own space), followed by a space.
  code = filter_string_trim (code);
  while (!code.empty ()) {
    size_t pos = code.find ("\\");
    if (pos == 0) {
      // Marker found.
      // The marker ends
      // - after the first space, or
      // - after the first asterisk (*), or
      // - at the first backslash (\), or
      // - at the end of the string,
      // whichever comes first.
      vector <size_t> positions;
      pos = code.find (" ");
      if (pos != string::npos) positions.push_back (pos + 1);
      pos = code.find ("*");
      if (pos != string::npos) positions.push_back (pos + 1);
      pos = code.find ("\\", 1);
      if (pos != string::npos) positions.push_back (pos);
      positions.push_back (code.length());
      sort (positions.begin (), positions.end());
      pos = positions[0];
      string marker = code.substr (0, pos);
      markers_and_text.push_back (marker);
      code = code.substr (pos);
    } else {
      // Text found. It ends at the next backslash or at the end of the string.
      pos = code.find ("\\");
      if (pos == string::npos) pos = code.length();
      string text = code.substr (0, pos);
      markers_and_text.push_back (text);
      code = code.substr (pos);
    }
  }
  return markers_and_text;
}


// Gets the marker from $usfm if it is there, else returns an empty string.
// Examples:
// "\id"    -> "id"
// "\id "   -> "id"
// "\add*"  -> "add"
// "\+add*" -> "add"
string usfm_get_marker (string usfm)
{
  if (usfm.empty ()) return usfm;
  size_t pos = usfm.find ("\\");
  if (pos == 0) {
    // Marker found.
    // Erase backslash.
    usfm = usfm.substr (1);
    // Optionally erase the + embedded marker.
    pos = usfm.find ("+");
    if (pos == 0) usfm = usfm.substr (1);
    // The marker ends
    // - at the first space, or
    // - at the first asterisk (*), or
    // - at the first backslash (\), or
    // - at the end of the string,
    // whichever comes first.
    vector <size_t> positions;
    pos = usfm.find (" ");
    if (pos != string::npos) positions.push_back (pos);
    pos = usfm.find ("*");
    if (pos != string::npos) positions.push_back (pos);
    pos = usfm.find ("\\");
    if (pos != string::npos) positions.push_back (pos);
    positions.push_back (usfm.length());
    sort (positions.begin(), positions.end());
    pos = positions[0];
    string marker = usfm.substr (0, pos);
    return marker;
  }
  // Text found. No marker.
  return "";
}


// This imports USFM $input.
// It takes raw $input,
// and returns a vector with objects with book_number, chapter_number, chapter_data.
vector <BookChapterData> usfm_import (string input, string stylesheet)
{
  vector <BookChapterData> result;

  int book_number = 0;
  int chapter_number = 0;
  string chapter_data = "";

  input = usfm_one_string (input);
  vector <string> markers_and_text = usfm_get_markers_and_text (input);
  bool retrieve_book_number_on_next_iteration = false;
  bool retrieve_chapter_number_on_next_iteration = false;

  for (string marker_or_text : markers_and_text) {
    if (retrieve_book_number_on_next_iteration) {
      book_number = Database_Books::getIdFromUsfm (marker_or_text.substr (0, 3));
      chapter_number = 0;
      retrieve_book_number_on_next_iteration = false;
    }
    if (retrieve_chapter_number_on_next_iteration) {
      retrieve_chapter_number_on_next_iteration = false;
      chapter_number = convert_to_int (marker_or_text);
    }
    string marker = usfm_get_marker (marker_or_text);
    if (marker != "") {
      // USFM marker found.
      bool store_chapter_data = false;
      if (marker == "id") {
        retrieve_book_number_on_next_iteration = true;
        store_chapter_data = true;
      }
      if (marker == "c") {
        retrieve_chapter_number_on_next_iteration = true;
        store_chapter_data = true;
      }
      if (store_chapter_data) {
        chapter_data = filter_string_trim (chapter_data);
        if (chapter_data != "") result.push_back ( { book_number, chapter_number, chapter_data } );
        chapter_number = 0;
        chapter_data = "";
        store_chapter_data = false;
      }
      Database_Styles database_styles = Database_Styles ();
      Database_Styles_Item marker_data = database_styles.getMarkerData (stylesheet, marker);
      int type = marker_data.type;
      int subtype = marker_data.subtype;
      if (styles_logic_starts_new_line_in_usfm (type, subtype)) {
        chapter_data += "\n";
      }
    }
    chapter_data += marker_or_text;
  }
  chapter_data = filter_string_trim (chapter_data);
  if (chapter_data != "") result.push_back (BookChapterData (book_number, chapter_number, chapter_data));
  return result;
}


// Returns an array with the verse numbers found in $usfm.
// It handles a single verse, a range of verses, or a sequence of verses.
// It locates separate whole verse numbers.
// Examples:
// 10
// 10-12b
// 10,11a
// 10,12
vector <int> usfm_get_verse_numbers (string usfm)
{
  vector <int> verse_numbers = { 0 };
  vector <string> markers_and_text = usfm_get_markers_and_text (usfm);
  bool extract_verse = false;
  for (string marker_or_text : markers_and_text) {
    if (extract_verse) {
      string verse = usfm_peek_verse_number (marker_or_text);
      
      // If there is a range, take the beginning and the end and fill up in between.
      if (verse.find("-") != string::npos) {
        size_t position;
        position = verse.find("-");
        string start_range, end_range;
        start_range = verse.substr (0, position);
        verse.erase (0, ++position);
        end_range = verse;
        unsigned int start_verse_i = convert_to_int(number_in_string(start_range));
        unsigned int end_verse_i = convert_to_int(number_in_string(end_range));
        for (unsigned int i = start_verse_i; i <= end_verse_i; i++) {
          if (i == start_verse_i)
            verse_numbers.push_back (convert_to_int (start_range));
          else if (i == end_verse_i)
            verse_numbers.push_back (convert_to_int (end_range));
          else
            verse_numbers.push_back (i);
        }
      }
      
      // Else if there is a sequence, take each verse in the sequence, and store it.
      else if (verse.find(",") != string::npos) {
        int iterations = 0;
        do {
          // In case of an unusual range formation, do not hang, but give message.
          iterations++;
          if (iterations > 50) {
            break;
          }
          size_t position = verse.find (",");
          string vs;
          if (position == string::npos) {
            vs = verse;
            verse.clear ();
          } else {
            vs = verse.substr (0, position);
            verse.erase(0, ++position);
          }
          verse_numbers.push_back (convert_to_int (vs));
        } while (!verse.empty());
      }
      
      // No range and no sequence: a single verse.
      else {
        verse_numbers.push_back (convert_to_int (verse));
      }
      
      extract_verse = false;
    }
    if (marker_or_text.substr (0, 2) == "\\v") {
      extract_verse = true;
    }
  }
  return verse_numbers;
}


// Returns the verse numbers in the string of $usfm code at line number $line_number.
vector <int> usfm_linenumber_to_versenumber (string usfm, unsigned int line_number)
{
  vector <int> verse_number = {0}; // Initial verse number.
  vector <string> lines = filter_string_explode (usfm, '\n');
  for (unsigned int i = 0; i < lines.size(); i++) {
    if (i <= line_number) {
      vector <int> verse_numbers = usfm_get_verse_numbers (lines[i]);
      if (verse_numbers.size() >= 2) {
        verse_number = filter_string_array_diff (verse_numbers, {0});
      }
    }
  }
  return verse_number;
}


// Returns the verse numbers in the string of $usfm code at offset $offset.
// Offset is calculated with unicode_string_length to support UTF-8.
vector <int> usfm_offset_to_versenumber (string usfm, unsigned int offset)
{
  unsigned int totalOffset = 0;
  vector <string> lines = filter_string_explode (usfm, '\n');
  for (unsigned int i = 0; i < lines.size(); i++) {
    int length = unicode_string_length (lines [i]);
    totalOffset += length;
    if (totalOffset >= offset) {
      return usfm_linenumber_to_versenumber (usfm, i);
    }
    // Add 1 for new line.
    totalOffset += 1;
  }
  return {0};
}



// Returns the offset within the $usfm code where $verse number starts.
int usfm_versenumber_to_offset (string usfm, int verse)
{
  // Verse number 0 starts at offset 0.
  if (verse == 0) return 0;
  int totalOffset = 0;
  vector <string> lines = filter_string_explode (usfm, '\n');
  for (string line : lines) {
    vector <int> verses = usfm_get_verse_numbers (line);
    for (auto & v : verses) {
      if (v == verse) return totalOffset;
    }
    totalOffset += unicode_string_length (line);
    // Add 1 for new line.
    totalOffset += 1;
  }
  return unicode_string_length (usfm);
}


// Returns the verse text given a $verse_number and $usfm code.
string usfm_get_verse_text (string usfm, int verse_number)
{
  // The start of the requested verse number.
  string sverse = convert_to_string (verse_number);
  size_t cleanPos = usfm.find ("\\v " + sverse + " ");
  size_t dirtyPos = usfm.find ("\\v " + sverse);
  size_t startPosition;
  if (verse_number == 0) {
    startPosition = 0;
  } else if (cleanPos != string::npos) {
    startPosition = cleanPos;
  } else if (dirtyPos != string::npos) {
    startPosition = dirtyPos;
  } else {
    // The verse number was not found.
    return "";
  }

  // The end of the requested verse number.
  size_t endPosition = usfm.find ("\\v", startPosition + 1);
  if (endPosition == string::npos) endPosition = usfm.length ();

  // Return the verse text.
  string verseText = usfm.substr (startPosition, endPosition - startPosition);
  verseText = filter_string_trim (verseText);
  return verseText;
}


// Returns true if the $code contains an USFM marker.
bool usfm_is_usfm_marker (string code)
{
  if (code.length () < 2) return false;
  if (code.substr (0, 1) == "\\") return true;
  return false;
}


// Returns true if the marker in $usfm is an opening marker.
// Else it returns false.
bool usfm_is_opening_marker (string usfm)
{
  return usfm.find ("*") == string::npos;
}


// Returns true if the marker in $usfm is an embedded marker.
// Else it returns false.
bool usfm_is_embedded_marker (string usfm)
{
  return usfm.find ( "+") != string::npos;
}


// Returns the USFM book identifier.
// $usfm: array of strings alternating between USFM code and subsequent text.
// $pointer: if increased by one, it should point to the \id in $usfm.
string usfm_get_book_identifier (const vector <string>& usfm, unsigned int pointer)
{
  string identifier = "XXX"; // Fallback value.
  if (++pointer < usfm.size ()) {
    identifier = usfm[pointer].substr (0, 3);
  }
  return identifier;
}


// Returns the text that follows a USFM marker.
// $usfm: array of strings alternating between USFM code and subsequent text.
// $pointer: should point to the marker in $usfm. It gets increased by one.
string usfm_get_text_following_marker (const vector <string>& usfm, unsigned int & pointer)
{
  string text = ""; // Fallback value.
  ++pointer;
  if (pointer < usfm.size()) {
    text = usfm [pointer];
  }
  return text;
}


// Returns the text that follows a USFM marker.
// $usfm: array of strings alternating between USFM code and subsequent text.
// $pointer: should point to the marker in $usfm. Pointer is left as it is.
string usfm_peek_text_following_marker (const vector <string>& usfm, unsigned int pointer)
{
  return usfm_get_text_following_marker (usfm, pointer);
}


// Returns the verse number in the $usfm code.
string usfm_peek_verse_number (string usfm)
{
  // Make it robust, even handling cases like \v 1-2“Moi - No space after verse number.
  string verseNumber = "";
  size_t usfmStringLength = usfm.length ();
  unsigned int i = 0;
  for (i = 0; i < usfmStringLength; i++) {
    string character = usfm.substr (i, 1);
    if (character == "0") continue;
    if (character == "1") continue;
    if (character == "2") continue;
    if (character == "3") continue;
    if (character == "4") continue;
    if (character == "5") continue;
    if (character == "6") continue;
    if (character == "7") continue;
    if (character == "8") continue;
    if (character == "9") continue;
    if (character == ",") continue;
    if (character == "-") continue;
    if (character == "a") continue;
    if (character == "b") continue;
    break;
  }
  verseNumber = usfm.substr (0, i);
  verseNumber = filter_string_trim (verseNumber);
  return verseNumber;
}


// Takes a marker in the form of text only, like "id" or "add",
// and converts it into opening USFM, like "\id " or "\add ".
// Supports the embedded markup "+".
string usfm_get_opening_usfm (string text, bool embedded)
{
  string embed = embedded ? "+" : "";
  return "\\" + embed + text + " ";
}


// Takes a marker in the form of text only, like "add",
// and converts it into closing USFM, like "\add*".
// Supports the embedded markup "+".
string usfm_get_closing_usfm (string text, bool embedded)
{
  string embed = embedded ? "+" : "";
  return "\\" + embed + text + "*";
}


// This function extracts notes, e.g. cross references.
// $usfm: Where to extract from.
// $markers: Array of possible markers surrounding the notes.
// It returns an array of array ($offset, $note).
vector <UsfmNote> usfm_extract_notes (string usfm, const vector <string> & markers)
{
  set <string> openers;
  set <string> closers;
  for (string marker : markers) {
    openers.insert (usfm_get_opening_usfm (marker));
    closers.insert (usfm_get_closing_usfm (marker));
  }

  vector <string> markers_and_text = usfm_get_markers_and_text (usfm);

  vector <UsfmNote> notes;
  
  bool within_note = false;
  size_t running_offset = 0;
  string running_note = "";

  for (string item : markers_and_text) {
    
    if (openers.find (item) != openers.end ()) within_note = true;

    // Caclulate the offset in the main text. 
    // That means not to consider the length of the notes.
    if (!within_note) {
      running_offset += unicode_string_length (item);
    }
    
    if (within_note) running_note += item;

    if (closers.find (item) != closers.end()) {
      notes.push_back (UsfmNote (running_offset, running_note));
      running_note = "";
      within_note = false;
    }
  }
  
  return notes;
}


// This function removes the notes from the USFM.
// $usfm: Where to extract from.
// $markers: Array of markers surrounding the notes.
// It returns the USFM without the notes.
string usfm_remove_notes (string usfm, const vector <string> & markers)
{
  vector <UsfmNote> notes = usfm_extract_notes (usfm, markers);
  for (auto note : notes) {
    string search = note.data;
    usfm = filter_string_str_replace (search, "", usfm);
  }
  return usfm;
}


// This function inserts notes into USFM.
// It replaces $search with $replace in the $notes.
// $usfm: string where to insert the notes - it is assumed not to contain any notes yet.
// $notes: object of offsets => notes.
// $ratio: ratio to multiply the offsets with.
// It returns the updated USFM.
string usfm_insert_notes (string usfm, vector <UsfmNote> notes, float ratio)
{
  if (usfm.empty()) return usfm;
  if (notes.empty()) return usfm;
  
  vector <string> markers = {"x", "f", "fe"};
  
  // Temporally extract existing notes.
  vector <UsfmNote> existing = usfm_extract_notes (usfm, markers);

  // Work with USFM without notes so that a note will not be inserted in another note.
  usfm = usfm_remove_notes (usfm, markers);

  // Calculate insertion positions before actually inserting the notes to avoid nested notes placement.
  for (UsfmNote & note : notes) {
    int position = note.offset;
    position = convert_to_int (position * ratio);
    position = usfm_get_new_note_position (usfm, position, 0);
    note.offset = position;
  }

  // Add existing notes data.
  notes.insert (notes.end(), existing.begin(), existing.end());

  // Sort the notes such that the last one gets inserted first.
  // This way inserting happens from the end of the USFM towards the start.
  // Inserted text does not affect any insertion positions this way.
  int highest_position = 0;
  for (UsfmNote note : notes) {
    int position = note.offset;
    if (position > highest_position) {
      highest_position = position;
    }
  }
  vector <UsfmNote> notes2;
  for (int i = highest_position; i >= 0; i--) {
    for (UsfmNote note : notes) {
      if (note.offset == i) {
        notes2.push_back (note);
      }
    }
  }

  // Insert the notes into the USFM at the correct position.
  for (UsfmNote note : notes2) {
    int position = note.offset;
    string text = note.data;
    usfm.insert (position, text);
  }

  return usfm;
}


// This function moves a note one word back or forth in the USFM data.
// $usfm: The USFM data.
// $direction: The direction into which to move the note.
// $number: The note number. Starts at 1.
// Returns the updated USFM.
string usfm_move_note (string usfm, int direction, int number)
{
  vector <string> markers = {"x", "f", "fe"};
  vector <UsfmNote> notes = usfm_extract_notes (usfm, markers);
  int key = number - 1;
  if (key < 0) return usfm;
  if (key >= (int)notes.size()) return usfm;
  usfm = usfm_remove_notes (usfm, markers);
  int position = notes[key].offset;
  position += (direction * 3);
  position = usfm_get_new_note_position (usfm, position, direction);
  notes[key].offset = position;
  usfm = usfm_insert_notes (usfm, notes, 1);
  return usfm;
}


// This function gets a new position to insert a note.
// $usfm: The USFM code to work with.
// $position: Current position.
// $direction: Direction where to go to find the new position:
//   -1: Go back to the previous slot.
//    0: Attempt current position, else take next slot.
//    1: Go forward to the next slot.
// The positions take the string as UTF8.
size_t usfm_get_new_note_position (string usfm, size_t position, int direction)
{
  vector <string> words = filter_string_explode (usfm, ' ');

  size_t length = 0;

  vector <size_t> lengths;

  for (string word : words) {

    // Add length of item.
    length += unicode_string_length (word);
    
    // Check whether at opening marker.
    bool opening_marker = usfm_is_usfm_marker (word);
    if (opening_marker) {
      opening_marker = usfm_is_opening_marker (word);
    }

    // Don't create a slot for placing a note right after an opening marker.
    if (!opening_marker) lengths.push_back (length);

    // Add length of space.
    length++;
    
  }
  
  bool found = false;
  
  if (direction > 0) {
    // Take next position.
    for (size_t length : lengths) {
      if (found) continue;
      if (length > position) {
        position = length;
        found = true;
      }
    }
  } else if (direction < 0) {
    // Take previous position.
    vector <size_t> lengths_r (lengths.begin(), lengths.end());
    reverse (lengths_r.begin(), lengths_r.end());
    for (size_t length : lengths_r) {
      if (found) continue;
      if (length < position) {
        position = length;
        found = true;
      }
    }
  } else {
    // Take optimal position.
    for (size_t length : lengths) {
      if (found) continue;
      if (length >= position) {
        position = length;
        found = true;
      }
    }
  }
  
  if (!found) {
    position = unicode_string_length (usfm);
  }
  
  // Move a note to before punctuation.
  set <string> punctuation = {".", ",", ";", ":", "?", "!"};
  string character = unicode_string_substr (usfm, position - 1, 1);
  if (punctuation.find (character) != punctuation.end()) position--;

  return position;
}


// This function compares the $newtext with the $oldtext.
// It returns true if the difference is below the limit set for the Bible.
// It returns false if the difference exceeds that limit.
bool usfm_save_is_safe (string bible, string oldtext, string newtext, bool chapter)
{
  // Two texts are equal: safe.
  if (newtext == oldtext) return true;
  
  // Allowed percentage difference.
  int allowed_percentage = Database_Config_Bible::getEditingAllowedDifferenceVerse (bible);
  if (chapter) allowed_percentage = Database_Config_Bible::getEditingAllowedDifferenceChapter (bible);

  // The length of the new text should not differ more than a set percentage from the old text.
  float existingLength = oldtext.length();
  float newLength = newtext.length ();
  int percentage = 100 * (newLength - existingLength) / existingLength;
  percentage = abs (percentage);
  if (percentage > allowed_percentage) {
    Database_Logs::log ("The text was not saved for safety reasons. The length differs " + convert_to_string (percentage) + "% from the existing text. Make smaller changes and save more often. Or relax the restriction in the Bible's editing settings.");
    Database_Logs::log (newtext);
    return false;
  }
  
  // The new text should be at least a set percentage similar to the old text.
  percentage = filter_diff_similarity (oldtext, newtext);
  if (percentage < (100 - allowed_percentage)) {
    Database_Logs::log ("The text was not saved for safety reasons. The new text is " + convert_to_string (percentage) + "% similar to the existing text. Make smaller changes and save more often. Or relax the restriction in the Bible's editing settings.");
    Database_Logs::log (newtext);
    return false;
  }
  
  // Safety checks have passed.
  return true;
}


// Function to safely store a chapter.
// It saves the chapter if the new USFM does not differ too much from the existing USFM.
// It returns true or false depending on success.
// This function proves useful in cases that the text in the Bible editor gets corrupted
// due to human error.
// It also is useful in cases where the session is deleted from the server,
// where the text in the editors would get corrupted.
// It also is useful in view of an unstable connection between browser and server, to prevent data corruption.
bool usfm_safely_store_chapter (void * webserver_request, string bible, int book, int chapter, string usfm)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  // Existing chapter contents.
  string existing = request->database_bibles()->getChapter (bible, book, chapter);
  
  // Bail out if the existing chapter equals the USFM to be saved.
  if (usfm == existing) return true;
  
  // Safety check.
  if (!usfm_save_is_safe (bible, existing, usfm, true)) return false;
  
  // Safety checks have passed: Save chapter.
  Bible_Logic::storeChapter (bible, book, chapter, usfm);
  return true;
}


// Function to safely store a verse.
// It saves the verse if the new USFM does not differ too much from the existing USFM.
// It returns true or false depending on success.
// This function proves useful in cases that the text in the Bible editor gets corrupted
// due to human error.
// It also is useful in cases where the session is deleted from the server,
// where the text in the editors would get corrupted.
// It also is useful in view of an unstable connection between browser and server, to prevent data corruption.
bool usfm_safely_store_verse (void * webserver_request, string bible, int book, int chapter, int verse, string usfm)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  usfm = filter_string_trim (usfm);

  // Check that the USFM to be saved is for the correct verse.
  vector <int> verses = usfm_get_verse_numbers (usfm);
  if ((verse != 0) && !verses.empty ()) {
    verses.erase (verses.begin());
  }
  if (verses.empty ()) {
    Database_Logs::log ("The USFM contains no verse information: " + usfm);
    return false;
  }
  if (verses.size () != 1) {
    Database_Logs::log ("The USFM contains more than one verse: " + usfm);
    return false;
  }
  int usfmverse = verses [0];
  if (verse != usfmverse) {
    Database_Logs::log ("The USFM contains verse " + convert_to_string (usfmverse) + ", which would overwrite existing verse " + convert_to_string (verse) + ": " + usfm);
    return false;
  }

  // Get the existing chapter USFM into a map of verse => USFM fragments.
  string usfmString = request->database_bibles()->getChapter (bible, book, chapter);
  verses = usfm_get_verse_numbers (usfmString);
  {
    set <int> unique (verses.begin(), verses.end());
    verses.assign (unique.begin(), unique.end());
  }
  sort (verses.begin(), verses.end());
  map <int, string> usfmMap;
  for (auto vs : verses) {
    usfmMap [vs] = usfm_get_verse_text (usfmString, vs);
  }
  
  // Bail out if the new USFM is the same as the existing.
  if (usfm == usfmMap [verse]) {
    return true;
  }
  
  
  // Check maximum difference between new and existing USFM.
  if (!usfm_save_is_safe (bible, usfmMap [verse], usfm, false)) {
    return false;
  }
  
  
  // Store the new verse USFM in the map.
  usfmMap [verse] = usfm;
  
  
  // Create the updated chapter USFM as a string.
  usfm.clear ();
  for (auto & element : usfmMap) {
    if (!usfm.empty ()) usfm.append ("\n");
    usfm.append (element.second);
  }
  
  
  // Safety checks have passed: Save chapter.
  Bible_Logic::storeChapter (bible, book, chapter, usfm);
  return true;
}

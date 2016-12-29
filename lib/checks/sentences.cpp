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


#include <checks/sentences.h>
#include <webserver/request.h>
#include <filter/string.h>


void Checks_Sentences::enterCapitals (string capitals_in)
{
  capitals = filter_string_explode (capitals_in, ' ');
}


void Checks_Sentences::enterSmallLetters (string small_letters_in)
{
  small_letters = filter_string_explode (small_letters_in, ' ');
}


void Checks_Sentences::enterEndMarks (string end_marks_in)
{
  end_marks = filter_string_explode (end_marks_in, ' ');
}


void Checks_Sentences::enterCenterMarks (string center_marks_in)
{
  center_marks = filter_string_explode (center_marks_in, ' ');
}


void Checks_Sentences::enterDisregards (string disregards_in)
{
  disregards = filter_string_explode (disregards_in, ' ');
}


void Checks_Sentences::enterNames (string names_in)
{
  names.clear ();
  names_in = filter_string_str_replace ("\n", " ", names_in);
  vector <string> names2 = filter_string_explode (names_in, ' ');
  for (auto name : names2) {
    if (name != "") {
      // Limit the length to the left of the suffix in the test.
      name = unicode_string_substr (name, 0, 11);
      names.push_back (name);
    }
  }
}


void Checks_Sentences::initialize ()
{
  currentPosition = 0;
  spacePosition = 0;
  capitalPosition = 0;
  smallLetterPosition = 0;
  endMarkPosition = 0;
  centerMarkPosition = 0;
  punctuationMarkPosition = 0;
  previousMarkPosition = 0;
  checkingResults.clear ();
  fullText.clear ();
}


void Checks_Sentences::check (map <int, string> texts)
{
  vector <int> verse_numbers;
  vector <string> characters;
  int iterations = 0;
  for (auto element : texts) {
    int verse = element.first;
    string text = element.second;
    // For the second and subsequent verse_numbers, add a space to the text,
    // because this is what is supposed to happen in USFM.
    if (iterations > 0) {
      verse_numbers.push_back (verse);
      characters.push_back (" ");
      fullText += " ";
    }
    // Split the UTF-8 text into characters and add them to the arrays of verse_numbers / characters.
    int count = unicode_string_length (text);
    for (int i = 0; i < count; i++) {
      character = unicode_string_substr (text, i, 1);
      // Skip characters to be disregarded.
      if (in_array (character, disregards)) continue;
      // Store verse numbers and characters.
      verse_numbers.push_back (verse);
      characters.push_back (character);
      fullText += character;
    }
    // Next iteration.
    iterations++;
  }
  
  // Go through the characters.
  int characterCount = characters.size ();
  for (int i = 0; i < characterCount; i++) {
    // Store current verse number in the object.
    verseNumber = verse_numbers [i];
    // Get the current character.
    character = characters [i];
    // Analyze the character.
    analyzeCharacters ();
    // Run the checks.
    checkUnknownCharacter ();
    checkCharacter ();
  }
}


void Checks_Sentences::checkCharacter ()
{
  // Handle a capital after a comma: ... He said, Go ...
  if (this->isCapital)
    if (this->spacePosition > 0)
      if (this->currentPosition == this->spacePosition + 1)
        if (this->currentPosition == this->centerMarkPosition + 2)
          this->addResult ("Capital follows mid-sentence punctuation mark", Checks_Sentences::skipNames);
  
  
  // Handle a small letter straight after mid-sentence punctuation: ... He said,go ...
  if (this->isSmallLetter)
    if (this->centerMarkPosition > 0)
      if (this->currentPosition == this->centerMarkPosition + 1)
        this->addResult ("Small letter follows straight after a mid-sentence punctuation mark", Checks_Sentences::displayContext);
  
  
  // Handle a capital straight after mid-sentence punctuation: ... He said,Go ...
  if (this->isCapital)
    if (this->centerMarkPosition > 0)
      if (this->currentPosition == this->centerMarkPosition + 1)
        this->addResult ("Capital follows straight after a mid-sentence punctuation mark", Checks_Sentences::displayContext);
  
  
  // Handle small letter or capital straight after end-sentence punctuation: He said.Go. // He said.go.
  if ((this->isSmallLetter) || (this->isCapital))
    if (this->endMarkPosition > 0)
      if (this->currentPosition == this->endMarkPosition + 1)
        this->addResult ("A letter follows straight after an end-sentence punctuation mark", Checks_Sentences::displayContext);
  
  
  // Handle case of no capital after end-sentence punctuation: He did that. he went.
  if (this->isSmallLetter)
    if (this->endMarkPosition > 0)
      if (this->currentPosition == this->endMarkPosition + 2)
        this->addResult ("No capital after an end-sentence punctuation mark", Checks_Sentences::displayContext);
  
  
  // Handle two punctuation marks in sequence.
  if (this->isEndMark || this->isCenterMark)
    if (this->currentPosition == this->previousMarkPosition + 1)
      this->addResult ("Two punctuation marks in sequence", Checks_Sentences::displayContext);
  
}


// Checks paragraphs of text whether they are start and end with correct capitalization and punctuation.
// $texts: fragments of texts per verse number.
// $paragraph_start_positions: The character positions where new paragraphs start.
// $paragraph_start_markers: The USFM markers that started the new paragraphs in $paragraph_start_positions.
// $within_sentence_paragraph_markers: The USFM markers that start paragraphs that do not need to start with the correct capitalization. Usually such markers are poetic markers like \q1 and so on.
void Checks_Sentences::paragraphs (map <int, string> texts,
                                   vector <int> paragraph_start_positions,
                                   vector <string> paragraph_start_markers,
                                   vector <string> within_sentence_paragraph_markers)
{
  vector <int> verses;
  vector <string> characters;
  
  // Put the UTF-8 text into the arrays of verses and characters.
  for (auto element : texts) {
    int verse = element.first;
    string text = element.second;
    int count = unicode_string_length (text);
    for (int i = 0; i < count; i++) {
      string character = unicode_string_substr (text, i, 1);
      verses.push_back (verse);
      characters.push_back (character);
    }
  }
  
  // Correct the positions where the paragraphs start.
  for (unsigned int i = 1; i < paragraph_start_positions.size(); i++) {
    unsigned int offset = paragraph_start_positions [i];
    int paragraphVerse = 0;
    if (offset < verses.size()) paragraphVerse = verses [offset];
    int twoVersesBack = 0;
    if ((offset - 2) < verses.size ()) twoVersesBack = verses [offset - 2];
    if (paragraphVerse != twoVersesBack) {
      for (unsigned int i2 = i; i2 < paragraph_start_positions.size(); i2++) {
        paragraph_start_positions [i2] = paragraph_start_positions [i2] - 1;
      }
    }
  }
  
  int paragraphCount = paragraph_start_positions.size();
  
  // Go through the paragraphs to see whether they start with capitals.
  for (int i = 0; i < paragraphCount; i++) {
    unsigned int offset = paragraph_start_positions [i];
    int verse = 0;
    if (offset < verses.size()) verse = verses [offset];
    string character;
    if (offset < characters.size ()) character = characters [offset];
    isCapital = in_array (character, capitals);
    if (!isCapital) {
      string paragraph_marker = paragraph_start_markers [i];
      if (!in_array (paragraph_marker, within_sentence_paragraph_markers)) {
        string context;
        for (unsigned int i2 = offset; i2 < offset + 10; i2++) {
          if (i2 < characters.size ()) context.append (characters[i2]);
        }
        checkingResults.push_back (make_pair (verse, "Paragraph does not start with a capital: " + context));
      }
    }
  }
  
  // Go through the paragraphs to see whether they end with proper punctuation.
  for (int i = 0; i < paragraphCount; i++) {
    unsigned int offset = 0;
    string next_paragraph_marker;
    if (i < (paragraphCount - 1)) {
      offset = paragraph_start_positions [i + 1];
      next_paragraph_marker = paragraph_start_markers [i + 1];
    } else {
      offset = characters.size();
    }
    offset--;
    int verse = 0;
    if (offset < verses.size()) verse = verses [offset];
    string character;
    if (offset < characters.size ()) character = characters [offset];
    string previous_character;
    if (offset) if (offset < characters.size ()) previous_character = characters [offset - 1];
    isEndMark = in_array (character, this->end_marks) || in_array (previous_character, this->end_marks);
    if (!isEndMark) {
      if (next_paragraph_marker.empty () || (!in_array (next_paragraph_marker, within_sentence_paragraph_markers))) {
        string context;
        for (int i2 = offset - 10; i2 <= (int)offset; i2++) {
          if (i2 >= 0) context.append (characters[i2]);
        }
        checkingResults.push_back (make_pair (verse, "Paragraph does not end with an end marker: " + context));
      }
    }
  }
  
}


vector <pair<int, string>> Checks_Sentences::getResults ()
{
  return checkingResults;
}


void Checks_Sentences::addResult (string text, int modifier)
{
  // Get previous and next text fragment.
  int start = currentPosition - 25;
  if (start < 0) start = 0;
  string previousFragment = unicode_string_substr (fullText, start, currentPosition - start - 1);
  int iterations = 5;
  while (iterations) {
    size_t pos = previousFragment.find (" ");
    if (pos != string::npos) {
      if ((previousFragment.length () - pos) > 10) {
        previousFragment.erase (0, pos + 1);
      }
    }
    iterations--;
  }
  string nextFragment = unicode_string_substr (fullText, currentPosition, 25);
  while (nextFragment.length () > 10) {
    size_t pos = nextFragment.rfind (" ");
    if (pos == string::npos) nextFragment.erase (nextFragment.length () - 1, 1);
    else nextFragment.erase (pos);
  }
  // Check whether the result can be skipped due to a name being involved.
  if (modifier == skipNames) {
    string haystack = character + nextFragment;
    for (auto name : names) {
      if (haystack.find (name) == 0) return;
    }
  }
  // Assemble text for checking result.
  if (modifier == displayCharacterOnly) {
    text += ": " + character;
  }
  if ((modifier == displayContext) || (modifier == skipNames)) {
    text += ": " + previousFragment + character + nextFragment;
  }
  // Store checking result.
  checkingResults.push_back (make_pair (verseNumber, text));
}


void Checks_Sentences::checkUnknownCharacter ()
{
  if (isSpace) return;
  if (isCapital) return;
  if (isSmallLetter) return;
  if (isEndMark) return;
  if (isCenterMark) return;
  addResult ("Unknown character", Checks_Sentences::displayCharacterOnly);
}


void Checks_Sentences::analyzeCharacters ()
{
  currentPosition++;
  
  isSpace = (character == " ");
  if (isSpace) {
    spacePosition = currentPosition;
  }
  
  isCapital = in_array (character, capitals);
  if (isCapital) {
    capitalPosition = currentPosition;
  }
  
  isSmallLetter = in_array (character, small_letters);
  if (isSmallLetter) {
    smallLetterPosition = currentPosition;
  }
  
  isEndMark = in_array (character, end_marks);
  if (isEndMark) {
    endMarkPosition = currentPosition;
    previousMarkPosition = punctuationMarkPosition;
    punctuationMarkPosition = currentPosition;
  }
  
  isCenterMark = in_array (character, center_marks);
  if (isCenterMark) {
    centerMarkPosition = currentPosition;
    previousMarkPosition = punctuationMarkPosition;
    punctuationMarkPosition = currentPosition;
  }
}

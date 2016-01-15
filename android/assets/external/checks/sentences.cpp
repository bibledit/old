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
  vector <string> graphemes;
  int iterations = 0;
  for (auto element : texts) {
    int verse = element.first;
    string text = element.second;
    // For the second and subsequent verse_numbers, add a space to the text,
    // because this is what is supposed to happen in USFM.
    if (iterations > 0) {
      verse_numbers.push_back (verse);
      graphemes.push_back (" ");
      fullText += " ";
    }
    // Split the UTF-8 text into graphemes and add them to the arrays of verse_numbers/graphemes.
    int count = unicode_string_length (text);
    for (int i = 0; i < count; i++) {
      grapheme = unicode_string_substr (text, i, 1);
      // Skip graphemes to be disregarded.
      if (find (disregards.begin(), disregards.end (), grapheme) != disregards.end()) continue;
      // Store verse numbers and graphemes.
      verse_numbers.push_back (verse);
      graphemes.push_back (grapheme);
      fullText += grapheme;
    }
    // Next iteration.
    iterations++;
  }
  
  // Go through the graphemes.
  int graphemeCount = graphemes.size ();
  for (int i = 0; i < graphemeCount; i++) {
    // Store current verse number in the object.
    verseNumber = verse_numbers [i];
    // Get the current grapheme.
    grapheme = graphemes [i];
    // Analyze the grapheme.
    analyzeGrapheme ();
    // Run the checks.
    checkUnknownCharacter ();
    checkGrapheme ();
  }
}


void Checks_Sentences::checkGrapheme ()
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


void Checks_Sentences::paragraphs (map <int, string> texts, vector <int> paragraphs)
{
  vector <int> verses;
  vector <string> graphemes;
  
  // Put the UTF-8 text into the arrays of verses and graphemes.
  for (auto element : texts) {
    int verse = element.first;
    string text = element.second;
    int count = unicode_string_length (text);
    for (int i = 0; i < count; i++) {
      string grapheme = unicode_string_substr (text, i, 1);
      verses.push_back (verse);
      graphemes.push_back (grapheme);
    }
  }
  
  // Correct the positions where the paragraphs start.
  for (unsigned int i = 1; i < paragraphs.size(); i++) {
    unsigned int offset = paragraphs [i];
    int paragraphVerse = 0;
    if (offset < verses.size()) paragraphVerse = verses [offset];
    int twoVersesBack = 0;
    if ((offset - 2) < verses.size ()) twoVersesBack = verses [offset - 2];
    if (paragraphVerse != twoVersesBack) {
      for (unsigned int i2 = i; i2 < paragraphs.size(); i2++) {
        paragraphs [i2] = paragraphs [i2] - 1;
      }
    }
  }
  
  int paragraphCount = paragraphs.size();
  
  // Go through the paragraphs to see whether they start with capitals.
  for (int i = 0; i < paragraphCount; i++) {
    unsigned int offset = paragraphs [i];
    int verse = 0;
    if (offset < verses.size()) verse = verses [offset];
    string grapheme;
    if (offset < graphemes.size ()) grapheme = graphemes [offset];
    isCapital = find (capitals.begin(), capitals.end(), grapheme) != capitals.end ();
    if (!isCapital) {
      checkingResults.push_back (make_pair (verse, "Paragraph does not start with a capital: " + grapheme));
    }
  }
  
  // Go through the paragraphs to see whether they end with proper punctuation.
  for (int i = 0; i < paragraphCount; i++) {
    unsigned int offset = 0;
    if (i < (paragraphCount - 1)) {
      offset = paragraphs [i + 1];
    } else {
      offset = graphemes.size();
    }
    offset--;
    int verse = 0;
    if (offset < verses.size()) verse = verses [offset];
    string grapheme;
    if (offset < graphemes.size ()) grapheme = graphemes [offset];
    string previousGrapheme;
    if (offset) if (offset < graphemes.size ()) previousGrapheme = graphemes [offset - 1];
    isEndMark = in_array (grapheme, this->end_marks) || in_array (previousGrapheme, this->end_marks);
    if (!isEndMark) {
      checkingResults.push_back (make_pair (verse, "Paragraph does not end with an end marker: " + grapheme));
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
    string haystack = grapheme + nextFragment;
    for (auto name : names) {
      if (haystack.find (name) == 0) return;
    }
  }
  // Assemble text for checking result.
  if (modifier == displayGraphemeOnly) {
    text += ": " + grapheme;
  }
  if ((modifier == displayContext) || (modifier == skipNames)) {
    text += ": " + previousFragment + grapheme + nextFragment;
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
  addResult ("Unknown character", Checks_Sentences::displayGraphemeOnly);
}


void Checks_Sentences::analyzeGrapheme ()
{
  currentPosition++;
  
  isSpace = (grapheme == " ");
  if (isSpace) {
    spacePosition = currentPosition;
  }
  
  isCapital = find (capitals.begin (), capitals.end (), grapheme) != capitals.end ();
  if (isCapital) {
    capitalPosition = currentPosition;
  }
  
  isSmallLetter = find (small_letters.begin(), small_letters.end(), grapheme) != small_letters.end ();
  if (isSmallLetter) {
    smallLetterPosition = currentPosition;
  }
  
  isEndMark = find (end_marks.begin (), end_marks.end (), grapheme) != end_marks.end ();
  if (isEndMark) {
    endMarkPosition = currentPosition;
    previousMarkPosition = punctuationMarkPosition;
    punctuationMarkPosition = currentPosition;
  }
  
  isCenterMark = find (center_marks.begin (), center_marks.end (), grapheme) != center_marks.end();
  if (isCenterMark) {
    centerMarkPosition = currentPosition;
    previousMarkPosition = punctuationMarkPosition;
    punctuationMarkPosition = currentPosition;
  }
}

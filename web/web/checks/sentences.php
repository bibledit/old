<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

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


class Checks_Sentences
{
  // Sentence structure parameters.
  private $capitals;
  private $small_letters;
  private $end_marks;
  private $center_marks;
  private $disregards;
  private $names;

  // State.
  private $verseNumber;
  private $currentPosition;
  private $withinSentence;

  // Grapheme analysis.
  private $grapheme;
  private $isSpace;
  private $spacePosition;
  private $isCapital;
  private $capitalPosition;
  private $isSmallLetter;
  private $smallLetterPosition;
  private $isEndMark;
  private $endMarkPosition;
  private $isCenterMark;
  private $centerMarkPosition;
  private $punctuationMarkPosition;
  private $previousMarkPosition;

  // Context.
  private $fullText;

  // Results of the checks.
  private $checkingResults;
  const displayGraphemeOnly = 1;
  const displayContext = 2;
  const skipNames = 3;


  public function enterCapitals ($capitals)
  {
    $this->capitals = explode (" ", $capitals);
  }

  public function enterSmallLetters ($small_letters)
  {
    $this->small_letters = explode (" ", $small_letters);
  }

  public function enterEndMarks ($end_marks)
  {
    $this->end_marks = explode (" ", $end_marks);
  }


  public function enterCenterMarks ($center_marks)
  {
    $this->center_marks = explode (" ", $center_marks);
  }


  public function enterDisregards ($disregards)
  {
    $this->disregards = explode (" ", $disregards);
  }


  public function enterNames ($names)
  {
    $this->names = array ();
    $names = str_replace ("\n", " ", $names);
    $names = explode (" ", $names);
    foreach ($names as $name) {
      if ($name != "") {
        // Limit the length to the left of the suffix in the test.
        $name = mb_substr ($name, 0, 11);
        $this->names [] = $name;
      }
    }
  }


  public function initialize ()
  {
    $this->currentPosition = 0;
    $this->spacePosition = 0;
    $this->capitalPosition = 0;
    $this->smallLetterPosition = 0;
    $this->endMarkPosition = 0;
    $this->centerMarkPosition = 0;
    $this->punctuationMarkPosition = 0;
    $this->previousMarkPosition = 0;
    $this->checkingResults = array ();
    $this->fullText = "";
  }


  public function check ($texts)
  {
    if (!is_array ($texts)) return;

    $verse_numbers = array ();
    $graphemes = array ();
    $iterations = 0;
    foreach ($texts as $verse => $text) {
      // For the second and subsequent verse_numbers, add a space to the text,
      // because this is what is supposed to happen in USFM.
      if ($iterations > 0) {
        $verse_numbers [] = $verse;
        $graphemes [] = " ";
        $this->fullText .= " ";
      }
      // Split the UTF-8 text into graphemes and add them to the arrays of verse_numbers/graphemes.
      $count = mb_strlen ($text);
      for ($i = 0; $i < $count; $i++) {
        $grapheme = mb_substr ($text, $i, 1);
        // Skip graphemes to be disregarded.
        if (in_array ($grapheme, $this->disregards)) continue;
        // Store verse numbers and graphemes.
        $verse_numbers [] = $verse;
        $graphemes [] = $grapheme;
        $this->fullText .= $grapheme;
      }
      // Next iteration.
      $iterations++;
    }

    // Go through the graphemes.
    $graphemeCount = count ($graphemes);
    for ($i = 0; $i < $graphemeCount; $i++) {
      // Store current verse number in the object.
      $this->verseNumber = $verse_numbers [$i];
      // Get the current grapheme.
      $this->grapheme = $graphemes [$i];
      // Analyze the grapheme.
      $this->analyzeGrapheme ();
      // Run the checks.
      $this->checkUnknownCharacter ();
      $this->checkGrapheme ();
    }

  }


  private function checkGrapheme ()
  {
    // Handle a capital after a comma: ... He said, Go ...
    if ($this->isCapital)
      if ($this->spacePosition > 0)
        if ($this->currentPosition == $this->spacePosition + 1)
          if ($this->currentPosition == $this->centerMarkPosition + 2)
            $this->addResult ("Capital follows mid-sentence punctuation mark", Checks_Sentences::skipNames);


    // Handle a small letter straight after mid-sentence punctuation: ... He said,go ...
    if ($this->isSmallLetter)
      if ($this->centerMarkPosition > 0)
        if ($this->currentPosition == $this->centerMarkPosition + 1)
          $this->addResult ("Small letter follows straight after a mid-sentence punctuation mark", Checks_Sentences::displayContext);


    // Handle a capital straight after mid-sentence punctuation: ... He said,Go ...
    if ($this->isCapital)
      if ($this->centerMarkPosition > 0)
        if ($this->currentPosition == $this->centerMarkPosition + 1)
          $this->addResult ("Capital follows straight after a mid-sentence punctuation mark", Checks_Sentences::displayContext);


    // Handle small letter or capital straight after end-sentence punctuation: He said.Go. // He said.go.
    if (($this->isSmallLetter) || ($this->isCapital))
      if ($this->endMarkPosition > 0)
        if ($this->currentPosition == $this->endMarkPosition + 1)
          $this->addResult ("A letter follows straight after an end-sentence punctuation mark", Checks_Sentences::displayContext);


    // Handle case of no capital after end-sentence punctuation: He did that. he went.
    if ($this->isSmallLetter)
      if ($this->endMarkPosition > 0)
        if ($this->currentPosition == $this->endMarkPosition + 2)
          $this->addResult ("No capital after an end-sentence punctuation mark", Checks_Sentences::displayContext);


    // Handle two punctuation marks in sequence.
    if ($this->isEndMark || $this->isCenterMark)
      if ($this->currentPosition == $this->previousMarkPosition + 1)
        $this->addResult ("Two punctuation marks in sequence", Checks_Sentences::displayContext);

  }


  public function paragraphs ($texts, $paragraphs)
  {
    if (!is_array ($texts)) return;
    if (!is_array ($paragraphs)) return;

    $verses = array ();
    $graphemes = array ();

    // Put the UTF-8 text into the arrays of verses and graphemes.
    foreach ($texts as $verse => $text) {
      $count = mb_strlen ($text);
      for ($i = 0; $i < $count; $i++) {
        $grapheme = mb_substr ($text, $i, 1);
        $verses [] = $verse;
        $graphemes [] = $grapheme;
      }
    }

    // Correct the positions where the paragraphs start.
    for ($i = 1; $i < count ($paragraphs); $i++) {
      $offset = $paragraphs [$i];
      $paragraphVerse = $verses [$offset];
      @$twoVersesBack = $verses [$offset - 2];
      if ($paragraphVerse != $twoVersesBack) {
        for ($i2 = $i; $i2 < count ($paragraphs); $i2++) {
          $paragraphs [$i2] = $paragraphs [$i2] - 1;
        }
      }
    }

    $paragraphCount = count ($paragraphs);

    // Go through the paragraphs to see whether they start with capitals.
    for ($i = 0; $i < $paragraphCount; $i++) {
      $offset = $paragraphs [$i];
      @$verse = $verses [$offset];
      @$grapheme = $graphemes [$offset];
      $isCapital = in_array ($grapheme, $this->capitals);
      if (!$isCapital) {
        $this->checkingResults [] = array ($verse => "Paragraph does not start with a capital" . ": " . $grapheme);
      }
    }

    // Go through the paragraphs to see whether they end with proper punctuation.
    for ($i = 0; $i < $paragraphCount; $i++) {
      if ($i < ($paragraphCount - 1)) {
        $offset = $paragraphs [$i + 1];
      } else {
        $offset = count ($graphemes);
      }
      $offset--;
      @$verse = $verses [$offset];
      @$grapheme = $graphemes [$offset];
      @$previousGrapheme = $graphemes [$offset - 1];
      $isEndMark = in_array ($grapheme, $this->end_marks) || in_array ($previousGrapheme, $this->end_marks);
      if (!$isEndMark) {
        $this->checkingResults [] = array ($verse => "Paragraph does not end with an end marker" . ": " . $grapheme);
      }
    }

  }


  public function getResults ()
  {
    return $this->checkingResults;
  }


  private function addResult ($text, $modifier)
  {
    // Get previous and next text fragment.
    $start = $this->currentPosition - 10;
    if ($start < 0) $start = 0;
    $previousFragment = mb_substr ($this->fullText, $start, $this->currentPosition - $start - 1);
    $nextFragment = mb_substr ($this->fullText, $this->currentPosition, 10);
    if ($nextFragment === false) $nextFragment = "";
    // Check whether the result can be skipped due to a name being involved.
    if ($modifier == 3) {
      $haystack = $this->grapheme . $nextFragment;
      foreach ($this->names as $name) {
        if (strpos ($haystack, $name) === 0) return;
      }
    }
    // Assemble text for checking result.
    if ($modifier == 1) {
      $text .= ": " . $this->grapheme;
    }
    if (($modifier == 2) || ($modifier == 3)) {
      $text .= ": " . $previousFragment . $this->grapheme . $nextFragment;
    }
    // Store checking result.
    $this->checkingResults [] = array ($this->verseNumber => $text);
  }


  private function checkUnknownCharacter ()
  {
    if ($this->isSpace) return;
    if ($this->isCapital) return;
    if ($this->isSmallLetter) return;
    if ($this->isEndMark) return;
    if ($this->isCenterMark) return;
    $this->addResult ("Unknown character", Checks_Sentences::displayGraphemeOnly);
  }


  private function analyzeGrapheme ()
  {
    $this->currentPosition++;

    $this->isSpace = ($this->grapheme == " ");
    if ($this->isSpace) {
      $this->spacePosition = $this->currentPosition;
    }

    $this->isCapital = in_array ($this->grapheme, $this->capitals);
    if ($this->isCapital) {
      $this->capitalPosition = $this->currentPosition;
    }

    $this->isSmallLetter = in_array ($this->grapheme, $this->small_letters);
    if ($this->isSmallLetter) {
      $this->smallLetterPosition = $this->currentPosition;
    }

    $this->isEndMark = in_array ($this->grapheme, $this->end_marks);
    if ($this->isEndMark) {
      $this->endMarkPosition = $this->currentPosition;
      $this->previousMarkPosition = $this->punctuationMarkPosition;
      $this->punctuationMarkPosition = $this->currentPosition;
    }

    $this->isCenterMark = in_array ($this->grapheme, $this->center_marks);
    if ($this->isCenterMark) {
      $this->centerMarkPosition = $this->currentPosition;
      $this->previousMarkPosition = $this->punctuationMarkPosition;
      $this->punctuationMarkPosition = $this->currentPosition;
    }
  }


}


?>

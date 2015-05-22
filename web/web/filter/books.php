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


class Filter_Books
{


  static public function passageDisplay ($book, $chapter, $verse)
  {
    $database_books = Database_Books::getInstance ();
    $book = Locale_Translate::_($database_books->getEnglishFromId ($book));
    return "$book $chapter:$verse";
  }


  // Returns the display string for the $passages as one line.
  // $passages is an array of array (book, chapter, verse).
  static public function passagesDisplayInline ($passages)
  {
    $display = "";
    foreach ($passages as $passage) {
      if ($display != "")
        $display .= " | ";
      $display .= Filter_Books::passageDisplay ($passage[0], $passage[1], $passage[2]);
    }
    return $display;
  }


  // Returns the display string for the $passages as several lines.
  // $passages is an array of array (book, chapter, verse).
  static public function passagesDisplayMultiline ($passages)
  {
    $display = "";
    foreach ($passages as $passage) {
      $display .= Filter_Books::passageDisplay ($passage[0], $passage[1], $passage[2]);
      $display .= "\n";
    }
    return $display;
  }

  
  // This function converts $passage to an integer, so that passages can be compared or stored.
  static public function passage2integer ($passage)
  {
    $integer = 1000000 * $passage [0] + 1000 * $passage [1] + $passage [2];
    return $integer;
  }

  
  // This converts the $integer, created above, to a passage.
  static public function integer2passage ($integer)
  {
    $book = intval ($integer / 1000000);
    $integer -= $book * 1000000;
    $chapter = intval ($integer / 1000);
    $integer -= $chapter * 1000;
    $verse = intval ($integer);
    return array ($book, $chapter, $verse);
  }


  // This filter takes $books as a string,
  // and looks whether it can be interpreted as a valid book in any way.
  // It returns a valid book identifier, or 0 in case no book could be interpreted.
  static public function interpretBook ($book)
  {
    $book = trim ($book);
    $identifier = 0;
    $database_books = Database_Books::getInstance();

    // Recognize names like "I Peter", where the "I" can also be "II" or "III".
    // Do the longest ones first.
    $book = str_replace ("III ", "3 ", $book);
    $book = str_replace ("II ", "2 ", $book);
    $book = str_replace ("I ", "1 ", $book);


    // Check on names entered like "Genesis" or "1 Corinthians", the full English name.
    // A bug was discovered so that "Judges" was interpreted as "Jude", because
    // of the three letters "Jud". Solved by checking on full English name first.
    $identifier = $database_books->getIdFromEnglish ($book);
    if ($identifier) {
      return $identifier;
    }

    // Recognise the USFM book abbreviations.
    $identifier = $database_books->getIdFromUsfm ($book);
    if ($identifier) {
      return $identifier;
    }

    // Try the OSIS abbreviations.
    $identifier = $database_books->getIdFromOsis ($book);
    if ($identifier) {
      return $identifier;
    }

    // Try the abbreviations of BibleWorks.
    $identifier = $database_books->getIdFromBibleworks ($book);
    if ($identifier) {
      return $identifier;
    }

    // Handle names from BibleWorks when copying the verse list to the clipboard.
    // These are not handled elsewhere.
    if ($book == "Cant") return 22;
    if ($book == "Mk") return 41;
    if ($book == "Lk") return 42;
    if ($book == "Jn") return 43;
    if ($book == "1 Jn") return 62;
    if ($book == "2 Jn") return 63;
    if ($book == "3 Jn") return 64;

    // Try the abbreviations of the Online Bible.
    $identifier = $database_books->getIdFromOnlinebible ($book);
    if ($identifier) {
      return $identifier;
    }

    // Do a case-insensitive search in the books database for something like the book given.
    $identifier = $database_books->getIdLikeText ($book);
    if ($identifier) {
      return $identifier;
    }

    /*
    Here's some code from Bibledit-Gtk that could help to further aid in interpreting book names.

    // Not found yet, check on names like "1Corinthians".
    if (rawbook.length() >= 1) {
      ustring s = rawbook.substr(0, 1);
      ustring s2 = rawbook;
      if (s == "1" || s == "2" || s == "3")
        s2.insert(1, " ");
      identifier = books_english_to_id(s2);
      if (identifier) {
        return identifier;
      }
    }
    // Not yet found.
    // Go through the language of the project, and see if the book is among the
    // booknames or abbreviations.
    {
      extern Settings *settings;
      ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
      identifier = books_name_to_id(projectconfig->language_get(), rawbook);
      if (identifier)
        return identifier;
      identifier = books_abbreviation_to_id(projectconfig->language_get(), rawbook);
      if (identifier)
        return identifier;
      identifier = books_abbreviation_to_id_loose(projectconfig->language_get(), rawbook);
      if (identifier)
        return identifier;
    }
    // Still not found.
    // Go through all available languages, and see if the book is among the
    // names or abbreviations of the book.
    {
      extern BookLocalizations *booklocalizations;
      vector < ustring > languages = booklocalizations->localizations_get();
      for (unsigned int i = 0; i < languages.size(); i++) {
        identifier = books_name_to_id(languages[i], rawbook);
        if (identifier)
          return identifier;
        identifier = books_abbreviation_to_id(languages[i], rawbook);
        if (identifier)
          return identifier;
        identifier = books_abbreviation_to_id_loose(languages[i], rawbook);
        if (identifier)
          return identifier;
      }
    }

    For language searches, one would need to load all book translations through a series of Locale_Translate::_calls,
    then sort these out.

    The soundex function of PHP could aid too.

    */

    return 0;
  }


  static private function cleanPassage ($text)
  {
    // Trim text.
    $text = trim ($text);
    // As references could be, e.g.: Genesis 10.2, or Genesis 10:2,
    // it needs to convert a the full stop and the colon to a space.
    $text = str_replace (".", " ", $text);
    $text = str_replace (":", " ", $text);
    // Change double spaces into single ones.
    $text = str_replace ("  ", " ", $text);
    $text = str_replace ("  ", " ", $text);
    // Trim again.
    $text = trim ($text);
    // Result.
    return $text;
  }


  // Takes the passage in $text, and explodes it into book, chapter, verse.
  // The book is the numerical identifier, not the string, e.g.,
  // it would not return "Genesis", but identifier 1.
  static public function explodePassage ($text)
  {
    $text = Filter_Books::cleanPassage ($text);
    // Cut the text in its parts.
    $text = explode (" ", $text);
    // Defaults to empty passage.
    $passage = array (0, 0, 0);
    // Take the bits.
    $verse = array_pop ($text);
    if (isset ($verse)) $passage[2] = $verse;
    $chapter = array_pop ($text);
    if (isset ($chapter)) $passage[1] = $chapter;
    $book = implode (" ", $text);
    if ($book != "") {
      $book =  Filter_Books::interpretBook ($book);
      $passage[0] = $book;
    }
    // Return the result.
    return $passage;
  }


  // Takes the passage in $rawPassage, and tries to interpret it.
  // The following situations can occur:
  // - Only book given, e.g. "Genesis".
  // - One number given, e.g. "10".
  // - Two numbers given, e.g. "1 2".
  // - Book and one number given, e.g. "Exodus 10".
  // - Book and two numbers given, e.g. "Song of Solomon 2 3".
  // It deals with these situations.
  // If needed, it bases the interpreted passage on $currentPassage,
  // which is an array of book, chapter, verse.
  static public function interpretPassage ($currentPassage, $rawPassage)
  {
    $rawPassage = Filter_Books::cleanPassage ($rawPassage);

    // Create an array with the bits of the raw input.
    $input = explode (" ", $rawPassage);

    // Go through the array from verse to chapter to book.
    // Check how many numerals it has after the book part.
    $numerals = array ();
    $book = "";
    $invertedInput = array_reverse ($input);
    foreach ($invertedInput as $bit) {
      if (Filter_Numeric::integer_in_string ($bit) != "") {
        $numerals [] = $bit;
        array_pop ($input);
      } else {
        $book = implode (" ", $input);
        break;
      }
    }

    // Deal with: Only book given, e.g. "Genesis".
    if (($book != "") && (count ($numerals) == 0)) {
      return Filter_Books::explodePassage ("$book 1 1");
    }

    // Deal with: One number given, e.g. "10".
    else if (($book == "") && (count ($numerals) == 1)) {
      $book = $currentPassage [0];
      $chapter = $currentPassage [1];
      $verse = $numerals [0];
      $passage = Filter_Books::explodePassage ("Unknown $chapter $verse");
      $passage [0] = $book;
      return $passage;
    }

    // Deal with: Two numbers given, e.g. "1 2".
    else if (($book == "") && (count ($numerals) == 2)) {
      $book = $currentPassage [0];
      $chapter = $numerals [1];
      $verse = $numerals [0];
      $passage = Filter_Books::explodePassage ("Unknown $chapter $verse");
      $passage [0] = $book;
      return $passage;
    }

    // Deal with: Book and one number given, e.g. "Exodus 10".
    else if (($book != "") && (count ($numerals) == 1)) {
      $chapter = $numerals [0];
      return Filter_Books::explodePassage ("$book $chapter 1");
    }

    // Deal with: Book and two numbers given, e.g. "Song of Solomon 2 3".
    else if (($book != "") && (count ($numerals) == 2)) {
      return Filter_Books::explodePassage ($rawPassage);
    }

    // Give up.
    return $currentPassage;
  }


  // This deals with sequences and ranges of verses, like the following:
  // Exod. 37:4-5, 14-15, 27-28
  // It puts each verse on a separate line.
  static public function handleSequencesRanges ($passage)
  {
    // A passage like Exod. 37:4-5, 14-15, 27-28 will be cut at the comma.
    // The resulting array contains the following:
    // Exod. 37:4-5
    // 14-15
    // 27-28
    // It implies that the first sequence has book and chapter.
    $sequences = explode (",", $passage);
    foreach ($sequences as &$line) $line = trim ($line);
    unset ($line);

    // Store output lines.
    $output = array ();

    // Cut the passages at the hyphen.
    foreach ($sequences as $offset => $sequence) {
      $range = explode ("-", $sequence);
      if (count ($range) == 1) {
        $output [] = trim ($range [0]);
      } else {
        $start = trim ($range [0]);
        $output [] = $start;
        if ($offset == 0) {
          // Since the first bit contains book / chapter / verse,
          // extract the verse number.
          $start = strrev ($start);
          $start = Filter_Numeric::integer_in_string ($start);
          $start = strrev ($start);
        }
        $end = trim ($range [1]);
        for ($i = $start + 1; $i <= $end; $i++) {
          $output [] = $i;
        }
      }
    }

    // Result.
    return $output;
  }


  public static function linkForOpeningEditorAt ($book, $chapter, $verse)
  {
    $passage = self::passageDisplay ($book, $chapter, $verse);
    $link = "<a href=\"../edit/edit.php?switchbook=$book&switchchapter=$chapter&switchverse=$verse\" target=\"_blank\">$passage</a>";
    return $link;
  }


  // A Bible has a standard order for the books, and it can have their books in a custom order.
  // This function returns either the standard order, or a custom order in case it is available for the $bible.
  public static function getOrdered ($bible)
  {
    $database_bibles = Database_Bibles::getInstance ();
    $database_config_bible = Database_Config_Bible::getInstance ();

    // The available books from the Bible.
    $projectbooks = $database_bibles->getBooks ($bible);

    // The book order from the settings, if any.
    $orderedbooks = $database_config_bible->getBookOrder ($bible);
    $orderedbooks = explode (" ", $orderedbooks);

    // Remove books not available in the Bible.
    $orderedbooks = array_intersect ($orderedbooks, $projectbooks);

    // Books in the Bible but not in the settings: Add them to the end.
    $orderedbooks = array_merge ($orderedbooks, $projectbooks);
    $orderedbooks = array_unique ($orderedbooks);

    $orderedbooks = array_values ($orderedbooks);

    return $orderedbooks;
  }
  

}

?>

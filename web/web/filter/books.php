<?php
/**
* @package bibledit
*/
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

class Filter_Books
{

  static public function passageDisplay ($book, $chapter, $verse)
  {
    $database_books = Database_Books::getInstance ();
    $book = gettext ($database_books->getEnglishFromId ($book));
    return "$book $chapter:$verse";
  }


  /**
  * Returns the display string for the $passages as one line.
  * $passages is an array of array (book, chapter, verse).
  */
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

  /**
  * Returns the display string for the $passages as several lines.
  * $passages is an array of array (book, chapter, verse).
  */
  static public function passagesDisplayMultiline ($passages)
  {
    $display = "";
    foreach ($passages as $passage) {
      $display .= Filter_Books::passageDisplay ($passage[0], $passage[1], $passage[2]);
      $display .= "\n";
    }
    return $display;
  }
  
  /**
  * This filter takes $books as a string, 
  * and looks whether it can be interpreted as a valid book in any way.
  * It returns a valid book identifier, or 0 in case no book could be interpreted.
  */
  static public function interpretBook ($book)
  {
    $book = trim ($book);
    $identifier = 0;
    $database_books = Database_Books::getInstance();

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
    
    What could be done too is to use the full-text search options of the database.
    library/computing/lamp/mysql-docs/refman-5.1-maria-en.html-chapter/functions.html#fulltext-search
    Tutorial at http://devzone.zend.com/article/1304
    
    For language searches, one would need to load all book translations through a series of gettext calls,
    then sort these out.
    
    The soundex function of PHP could aid too.

    */

    return 0;
  }


  /**
  * Takes the passage in $text, and explodes it into book, chapter, verse.
  * The book is the numerical identifier, not the string, e.g.,
  * it would not return "Genesis", but identifier 1.
  */
  static public function explodePassage ($text) 
  {
    // Trim text.
    $text = trim ($text);
    // As references could be, e.g.: Genesis 10.2, or Genesis 10:2,
    // it needs to make spaces of the full stop and the colon.
    $text = str_replace (".", " ", $text);
    $text = str_replace (":", " ", $text);
    // Change double spaces into single ones.
    $text = str_replace ("  ", " ", $text);
    $text = str_replace ("  ", " ", $text);
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
 


}

?>

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


// This is a database for the various book names.

// It is safe to store this in a SQLite database, 
// because normally only read operations occur, 
// so there is a very low risk of database corruption.


class Database_Books
{
  private static $instance;
  private $db;
  private function __construct() {
    $this->db = Database_SQLite::connect ("books");
  }
  public static function getInstance()
  {
    if (empty (self::$instance)) {
      self::$instance = new Database_Books();
    }
    return self::$instance;
  }


  public function create ()
  {
    Database_SQLite::exec ($this->db, "DROP TABLE IF EXISTS books;");

$sql = <<<'EOD'
CREATE TABLE IF NOT EXISTS books (
  id integer,
  english text,
  osis text,
  usfm text,
  bibleworks text,
  onlinebible text,
  type text,
  onechapter integer
);
EOD;
    Database_SQLite::exec ($this->db, $sql);

    /*
    The following contains book names as follows:
    ID | English name | OSIS | USFM | BibleWorks | Online Bible | type | chapter
    Clarifications:
    ID: Bibledit internal book ID.
    English name:
    OSIS: The OSIS abbreviation.
    USFM: The USFM ID.
    BibleWorks: The BibleWorks abbreviation.
    Online Bible: Online Bible abbreviation.
    type: One of the following
          frontback - Frontmatter / Backmatter
          ot        - Old Testament
          nt        - New Testament
          other     - Other matter
          ap        - Apocrypha
    One chapter: If true, this book has only one chapter.
    The order in which the books are presented is the standard order.
    */

    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (1, 'Genesis', 'Gen', 'GEN', 'Gen', 'Ge', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (2, 'Exodus', 'Exod', 'EXO', 'Exo', 'Ex', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (3, 'Leviticus', 'Lev', 'LEV', 'Lev', 'Le', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (4, 'Numbers', 'Num', 'NUM', 'Num', 'Nu', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (5, 'Deuteronomy', 'Deut', 'DEU', 'Deu', 'De', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (6, 'Joshua', 'Josh', 'JOS', 'Jos', 'Jos', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (7, 'Judges', 'Judg', 'JDG', 'Jdg', 'Jud', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (8, 'Ruth', 'Ruth', 'RUT', 'Rut', 'Ru', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (9, '1 Samuel', '1Sam', '1SA', '1Sa', '1Sa', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (10, '2 Samuel', '2Sam', '2SA', '2Sa', '2Sa', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (11, '1 Kings', '1Kgs', '1KI', '1Ki', '1Ki', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (12, '2 Kings', '2Kgs', '2KI', '2Ki', '2Ki', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (13, '1 Chronicles', '1Chr', '1CH', '1Ch', '1Ch', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (14, '2 Chronicles', '2Chr', '2CH', '2Ch', '2Ch', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (15, 'Ezra', 'Ezra', 'EZR', 'Ezr', 'Ezr', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (16, 'Nehemiah', 'Neh', 'NEH', 'Neh', 'Ne', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (17, 'Esther', 'Esth', 'EST', 'Est', 'Es', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (18, 'Job', 'Job', 'JOB', 'Job', 'Job', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (19, 'Psalms', 'Ps', 'PSA', 'Psa', 'Ps', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (20, 'Proverbs', 'Prov', 'PRO', 'Pro', 'Pr', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (21, 'Ecclesiastes', 'Eccl', 'ECC', 'Ecc', 'Ec', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (22, 'Song of Solomon', 'Song', 'SNG', 'Sol', 'So', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (23, 'Isaiah', 'Isa', 'ISA', 'Isa', 'Isa', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (24, 'Jeremiah', 'Jer', 'JER', 'Jer', 'Jer', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (25, 'Lamentations', 'Lam', 'LAM', 'Lam', 'La', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (26, 'Ezekiel', 'Ezek', 'EZK', 'Eze', 'Eze', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (27, 'Daniel', 'Dan', 'DAN', 'Dan', 'Da', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (28, 'Hosea', 'Hos', 'HOS', 'Hos', 'Ho', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (29, 'Joel', 'Joel', 'JOL', 'Joe', 'Joe', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (30, 'Amos', 'Amos', 'AMO', 'Amo', 'Am', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (31, 'Obadiah', 'Obad', 'OBA', 'Oba', 'Ob', 'ot', 1);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (32, 'Jonah', 'Jonah', 'JON', 'Jon', 'Jon', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (33, 'Micah', 'Mic', 'MIC', 'Mic', 'Mic', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (34, 'Nahum', 'Nah', 'NAM', 'Nah', 'Na', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (35, 'Habakkuk', 'Hab', 'HAB', 'Hab', 'Hab', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (36, 'Zephaniah', 'Zeph', 'ZEP', 'Zep', 'Zep', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (37, 'Haggai', 'Hag', 'HAG', 'Hag', 'Hag', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (38, 'Zechariah', 'Zech', 'ZEC', 'Zec', 'Zec', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (39, 'Malachi', 'Mal', 'MAL', 'Mal', 'Mal', 'ot', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (40, 'Matthew', 'Matt', 'MAT', 'Mat', 'Mt', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (41, 'Mark', 'Mark', 'MRK', 'Mar', 'Mr', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (42, 'Luke', 'Luke', 'LUK', 'Luk', 'Lu', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (43, 'John', 'John', 'JHN', 'Joh', 'Joh', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (44, 'Acts', 'Acts', 'ACT', 'Act', 'Ac', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (45, 'Romans', 'Rom', 'ROM', 'Rom', 'Ro', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (46, '1 Corinthians', '1Cor', '1CO', '1Co', '1Co', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (47, '2 Corinthians', '2Cor', '2CO', '2Co', '2Co', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (48, 'Galatians', 'Gal', 'GAL', 'Gal', 'Ga', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (49, 'Ephesians', 'Eph', 'EPH', 'Eph', 'Eph', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (50, 'Philippians', 'Phil', 'PHP', 'Phi', 'Php', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (51, 'Colossians', 'Col', 'COL', 'Col', 'Col', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (52, '1 Thessalonians', '1Thess', '1TH', '1Th', '1Th', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (53, '2 Thessalonians', '2Thess', '2TH', '2Th', '2Th', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (54, '1 Timothy', '1Tim', '1TI', '1Ti', '1Ti', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (55, '2 Timothy', '2Tim', '2TI', '2Ti', '2Ti', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (56, 'Titus', 'Titus', 'TIT', 'Tit', 'Tit', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (57, 'Philemon', 'Phlm', 'PHM', 'Phm', 'Phm', 'nt', 1);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (58, 'Hebrews', 'Heb', 'HEB', 'Heb', 'Heb', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (59, 'James', 'Jas', 'JAS', 'Jam', 'Jas', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (60, '1 Peter', '1Pet', '1PE', '1Pe', '1Pe', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (61, '2 Peter', '2Pet', '2PE', '2Pe', '2Pe', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (62, '1 John', '1John', '1JN', '1Jo', '1Jo', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (63, '2 John', '2John', '2JN', '2Jo', '2Jo', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (64, '3 John', '3John', '3JN', '3Jo', '3Jo', 'nt', 1);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (65, 'Jude', 'Jude', 'JUD', 'Jud', 'Jude', 'nt', 1);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (66, 'Revelation', 'Rev', 'REV', 'Rev', 'Re', 'nt', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (67, 'Front Matter', '', 'FRT', '', '', 'frontback', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (68, 'Back Matter', '', 'BAK', '', '', 'frontback', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (69, 'Other Material', '', 'OTH', '', '', 'other', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (70, 'Tobit', 'Tob', 'TOB', 'Tob', '', 'ap', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (71, 'Judith', 'Jdt', 'JDT', 'Jdt', '', 'ap', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (72, 'Esther (Greek)', 'AddEsth', 'ESG', 'EsG', '', 'ap', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (73, 'Wisdom of Solomon', 'Wis', 'WIS', 'Wis', '', 'ap', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (74, 'Sirach', 'Sir', 'SIR', 'Sir', '', 'ap', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (75, 'Baruch', 'Bar', 'BAR', 'Bar', '', 'ap', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (76, 'Letter of Jeremiah', 'EpJer', 'LJE', 'LJe', '', 'ap', 1);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (77, 'Song of the Three Children', 'PrAzar', 'S3Y', 'S3Y', '', 'ap', 1);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (78, 'Susanna', 'Sus', 'SUS', 'Sus', '', 'ap', 1);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (79, 'Bel and the Dragon', 'Bel', 'BEL', 'Bel', '', 'ap', 1);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (80, '1 Maccabees', '1Macc', '1MA', '1Ma', '', 'ap', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (81, '2 Maccabees', '2Macc', '2MA', '2Ma', '', 'ap', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (82, '1 Esdras', '1Esd', '1ES', '1Es', '', 'ap', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (83, 'Prayer of Manasses', 'PrMan', 'MAN', 'Man', '', 'ap', 1);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (84, 'Psalm 151', 'Ps151', 'PS2', 'Ps2', '', 'ap', 1);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (85, '3 Maccabees', '3Macc', '3MA', '3Ma', '', 'ap', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (86, '2 Esdras', '2Esd', '2ES', '2Es', '', 'ap', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (87, '4 Maccabees', '4Macc', '4MA', '4Ma', '', 'ap', 0);");
    Database_SQLite::exec ($this->db, "INSERT INTO books (id, english, osis, usfm, bibleworks, onlinebible, type, onechapter) VALUES (88, 'Daniel (Greek)', 'AddDan', 'DNT', 'Dnt', '', 'ap', 0);");
  }


  public function optimize () {
    Database_SQLite::exec ($this->db, "REINDEX books;");
    Database_SQLite::exec ($this->db, "VACUUM books;");
  }


  public function getIDs ()
  {
    $ids = array ();
    // Excludes Apocrypha.
    $query = "SELECT id FROM books WHERE id < 70 ORDER BY rowid;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      $ids[] = $row[0];
    }
    return $ids;
  }


  public function getIdFromEnglish ($english)
  {
    $english = Database_SQLiteInjection::no ($english);
    $query = "SELECT id FROM books WHERE english = '$english';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }
    return 0;
  }


  public function getEnglishFromId ($id)
  {
    // Care for empty input.
    if ($id == "") return "Unknown";
    // Get the English name of the book.
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT english FROM books WHERE id = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }
    return Locale_Translate::_("Unknown");
  }


  public function getUsfmFromId ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT usfm FROM books WHERE id = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }
    return "XXX";
  }


  public function getBibleworksFromId ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT bibleworks FROM books WHERE id = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }
    return "Xxx";
  }


  public function getIdFromUsfm ($usfm)
  {
    $usfm = Database_SQLiteInjection::no ($usfm);
    $query = "SELECT id FROM books WHERE usfm = '$usfm';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }
    return 0;
  }


  public function getIdFromOsis ($osis)
  {
    $osis = Database_SQLiteInjection::no ($osis);
    $query = "SELECT id FROM books WHERE osis = '$osis';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }
    return 0;
  }


  public function getIdFromBibleworks ($bibleworks)
  {
    $bibleworks = Database_SQLiteInjection::no ($bibleworks);
    $query = "SELECT id FROM books WHERE bibleworks = '$bibleworks';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }
    return 0;
  }


  /**
  * Tries to interprete $text as the name of a Bible book.
  * Returns the book's identifier if it succeeds.
  * If it fails, it returns 0.
  */
  public function getIdLikeText ($text)
  {
    $text = Database_SQLiteInjection::no ($text);
    $query = "SELECT id FROM books WHERE english LIKE '$text%' OR osis LIKE '$text%' OR usfm LIKE '$text%' or bibleworks LIKE '$text%';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }
    return 0;
  }


  public function getIdFromOnlinebible ($onlinebible)
  {
    $onlinebible = Database_SQLiteInjection::no ($onlinebible);
    $query = "SELECT id FROM books WHERE onlinebible = '$onlinebible';";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }
    return 0;
  }


  public function getOnlinebibleFromId ($id)
  {
    if ($id == "") return "";
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT onlinebible FROM books WHERE id = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }
    return "";
  }


  public function getSequenceFromId ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT rowid FROM books WHERE id = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }
    return 0;
  }


  public function getType ($id)
  {
    $id = Database_SQLiteInjection::no ($id);
    $query = "SELECT type FROM books WHERE id = $id;";
    $result = Database_SQLite::query ($this->db, $query);
    foreach ($result as $row) {
      return $row[0];
    }
    return "";
  }


}



?>

USE `BIBLEDITDATABASE`;

DROP TABLE IF EXISTS books;

CREATE TABLE IF NOT EXISTS books (
  sequence int auto_increment primary key,
  id int,
  english varchar(256),
  osis varchar(10),
  usfm varchar(3),
  bibleworks varchar(10),
  onlinebible varchar(10),
  type varchar(20),
  onechapter int
) engine = MyISAM;

# The following contains book names as follows:
# ID | English name | OSIS | USFM | BibleWorks | Online Bible | type | chapter
# Clarifications:
# ID: Bibledit internal book ID.
# English name: 
# OSIS: The OSIS abbreviation.
# USFM: The USFM ID.
# BibleWorks: The BibleWorks abbreviation.
# Online Bible: Online Bible abbreviation.
# type: One of the following
#       frontback - Frontmatter / Backmatter
#       ot        - Old Testament
#       nt        - New Testament
#       other     - Other matter
#       ap        - Apocrypha
# One chapter: If true, this book has only one chapter.              
# The order in which the books are presented is the standard order.

INSERT INTO books VALUES (NULL, 1, 'Genesis', 'Gen', 'GEN', 'Gen', 'Ge', 'ot', 0);
INSERT INTO books VALUES (NULL, 2, 'Exodus', 'Exod', 'EXO', 'Exo', 'Ex', 'ot', 0);
INSERT INTO books VALUES (NULL, 3, 'Leviticus', 'Lev', 'LEV', 'Lev', 'Le', 'ot', 0);
INSERT INTO books VALUES (NULL, 4, 'Numbers', 'Num', 'NUM', 'Num', 'Nu', 'ot', 0);
INSERT INTO books VALUES (NULL, 5, 'Deuteronomy', 'Deut', 'DEU', 'Deu', 'De', 'ot', 0);
INSERT INTO books VALUES (NULL, 6, 'Joshua', 'Josh', 'JOS', 'Jos', 'Jos', 'ot', 0);
INSERT INTO books VALUES (NULL, 7, 'Judges', 'Judg', 'JDG', 'Jdg', 'Jud', 'ot', 0);
INSERT INTO books VALUES (NULL, 8, 'Ruth', 'Ruth', 'RUT', 'Rut', 'Ru', 'ot', 0);
INSERT INTO books VALUES (NULL, 9, '1 Samuel', '1Sam', '1SA', '1Sa', '1Sa', 'ot', 0);
INSERT INTO books VALUES (NULL, 10, '2 Samuel', '2Sam', '2SA', '2Sa', '2Sa', 'ot', 0);
INSERT INTO books VALUES (NULL, 11, '1 Kings', '1Kgs', '1KI', '1Ki', '1Ki', 'ot', 0);
INSERT INTO books VALUES (NULL, 12, '2 Kings', '2Kgs', '2KI', '2Ki', '2Ki', 'ot', 0);
INSERT INTO books VALUES (NULL, 13, '1 Chronicles', '1Chr', '1CH', '1Ch', '1Ch', 'ot', 0);
INSERT INTO books VALUES (NULL, 14, '2 Chronicles', '2Chr', '2CH', '2Ch', '2Ch', 'ot', 0);
INSERT INTO books VALUES (NULL, 15, 'Ezra', 'Ezra', 'EZR', 'Ezr', 'Ezr', 'ot', 0);
INSERT INTO books VALUES (NULL, 16, 'Nehemiah', 'Neh', 'NEH', 'Neh', 'Ne', 'ot', 0);
INSERT INTO books VALUES (NULL, 17, 'Esther', 'Esth', 'EST', 'Est', 'Es', 'ot', 0);
INSERT INTO books VALUES (NULL, 18, 'Job', 'Job', 'JOB', 'Job', 'Job', 'ot', 0);
INSERT INTO books VALUES (NULL, 19, 'Psalms', 'Ps', 'PSA', 'Psa', 'Ps', 'ot', 0);
INSERT INTO books VALUES (NULL, 20, 'Proverbs', 'Prov', 'PRO', 'Pro', 'Pr', 'ot', 0);
INSERT INTO books VALUES (NULL, 21, 'Ecclesiastes', 'Eccl', 'ECC', 'Ecc', 'Ec', 'ot', 0);
INSERT INTO books VALUES (NULL, 22, 'Song of Solomon', 'Song', 'SNG', 'Sol', 'So', 'ot', 0);
INSERT INTO books VALUES (NULL, 23, 'Isaiah', 'Isa', 'ISA', 'Isa', 'Isa', 'ot', 0);
INSERT INTO books VALUES (NULL, 24, 'Jeremiah', 'Jer', 'JER', 'Jer', 'Jer', 'ot', 0);
INSERT INTO books VALUES (NULL, 25, 'Lamentations', 'Lam', 'LAM', 'Lam', 'La', 'ot', 0);
INSERT INTO books VALUES (NULL, 26, 'Ezekiel', 'Ezek', 'EZK', 'Eze', 'Eze', 'ot', 0);
INSERT INTO books VALUES (NULL, 27, 'Daniel', 'Dan', 'DAN', 'Dan', 'Da', 'ot', 0);
INSERT INTO books VALUES (NULL, 28, 'Hosea', 'Hos', 'HOS', 'Hos', 'Ho', 'ot', 0);
INSERT INTO books VALUES (NULL, 29, 'Joel', 'Joel', 'JOL', 'Joe', 'Joe', 'ot', 0);
INSERT INTO books VALUES (NULL, 30, 'Amos', 'Amos', 'AMO', 'Amo', 'Am', 'ot', 0);
INSERT INTO books VALUES (NULL, 31, 'Obadiah', 'Obad', 'OBA', 'Oba', 'Ob', 'ot', 1);
INSERT INTO books VALUES (NULL, 32, 'Jonah', 'Jonah', 'JON', 'Jon', 'Jon', 'ot', 0);
INSERT INTO books VALUES (NULL, 33, 'Micah', 'Mic', 'MIC', 'Mic', 'Mic', 'ot', 0);
INSERT INTO books VALUES (NULL, 34, 'Nahum', 'Nah', 'NAM', 'Nah', 'Na', 'ot', 0);
INSERT INTO books VALUES (NULL, 35, 'Habakkuk', 'Hab', 'HAB', 'Hab', 'Hab', 'ot', 0);
INSERT INTO books VALUES (NULL, 36, 'Zephaniah', 'Zeph', 'ZEP', 'Zep', 'Zep', 'ot', 0);
INSERT INTO books VALUES (NULL, 37, 'Haggai', 'Hag', 'HAG', 'Hag', 'Hag', 'ot', 0);
INSERT INTO books VALUES (NULL, 38, 'Zechariah', 'Zech', 'ZEC', 'Zec', 'Zec', 'ot', 0);
INSERT INTO books VALUES (NULL, 39, 'Malachi', 'Mal', 'MAL', 'Mal', 'Mal', 'ot', 0);
INSERT INTO books VALUES (NULL, 40, 'Matthew', 'Matt', 'MAT', 'Mat', 'Mt', 'nt', 0);
INSERT INTO books VALUES (NULL, 41, 'Mark', 'Mark', 'MRK', 'Mar', 'Mr', 'nt', 0);
INSERT INTO books VALUES (NULL, 42, 'Luke', 'Luke', 'LUK', 'Luk', 'Lu', 'nt', 0);
INSERT INTO books VALUES (NULL, 43, 'John', 'John', 'JHN', 'Joh', 'Joh', 'nt', 0);
INSERT INTO books VALUES (NULL, 44, 'Acts', 'Acts', 'ACT', 'Act', 'Ac', 'nt', 0);
INSERT INTO books VALUES (NULL, 45, 'Romans', 'Rom', 'ROM', 'Rom', 'Ro', 'nt', 0);
INSERT INTO books VALUES (NULL, 46, '1 Corinthians', '1Cor', '1CO', '1Co', '1Co', 'nt', 0);
INSERT INTO books VALUES (NULL, 47, '2 Corinthians', '2Cor', '2CO', '2Co', '2Co', 'nt', 0);
INSERT INTO books VALUES (NULL, 48, 'Galatians', 'Gal', 'GAL', 'Gal', 'Ga', 'nt', 0);
INSERT INTO books VALUES (NULL, 49, 'Ephesians', 'Eph', 'EPH', 'Eph', 'Eph', 'nt', 0);
INSERT INTO books VALUES (NULL, 50, 'Philippians', 'Phil', 'PHP', 'Phi', 'Php', 'nt', 0);
INSERT INTO books VALUES (NULL, 51, 'Colossians', 'Col', 'COL', 'Col', 'Col', 'nt', 0);
INSERT INTO books VALUES (NULL, 52, '1 Thessalonians', '1Thess', '1TH', '1Th', '1Th', 'nt', 0);
INSERT INTO books VALUES (NULL, 53, '2 Thessalonians', '2Thess', '2TH', '2Th', '2Th', 'nt', 0);
INSERT INTO books VALUES (NULL, 54, '1 Timothy', '1Tim', '1TI', '1Ti', '1Ti', 'nt', 0);
INSERT INTO books VALUES (NULL, 55, '2 Timothy', '2Tim', '2TI', '2Ti', '2Ti', 'nt', 0);
INSERT INTO books VALUES (NULL, 56, 'Titus', 'Titus', 'TIT', 'Tit', 'Tit', 'nt', 0);
INSERT INTO books VALUES (NULL, 57, 'Philemon', 'Phlm', 'PHM', 'Phm', 'Phm', 'nt', 1);
INSERT INTO books VALUES (NULL, 58, 'Hebrews', 'Heb', 'HEB', 'Heb', 'Heb', 'nt', 0);
INSERT INTO books VALUES (NULL, 59, 'James', 'Jas', 'JAS', 'Jam', 'Jas', 'nt', 0);
INSERT INTO books VALUES (NULL, 60, '1 Peter', '1Pet', '1PE', '1Pe', '1Pe', 'nt', 0);
INSERT INTO books VALUES (NULL, 61, '2 Peter', '2Pet', '2PE', '2Pe', '2Pe', 'nt', 0);
INSERT INTO books VALUES (NULL, 62, '1 John', '1John', '1JN', '1Jo', '1Jo', 'nt', 0);
INSERT INTO books VALUES (NULL, 63, '2 John', '2John', '2JN', '2Jo', '2Jo', 'nt', 0);
INSERT INTO books VALUES (NULL, 64, '3 John', '3John', '3JN', '3Jo', '3Jo', 'nt', 1);
INSERT INTO books VALUES (NULL, 65, 'Jude', 'Jude', 'JUD', 'Jud', 'Jude', 'nt', 1);
INSERT INTO books VALUES (NULL, 66, 'Revelation', 'Rev', 'REV', 'Rev', 'Re', 'nt', 0);
INSERT INTO books VALUES (NULL, 67, 'Front Matter', '', 'FRT', '', '', 'frontback', 0);
INSERT INTO books VALUES (NULL, 68, 'Back Matter', '', 'BAK', '', '', 'frontback', 0);
INSERT INTO books VALUES (NULL, 69, 'Other Material', '', 'OTH', '', '', 'other', 0);
INSERT INTO books VALUES (NULL, 70, 'Tobit', 'Tob', 'TOB', 'Tob', '', 'ap', 0);
INSERT INTO books VALUES (NULL, 71, 'Judith', 'Jdt', 'JDT', 'Jdt', '', 'ap', 0);
INSERT INTO books VALUES (NULL, 72, 'Esther (Greek)', 'AddEsth', 'ESG', 'EsG', '', 'ap', 0);
INSERT INTO books VALUES (NULL, 73, 'Wisdom of Solomon', 'Wis', 'WIS', 'Wis', '', 'ap', 0);
INSERT INTO books VALUES (NULL, 74, 'Sirach', 'Sir', 'SIR', 'Sir', '', 'ap', 0);
INSERT INTO books VALUES (NULL, 75, 'Baruch', 'Bar', 'BAR', 'Bar', '', 'ap', 0);
INSERT INTO books VALUES (NULL, 76, 'Letter of Jeremiah', 'EpJer', 'LJE', 'LJe', '', 'ap', 1);
INSERT INTO books VALUES (NULL, 77, 'Song of the Three Children', 'PrAzar', 'S3Y', 'S3Y', '', 'ap', 1);
INSERT INTO books VALUES (NULL, 78, 'Susanna', 'Sus', 'SUS', 'Sus', '', 'ap', 1);
INSERT INTO books VALUES (NULL, 79, 'Bel and the Dragon', 'Bel', 'BEL', 'Bel', '', 'ap', 1);
INSERT INTO books VALUES (NULL, 80, '1 Maccabees', '1Macc', '1MA', '1Ma', '', 'ap', 0);
INSERT INTO books VALUES (NULL, 81, '2 Maccabees', '2Macc', '2MA', '2Ma', '', 'ap', 0);
INSERT INTO books VALUES (NULL, 82, '1 Esdras', '1Esd', '1ES', '1Es', '', 'ap', 0);
INSERT INTO books VALUES (NULL, 83, 'Prayer of Manasses', 'PrMan', 'MAN', 'Man', '', 'ap', 1);
INSERT INTO books VALUES (NULL, 84, 'Psalm 151', 'Ps151', 'PS2', 'Ps2', '', 'ap', 1);
INSERT INTO books VALUES (NULL, 85, '3 Maccabees', '3Macc', '3MA', '3Ma', '', 'ap', 0);
INSERT INTO books VALUES (NULL, 86, '2 Esdras', '2Esd', '2ES', '2Es', '', 'ap', 0);
INSERT INTO books VALUES (NULL, 87, '4 Maccabees', '4Macc', '4MA', '4Ma', '', 'ap', 0);
INSERT INTO books VALUES (NULL, 88, 'Daniel (Greek)', 'AddDan', 'DNT', 'Dnt', '', 'ap', 0);

DROP PROCEDURE IF EXISTS upgrades;
DELIMITER ;;
CREATE PROCEDURE upgrades ()
BEGIN
  DECLARE CONTINUE HANDLER FOR 1060 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1061 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1091 BEGIN END;
  ALTER TABLE books ENGINE = MYISAM;
END;;
CALL upgrades();;
DROP PROCEDURE upgrades;

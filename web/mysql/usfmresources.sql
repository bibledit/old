USE `BIBLEDITDATABASE`;

CREATE TABLE IF NOT EXISTS usfmresources (
  id int auto_increment primary key,
  name varchar(256),
  book int,
  chapter int,
  usfm text,
  INDEX speedup (name, book, chapter)
) engine = MyISAM;


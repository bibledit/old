USE `BIBLEDITDATABASE`;

CREATE TABLE IF NOT EXISTS offlineresources (
  id int auto_increment primary key,
  name varchar(256),
  book int,
  chapter int,
  verse int,
  html text,
  INDEX speedup (name, book, chapter, verse)
) engine = MyISAM;


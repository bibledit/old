USE `BIBLEDITDATABASE`;

CREATE TABLE IF NOT EXISTS changes (
  id int auto_increment primary key,
  timestamp int,
  username varchar (256),
  bible int,
  book int,
  chapter int,
  verse int,
  oldtext text,
  modification text,
  newtext text
) engine = MyISAM;


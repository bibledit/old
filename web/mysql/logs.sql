USE `BIBLEDITDATABASE`;

CREATE TABLE IF NOT EXISTS logs (
  id int auto_increment primary key,
  timestamp int,
  level int,
  event text
) engine = MyISAM;


USE `BIBLEDITDATABASE`;

CREATE TABLE IF NOT EXISTS version (
  id int auto_increment primary key,
  name varchar (256),
  version int
) engine = MyISAM;


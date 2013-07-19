USE `BIBLEDITDATABASE`;

CREATE TABLE IF NOT EXISTS resources (
  id int auto_increment primary key,
  name varchar (256),
  command varchar (256),
  code text
) engine = MyISAM;


USE `BIBLEDITDATABASE`;

CREATE TABLE IF NOT EXISTS menu (
  id int auto_increment primary key,
  username varchar (256),
  url varchar (256),
  hits int
) engine = MyISAM;


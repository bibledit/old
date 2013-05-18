USE `BIBLEDITDATABASE`;

CREATE TABLE IF NOT EXISTS bible_names (
  id int auto_increment primary key,
  bible int,
  name varchar(256),
  versification varchar(256)
) engine = MyISAM;

CREATE TABLE IF NOT EXISTS bible_data (
  id int auto_increment primary key,
  bible int,
  book int,
  chapter int,
  data text
) engine = MyISAM;

CREATE TABLE IF NOT EXISTS bible_diff (
  id int auto_increment primary key,
  bible int,
  book int,
  chapter int,
  data text
) engine = MyISAM;

DROP PROCEDURE IF EXISTS upgrades;
DELIMITER ;;
CREATE PROCEDURE upgrades ()
BEGIN
  DECLARE CONTINUE HANDLER FOR 1060 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1061 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1091 BEGIN END;
  ALTER TABLE bible_names ENGINE = MYISAM;
  ALTER TABLE bible_data ENGINE = MYISAM;
  ALTER TABLE bible_diff ENGINE = MYISAM;
END;;
CALL upgrades();;
DROP PROCEDURE upgrades;

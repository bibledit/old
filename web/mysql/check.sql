USE `BIBLEDITDATABASE`;

CREATE TABLE IF NOT EXISTS check_output (
  id int auto_increment primary key,
  bible int,
  book int,
  chapter int,
  verse int,
  data text
) engine = MyISAM;

CREATE TABLE IF NOT EXISTS check_suppress (
  id int auto_increment primary key,
  bible int,
  book int,
  chapter int,
  verse int,
  data text
) engine = MyISAM;

DROP PROCEDURE IF EXISTS upgrades;
DELIMITER ;;
CREATE PROCEDURE upgrades ()
BEGIN
  DECLARE CONTINUE HANDLER FOR 1060 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1061 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1091 BEGIN END;
END;;
CALL upgrades();;
DROP PROCEDURE upgrades;

USE `BIBLEDITDATABASE`;

CREATE TABLE IF NOT EXISTS snapshots (
  id int auto_increment primary key,
  bible int,
  book int,
  chapter int,
  data text,
  seconds int,
  persistent int
) engine = MyISAM;

DROP PROCEDURE IF EXISTS upgrades;
DELIMITER ;;
CREATE PROCEDURE upgrades ()
BEGIN
  DECLARE CONTINUE HANDLER FOR 1060 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1061 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1091 BEGIN END;
  ALTER TABLE snapshots ENGINE = MYISAM;
END;;
CALL upgrades();;
DROP PROCEDURE upgrades;

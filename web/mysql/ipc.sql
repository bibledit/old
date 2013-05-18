USE `BIBLEDITDATABASE`;

CREATE TABLE IF NOT EXISTS ipc (
  id int auto_increment primary key,
  channel varchar(256),
  command varchar(30),
  message text
) engine = MyISAM;

DROP PROCEDURE IF EXISTS upgrades;
DELIMITER ;;
CREATE PROCEDURE upgrades ()
BEGIN
  DECLARE CONTINUE HANDLER FOR 1060 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1061 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1091 BEGIN END;
  ALTER TABLE ipc ADD user varchar(30) AFTER id;
  UPDATE ipc SET user = '';
END;;
CALL upgrades();;
DROP PROCEDURE upgrades;

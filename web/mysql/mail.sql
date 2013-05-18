USE `BIBLEDITDATABASE`;

CREATE TABLE IF NOT EXISTS mail (
  id int auto_increment primary key,
  username varchar(256),
  timestamp int,
  label varchar(30),
  subject varchar(256),
  body text
) engine = MyISAM;

DROP PROCEDURE IF EXISTS upgrades;
DELIMITER ;;
CREATE PROCEDURE upgrades ()
BEGIN
  DECLARE CONTINUE HANDLER FOR 1060 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1061 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1091 BEGIN END;
  # The table for the mail needs a bigger body size than 65535, so change the column type to MEDIUMTEXT
  ALTER TABLE mail MODIFY body MEDIUMTEXT;
END;;
CALL upgrades();;
DROP PROCEDURE upgrades;

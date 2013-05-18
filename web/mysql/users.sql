USE `BIBLEDITDATABASE`;

CREATE TABLE IF NOT EXISTS users (
  username varchar(30) primary key,
  password varchar(32),
  id varchar(32),
  level tinyint(1) unsigned not null,
  email varchar(256),
  timestamp int(11) unsigned not null
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

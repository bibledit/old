USE `BIBLEDITDATABASE`;

CREATE TABLE IF NOT EXISTS config_general (
  ident VARCHAR(100) NOT NULL,
  value VARCHAR(1000),
  offset INT NOT NULL
) engine = MyISAM;

CREATE TABLE IF NOT EXISTS config_user (
  id int auto_increment primary key,
  username varchar(30),
  bible varchar(256),
  ident varchar(100) not null,
  value varchar(1000),
  offset int not null
) engine = MyISAM;

DROP PROCEDURE IF EXISTS upgrades;
DELIMITER ;;
CREATE PROCEDURE upgrades ()
BEGIN
  DECLARE CONTINUE HANDLER FOR 1060 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1061 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1091 BEGIN END;
  ALTER TABLE config_general ENGINE = MYISAM;
  ALTER TABLE config_user ENGINE = MYISAM;
END;;
CALL upgrades();;
DROP PROCEDURE upgrades;

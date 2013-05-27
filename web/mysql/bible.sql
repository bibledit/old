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
  data text,
  forward text,
  reverse text,
  FULLTEXT forward (forward),
  FULLTEXT reverse (reverse)
) engine = MyISAM;

CREATE TABLE IF NOT EXISTS bible_diff (
  id int auto_increment primary key,
  bible int,
  book int,
  chapter int,
  data text
) engine = MyISAM;

DROP PROCEDURE IF EXISTS upgrade_one;
DELIMITER //
CREATE PROCEDURE upgrade_one () 
BEGIN
  DECLARE CONTINUE HANDLER FOR 1060 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1061 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1091 BEGIN END;
  SET @version := (SELECT version FROM version WHERE NAME = 'bible');
  IF @version IS NULL THEN 
    ALTER TABLE bible_names ENGINE = MYISAM;
    ALTER TABLE bible_data ENGINE = MYISAM;
    ALTER TABLE bible_diff ENGINE = MYISAM;
    # Add columns for searching the text of the Bible.
    ALTER TABLE bible_data ADD forward text AFTER data;
    ALTER TABLE bible_data ADD reverse text AFTER forward;
    ALTER TABLE bible_data DROP INDEX forward;
    ALTER TABLE bible_data DROP INDEX reverse;
    ALTER TABLE bible_data ADD FULLTEXT forward (forward);
    ALTER TABLE bible_data ADD FULLTEXT reverse (reverse);
    INSERT INTO version VALUES (NULL, 'bible', 1);
  END IF;
END;
//
DELIMITER ;
CALL upgrade_one ();
DROP PROCEDURE upgrade_one;


USE `BIBLEDITDATABASE`;

CREATE TABLE IF NOT EXISTS notes (
  id int auto_increment primary key,
  identifier int NOT NULL,
  modified int NOT NULL,
  assigned text,
  subscriptions text,
  bible varchar (256),
  passage text,
  status varchar (256),
  severity int,
  private tinyint,
  summary varchar (256),
  contents text,
  cleantext text,
  reversedtext text,
  FULLTEXT summary (summary, contents),
  FULLTEXT searching (cleantext),
  FULLTEXT reversedsearch (reversedtext)
) engine = MyISAM;

DROP PROCEDURE IF EXISTS upgrades;
DELIMITER ;;
CREATE PROCEDURE upgrades ()
BEGIN
  DECLARE CONTINUE HANDLER FOR 1060 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1061 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1091 BEGIN END;
  ALTER TABLE notes ENGINE = MYISAM;
  # Table update. Create index on identifier for much faster lookup.
  # This makes a huge difference if the number of notes gets more than, say, 1000.
  CREATE INDEX id_index ON notes (identifier);
  # Add column for searching the clean text of the note.
  ALTER TABLE notes ADD cleantext text AFTER contents;
  ALTER TABLE notes DROP INDEX searching;
  ALTER TABLE notes ADD FULLTEXT searching (cleantext);
  # Add column for searching the reversed clean text of the note.
  ALTER TABLE notes ADD reversedtext text AFTER cleantext;
  ALTER TABLE notes DROP INDEX reversedsearch;
  ALTER TABLE notes ADD FULLTEXT reversedsearch (reversedtext);
  # Drop the summary fulltext index.
  ALTER TABLE notes DROP INDEX summary;
END;;
CALL upgrades();;
DROP PROCEDURE upgrades;

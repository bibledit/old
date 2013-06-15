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


DROP PROCEDURE IF EXISTS upgrade_one;
DELIMITER //
CREATE PROCEDURE upgrade_one () 
BEGIN
  DECLARE CONTINUE HANDLER FOR 1060 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1061 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1091 BEGIN END;
  SET @version := (SELECT version FROM version WHERE NAME = 'notes');
  IF @version IS NULL THEN 
    ALTER TABLE notes ENGINE = MYISAM;
    # Table update. Create index on identifier for much faster lookup.
    # This makes a huge difference if the number of notes gets more than, say, 1000.
    CREATE INDEX id_index ON notes (identifier);
    # Drop the summary fulltext index.
    ALTER TABLE notes DROP INDEX summary;
    # Add column for searching the clean text of the note.
    ALTER TABLE notes ADD cleantext text AFTER contents;
    # Add column for searching the reversed clean text of the note.
    ALTER TABLE notes ADD reversedtext text AFTER cleantext;
    # Index for searching the clean text of the note.
    ALTER TABLE notes DROP INDEX searching;
    ALTER TABLE notes ADD FULLTEXT searching (cleantext);
    # Index for searching the reversed clean text of the note.
    ALTER TABLE notes DROP INDEX reversedsearch;
    ALTER TABLE notes ADD FULLTEXT reversedsearch (reversedtext);
    INSERT INTO version VALUES (NULL, 'notes', 1);
  END IF;
END;
//
DELIMITER ;
CALL upgrade_one ();
DROP PROCEDURE upgrade_one;



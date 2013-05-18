USE `BIBLEDITDATABASE`;

CREATE TABLE IF NOT EXISTS notes (
  id int auto_increment primary key,
  identifier int NOT NULL,
  modified int NOT NULL,
  assigned text,
  bible varchar (256),
  passage text,
  status varchar (256),
  severity int,
  private tinyint,
  summary varchar (256),
  contents text
) engine = MyISAM;

DROP PROCEDURE IF EXISTS upgrades;
DELIMITER ;;
CREATE PROCEDURE upgrades ()
BEGIN
  DECLARE CONTINUE HANDLER FOR 1060 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1061 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1091 BEGIN END;
  ALTER TABLE notes ENGINE = MYISAM;
  # Table update. Subscriptions: Contains users subscribed to this note.
  ALTER TABLE notes ADD subscriptions text AFTER assigned;
  # Table update. Allow full text search on summary and contents.
  # First drop the fulltext index. If this were not done, it would create multiple indices.
  ALTER TABLE notes DROP INDEX summary;
  ALTER TABLE notes ADD FULLTEXT(summary, contents);
  # Table update. Create index on identifier for much faster lookup.
  # This makes a huge difference if the number of notes gets more than, say, 1000.
  CREATE INDEX id_index ON notes (identifier);
END;;
CALL upgrades();;
DROP PROCEDURE upgrades;

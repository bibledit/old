-- Copyright (©) 2003-2013 Teus Benschop.

-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 3 of the License, or
-- (at your option) any later version.

-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.

-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

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

CREATE TABLE IF NOT EXISTS notes_del (
  note int primary key,
  days int
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


DROP PROCEDURE IF EXISTS upgrade_two;
DELIMITER //
CREATE PROCEDURE upgrade_two () 
BEGIN
  SET @version := (SELECT version FROM version WHERE NAME = 'notes');
  IF @version = 1 THEN 
    ALTER TABLE notes DROP private;
    UPDATE version SET version = 2 WHERE name = 'notes';
  END IF;
END;
//
DELIMITER ;
CALL upgrade_two ();
DROP PROCEDURE upgrade_two;



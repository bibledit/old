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

CREATE TABLE IF NOT EXISTS changes (
  id int auto_increment primary key,
  timestamp int,
  username varchar (256),
  bible int,
  book int,
  chapter int,
  verse int,
  oldtext text,
  modification text,
  newtext text
) engine = MyISAM;

DROP PROCEDURE IF EXISTS upgrade_one;
DELIMITER //
CREATE PROCEDURE upgrade_one () 
BEGIN
  SET @version := (SELECT version FROM version WHERE NAME = 'changes');
  IF @version IS NULL THEN 
    ALTER TABLE changes ADD category varchar (256) AFTER username;
    INSERT INTO version VALUES (NULL, 'changes', 1);
  END IF;
END;
//
DELIMITER ;
CALL upgrade_one ();
DROP PROCEDURE upgrade_one;


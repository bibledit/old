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

CREATE TABLE IF NOT EXISTS styles (
  id int auto_increment primary key,
  sheet varchar(256),
  marker varchar(10),
  name varchar(256),
  info text,
  category varchar(10),
  type int,
  subtype int,
  fontsize float,
  italic int,
  bold int,
  underline int,
  smallcaps int,
  superscript int,
  justification int,
  spacebefore float,
  spaceafter float,
  leftmargin float,
  rightmargin float,
  firstlineindent float,
  spancolumns int,
  color varchar(6),
  print int,
  userbool1 int,
  userbool2 int,
  userbool3 int,
  userint1 int,
  userint2 int,
  userint3 int,
  userstring1 varchar (512),
  userstring2 varchar (512),
  userstring3 varchar (512)
) engine = MyISAM;

UPDATE styles SET leftmargin = 5 WHERE (marker = 'ft' OR marker = 'xt') AND leftmargin = 0;

UPDATE styles SET firstlineindent = -5 WHERE (marker = 'ft' OR marker = 'xt') AND firstlineindent = 0;

DROP PROCEDURE IF EXISTS upgrades;
DELIMITER ;;
CREATE PROCEDURE upgrades ()
BEGIN
  DECLARE CONTINUE HANDLER FOR 1060 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1061 BEGIN END;
  DECLARE CONTINUE HANDLER FOR 1091 BEGIN END;
  ALTER TABLE styles ENGINE = MYISAM;
END;;
CALL upgrades();;
DROP PROCEDURE upgrades;

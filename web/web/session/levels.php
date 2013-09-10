<?
/*
Copyright (©) 2003-2013 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
  
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

// Contains the access control levels.

// Special Names and Level Constants for user access control.
// Levels must be digits between 0-9.

if (!defined ("GUEST_LEVEL"))      define("GUEST_LEVEL",      1);
if (!defined ("MEMBER_LEVEL"))     define("MEMBER_LEVEL",     2);
if (!defined ("CONSULTANT_LEVEL")) define("CONSULTANT_LEVEL", 3);
if (!defined ("TRANSLATOR_LEVEL")) define("TRANSLATOR_LEVEL", 4);
if (!defined ("MANAGER_LEVEL"))    define("MANAGER_LEVEL",    5);
if (!defined ("ADMIN_LEVEL"))      define("ADMIN_LEVEL",      6);

$roles = array
(
  GUEST_LEVEL      => gettext ("Guest"),
  MEMBER_LEVEL     => gettext ("Member"),
  CONSULTANT_LEVEL => gettext ("Consultant"),
  TRANSLATOR_LEVEL => gettext ("Translator"),
  MANAGER_LEVEL    => gettext ("Manager"),
  ADMIN_LEVEL      => gettext ("Administrator")
);

?>
<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

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


class Filter_User
{


  // Returns an integer identifier based on the name of the current user.
  public static function myIdentifier ()
  {
    $session_logic = Session_Logic::getInstance ();
    $identifier = hexdec (substr (md5 ($session_logic->currentUser ()), 0, 10));
    return $identifier;
  }


}

?>

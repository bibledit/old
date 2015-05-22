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


require_once ("bootstrap/bootstrap.php");


class Menu_Logic
{


  public static function href ($href)
  {
    return "../menu/index.php?url=$href";
  }


  public static function check_user_access ($url)
  {
    // Get the access level of the user.
    $session_logic = Session_Logic::getInstance ();
    $user_level = $session_logic->currentLevel ();

    // Check the required access level for the URL.
    // Do not check on e.g. "Filter_Roles::MEMBER_LEVEL" only,
    // but check on the fuller string, as the smaller string may occur
    // elsewhere in a script and so give the wrong access level for the script.
    @$contents = file_get_contents ("../$url.php");
    $menu_level = Filter_Roles::MEMBER_LEVEL;
    if (strpos ($contents, "page_access_level (Filter_Roles::GUEST_LEVEL)")      !== false) $menu_level = Filter_Roles::GUEST_LEVEL;
    if (strpos ($contents, "page_access_level (Filter_Roles::MEMBER_LEVEL)")     !== false) $menu_level = Filter_Roles::MEMBER_LEVEL;
    if (strpos ($contents, "page_access_level (Filter_Roles::CONSULTANT_LEVEL)") !== false) $menu_level = Filter_Roles::CONSULTANT_LEVEL;
    if (strpos ($contents, "page_access_level (Filter_Roles::TRANSLATOR_LEVEL)") !== false) $menu_level = Filter_Roles::TRANSLATOR_LEVEL;
    if (strpos ($contents, "page_access_level (Filter_Roles::MANAGER_LEVEL)")    !== false) $menu_level = Filter_Roles::MANAGER_LEVEL;
    if (strpos ($contents, "page_access_level (Filter_Roles::ADMIN_LEVEL)")      !== false) $menu_level = Filter_Roles::ADMIN_LEVEL;

    // Result: true: user has access; false: user has no access.
    if ($menu_level > $user_level) return false;
    return true;
  }


}


?>

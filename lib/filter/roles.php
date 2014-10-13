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


class Filter_Roles
{


  const GUEST_LEVEL = 1;
  const MEMBER_LEVEL = 2;
  const CONSULTANT_LEVEL = 3;
  const TRANSLATOR_LEVEL = 4;
  const MANAGER_LEVEL = 5;
  const ADMIN_LEVEL = 6;


  public static function lowest ()
  {
    return self::GUEST_LEVEL;
  }


  public static function highest ()
  {
    return self::ADMIN_LEVEL;
  }


  public static function text ($role)
  {
    switch ($role)
    {
      case self::GUEST_LEVEL:      return Locale_Translate::_("Guest");
      case self::MEMBER_LEVEL:     return Locale_Translate::_("Member");
      case self::CONSULTANT_LEVEL: return Locale_Translate::_("Consultant");
      case self::TRANSLATOR_LEVEL: return Locale_Translate::_("Translator");
      case self::MANAGER_LEVEL:    return Locale_Translate::_("Manager");
      case self::ADMIN_LEVEL:      return Locale_Translate::_("Administrator");
    }
    return Locale_Translate::_("Guest");
  }


}

?>

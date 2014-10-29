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


class Filter_Datetime
{
  /**
  * Returns an array of possible timezones.
  */
  public static function timezones ()
  {
    $identifiers = DateTimeZone::listIdentifiers();
    sort ($identifiers);
    foreach ($identifiers as $identifier) {
      // Keep the preferred identifiers, and drop the deprecated ones.
      if ((strstr ($identifier, "Africa/") !== false) ||
          (strstr ($identifier, "America/") !== false) ||
          (strstr ($identifier, "Antarctica/") !== false) ||
          (strstr ($identifier, "Arctic/") !== false) ||
          (strstr ($identifier, "Asia/") !== false) ||
          (strstr ($identifier, "Atlantic/") !== false) ||
          (strstr ($identifier, "Australia/") !== false) ||
          (strstr ($identifier, "Europe/") !== false) ||
          (strstr ($identifier, "Indian/") !== false) ||
          (strstr ($identifier, "Pacific/") !== false)) {
        $zones[] = $identifier;
      }
    }
    return $zones;
  }


  public static function user_zone (&$datetime)
  {
    $database_config_general = Database_Config_General::getInstance();
    $timezone = $database_config_general->getTimezone();
    if ($timezone != "") {
      $datetimezone = new DateTimeZone ($timezone);
      $datetime->setTimezone ($datetimezone);
    }
  }


  public static function isFirstWorkingDayOfMonth ($monthday, $weekday)
  {
    if (($monthday == 1) && in_array ($weekday, array (1, 2, 3, 4, 5))) return true;
    if (in_array ($monthday, array (2, 3)) && ($weekday == 1)) return true;
    return false;
  }


  public static function getLastBusinessDayOfMonth ($year, $month)
  {
    $time = mktime (0, 0, 0, $month, 1, $year);
    $lastday = date ("t", $time);
    $time = mktime (0, 0, 0, $month, $lastday, $year);
    $lastweekday = date ("w", $time);
    if ($lastweekday == 0) $lastday -= 2;
    if ($lastweekday == 6) $lastday--;
    return $lastday;
  }


  public static function isBusinessDay ($year, $month, $day)
  {
    $time = mktime (0, 0, 0, $month, $day, $year);
    $weekday = date ("w", $time);
    return in_array ($weekday, array (1, 2, 3, 4, 5));
  }


}

?>

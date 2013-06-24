<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2013 Teus Benschop.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


class Locale_Logic
{


  /**
  * Gets the available localizations.
  * Returns an array of them.
  */
  static public function getLocalizations ()
  {
    $localizations = array ("default");
    $localeFolder = dirname (__FILE__);
    foreach (new DirectoryIterator ($localeFolder) as $fileInfo) {
      if($fileInfo->isDot()) continue;
      if($fileInfo->isDir()) {
        $localizations [] = $fileInfo->getFilename();
      }
    }
    return $localizations;
  }


  /**
  * Gets the language of a localization.
  * Returns a string.
  */
  static public function getLanguage ($localization)
  {
    $language = "Default";
    $filename = dirname (__FILE__) . "/$localization.po";
    @$data = file_get_contents ($filename);
    if ($data !== false) {
      $data = explode ("\n", $data);
      foreach ($data as $line) {
        if (strpos ($line, "\"Language-Team: ") !== false) {
          $line = substr ($line, 16);
          $pos = strpos ($line, " <");
          $language = substr ($line, 0, $pos);
          break;
        }
      }
    }
    return $language;
  }


  /**
  * Filters out the default language.
  * Returns the $localization, or "" in case of the default language.
  */
  public static function filterDefault ($localization)
  {
    if ($localization == "default") $localization = "";
    return $localization;
  }


}  


?>

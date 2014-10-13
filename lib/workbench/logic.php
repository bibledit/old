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


class Workbench_Logic
{


  static public function defaultURLs ($id)
  {
    $urls = array_fill (0, 15, "");
    switch ($id) {
      case 1:
        $urls [0] = "edit/index.php";
        $urls [5] = "resource/index.php";
        break;
      case 2:
        $urls [0] = "edit/index.php";
        $urls [1] = "notes/index.php";
        break;
      case 3:
        $urls [0] = "resource/index.php";
        $urls [1] = "edit/index.php";
        $urls [2] = "notes/index.php";
        break;
      case 4:
        $urls [0] = "edit/index.php";
        $urls [1] = "consistency/index.php";
        break;
      default:
        $urls [0] = "edit/index.php";
        $urls [1] = "resource/index.php";
        $urls [2] = "notes/index.php";
        $urls [3] = "search/index.php";
        break;
    }
    return $urls;
  }


  static public function getURLs ($use)
  {
    $database_config_user = Database_Config_User::getInstance ();
    $workbench = Workbench_Logic::getActiveWorkbench ();
    
    $urls = $database_config_user->getWorkbenchURLs ();
    $urls = unserialize ($urls);
    @$urls = $urls [$workbench];
    if (!is_array ($urls)) $urls = array ();
    if (count ($urls) != 15) {
      $urls = self::defaultURLs ();
    }
    
    if ($use) {
      // Transform the internal URLs to full ones.
      foreach ($urls as &$url) {
        if ($url == "") continue;
        $parts = pathinfo ($url);
        $dirname = $parts ['dirname'];
        if ($dirname != basename ($dirname)) continue;
        @$extension = substr ($parts ['extension'], 0, 3);
        if ($extension != 'php') continue;
        // Add the prefix.
        $url = "../$url";
      }
    }
    
    if ($use) {
      // Add query value for suppressing the topbar as the workbench already has one.
      foreach ($urls as &$url) {
        if ($url == "") continue;
        $pos = strpos ($url, "?");
        if ($pos === false) {
          $url .= "?";
        } else {
          $url .= "&";
        }
        $url .= "topbar=0";
      }
    }

    return $urls;
  }


  static public function setURLs ($values)
  {
    $database_config_user = Database_Config_User::getInstance ();
    $workbench = Workbench_Logic::getActiveWorkbench ();
    $urls = $database_config_user->getWorkbenchURLs ();
    $urls = unserialize ($urls);
    $urls [$workbench] = $values;
    $urls = serialize ($urls);
    $database_config_user->setWorkbenchURLs ($urls);
    // For a client, store the setting for sending to the server.
    $database_config_user->addUpdatedSetting (Sync_Logic::WORKBENCH_SETTING);
  }


  static public function defaultWidths ($id)
  {
    $widths = array_fill (0, 15, 0);
    switch ($id) {
      case 1:
        $widths [0] = "100%";
        $widths [5] = "100%";
        break;
      case 2:
        $widths [0] = "75%";
        $widths [1] = "20%";
        break;
      case 3:
        $widths [0] = "32%";
        $widths [1] = "42%";
        $widths [2] = "22%";
        break;
      case 4:
        $widths [0] = "45%";
        $widths [1] = "50%";
        break;
      default:
        $widths [0] = "24%";
        $widths [1] = "24%";
        $widths [2] = "24%";
        $widths [3] = "24%";
        break;
    }
    return $widths;
  }
  
  
  static public function getWidths ()
  {
    $database_config_user = Database_Config_User::getInstance ();
    $workbench = Workbench_Logic::getActiveWorkbench ();
    $widths = $database_config_user->getWorkbenchWidths ();
    $widths = unserialize ($widths);
    @$widths = $widths [$workbench];
    if (!is_array ($widths)) $widths = array ();
    foreach ($widths as $offset => $width) {
      $widths [$offset] = Workbench_Logic::processUnits ($widths [$offset]);
    }
    if (count ($widths) != 15) {
      $widths = self::defaultWidths ();
    }
    return $widths;
  }


  public static function setWidths ($values)
  {
    $database_config_user = Database_Config_User::getInstance ();
    $workbench = Workbench_Logic::getActiveWorkbench ();
    $widths = $database_config_user->getWorkbenchWidths ();
    $widths = unserialize ($widths);
    $widths [$workbench] = $values;
    $widths = serialize ($widths);
    $database_config_user->setWorkbenchWidths ($widths);
  }


  static public function defaultHeights ($id)
  {
    $heights = array (0, 0, 0);
    switch ($id) {
      case 1:
        $heights [0] = "25%";
        $heights [1] = "65%";
        break;
      case 2:
      case 3:
      case 4:
        $heights [0] = "80%";
        break;
      default:
        $heights [0] = "90%";
        break;
    }
    return $heights;
  }
  

  static public function getHeights ()
  {
    $database_config_user = Database_Config_User::getInstance ();
    $workbench = Workbench_Logic::getActiveWorkbench ();
    $heights = $database_config_user->getWorkbenchHeights ();
    $heights = unserialize ($heights);
    @$heights = $heights [$workbench];
    if (!is_array ($heights)) $heights = array ();
    foreach ($heights as $offset => $height) {
      $heights [$offset] = Workbench_Logic::processUnits ($heights [$offset]);
    }
    if (count ($heights) != 3) {
      $heights = self::defaultHeights ();
    }
    return $heights;
  }


  public static function setHeights ($values)
  {
    $database_config_user = Database_Config_User::getInstance ();
    $workbench = Workbench_Logic::getActiveWorkbench ();
    $heights = $database_config_user->getWorkbenchHeights ();
    $heights = unserialize ($heights);
    $heights [$workbench] = $values;
    $heights = serialize ($heights);
    $database_config_user->setWorkbenchHeights ($heights);
  }


  public static function getActiveWorkbench ()
  {
    $database_config_user = Database_Config_User::getInstance ();
    $workbench = $database_config_user->getActiveWorkbench ();
    if ($workbench == "") {
      $workbench = "default";
    }
    return $workbench;
  }


  public static function getWorkbenches ()
  {
    $database_config_user = Database_Config_User::getInstance ();
    $urls = $database_config_user->getWorkbenchURLs ();
    $urls = unserialize ($urls);
    if (!is_array ($urls)) {
      return array (Workbench_Logic::getActiveWorkbench ());
    }
    $workbenches = array_keys ($urls);
    if (count ($workbenches) == 0) {
      return array (Workbench_Logic::getActiveWorkbench ());
    }
    return $workbenches;
  }
  
  
  // This orders the workbenches.
  // It takes the order as in array $workbenches.
  public static function orderWorkbenches ($workbenches)
  {
    $database_config_user = Database_Config_User::getInstance ();

    // Retrieve the old order of the workbenches, plus their details.
    $urls = $database_config_user->getWorkbenchURLs ();
    $urls = unserialize ($urls);

    $widths = $database_config_user->getWorkbenchWidths ();
    $widths = unserialize ($widths);

    $heights = $database_config_user->getWorkbenchHeights ();
    $heights = unserialize ($heights);

    // Create an new arrays that have the sorted workbench definitions.
    $newurls = array ();
    $newwidths = array ();
    $newheights = array ();
    foreach ($workbenches as $workbench) {
      $newurls [$workbench] = $urls [$workbench];
      $newwidths [$workbench] = $widths [$workbench];
      $newheights [$workbench] = $heights [$workbench];
    }
    
    // Save everything.
    $urls = serialize ($newurls);
    $database_config_user->setWorkbenchURLs ($urls);
    $widths = serialize ($newwidths);
    $database_config_user->setWorkbenchWidths ($widths);
    $heights = serialize ($newheights);
    $database_config_user->setWorkbenchHeights ($heights);
  }


  public static function deleteWorkbench ($workbench)
  {
    $workbenches = Workbench_Logic::getWorkbenches ();

    $database_config_user = Database_Config_User::getInstance ();

    $urls = $database_config_user->getWorkbenchURLs ();
    $urls = unserialize ($urls);
    unset ($urls [$workbench]);
    $urls = serialize ($urls);
    $database_config_user->setWorkbenchURLs ($urls);

    $widths = $database_config_user->getWorkbenchWidths ();
    $widths = unserialize ($widths);
    unset ($widths [$workbench]);
    $widths = serialize ($widths);
    $database_config_user->setWorkbenchWidths ($widths);

    $heights = $database_config_user->getWorkbenchHeights ();
    $heights = unserialize ($heights);
    unset ($heights [$workbench]);
    $heights = serialize ($heights);
    $database_config_user->setWorkbenchHeights ($heights);
    $database_config_user->setActiveWorkbench ("");
    
    // For a client, store the setting for sending to the server.
    $database_config_user->addUpdatedSetting (Sync_Logic::WORKBENCH_SETTING);

  }


  // This function processes the units for a $length value.
  public static function processUnits ($length)
  {
    // If no unit is found, add %.
    if ($length == Filter_Numeric::integer_in_string ($length)) {
      return $length . '%';
    }
    // If % is found, clean the value up.
    if (strpos ($length, '%') !== false) {
      $length = Filter_Numeric::integer_in_string ($length) . '%';
      return $length;
    }
    // If px is found, clean the value up.
    if (strpos ($length, 'px') !== false) {
      $length = Filter_Numeric::integer_in_string ($length) . 'px';
      return $length;
    }
    // Pass the length through.
    return $length;
  }

}


?>

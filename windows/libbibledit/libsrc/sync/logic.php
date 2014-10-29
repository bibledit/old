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


class Sync_Logic
{


  // Sends a $post request to the $url.
  // On failure it retries a few times.
  // It returns the server's response, or false on failure.
  public static function post ($post, $url)
  {
    // Use key 'http' even if you send the request to https.
    $options = array (
      'http' => array (
        'header'  => "Content-type: application/x-www-form-urlencoded\r\n",
        'method'  => 'POST',
        'content' => http_build_query ($post),
        'timeout' => 3600
      ),
    );
    $context  = stream_context_create ($options);

    // After failure, retry a few times more.
    $retry = 0;
    while (++$retry <= 3) {
      @$response = file_get_contents ($url, false, $context);
      if ($response === false) {
        // Log failure.
        $database_logs = Database_Logs::getInstance ();
        // If the server or connection is down entirely there won't be even a response header.
        @$response_header = $http_response_header [0];
        if ($response_header) $database_logs->log ($response_header, Filter_Roles::TRANSLATOR_LEVEL);
      } else {
        // Return successful response.
        return $response;
      }
    }
    // Failure: Give up.
    return false;
  }


  // This function takes a $start-ing note identifier,
  // and an $end-ing note identifier.
  // It divides this range of identifier into ten 
  // smaller ranges.
  // It returns an array of array ($start, $end) with the new ranges 
  // for the note identifiers.
  public static function create_range ($start, $end)
  {
    $range = $end - $start;
    $range = intval ($range / 10);
    $ranges = array ();
    for ($i = 0; $i <= 9; $i++) {
      $first = $start + ($i * $range);
      $last = $start + (($i + 1) * $range) - 1;
      if ($i == 9) {
        $last = $end;
      }
      $ranges [] = array ($first, $last);
    }
    return $ranges;
  }
  
  
  // Calculates the checksum of the array of note $identifiers.
  public static function checksum ($identifiers)
  {
    $database_notes = Database_Notes::getInstance ();
    $checksums = array ();
    foreach ($identifiers as $identifier) {
      $checksums [] = $database_notes->getChecksum ($identifier);
    }
    $checksum = implode ("", $checksums);
    $checksum = md5 ($checksum);
    return $checksum;
  }


  // Calculates the checksum of all settings that to be kept in sync between server and client.
  static public function settings_checksum ()
  {
    $checksum = "";
    $checksum .= Sync_Logic::workbench_checksum ();
    $checksum = md5 ($checksum);
    return $checksum;
  }
  
  
  // Checksum calculation for workbench settings.
  const WORKBENCH_SETTING = 1;
  static public function workbench_checksum ()
  {
    $database_config_user = Database_Config_User::getInstance ();
    $urls = $database_config_user->getWorkbenchURLs ();
    $widths = $database_config_user->getWorkbenchWidths ();
    $heights = $database_config_user->getWorkbenchHeights ();
    $checksum = md5 ($urls . $widths . $heights);
    return $checksum;
  }
  
  
  // Calculates the checksum of all offline resources.
  static public function offline_resources_checksum ()
  {
    $checksum = "";
    $database_offlineresources = Database_OfflineResources::getInstance ();
    $names = $database_offlineresources->names ();
    foreach ($names as $name) {
      $checksum .= self::offline_resource_checksum ($name);
    }
    $checksum = md5 ($checksum);
    return $checksum;
  }


  // Calculates the checksum of offline resource $name.
  static public function offline_resource_checksum ($name)
  {
    $checksum = array ();
    $database_offlineresources = Database_OfflineResources::getInstance ();
    $files = $database_offlineresources->files ($name);
    foreach ($files as $file) {
      $checksum [] = $name;
      $checksum [] = self::offline_resource_file_checksum ($name, $file);
    }
    $checksum = implode ("", $checksum);
    $checksum = md5 ($checksum);
    return $checksum;    
  }


  // Calculates the checksum of offline resource $name the $file.
  static public function offline_resource_file_checksum ($name, $file)
  {
    $database_offlineresources = Database_OfflineResources::getInstance ();
    $checksum = $database_offlineresources->size ($name, $file);
    return $checksum;
  }

  
  // Calculates the checksum of all USFM resources.
  static public function usfm_resources_checksum ()
  {
    $checksum = array ();
    $database_usfmresources = Database_UsfmResources::getInstance ();
    $resources = $database_usfmresources->getResources ();
    foreach ($resources as $resource) {
      $checksum [] = self::usfm_resource_checksum ($resource);
    }
    $checksum = implode ("", $checksum);
    $checksum = md5 ($checksum);
    return $checksum;    
  }


  // Calculates the checksum of USFM resource $name.
  static public function usfm_resource_checksum ($name)
  {
    $checksum = array ();
    $database_usfmresources = Database_UsfmResources::getInstance ();
    $books = $database_usfmresources->getBooks ($name);
    foreach ($books as $book) {
      $checksum [] = $book;
      $checksum [] = self::usfm_resource_book_checksum ($name, $book);
    }
    $checksum = implode ("", $checksum);
    $checksum = md5 ($checksum);
    return $checksum;    
  }


  // Calculates the checksum of USFM resource $name $book.
  static public function usfm_resource_book_checksum ($name, $book)
  {
    $checksum = array ();
    $database_usfmresources = Database_UsfmResources::getInstance ();
    $chapters = $database_usfmresources->getChapters ($name, $book);
    foreach ($chapters as $chapter) {
      $checksum [] = $chapter;
      $checksum [] = self::usfm_resource_chapter_checksum ($name, $book, $chapter);
    }
    $checksum = implode ("", $checksum);
    $checksum = md5 ($checksum);
    return $checksum;    
  }


  // Calculates the checksum of USFM resource $name $book $chapter.
  static public function usfm_resource_chapter_checksum ($name, $book, $chapter)
  {
    $database_usfmresources = Database_UsfmResources::getInstance ();
    $checksum = $database_usfmresources->getSize ($name, $book, $chapter);
    return $checksum;
  }

  
}


?>

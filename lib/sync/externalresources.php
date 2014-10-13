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


require_once ("../bootstrap/bootstrap.php");


// Processing things may take time.
// Set the maximum execution time to indefinite.
set_time_limit (300);
ini_set ('max_execution_time', 300);


$database_logs = Database_Logs::getInstance ();
$database_offlineresources = Database_OfflineResources::getInstance ();


$action = $_POST ['a'];
@$resource = $_POST ['r'];
@$file = $_POST ['f'];


if ($action == "total") {

  $checksum = Sync_Logic::offline_resources_checksum ();
  $checksum = serialize ($checksum);
  echo $checksum;

} else if ($action == "resources") {
  
  $resources = $database_offlineresources->names ();
  $resources = serialize ($resources);
  echo $resources;
  
} else if ($action == "resource") {

  $checksum = Sync_Logic::offline_resource_checksum ($resource);
  $checksum = serialize ($checksum);
  echo $checksum;
  
} else if ($action == "files") {
  
  $files = $database_offlineresources->files ($resource);
  $files = serialize ($files);
  echo $files;
  
} else if ($action == "file") {

  $checksum = Sync_Logic::offline_resource_file_checksum ($resource, $file);
  $checksum = serialize ($checksum);
  echo $checksum;
  
} else if ($action == "download") {
  
  $contents = $database_offlineresources->load ($resource, $file);
  $contents = serialize ($contents);
  echo $contents;
  
}


?>

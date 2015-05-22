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


// Security: The script runs from the cli SAPI only.
Filter_Cli::assert ();


$database_config_general = Database_Config_General::getInstance ();
$database_logs = Database_Logs::getInstance ();
$database_offlineresources = Database_OfflineResources::getInstance ();


$database_logs->log (Locale_Translate::_("Synchronizing external resources"), Filter_Roles::TRANSLATOR_LEVEL);


$address = $database_config_general->getServerAddress ();
$url = "$address/sync/externalresources.php";


// Request the checksum of all offline resources from the server.
// Compare it with the local checksum.
// If the two match: Ready.
$post = array (
  "a" => "total",
);
$response = Sync_Logic::post ($post, $url);
@$response = unserialize ($response);
if ($response === false) {
  $database_logs->log ("Failure requesting checksum for the external resources", Filter_Roles::TRANSLATOR_LEVEL);
  die;
}
$checksum = Sync_Logic::offline_resources_checksum ();
if ($response == $checksum) {
  $database_logs->log ("The external resources are up to date", Filter_Roles::TRANSLATOR_LEVEL);
  die;
}


// Request a list of all offline resources available on the server.
$post = array (
  "a" => "resources",
);
$response = Sync_Logic::post ($post, $url);
@$response = unserialize ($response);
if ($response === false) {
  $database_logs->log ("Failure requesting available external resources", Filter_Roles::TRANSLATOR_LEVEL);
  die;
}
$server_resources = $response;


// Delete resources that exist locally but not on the server.
$client_resources = $database_offlineresources->names ();
$resources = array_diff ($client_resources, $server_resources);
foreach ($resources as $resource) {
  $database_offlineresources->delete ($resource);
}


// Deal with each offline resource individually.
foreach ($server_resources as $resource) {


  // Request the checksum of the resources from the server.
  // Compare it with the checksum of the local resource.
  // If they match: Go to next resource.
  $post = array (
    "a" => "resource",
    "r" => $resource
  );
  $response = Sync_Logic::post ($post, $url);
  @$response = unserialize ($response);
  if ($response === false) {
    $database_logs->log ("Failure requesting checksum of external resource", Filter_Roles::TRANSLATOR_LEVEL);
    continue;
  }
  $checksum = Sync_Logic::offline_resource_checksum ($resource);
  if ($response == $checksum) {
    continue;
  }
  
  
  // Request a list of all files in the resource on the server.
  $post = array (
    "a" => "files",
    "r" => $resource
  );
  $response = Sync_Logic::post ($post, $url);
  @$response = unserialize ($response);
  if ($response === false) {
    $database_logs->log ("Failure requesting files of external resource", Filter_Roles::TRANSLATOR_LEVEL);
    continue;
  }
  $server_files = $response;


  // Delete files that exist locally but not on the server.
  $client_files = $database_offlineresources->files ($resource);
  $files = array_diff ($client_files, $server_files);
  foreach ($files as $file) {
    $database_offlineresources->unlink ($resource, $file);
  }
 
  
  // Deal with each file individually.
  foreach ($server_files as $file) {


    // Request checksum of this file,
    // compare it with the local checksum,
    // and skip the file if the checksums match.
    $post = array (
      "a" => "file",
      "r" => $resource,
      "f" => $file
    );
    $response = Sync_Logic::post ($post, $url);
    @$response = unserialize ($response);
    if ($response === false) {
      $database_logs->log ("Failure requesting checksum of external resource file", Filter_Roles::TRANSLATOR_LEVEL);
      continue;
    }
    $checksum = Sync_Logic::offline_resource_file_checksum ($resource, $file);
    if ($response == $checksum) {
      continue;
    }


    // Download the file from the server, and store it locally on the client.
    $database_logs->log ("Downloading external resource" . " $resource $file", Filter_Roles::TRANSLATOR_LEVEL);
    $post = array (
      "a" => "download",
      "r" => $resource,
      "f" => $file
    );
    $response = Sync_Logic::post ($post, $url);
    @$response = unserialize ($response);
    if ($response === false) {
      $database_logs->log ("Failure downloading external resource file", Filter_Roles::TRANSLATOR_LEVEL);
      continue;
    }
    $database_offlineresources->save ($resource, $file, $response);


  }

  
}


// Done.
$database_logs->log ("The external resources are now up to date", Filter_Roles::TRANSLATOR_LEVEL);


?>

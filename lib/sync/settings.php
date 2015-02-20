<?php

// C++Port for notes.




/*


$setting = unserialize (request->post ['s']);





  $user = Filter_Hex::hex2bin (request->post ['u']);
  if (!request->database_users ()->usernameExists ($user)) {
    Database_Logs::log ("A client passes non existing user $user", Filter_Roles::manager ());
    die;
  }

  $md5 = request->post ['p'];
  if ($md5 != request->database_users ()->getmd5 ($user)) {
    Database_Logs::log ("A client provides incorrect password for user $user", Filter_Roles::manager ()); // Test it.
    die;
  }

  $bibles = access_bible_bibles ($user);

  $lowId = request->post ['l'];
  $highId = request->post ['h'];

  $identifiers = $database_notes->getNotesInRangeForBibles ($lowId, $highId, $bibles);
  $checksum = $database_notes->getMultipleChecksum ($identifiers);
  $response ['t'] = count ($identifiers);
  $response ['c'] = $checksum;
  die_if_databases_unhealthy_or_busy ();
  echo serialize ($response);

} else if ($action == "identifiers") {

  $user = Filter_Hex::hex2bin (request->post ['u']);
  if (!request->database_users ()->usernameExists ($user)) {
    die;
  }
  $md5 = request->post ['p'];
  if ($md5 != request->database_users ()->getmd5 ($user)) {
    die;
  }

  $bibles = access_bible_bibles ($user);

  $lowId = request->post ['l'];
  $highId = request->post ['h'];

  $identifiers = $database_notes->getNotesInRangeForBibles ($lowId, $highId, $bibles);
  $checksums = array ();
  for ($identifiers as $identifier) {
    $checksum = $database_notes->getChecksum ($identifier);
    $checksums [] = $checksum;
  }
  $response ['i'] = $identifiers;
  $response ['c'] = $checksums;
  die_if_databases_unhealthy_or_busy ();
  echo serialize ($response);

} else if ($action == "fetch") {

  $identifier = request->post ['i'];

  // Update search and checksum.
  $database_notes->updateSearchFields ($identifier);
  $database_notes->updateChecksum ($identifier);

  $modified = $database_notes->getModified ($identifier);
  $assignees = $database_notes->getAssignees ($identifier);
  $subscribers = $database_notes->getSubscribers ($identifier);
  $bible = $database_notes->getBible ($identifier);
  $passages = $database_notes->getPassages ($identifier);
  $status = $database_notes->getRawStatus ($identifier);
  $severity = $database_notes->getRawSeverity ($identifier);
  $summary = $database_notes->getSummary ($identifier);
  $contents = $database_notes->getContents ($identifier);

  Database_Logs::log ("Client requested a note from server" . ": " . $summary, Filter_Roles::manager ());

  $response = array (
    'm'  => $modified,
    'a'  => $assignees,
    'sub' => $subscribers,
    'b'  => $bible,
    'p'  => $passages,
    'st' => $status,
    'se' => $severity,
    'sum' => $summary,
    'c'  => $contents
  );

  $response = serialize ($response);
  die_if_databases_unhealthy_or_busy ();
  echo $response;
  
} else {

  echo "The server did not recognize the sync action";
  
}


function die_if_databases_unhealthy_or_busy ()
{
  $database_notes = Database_Notes::getInstance ();
  $available = true;
  if (!$database_notes->healthy ()) $available = false;
  if (!$database_notes->checksums_healthy ()) $available = false;
  if (!$database_notes->available ()) $available = false;
  if (!$available) {
    $database_logs = Database_Logs::getInstance ();
    Database_Logs::log ("Notes databases are unhealthy or unavailable", Filter_Roles::translator ());
    die;
  }
}

*/


?>

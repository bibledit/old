void notes_cleancli () {}
/*
require_once ("../bootstrap/bootstrap");

$database_logs = Database_Logs::getInstance ();
$database_notes = Database_Notes::getInstance ();


Database_Logs::log (translate("Removal of duplicate consultation notes has started"), true);


// Security: Page only runs from the cli SAPI.
Filter_Cli::assert ();


// Go through all note identifiers.
$identifiers = $database_notes->getIdentifiers ();
for ($identifiers as $identifier) {
  Database_Logs::log (translate("Looking into note") . " " . $identifier, true);
  // The note may have been deleted already. Check on that.
  if ($database_notes->identifierExists ($identifier)) {
    // Fields that must match in duplicates: passage summary contents
    $rawpassage = $database_notes->getRawPassage ($identifier);
    $summary = $database_notes->getSummary ($identifier);
    $contents = $database_notes->getContents ($identifier);
    $duplicates = $database_notes->selectDuplicateNotes ($rawpassage, $summary, $contents);
    $duplicate_count = count ($duplicates);
    if ($duplicate_count > 1) {
      Database_Logs::log ("Passage $rawpassage, Summary $summary, Duplicate count $duplicate_count", true);
      for ($i = 1; $i < $duplicate_count; $i++) {
        $duplicate = $duplicates[$i];
        Database_Logs::log (translate("Deleting duplicate note") . " " . $duplicate, true);
        $database_notes->delete ($duplicate);
      }
    }
  } else {
    Database_Logs::log (translate("This note no longer exists"), true);
  }
}
unset ($identifier);


Database_Logs::log (translate("Removal of duplicate consultation notes has finished"), true);
*/

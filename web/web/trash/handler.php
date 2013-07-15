<?php


require_once ("bootstrap/bootstrap.php");


/*

The purpose of this class is to partly simulate a Trash.
This is how it works:
It gets information from the object to delete.
It converts this information to plain text.
It adds information as to who did what.
It logs the event.
The user can consult the log later, and partially reconstruct the original item.

*/

class Trash_Handler
{

  // Singleton instance.
  private static $instance;
  public static function getInstance() 
  {
    if ( empty( self::$instance ) ) {
      self::$instance = new Trash_Handler ();
    }
    return self::$instance;
  }


  public function changeNotification ($id)
  {
    $database_changes = Database_Changes::getInstance ();
    $database_logs = Database_Logs::getInstance ();
    $passage = $database_changes->getPassage ($id);
    $passageText = Filter_Books::passagesDisplayInline (array (array ($passage['book'], $passage['chapter'], $passage['verse'])));
    $modification = $database_changes->getModification ($id);
    $session_logic = Session_Logic::getInstance ();
    $username = $session_logic->currentUser ();
    $database_logs->log ("$username removed change notification $passageText: $modification"); 
  }


  public function consultationNote ($id)
  {
    $database_notes = Database_Notes::getInstance ();
    $database_logs = Database_Logs::getInstance ();
    $passage = $database_notes->getPassages ($id);
    $passageText = Filter_Books::passagesDisplayInline ($passage);
    $summary = $database_notes->getSummary ($id);
    $contents = $database_notes->getContents ($id);
    $contents = Filter_Html::html2text ($contents);
    $session_logic = Session_Logic::getInstance ();
    $username = $session_logic->currentUser ();
    $database_logs->log ("$username removed consultation note $passageText | $summary | $contents"); 
  }



}


?>

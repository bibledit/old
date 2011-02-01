<?php


class Dialog_Yes2
{
  private $smarty;

  /**
  * Dialog that asks the user for confirmation to perform an action.
  * $question: The question to ask.
  * $action: String with the query parameter to add to the basic query.
  * In case the user gives confirmation, then the following parameters are added: &confirm=yes
  */
  public function __construct ($question, $action)
  {
    $this->smarty = new Smarty_Bibledit (__FILE__);
    $database_sessions = Database_Sessions::getInstance ();
    $caller_url = $_SERVER["PHP_SELF"] . "?" . http_build_query (array ("session" => $database_sessions->getCurrentSessionId ()));
    $this->smarty->assign ("caller_url", $caller_url);
    $this->smarty->assign ("action", $action);
    $this->smarty->assign ("question",     $question);
    $this->smarty->display("yes2.tpl");
    Assets_Page::footer ();
    die ();
  }
}


?>

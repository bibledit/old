<?php


class Dialog_Yes2
{
  private $view;

  /**
  * Dialog that asks the user for confirmation to perform an action.
  * $question: The question to ask.
  * $action: String with the query parameter to add to the basic query.
  * In case the user gives confirmation, then the following parameters are added: &confirm=yes
  */
  public function __construct ($question, $action)
  {
    $this->view = new Assets_View (__FILE__);
    $caller_url = $_SERVER["PHP_SELF"] . "?" . http_build_query (array ());
    $this->view->view->caller_url = $caller_url;
    $this->view->view->action = $action;
    $this->view->view->question = $question;
    $this->view->render ("yes2.php");
    Assets_Page::footer ();
    die ();
  }
}


?>

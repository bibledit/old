<?php


class Dialog_Yes
{
  private $view;

  /**
  * Dialog that asks the user for confirmation to perform an action.
  * $query: Array with the basic query parameters for the page where to go on clicking Cancel or Yes.
  * $question: The question to ask.
  * $action: String with the query parameter to add to the basic query in order to perform the desired action.
  */
  public function __construct ($query, $question, $action) 
  {
    $this->view = new Assets_View (__FILE__);

    $caller_url = $_SERVER["PHP_SELF"];
    if (is_array ($query)) {
      $full_query = array ();
      foreach ($query as $value) {
        $full_query = array_merge ($full_query, array ($value => $_GET[$value]));
      }
      $caller_url .= "?" . http_build_query ($full_query);
    }
    $this->view->view->caller_url = $caller_url;

    if ($action != "") {
      $full_query = array ($action => $_GET[$action]);
      if (is_array ($query)) $action = "&";
      else $action = "?";
      $action .= http_build_query ($full_query);
    }
    $this->view->view->action = $action;

    $this->view->view->question = $question;
    $this->view->render ("yes.php");
    Assets_Page::footer ();
  }
}


?>

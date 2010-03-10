<?php


class Dialog_Yes
{
  private $smarty;

  /**
  * Dialog that asks the user for confirmation to perform an action
  * $query: Array with the basic query parameters for the page where to go
  * on clicking Cancel or Yes.
  * $question: The question to ask.
  * $action: String with the query parameter to add to the basic query in order to perform the desired action.
  */
  public function __construct ($query, $question, $action) 
  {
    $this->smarty = new Smarty_Bibledit (__FILE__);

    $caller_url = $_SERVER["PHP_SELF"];
    if (is_array ($query)) {
      $full_query = array ();
      foreach ($query as $value) {
        $full_query = array_merge ($full_query, array ($value => $_GET[$value]));
      }
      $caller_url .= "?" . http_build_query ($full_query);
    }
    $this->smarty->assign ("caller_url", $caller_url);

    if ($action != "") {
      $full_query = array ($action => $_GET[$action]);
      if (is_array ($query)) $action = "&";
      else $action = "?";
      $action .= http_build_query ($full_query);
    }
    $this->smarty->assign ("action", $action);

    $this->smarty->assign ("question",     $question);
    $this->smarty->display("yes.tpl");
  }
}


?>

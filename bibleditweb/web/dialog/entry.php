<?php


class Dialog_Entry
{
  private $smarty;


  /**
  * Entry dialog constructor
  * $query   : Nothing, or array with query variables, e.g. array ("search" => "bibledit").
  *            If any $query is passed, if Cancel is clicked in this dialog, it should go go back
  *            to the original caller page  with the $query added. Same for Ok, it would post
  *            the value entered, but also add the $query to the url.
  * $question: The question to be asked.
  * $value   : The initial value to be put into the entry.
  * $submit  : Name of POST request to submit the information.
  * $help    : Help information explaining to the user what's going on.
  */
  public function __construct ($query, $question, $value, $submit, $help)
  {
    $this->smarty = new Smarty_Bibledit (__FILE__);
    $base_url = $_SERVER['PHP_SELF'];
    if (is_array ($query)) {
      $base_url .= "?" . http_build_query ($query);
    }
    $this->smarty->assign ("base_url", $base_url);
    $this->smarty->assign ("question", $question);
    $this->smarty->assign ("value",    $value);
    $this->smarty->assign ("submit",   $submit);
    $this->smarty->assign ("help",     $help);
    $this->smarty->display("entry.tpl");
  }
}


?>


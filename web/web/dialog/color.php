<?php


class Dialog_Color
{
  private $view;

  public function __construct ($query, $question, $item) 
  {
    $this->view = new Assets_View (__FILE__);
    $base_url = $_SERVER['PHP_SELF'];
    if (is_array ($query)) {
      $base_url .= "?" . http_build_query ($query);
      $active_url = $base_url . "&";
    } else {
      $active_url = $base_url . "?";
    }
    $active_url .= $item . "=";
    $this->view->view->base_url = $base_url;
    $this->view->view->active_url = $active_url;
    $this->view->view->question = $question;
    $this->view->render ("color.php");
    Assets_Page::footer ();
  }
}


?>

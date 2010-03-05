<?php


class Dialog_Color
{
  private $smarty;

  public function __construct ($query, $question, $item) 
  {
    $this->smarty = new Smarty_Bibledit (__FILE__);
    $base_url = $_SERVER['PHP_SELF'];
    if (is_array ($query)) {
      $base_url .= "?" . http_build_query ($query);
      $active_url = $base_url . "&";
    } else {
      $active_url = $base_url . "?";
    }
    $active_url .= $item . "=";
    $this->smarty->assign ("base_url",   $base_url);
    $this->smarty->assign ("active_url", $active_url);
    $this->smarty->assign ("question",   $question);
    $this->smarty->display("color.tpl");
  }
}


?>


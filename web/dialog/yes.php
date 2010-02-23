<?php


class Dialog_Yes
{
  private $smarty;

  public function __construct ($question) 
  {
    $this->smarty = new Smarty_Bibledit (__FILE__);
    $this->smarty->assign ("php_self",     $_SERVER['PHP_SELF']);
    $this->smarty->assign ("question",     $question);
    $this->smarty->assign ("query_string", $_SERVER['QUERY_STRING']);
    $this->smarty->display("yes.tpl");
  }
}


?>


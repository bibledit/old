<?php


class Dialog_Entry
{
  private $smarty;

  public function __construct ($question, $submit) 
  {
    $this->smarty = new Smarty_Bibledit (__FILE__);
    $this->smarty->assign ("php_self", $_SERVER['PHP_SELF']);
    $this->smarty->assign ("question", $question);
    $this->smarty->assign ("submit",   $submit);
    $this->smarty->display("entry.tpl");
  }
}


?>


<?php


class Dialog_List
{

  private $smarty;
  private $text_lines;
  private $get_parameters;
    

  public function __construct ($caller_url, $header, $info_top, $info_bottom) 
  {
    $this->smarty = new Smarty_Bibledit (__FILE__);
    list($caller_url) = explode('?', $caller_url); // Remove query string.
    $this->smarty->assign ("caller_url", $caller_url);
    $this->smarty->assign ("header", $header);
    $this->smarty->assign ("info_top", $info_top);
    $this->smarty->assign ("info_bottom", $info_bottom);
  }


  public function add_row ($text_line, $get_parameter)
  {
    $this->text_lines[]     = $text_line;
    $this->get_parameters[] = $get_parameter;
  }

  
  public function run ()
  {
    $this->smarty->assign ("text_lines", $this->text_lines);
    $this->smarty->assign ("get_parameters", $this->get_parameters);
    $this->smarty->display("list.tpl");
  }  

  
}
?>


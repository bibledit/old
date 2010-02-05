<?php


class Dialog_List
{

  private $smarty;
  private $text_lines;
  private $sql_queries;
    

  public function __construct ($caller_url, $header, $info_top, $info_bottom) 
  {
    $this->smarty = new Smarty_Bibledit (__FILE__);
    $this->smarty->assign ("caller_url", $caller_url);
    $this->smarty->assign ("header", $header);
    $this->smarty->assign ("info_top", $info_top);
    $this->smarty->assign ("info_bottom", $info_bottom);
  }


  public function add_row ($text_line, $sql_query)
  {
    $this->text_lines[] = $text_line;
    $this->sql_queries[] = $sql_query;
  }

  
  public function run ()
  {
    $this->smarty->assign ("text_lines", $this->text_lines);
    $this->smarty->assign ("sql_queries", $this->sql_queries);
    $this->smarty->display("list.tpl");
  }  

  
}
?>


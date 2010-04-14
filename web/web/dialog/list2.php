<?php


/**
* Dialog that presents the user with a list of options and asks the user to pick one.
*/
class Dialog_List2
{
  private $smarty;
  private $text_lines;
  private $get_parameters;
    

  public function __construct ($header)
  {
    $this->smarty = new Smarty_Bibledit (__FILE__);
    $database_sessions = Database_Sessions::getInstance ();
    $caller_url = $_SERVER["PHP_SELF"] . "?" . http_build_query (array ("session" => $database_sessions->getCurrentSessionId ()));
    $this->smarty->assign ("caller_url", $caller_url);
    $this->smarty->assign ("header", $header);
    $this->smarty->assign ("info_top", gettext ("Here are the various options:"));
    $this->smarty->assign ("info_bottom", gettext ("Please pick one."));
  }


  public function info_top ($text)
  {
    $this->smarty->assign ("info_top", $text);
  }
  
  
  public function info_bottom ($text)
  {
    $this->smarty->assign ("info_bottom", $text);
  }


  public function horizontal ()
  {
    $this->smarty->assign ("horizontal", true);
  }
      

  public function add_row ($text_line, $get_parameter)
  {
    $this->text_lines[]     = $text_line;
    $this->get_parameters[] = $get_parameter;
  }

  
  public function run ()
  {
    $this->smarty->assign ("text_lines",     $this->text_lines);
    $this->smarty->assign ("get_parameters", $this->get_parameters);
    $this->smarty->display("list2.tpl");
    Assets_Page::footer ();
    die ();
  }  

  
}
?>


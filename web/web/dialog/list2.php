<?php


/**
* Dialog that presents the user with a list of options and asks the user to pick one.
*/
class Dialog_List2
{
  private $view;
  private $text_lines;
  private $get_parameters;
    

  public function __construct ($header)
  {
    $this->view = new Assets_View (__FILE__);
    $database_sessions = Database_Sessions::getInstance ();
    $caller_url = $_SERVER["PHP_SELF"] . "?" . http_build_query (array ("session" => $database_sessions->getCurrentSessionId ()));
    $this->view->view->caller_url = $caller_url;
    $this->view->view->header = $header;
    $this->view->view->info_top = gettext ("Here are the various options:");
    $this->view->view->info_bottom = gettext ("Please pick one.");
  }


  public function info_top ($text)
  {
    $this->view->view->info_top = $text;
  }
  
  
  public function info_bottom ($text)
  {
    $this->view->view->info_bottom = $text;
  }


  public function horizontal ()
  {
    $this->view->view->horizontal = true;
  }
      

  public function add_row ($text_line, $get_parameter)
  {
    $this->text_lines[]     = $text_line;
    $this->get_parameters[] = $get_parameter;
  }

  
  public function run ()
  {
    $this->view->view->text_lines = $this->text_lines;
    $this->view->view->get_parameters = $this->get_parameters;
    $this->view->render ("list2.php");
    Assets_Page::footer ();
    die ();
  }  

  
}
?>

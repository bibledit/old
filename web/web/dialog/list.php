<?php


class Dialog_List
{

  private $view;
  private $text_lines;
  private $get_parameters;
    

  /**
  * Dialog that presents the user with a list of options and asks the user to pick one.
  * $query: Array with the basic query parameters for the page where to go on clicking Cancel or making a choice.
  * $info_top, $info_bottom - If these are left empty, they take standard values.
  * $horizontal - if true the list of options shows horizontally, rather than vertically, and the $info_ does not show.
  */
  public function __construct ($query, $header, $info_top, $info_bottom, $horizontal = false)
  {
    Assets_Page::header (gettext ("List"));
    $this->view = new Assets_View (__FILE__);

    $caller_url = $_SERVER["PHP_SELF"];

    if (is_array ($query)) {
      $full_query = array ();
      foreach ($query as $value) {
        $full_query = array_merge ($full_query, array ($value => $_GET[$value]));
      }
      $caller_url .= "?" . http_build_query ($full_query);
    }

    $this->view->view->caller_url = $caller_url;

    $this->view->view->header = $header;

    if ($info_top == "") 
      $info_top = gettext ("Here are the various options:");
    $this->view->view->info_top = $info_top;

    if ($info_bottom == "") 
      $info_bottom = gettext ("Please pick one.");
    $this->view->view->info_bottom = $info_bottom;
    
    $this->view->view->horizontal = $horizontal;
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
    $this->view->render ("list.php");
    Assets_Page::footer ();
  }  

  
}
?>

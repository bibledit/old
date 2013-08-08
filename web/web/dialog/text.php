<?php


/**
* Dialog that presents the user with a multi-line text area for text entry.
*/
class Dialog_Text
{
  private $view;
    
  /**
  * Text dialog constructor.
  * $header: What header to show.
  * $text: Initial text for the user to edit.
  * $parameter: The GET parameter to be used to save the text.
  */
  public function __construct ($header, $text, $parameter)
  {
    $this->view = new Assets_View (__FILE__);
    $this->view->view->caller = $_SERVER["PHP_SELF"];
    $this->view->view->header = $header;
    $this->view->view->parameter = $parameter;
    $this->view->view->text = $text;
  }


  public function info_top ($text)
  {
    $this->view->view->info_top = $text;
  }
  
  
  public function info_bottom ($text)
  {
    $this->view->view->info_bottom = $text;
  }


  public function run ()
  {
    $this->view->render ("text.php");
    Assets_Page::footer ();
    die ();
  }  

  
}
?>

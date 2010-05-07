<?php


/**
* Dialog that presents the user with a multi-line text area for text entry.
*/
class Dialog_Text
{
  private $smarty;
    
  /**
  * Text dialog constructor.
  * $header: What header to show.
  * $text: Initial text for the user to edit.
  * $parameter: The GET parameter to be used to save the text.
  */
  public function __construct ($header, $text, $parameter)
  {
    $this->smarty = new Smarty_Bibledit (__FILE__);
    $this->smarty->assign ("caller", $_SERVER["PHP_SELF"]);
    $database_sessions = Database_Sessions::getInstance ();
    $this->smarty->assign ("session", $database_sessions->getCurrentSessionId ());
    $this->smarty->assign ("header", $header);
    $this->smarty->assign ("parameter", $parameter);
    $this->smarty->assign ("text", $text);
  }


  public function info_top ($text)
  {
    $this->smarty->assign ("info_top", $text);
  }
  
  
  public function info_bottom ($text)
  {
    $this->smarty->assign ("info_bottom", $text);
  }


  public function run ()
  {
    $this->smarty->display("text.tpl");
    Assets_Page::footer ();
    die ();
  }  

  
}
?>


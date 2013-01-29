<?php

require_once ("../bootstrap/bootstrap.php");

page_access_level (ADMIN_LEVEL);

Assets_Page::header (gettext ("Searching"));

$smarty = new Smarty_Bibledit (__FILE__);

$config_general = Database_Config_General::getInstance ();

if (isset($_POST['submit'])) {
  $port = $_POST['port'];
  $port = Filter_Numeric::integer_in_string ($port);
  // Fall back on default Sphinx search port number.
  if ($port == 0) $port = 9312; 
  if ($config_general->getSearchDaemonPort () != $port) {
    $smarty->assign ("success", gettext ("The port number has been set"));
  } else {
    $smarty->assign ("success", gettext ("The port number was not changed"));
  }
  $config_general->setSearchDaemonPort ($port);
}

$smarty->assign ("port", $config_general->getSearchDaemonPort ());
$smarty->display("searching.tpl");

Assets_Page::footer ();

?>

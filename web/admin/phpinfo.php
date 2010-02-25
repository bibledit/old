<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("phpinfo.tpl");
phpinfo ();
?>

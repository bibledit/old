<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
Assets_Page::header (gettext ("PHP Info"));
$view = new Assets_View (__FILE__);
$view->render ("phpinfo.php");
phpinfo ();
Assets_Page::footer ();
?>

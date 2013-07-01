<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
Assets_Page::header (gettext ("Timezone"));
$database_config_general = Database_Config_General::getInstance ();
@$timezone =  $_GET['timezone'];
if (isset ($timezone)) {
  if ($timezone == "") {
    $dialog_list = new Dialog_List2 (gettext ("Would you like to change the default timezone for the site?"));
    $timezone_identifiers = Filter_Datetime::timezones();
    foreach ($timezone_identifiers as $timezone_identifier) {
      $dialog_list->add_row ($timezone_identifier, "&timezone=$timezone_identifier");
    }
    $dialog_list->run ();
  } else {
    $database_config_general->setTimezone ($timezone);
  }
}
$view = new Assets_View (__FILE__);
$timezone = $database_config_general->getTimezone();
$view->view->timezone = $timezone;
$view->render ("timezone.php");
Assets_Page::footer ();
?>

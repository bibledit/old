<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (MEMBER_LEVEL);
Assets_Page::header (gettext ("Timezone"));
$database_config_user = Database_Config_User::getInstance ();
$timezone =  $_GET['timezone'];
if (isset ($timezone)) {
  if ($timezone == "") {
    $dialog_list = new Dialog_List2 (gettext ("Would you like to change your timezone?"));
    $timezone_identifiers = Filter_Datetime::timezones();
    foreach ($timezone_identifiers as $timezone_identifier) {
      $dialog_list->add_row ($timezone_identifier, "&timezone=$timezone_identifier");
    }
    $dialog_list->run ();
  } else {
    $database_config_user->setTimezone ($timezone);
  }
}
$smarty = new Smarty_Bibledit (__FILE__);
$timezone = $database_config_user->getTimezone();
$smarty->assign ("timezone", $timezone);
$smarty->display("timezone.tpl");
Assets_Page::footer ();
?>

<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (TRANSLATOR_LEVEL);


$header = gettext ("Would you like to switch to another stylesheet?");
$info_top = gettext ("Here are the various stylesheets available to you:");
$info_bottom = gettext ("Please pick one.");
$dialog_list = new Dialog_List ("", $header, $info_top, $info_bottom);
$database_styles = Database_Styles::getInstance();
$sheets = $database_styles->getSheets();
foreach ($sheets as $sheet) {
  $parameter = "?sheet=$sheet";
  $dialog_list->add_row ($sheet, $parameter);
}
$dialog_list->run ();


?>

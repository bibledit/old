<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);

$header = gettext ("Would you like to change whether this style is in bold?");
$info_top = gettext ("Here are the various options:");
$info_bottom = gettext ("Please pick one.");
$sheet = $_GET['sheet'];
$style = $_GET['style'];
$dialog_list = new Dialog_List (array ("sheet"=> $sheet, "style" => $style), $header, $info_top, $info_bottom);
$database_styles = Database_Styles::getInstance();
$styles_logic = Styles_Logic::getInstance ();
$marker_data = $database_styles->getMarkerData ($sheet, $style);
$last_value = ooitOn;
if ($styles_logic->italicBoldUnderLineSmallcapsAreFull ($marker_data['type'], $marker_data['subtype']))
  $last_value = ooitToggle;
for ($i = 0; $i <= $last_value; $i++) {
  $dialog_list->add_row ($styles_logic->OffOnInheritToggleText ($i), "&bold=$i");
}
$dialog_list->run ();


?>

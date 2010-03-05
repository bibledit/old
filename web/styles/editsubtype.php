<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);


$header = gettext ("Would you like to change the sub type of this style?");
$info_top = gettext ("Here are the various options:");
$info_bottom = gettext ("Please pick one.");
$sheet = $_GET['sheet'];
$style = $_GET['style'];
$dialog_list = new Dialog_List (array ("sheet"=> $sheet, "style" => $style), $header, $info_top, $info_bottom);
$database_styles = Database_Styles::getInstance();
$styles_logic = Styles_Logic::getInstance ();
$marker_data = $database_styles->getMarkerData ($sheet, $style);
$type = $marker_data['type'];
for ($i = 0; $i < 99; $i++) {
  $text = $styles_logic->subtypeText ($type, $i);
  if (strlen ($text) > 2) {
    $dialog_list->add_row ($text, "&subtype=$i");
  }
}
$dialog_list->run ();


?>

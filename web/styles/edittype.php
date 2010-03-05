<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);


$header = gettext ("Would you like to change the type of this style?");
$info_top = gettext ("Here are the various types:");
$info_bottom = gettext ("Please pick one.");
$sheet = $_GET['sheet'];
$style = $_GET['style'];
$dialog_list = new Dialog_List (array ("sheet"=> $sheet, "style" => $style), $header, $info_top, $info_bottom);

$styles_logic = Styles_Logic::getInstance ();

for ($i = 0; $i < 99; $i++) {
  $text = $styles_logic->typeText ($i);
  if (strlen ($text) > 2) {
    $dialog_list->add_row ($text, "&type=$i");
  }
}

$dialog_list->run ();


?>

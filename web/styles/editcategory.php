<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);


$header = gettext ("Would you like to change the category of this style?");
$info_top = gettext ("Here are the various categories:");
$info_bottom = gettext ("Please pick one.");
$sheet = $_GET['sheet'];
$style = $_GET['style'];
$dialog_list = new Dialog_List (array ("sheet"=> $sheet, "style" => $style), $header, $info_top, $info_bottom);

$styles_logic = Styles_Logic::getInstance ();

$dialog_list->add_row ($styles_logic->categoryText ("id"),   "&category=id");
$dialog_list->add_row ($styles_logic->categoryText ("ith"),  "&category=ith");
$dialog_list->add_row ($styles_logic->categoryText ("ipp"),  "&category=ipp");
$dialog_list->add_row ($styles_logic->categoryText ("ioe"),  "&category=ioe");
$dialog_list->add_row ($styles_logic->categoryText ("t"),    "&category=t");
$dialog_list->add_row ($styles_logic->categoryText ("h"),    "&category=h");
$dialog_list->add_row ($styles_logic->categoryText ("cv"),   "&category=cv");
$dialog_list->add_row ($styles_logic->categoryText ("p"),    "&category=p");
$dialog_list->add_row ($styles_logic->categoryText ("l"),    "&category=l");
$dialog_list->add_row ($styles_logic->categoryText ("pe"),   "&category=pe");
$dialog_list->add_row ($styles_logic->categoryText ("te"),   "&category=te");
$dialog_list->add_row ($styles_logic->categoryText ("f"),    "&category=f");
$dialog_list->add_row ($styles_logic->categoryText ("x"),    "&category=x");
$dialog_list->add_row ($styles_logic->categoryText ("xsn"),  "&category=xsn");
$dialog_list->add_row ($styles_logic->categoryText ("st"),   "&category=st");
$dialog_list->add_row ($styles_logic->categoryText ("cs"),   "&category=cs");
$dialog_list->add_row ($styles_logic->categoryText ("sb"),   "&category=sb");
$dialog_list->add_row ($styles_logic->categoryText ("sf"),   "&category=sf");
$dialog_list->add_row ($styles_logic->categoryText ("pm"),   "&category=pm");
$dialog_list->add_row ($styles_logic->categoryText (""),     "&category=");

$dialog_list->run ();


?>

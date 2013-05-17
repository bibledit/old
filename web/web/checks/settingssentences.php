<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);


Assets_Page::header (gettext ("Sentence Structure"));
$smarty = new Smarty_Bibledit (__FILE__);
$database_config_general = Database_Config_General::getInstance();


if (isset($_POST['capitals'])) {
  $database_config_general->setSentenceStructureCapitals ($_POST['capitals']);
  $smarty->assign ("success", gettext ("The capitals were stored"));
}


if (isset($_POST['smallletters'])) {
  $database_config_general->setSentenceStructureSmallLetters ($_POST['smallletters']);
  $smarty->assign ("success", gettext ("The small letters were stored"));
}


if (isset($_POST['endpunctuationmarks'])) {
  $database_config_general->setSentenceStructureEndPunctuation ($_POST['endpunctuationmarks']);
  $smarty->assign ("success", gettext ("The punctuation marks at the ends of sentences were stored"));
}


if (isset($_POST['middlepunctuationmarks'])) {
  $database_config_general->setSentenceStructureMiddlePunctuation ($_POST['middlepunctuationmarks']);
  $smarty->assign ("success", gettext ("The punctuation marks within the sentences were stored"));
}


if (isset($_POST['disregards'])) {
  $database_config_general->setSentenceStructureDisregards ($_POST['disregards']);
  $smarty->assign ("success", gettext ("The characters that should be disregarded within the sentences were stored"));
}


if (isset($_POST['names'])) {
  $database_config_general->setSentenceStructureNames ($_POST['names']);
  $smarty->assign ("success", gettext ("The names that may occur after mid-sentence punctuation were stored"));
}


$smarty->assign ("capitals", Filter_Html::sanitize ($database_config_general->getSentenceStructureCapitals ()));
$smarty->assign ("smallletters", Filter_Html::sanitize ($database_config_general->getSentenceStructureSmallLetters ()));
$smarty->assign ("endpunctuationmarks", Filter_Html::sanitize ($database_config_general->getSentenceStructureEndPunctuation ()));
$smarty->assign ("middlepunctuationmarks", Filter_Html::sanitize ($database_config_general->getSentenceStructureMiddlePunctuation ()));
$smarty->assign ("disregards", Filter_Html::sanitize ($database_config_general->getSentenceStructureDisregards ()));
$smarty->assign ("names", Filter_Html::sanitize ($database_config_general->getSentenceStructureNames ()));
$smarty->display("settingssentences.tpl");
Assets_Page::footer ();


?>

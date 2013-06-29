<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);


Assets_Page::header (gettext ("Sentence Structure"));
$view = new Assets_View (__FILE__);
$database_config_general = Database_Config_General::getInstance();


if (isset($_POST['capitals'])) {
  $database_config_general->setSentenceStructureCapitals ($_POST['capitals']);
  $view->view->success = gettext ("The capitals were stored");
}


if (isset($_POST['smallletters'])) {
  $database_config_general->setSentenceStructureSmallLetters ($_POST['smallletters']);
  $view->view->success = gettext ("The small letters were stored");
}


if (isset($_POST['endpunctuationmarks'])) {
  $database_config_general->setSentenceStructureEndPunctuation ($_POST['endpunctuationmarks']);
  $view->view->success = gettext ("The punctuation marks at the ends of sentences were stored");
}


if (isset($_POST['middlepunctuationmarks'])) {
  $database_config_general->setSentenceStructureMiddlePunctuation ($_POST['middlepunctuationmarks']);
  $view->view->success = gettext ("The punctuation marks within the sentences were stored");
}


if (isset($_POST['disregards'])) {
  $database_config_general->setSentenceStructureDisregards ($_POST['disregards']);
  $view->view->success = gettext ("The characters that should be disregarded within the sentences were stored");
}


if (isset($_POST['names'])) {
  $names = $_POST['names'];
  $names = explode ("\n", $names);
  foreach ($names as &$name) $name = trim ($name);
  $database_config_general->setSentenceStructureNames ($names);
  $view->view->success = gettext ("The names that may occur after mid-sentence punctuation were stored");
}


$view->view->capitals = Filter_Html::sanitize ($database_config_general->getSentenceStructureCapitals ());
$view->view->smallletters = Filter_Html::sanitize ($database_config_general->getSentenceStructureSmallLetters ());
$view->view->endpunctuationmarks = Filter_Html::sanitize ($database_config_general->getSentenceStructureEndPunctuation ());
$view->view->middlepunctuationmarks = Filter_Html::sanitize ($database_config_general->getSentenceStructureMiddlePunctuation ());
$view->view->disregards = Filter_Html::sanitize ($database_config_general->getSentenceStructureDisregards ());
$view->view->names = Filter_Html::sanitize (implode ("\n", $database_config_general->getSentenceStructureNames ()));
$view->render ("settingssentences.php");
Assets_Page::footer ();


?>

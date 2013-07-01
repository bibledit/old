<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
Assets_Page::header (gettext ("Language"));
$database_config_general = Database_Config_General::getInstance ();
@$language =  $_GET['language'];
if (isset ($language)) {
  if ($language == "") {
    $dialog_list = new Dialog_List2 (gettext ("Set the default language for the site"));
    $languages = Locale_Logic::getLocalizations ();
    foreach ($languages as $language) {
      $dialog_list->add_row (Locale_Logic::getLanguage ($language), "&language=$language");
    }
    $dialog_list->run ();
  } else {
    $database_config_general->setSiteLanguage (Locale_Logic::filterDefault ($language));
  }
}
$view = new Assets_View (__FILE__);
$view->view->language = Locale_Logic::getLanguage ($database_config_general->getSiteLanguage());
$view->render ("language.php");
Assets_Page::footer ();
?>

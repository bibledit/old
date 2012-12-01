<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
Assets_Page::header (gettext ("Hyphenation"));
$smarty = new Smarty_Bibledit (__FILE__);

$config_general = Database_Config_General::getInstance ();
$success = "";
$error = "";


// Character sets submission.
if (isset($_POST['sets'])) {
  $firstset = $_POST['firstset'];
  $config_general->setHyphenationFirstSet ($firstset);
  $secondset = $_POST['secondset'];
  $config_general->setHyphenationSecondSet ($secondset);
  $success = gettext ("The two sets of characters were saved");
}
$firstset = $config_general->getHyphenationFirstSet ();
$secondset = $config_general->getHyphenationSecondSet ();


@$bible = $_GET ['bible'];
@$changebible = $_GET['changebible'];
if (isset ($changebible)) {
  if ($changebible == "") {
    $dialog_list = new Dialog_List2 (gettext ("Which Bible would you like to take the data from?"));
    $database_bibles = Database_Bibles::getInstance ();
    $bibles = $database_bibles->getBibles ();
    foreach ($bibles as $item) {
      $dialog_list->add_row ($item, "&bible=$bible&changebible=$item");
    }
    $dialog_list->run();
  } else {
    $bible = $changebible;
  }
}


if (isset($_GET['run'])) {
  if ($bible == "") {
    $error = gettext ("No Bible given");
  } else if ($firstset == "") {
    $error = gettext ("No first set of characters given");
  } else if ($secondset == "") {
    $error = gettext ("No second set of characters given");
  } else {
    $workingdirectory = escapeshellarg (dirname (__FILE__));
    $input = escapeshellarg ($bible);
    shell_exec ("cd $workingdirectory; php hyphenate.php $input > /dev/null 2>&1 &");
    $success = gettext ("The Bible is being hyphenated. See the logbook for details.");
  }
}


$smarty->assign ("firstset", $firstset);
$smarty->assign ("secondset", $secondset);
$smarty->assign ("bible", $bible);
$smarty->assign ("success", $success);
$smarty->assign ("error", $error);
$smarty->display("hyphenation.tpl");
Assets_Page::footer ();
?>
<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
Assets_Page::header (gettext ("Mail"));


$smarty = new Smarty_Bibledit (__FILE__);


// Email form submission.
if (isset($_POST['email'])) {
  $form_is_valid = true;
  $sitename = $_POST['sitename'];
  $sitemail = $_POST['sitemail'];
  if (strlen ($sitemail) > 0) {
    $validator = new Zend_Validate_EmailAddress ();
    if (!$validator->isValid ($sitemail)) {
      $form_is_valid = false;
      $smarty->assign ('site_name_error', gettext ("The email address does not appear to be valid"));
    }
  }
  if ($form_is_valid) {
    $config_general = Database_Config_General::getInstance ();
    $config_general->setSiteMailName ($sitename);
    $config_general->setSiteMailAddress ($sitemail);
    $smarty->assign ("site_name_success", gettext ("The name and email address were saved"));
  }
}


// Mail storage form submission.
if (isset($_POST['retrieve'])) {
  $storagehost =     $_POST['storagehost'];
  $storageusername = $_POST['storageusername'];
  $storagepassword = $_POST['storagepassword'];
  $storagesecurity = $_POST['storagesecurity'];
  $storageport     = $_POST['storageport'];
  $storage_success = "";
  $storage_error = "";
  $config_general = Database_Config_General::getInstance ();
  $config_general->setMailStorageHost ($storagehost);
  $config_general->setMailStorageUsername ($storageusername);
  $config_general->setMailStoragePassword ($storagepassword);
  $config_general->setMailStorageSecurity ($storagesecurity);
  $config_general->setMailStoragePort     ($storageport);
  $storage_success .= " " . gettext ("The details were saved.");
  try {
    $mail = new Mail_Receive ();
    $storage_success .= " " . gettext ("The account was accessed successfully.") . " " . gettext ("Messages on server:") . " " . $mail->count . ".";
  } catch (Exception $e) {
    $storage_error .= " " . $e->getMessage ();
  }
  $smarty->assign ("storage_success", $storage_success);
  $smarty->assign ("storage_error", $storage_error);
}


// Mail sending form submission.
if (isset($_POST['send'])) {
  $sendhost =           $_POST['sendhost'];
  $sendauthentication = $_POST['sendauthentication'];
  $sendusername =       $_POST['sendusername'];
  $sendpassword =       $_POST['sendpassword'];
  $sendsecurity =       $_POST['sendsecurity'];
  $sendport     =       $_POST['sendport'];
  $config_general = Database_Config_General::getInstance ();
  $config_general->setMailSendHost           ($sendhost);
  $config_general->setMailSendAuthentication ($sendauthentication);
  $config_general->setMailSendUsername       ($sendusername);
  $config_general->setMailSendPassword       ($sendpassword);
  $config_general->setMailSendSecurity       ($sendsecurity);
  $config_general->setMailSendPort           ($sendport);
  @$send_success .= " " . gettext ("The details were saved.");
  try {
    $mail = new Mail_Send($config_general->getSiteMailAddress(), $config_general->getSiteMailName(), "Test", "This is to try out whether Bibledit-Web can send email.");
    $send_success .= " " . gettext ("For the purpose of trying whether Bibledit-Web can send email, a test email was sent out to the account above:") . " " . $config_general->getSiteMailAddress();
  } catch (Exception $e) {
    $send_error .= " " . $e->getMessage ();
  }
  @$smarty->assign ("send_success", $send_success);
  @$smarty->assign ("send_error", $send_error);

}


/**
* Normal page display.
*/
$config_general = Database_Config_General::getInstance ();
$smarty->assign ("sitename",           $config_general->getSiteMailName ());
$smarty->assign ("sitemail",           $config_general->getSiteMailAddress ());
$smarty->assign ("storagehost",        $config_general->getMailStorageHost ());
$smarty->assign ("storageusername",    $config_general->getMailStorageUsername ());
$smarty->assign ("storagepassword",    $config_general->getMailStoragePassword ());
$smarty->assign ("storagesecurity",    $config_general->getMailStorageSecurity ());
$smarty->assign ("storageport",        $config_general->getMailStoragePort ());
$smarty->assign ("sendhost",           $config_general->getMailSendHost ());
$smarty->assign ("sendauthentication", $config_general->getMailSendAuthentication ());
$smarty->assign ("sendusername",       $config_general->getMailSendUsername ());
$smarty->assign ("sendpassword",       $config_general->getMailSendPassword ());
$smarty->assign ("sendsecurity",       $config_general->getMailSendSecurity ());
$smarty->assign ("sendport",           $config_general->getMailSendPort ());
$smarty->display("mail.tpl");


Assets_Page::footer ();

?>

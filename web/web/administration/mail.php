<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
Assets_Page::header (gettext ("Mail"));


$view = new Assets_View (__FILE__);


// Email form submission.
if (isset($_POST['email'])) {
  $form_is_valid = true;
  $sitename = $_POST['sitename'];
  $sitemail = $_POST['sitemail'];
  if (strlen ($sitemail) > 0) {
    $validator = new Zend_Validate_EmailAddress ();
    if (!$validator->isValid ($sitemail)) {
      $form_is_valid = false;
      $view->view->site_name_error = gettext ("The email address does not appear to be valid");
    }
  }
  if ($form_is_valid) {
    $config_general = Database_Config_General::getInstance ();
    $config_general->setSiteMailName ($sitename);
    $config_general->setSiteMailAddress ($sitemail);
    $view->view->site_name_success = gettext ("The name and email address were saved");
  }
}


// Mail storage form submission.
if (isset($_POST['retrieve'])) {
  $storagehost = $_POST['storagehost'];
  $storageusername = $_POST['storageusername'];
  $storagepassword = $_POST['storagepassword'];
  $storagesecurity = $_POST['storagesecurity'];
  $storageport = $_POST['storageport'];
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
  $view->view->storage_success = $storage_success;
  $view->view->storage_error = $storage_error;
}


// Mail sending form submission.
if (isset($_POST['send'])) {
  $sendhost = $_POST['sendhost'];
  $sendauthentication = $_POST['sendauthentication'];
  $sendusername = $_POST['sendusername'];
  $sendpassword = $_POST['sendpassword'];
  $sendsecurity = $_POST['sendsecurity'];
  $sendport  = $_POST['sendport'];
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
  @$view->view->send_success = $send_success;
  @$view->view->send_error = $send_error;

}


/**
* Normal page display.
*/
$config_general = Database_Config_General::getInstance ();
$view->view->sitename = $config_general->getSiteMailName ();
$view->view->sitemail = $config_general->getSiteMailAddress ();
$view->view->storagehost = $config_general->getMailStorageHost ();
$view->view->storageusername = $config_general->getMailStorageUsername ();
$view->view->storagepassword = $config_general->getMailStoragePassword ();
$view->view->storagesecurity = $config_general->getMailStorageSecurity ();
$view->view->storageport = $config_general->getMailStoragePort ();
$view->view->sendhost = $config_general->getMailSendHost ();
$view->view->sendauthentication = $config_general->getMailSendAuthentication ();
$view->view->sendusername = $config_general->getMailSendUsername ();
$view->view->sendpassword = $config_general->getMailSendPassword ();
$view->view->sendsecurity = $config_general->getMailSendSecurity ();
$view->view->sendport = $config_general->getMailSendPort ();
$view->render ("mail.php");


Assets_Page::footer ();

?>

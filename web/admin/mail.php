<?php


require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);


$smarty = new Smarty_Bibledit (__FILE__);


// Email form submission.
if (isset($_POST['email'])) {
  $form_is_valid = true;
  $sitename = $_POST['sitename'];
  $sitemail = $_POST['sitemail'];
  if (strlen ($sitemail) > 0) {
    if (!Validate_Email::valid ($sitemail)) {
      $form_is_valid = false;
      $smarty->assign ('site_name_error', gettext ("The email address does not appear to be valid"));
    }
  }
  if ($form_is_valid) {
    $config_general = Config_General::getInstance ();
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
  $storage_success;
  $storage_error;
  $config_general = Config_General::getInstance ();
  $config_general->setMailStorageHost ($storagehost);
  $config_general->setMailStorageUsername ($storageusername);
  $config_general->setMailStoragePassword ($storagepassword);
  $storage_success .= " " . gettext ("The host, the username and the password were saved.");
  try {
    $config = array('host' => $storagehost, 'user' => $storageusername, 'password' => $storagepassword);
    $mail = new Zend_Mail_Storage_Pop3($config);
    $storage_success .= " " . gettext ("The account was accessed successfully.") . " " . gettext ("Messages on server:") . " " . $mail->countMessages() . ".";
  } catch (Exception $e) {
    $storage_error .= " " . $e->getMessage ();
  }
  $smarty->assign ("storage_success", $storage_success);
  $smarty->assign ("storage_error", $storage_error);

}


// Mail sending form submission.
if (isset($_POST['send'])) {
  $sendhost =     $_POST['sendhost'];
  $sendusername = $_POST['sendusername'];
  $sendpassword = $_POST['sendpassword'];
  $send_success;
  $send_error;
  $config_general = Config_General::getInstance ();
  $config_general->setMailSendHost ($sendhost);
  $config_general->setMailSendUsername ($sendusername);
  $config_general->setMailSendPassword ($sendpassword);
  $send_success .= " " . gettext ("The host was saved.");
  try {
    $mail = new Mail_Send($config_general->getSiteMailAddress(), $config_general->getSiteMailName(), "Test", "This is to try out whether Bibledit can send email.");
    $send_success .= " " . gettext ("For the purpose of trying whether Bibledit can send email, a test email was sent out to the account above:") . " " . $config_general->getSiteMailAddress();
  } catch (Exception $e) {
    $send_error .= " " . $e->getMessage ();
  }
  $smarty->assign ("send_success", $send_success);
  $smarty->assign ("send_error", $send_error);

}


/**
* Normal page display.
*/
$config_general = Config_General::getInstance ();
$smarty->assign ("sitename", $config_general->getSiteMailName ());
$smarty->assign ("sitemail", $config_general->getSiteMailAddress ());
$smarty->assign ("storagehost", $config_general->getMailStorageHost ());
$smarty->assign ("storageusername", $config_general->getMailStorageUsername ());
$smarty->assign ("storagepassword", $config_general->getMailStoragePassword ());
$smarty->assign ("sendhost", $config_general->getMailSendHost ());
$smarty->assign ("sendusername", $config_general->getMailSendUsername ());
$smarty->assign ("sendpassword", $config_general->getMailSendPassword ());
$smarty->display("mail.tpl");



/*

Todo create site mailer.



store incoming and outgoing mail.
Steps: 
- system in database for storing mails to be sent out.
- each user has a page to display the mails in the inbox and the ones that were mailed out on his email.
* the site also has a page, but differently, since it processes mail upon arrival - yes, it stores outgoing mail
- upon login the number of messages in the inbox should be displayed, with a link to go there.
- messages can be deleted.
- new ones to other members of the site can be written.
* The site mailer's errors go to the administrator's email box.


There should be a page for setting up the crontab for the web server's user.
The cron job should call a certain page every minute. 
There's a lock in the server / php's memory so that if one task is still going on,
others are not started.



*/
?>

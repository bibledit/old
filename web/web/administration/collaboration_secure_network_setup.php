<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);

$smarty = new Smarty_Bibledit (__FILE__);

$which_git = new Filter_Which ("git");
$smarty->assign ("git", $which_git->available);
$which_ssh = new Filter_Which ("ssh");
$smarty->assign ("ssh", $which_ssh->available);

$database_config_user = Database_Config_User::getInstance();
$database_config_general = Database_Config_General::getInstance();

$object = $_GET ['object'];
$smarty->assign ("object", $object);

if (isset($_POST['url'])) {
  $url = $_POST['urlvalue'];
  $database_config_user->setRemoteRepositoryUrl ($object, $url);
}
$url = $database_config_user->getRemoteRepositoryUrl ($object);
$smarty->assign ("url", $url);

if (isset($_POST['keys'])) {
  $database_config_general->setPrivateSshKey($_POST['privatevalue']);
  $database_config_general->setPublicSshKey($_POST['publicvalue']);
}
$privatekey = $database_config_general->getPrivateSshKey();
$publickey = $database_config_general->getPublicSshKey();
if (($privatekey == "") || ($publickey == "") || (isset($_GET['newkeys']))) {
  $filename = tempnam (sys_get_temp_dir(), '');
  unlink ($filename);
  ob_start ();
  system ("ssh-keygen -t dsa -b 1024 -N '' -f $filename");
  ob_end_clean();
  /*
  This generates a passphrase-less key pair with secure permissions:
  -rw------- 1 www-data www-data 668 2010-04-02 14:25 key
  -rw-r--r-- 1 www-data www-data 604 2010-04-02 14:25 key.pub
  */
  $privatekey = file_get_contents ($filename);
  unlink ($filename); // Private key should not be lying around.
  $publickey  = file_get_contents ("$filename.pub");
  $database_config_general->setPrivateSshKey($privatekey);
  $database_config_general->setPublicSshKey($publickey);
}
$smarty->assign ("privatekey", $privatekey);
$smarty->assign ("publickey", $publickey);

$smarty->display("collaboration_secure_network_setup.tpl");
?>
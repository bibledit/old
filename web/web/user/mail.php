<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MEMBER_LEVEL);

$smarty = new Smarty_Bibledit (__FILE__);
$database_mail = Database_Mail::getInstance();

if (isset ($_GET['delete'])) {
  $id = $_GET['delete'];
  $database_mail->delete ($id);
  $database_mailer = Database_Mailer::getInstance();
  $database_mailer->delete ($id);
}

if (isset ($_GET['view'])) {
  $result  = $database_mail->get ($_GET['view']);
  $row     = $result->fetch_assoc();
  $subject = $row['subject'];
  $subject = strip_tags ($subject);
  $body    = $row['body'];
  $body    = strip_tags ($body, '<p>');
  $smarty->assign ("subject", Filter_Html::sanitize ($subject));
  $smarty->assign ("body",    Filter_Html::sanitize ($body));
}

@$active_label = $_GET['label'];
if ($active_label == "") $active_label = $database_mail->labelInbox ();
$smarty->assign ("active_label", $active_label);

$label_inbox   = $database_mail->labelInbox ();
$link_inbox    = "mail.php?label=$label_inbox";
$smarty->assign ("link_inbox", $link_inbox);

$label_trash   = $database_mail->labelTrash ();
$link_trash    = "mail.php?label=$label_trash";
$smarty->assign ("link_trash", $link_trash);

$ids = array ();
$timestamps = array ();
$subjects = array ();
$deletes = array ();
$views = array ();
$mails = $database_mail->getMails ($active_label);
while ($row = $mails->fetch_assoc()) {
  $id             = $row["id"];
  $ids         [] = $id;
  $time           = date ('j F Y, g:i a', $row["timestamp"]);
  $timestamps  [] = $time;
  $subjects    [] = Filter_Html::sanitize ($row["subject"]);
  $deletes     [] = "mail.php?label=$active_label&delete=$id";
  $views       [] = "mail.php?label=$active_label&view=$id";
}
$smarty->assign ("ids",          $ids);
$smarty->assign ("timestamps",   $timestamps);
$smarty->assign ("subjects",     $subjects);
$smarty->assign ("deletes",      $deletes);
$smarty->assign ("views",        $views);

$smarty->display ("mail.tpl");


?>
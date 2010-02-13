<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MEMBER_LEVEL);

$smarty = new Smarty_Bibledit (__FILE__);
$database_mail = Database_Mail::getInstance();

if ($_GET['delete'] != "") {
  $database_mail->delete ($_GET['delete']);
}

$active_label = $_GET['label'];
if ($active_label == "") $active_label = $database_mail->labelInbox ();
$smarty->assign ("active_label", $active_label);

$label_inbox   = $database_mail->labelInbox ();
$link_inbox    = "mail.php?label=$label_inbox";
$smarty->assign ("link_inbox", $link_inbox);

$label_emailed = $database_mail->labelEmailed ();
$link_emailed  = "mail.php?label=$label_emailed";
$smarty->assign ("link_emailed", $link_emailed);

$label_sent    = $database_mail->labelSent ();
$link_sent     = "mail.php?label=$label_sent";
$smarty->assign ("link_sent", $link_sent);

$label_trash   = $database_mail->labelTrash ();
$link_trash    = "mail.php?label=$label_trash";
$smarty->assign ("link_trash", $link_trash);

$mails = $database_mail->getMails ($active_label);
while ($row = $mails->fetch_assoc()) {
  $id             = $row["id"];
  $ids         [] = $id;
  $time           = date ('j F Y, g:i a', $row["timestamp"]);
  $timestamps  [] = $time;
  $sources     [] = $row["source"];
  $destinations[] = $row["destination"];
  $subjects    [] = $row["subject"];
  $deletes     [] = "mail.php?label=$active_label&delete=$id";
}
$smarty->assign ("ids",          $ids);
$smarty->assign ("timestamps",   $timestamps);
$smarty->assign ("sources",      $sources);
$smarty->assign ("destinations", $destinations);
$smarty->assign ("subjects",     $subjects);
$smarty->assign ("deletes",      $deletes);

$smarty->display ("mail.tpl");

?>

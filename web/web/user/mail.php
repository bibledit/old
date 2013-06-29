<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MEMBER_LEVEL);

Assets_Page::header (gettext ("Mail"));
$view = new Assets_View (__FILE__);
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
  $view->view->subject = Filter_Html::sanitize ($subject);
  $view->view->body = Filter_Html::sanitize ($body);
}

@$active_label = $_GET['label'];
if ($active_label == "") $active_label = $database_mail->labelInbox ();
$view->view->active_label = $active_label;

$label_inbox   = $database_mail->labelInbox ();
$link_inbox    = "mail.php?label=$label_inbox";
$view->view->link_inbox = $link_inbox;

$label_trash   = $database_mail->labelTrash ();
$link_trash    = "mail.php?label=$label_trash";
$view->view->link_trash = $link_trash;

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
$view->view->ids = $ids;
$view->view->timestamps = $timestamps;
$view->view->subjects = $subjects;
$view->view->deletes = $deletes;
$view->view->views = $views;

$view->render ("mail.php");

Assets_Page::footer ();

?>

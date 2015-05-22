<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

require_once ("../bootstrap/bootstrap.php");

// The script runs through the cli Server API only.
Filter_Cli::assert ();

$database_log = Database_Logs::getInstance();

// Bail out when the mail storage host has not been defined, rather than giving an error message.
$config_general = Database_Config_General::getInstance ();
if ($config_general->getMailStorageHost () == "") return;

try {
  $mail_receiver = new Mail_Receive ();
  // Messages start at number 1 instead of 0.
  for ($i = 1; $i <= $mail_receiver->count; $i++) {
    $message = $mail_receiver->storage->getMessage ($i);
    $from = $message->from;
    $subject = $message->subject;
    $log = "Processing email from $from with subject $subject";
    $database_log->log ($log);
    $body = Filter_Email::extractPlainTextMessage ($message);
    $body = $body->__toString ();
    $body = strip_tags ($body);
    $confirm_worker = Confirm_Worker::getInstance ();
    $notes_logic = Notes_Logic::getInstance ();
    if ($confirm_worker->handleEmail ($from, $subject, $body)) {
    }
    else if ($notes_logic->handleEmailComment ($from, $subject, $body)) {
    }
    else if ($notes_logic->handleEmailNew ($from, $subject, $body)) {
    }
    else {
      $log = "Could not allocate email from $from, subject $subject";
      $database_log->log ($log);
      $database_log->log ($body);
    }
    $mail_receiver->storage->removeMessage ($i);
  }
  unset ($mail_receiver);
} catch (Exception $e) {
  $message = $e->getMessage ();
  $database_log->log ($message);
}

?>

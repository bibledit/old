<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2009 Teus Benschop.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


//require_once ("../bootstrap/bootstrap.php"); // comment out.
//$timer_receiver = new Timer_Receiver ();
//$timer_receiver->run ();



class Timer_Receiver 
{

  public function run () 
  {
    $database_log = Database_Logs::getInstance();
    try {
      $mail_receiver = new Mail_Receive ();
      // Messages start at number 1 instead of at 0.
      for ($i = 1; $i <= $mail_receiver->count; $i++) {
        $message = $mail_receiver->storage->getMessage ($i);
        $from = $message->from;
        $subject = $message->subject;
        $log = "Processing email from $from with subject $subject";
        $database_log->log ($log);
        $body = $message->getContent ();
        $body = strip_tags ($body);
        $confirm_worker = Confirm_Worker::getInstance ();
        if (!$confirm_worker->handleEmail ($from, $subject, $body)) {
          $log = "Could not allocate email from $from, subject $subject";
          $database_log->log ($log);
        }
        $mail_receiver->storage->removeMessage ($i);
      }
      unset ($mail_receiver);
    } catch (Exception $e) {
      $message = $e->getMessage ();
      $database_log->log ($message);
    }
  }


}


?>

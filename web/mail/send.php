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


class Mail_Send
{
  public function __construct($to_mail, $to_name, $subject, $body)
  {  
    $config_general = Config_General::getInstance ();
    $mail = new Zend_Mail();
    $mail->setFrom($config_general->getSiteMailAddress(), $config_general->getSiteMailName());
    $mail->addTo($to_mail, $to_name);
    $mail->setSubject($subject);
    $mail->setBodyText($body);
    $smtp_host = $config_general->getMailSendHost();
    //$smtp_user = $config_general->getMailSendUsername();
    //$smtp_password = $config_general->getMailSendPassword();
    if ($smtp_host != "") {
      if ($smtp_user == "") {
        $mta = new Zend_Mail_Transport_Smtp($smtp_host);
      } else {
        $mta = new Zend_Mail_Transport_Smtp($smtp_host); // Temporal filler, should use username and password, but this is not yet implemented.
      }
      $mail->send($mta);
    } else {
      // If no smtp host is given, it uses the default sendmail mail transport agent.
      $mail->send(); 
    }
  }
}


?>

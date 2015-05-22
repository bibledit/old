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


class Mail_Send
{
  public function __construct($to_mail, $to_name, $subject, $body)
  {
    $config_general = Database_Config_General::getInstance ();
    $mail = new Zend_Mail();
    $mail->setFrom($config_general->getSiteMailAddress(), $config_general->getSiteMailName());
    $mail->addTo($to_mail, $to_name);
    $mail->setSubject($subject);
    $mail->setBodyHtml($body, "UTF-8", Zend_Mime::ENCODING_8BIT);
    $smtp_host =           $config_general->getMailSendHost();
    $smtp_authentication = $config_general->getMailSendAuthentication ();
    $smtp_user =           $config_general->getMailSendUsername();
    $smtp_password =       $config_general->getMailSendPassword();
    $smtp_security =       $config_general->getMailSendSecurity();
    $smtp_port =           $config_general->getMailSendPort();
    if ($smtp_host != "") {
      if ($smtp_authentication != "None") {
        $config = array ('auth' => $smtp_authentication, 'username' => $smtp_user, 'password' => $smtp_password);
        $mta = new Zend_Mail_Transport_Smtp($smtp_host);
      }
      if ($smtp_security != "NONE") {
        $config = array_merge ($config, array ('ssl' => $smtp_security));
      }
      if ($smtp_port != "") {
        $config = array_merge ($config, array ('port' => $smtp_port));
      }
      if (isset ($config)) {
        $mta = new Zend_Mail_Transport_Smtp($smtp_host, $config);
      } else {
        $mta = new Zend_Mail_Transport_Smtp($smtp_host);
      }
      $mail->send($mta);
    } else {
      // If no smtp host is given, it uses the default sendmail mail transport agent.
      $mail->send();
    }
  }
}

?>

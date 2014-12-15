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


#include <email/send.h>
#include <webserver/request.h>
#include <database/logs.h>
#include <database/mail.h>
#include <database/users.h>
#include <filter/url.h>
#include <filter/roles.h>


void email_send ()
{
  // The databases involved.
  Webserver_Request request;
  Database_Mail database_mail = Database_Mail (&request);
  Database_Users database_users = Database_Users ();

  vector <int> mails = database_mail.getMailsToSend ();
  for (auto id : mails) {

    // Get all details of the mail.
    Database_Mail_Item details = database_mail.get (id);
    string username = details.username;
    string email = database_users.getUserToEmail (username);
    string subject = details.subject;
    string body = details.body;

    // Bail out when username was empty.
    if (username == "") return;
  
    // If this username was not found, it could be that the email was addressed to a non-user,
    // and that the To: address was actually contained in the username.
    if (email == "") {
      email = username;
      username = "";
    }
  
    // If the email address validates, ok, else remove this mail from the queue and log the action.
    if (!filter_url_email_is_valid (email)) {
      database_mail.erase (id);
      Database_Logs::log ("Email to email was deleted because of an invalid email address");
      continue;
    }
  
    // Send the email.
    // Todo mail = new Mail_Send (email, username, subject, body);
    database_mail.erase (id);
    string message = "Email to " + email + " with subject " + subject + " was sent successfully";
    Database_Logs::log (message, Filter_Roles::manager ());
    /* Todo 
    database_mail.postpone (id);
    message = e.getMessage ();
    message = "Email to email could not be sent - reason: message";
    Database_Logs::log (message);
    */
  }
}


bool email_send (string to_mail, string to_name, string subject, string body)
{

  /* // Todo
  config_general = Database_Config_General::getInstance ();
  mail = new Zend_Mail();
  mail.setFrom(config_general.getSiteMailAddress(), config_general.getSiteMailName());
  mail.addTo(to_mail, to_name);
  mail.setSubject(subject);
  mail.setBodyHtml(body, "UTF-8", Zend_Mime::ENCODING_8BIT);
  smtp_host =           config_general.getMailSendHost();
  smtp_authentication = config_general.getMailSendAuthentication ();
  smtp_user =           config_general.getMailSendUsername();
  smtp_password =       config_general.getMailSendPassword();
  smtp_security =       config_general.getMailSendSecurity();
  smtp_port =           config_general.getMailSendPort();
  if (smtp_host != "") {
    if (smtp_authentication != "None") {
      config = array ('auth' => smtp_authentication, 'username' => smtp_user, 'password' => smtp_password);
      mta = new Zend_Mail_Transport_Smtp(smtp_host);
    }
    if (smtp_security != "NONE") {
      config = array_merge (config, array ('ssl' => smtp_security));
    }
    if (smtp_port != "") {
      config = array_merge (config, array ('port' => smtp_port));
    }
    if (isset (config)) {
      mta = new Zend_Mail_Transport_Smtp(smtp_host, config);
    } else {
      mta = new Zend_Mail_Transport_Smtp(smtp_host);
    }
    mail.send(mta);
  } else {
    // If no smtp host is given, it uses the default sendmail mail transport agent.
    mail.send();
  }
  */
  return true;
}


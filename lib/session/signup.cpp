/*
Copyright (©) 2003-2015 Teus Benschop.

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


#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <session/login.h>
#include <locale/translate.h>
#include <webserver/request.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/url.h>
#include <confirm/worker.h>


class Verification
{
public:
  string question;
  string answer;
  string passage;
};


const char * session_signup_url ()
{
  return "session/signup";
}


bool session_signup_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::guest ());
}


string session_signup (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  string page;

  page += Assets_Page::header (gettext ("Signup"), webserver_request, "");
  Assets_View view = Assets_View ();

  // Some security questions.
  vector <Verification> verifications;
  Verification verification;

  verification.question = gettext("To which city was Paul travelling when a light from heaven shone round about him?");
  verification.answer   = gettext("Damascus");
  verification.passage  = gettext("And while he travelled, he came near Damascus; and suddenly a light from heaven shone round about him.");
  verifications.push_back (verification);
 
  verification.question = gettext("What is the name of the brother of Aaron the high priest?");
  verification.answer   = gettext("Moses");
  verification.passage  = gettext("And the anger of Jehova was kindled against Moses, and he said: Is not Aaron the Levite your brother?");
  verifications.push_back (verification);
  
  verification.question = gettext("What is the name of the city where Jesus was born?");
  verification.answer   = gettext("Bethlehem");
  verification.passage  = gettext("When Jesus was born in Bethlehem of Judaea in the days of Herod the king, behold, wise men from the east came to Jerusalem.");
  verifications.push_back (verification);
  
  verification.question = gettext("What is the name of the island where John was sent to?");
  verification.answer   = gettext("Patmos");
  verification.passage  = gettext("I, John, your brother and companion in the persecution, and in the kingdom and endurance of Jesus Christ, was in the island which is called Patmos, because of the word of God, and because of the testimony of Jesus Christ.");
  verifications.push_back (verification);
  
  int question_number = filter_string_rand (0, 3);
  view.set_variable ("question", verifications[question_number].question);
  view.set_variable ("passage", verifications[question_number].passage);
  // The form has a hidden text entry. This text entry stores the right answer to the questions.
  // When the form is submitted, this right answer is submitted too, and we can check whether
  // the user gave the right answer.
  view.set_variable ("standard", verifications[question_number].answer);
  
  // Form submission handler.
  bool signed_up = false;
  if (request->post["submit"] != "") {
    bool form_is_valid = true;
    string user     = request->post["user"];
    string pass     = request->post["pass"];
    string mail     = request->post["mail"];
    string answer   = request->post["answer"];
    string standard = request->post["standard"];
    if (user.length () < 2) {
      form_is_valid = false;
      view.set_variable ("username_invalid_message", gettext("Username should be at least two characters long"));
    }
    if (pass.length () < 2) {
      form_is_valid = false;
      view.set_variable ("password_invalid_message", gettext("Password should be at least two characters long"));
    }
    if (!filter_url_email_is_valid (mail)) {
      form_is_valid = false;
      view.set_variable ("email_invalid_message", gettext("The email address is not valid"));
    }
    if (answer != standard) {
      form_is_valid = false;
      view.set_variable ("answer_invalid_message", gettext("The answer to the question is not correct"));
    }
    Database_Users database_users = Database_Users ();
    if (form_is_valid) {
      if (database_users.usernameExists (user)) {
        view.set_variable ("error_message", gettext("The username that you have chosen has already been taken. Please choose another one."));
        form_is_valid = false;
      }
    }
    if (form_is_valid) {
      if (database_users.emailExists (mail)) {
        view.set_variable ("error_message", gettext("The email address that you have chosen has already been taken. Please choose another one."));
        form_is_valid = false;
      }
    }
    if (form_is_valid) {
      Confirm_Worker confirm_worker = Confirm_Worker (webserver_request);
      string initial_subject = gettext("Signup verification");
      string initial_body = gettext("There is a request to open an account with this email address.");
      string query = database_users.addNewUserQuery (user, pass, Filter_Roles::member (), mail);
      string subsequent_subject = gettext("Account opened");
      string subsequent_body = gettext("Welcome! Your account is now active.");
      confirm_worker.setup (mail, initial_subject, initial_body, query, subsequent_subject, subsequent_body);
      signed_up = true;
    }
  }

  if (signed_up) page += view.render ("session", "signedup");
  else page += view.render ("session", "signup");

  page += Assets_Page::footer ();

  return page;
}
